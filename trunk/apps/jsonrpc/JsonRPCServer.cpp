/*
 * $Id: ModMysql.cpp 1764 2010-04-01 14:33:30Z peter_lemenkov $
 *
 * Copyright (C) 2010 TelTech Systems Inc.
 * 
 * This file is part of SEMS, a free SIP media server.
 *
 * SEMS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "JsonRPCServer.h"
#include "RpcPeer.h"
#include "JsonRPCEvents.h"
#include "jsonArg.h"

#include "AmEventDispatcher.h"
#include "AmPlugIn.h"
#include "log.h"
#include "AmApi.h"
#include "AmSession.h"
#include "AmUtils.h"

int JsonRpcServer::createRequest(const string& evq_link, const string& method, AmArg& params, JsonrpcNetstringsConnection* peer, 
				 bool is_notification) {
  AmArg rpc_params;
  rpc_params["jsonrpc"] = "2.0";
  rpc_params["method"] = method;
  rpc_params["params"] = params;
  if (!is_notification) {
    peer->req_id++;
    string req_id = int2str(peer->req_id);
    rpc_params["id"] = req_id;
    peer->replyReceivers[req_id] = evq_link;
  }

  string rpc_params_json = arg2json(rpc_params);
  if (rpc_params_json.length() > MAX_RPC_MSG_SIZE) {
    ERROR("internal error: message exceeded MAX_RPC_MSG_SIZE (%d)\n", 
	  MAX_RPC_MSG_SIZE);
    return -3;
  }

  DBG("RPC message: >>%.*s<<\n", rpc_params_json.length(), rpc_params_json.c_str());
  memcpy(peer->msgbuf, rpc_params_json.c_str(), rpc_params_json.length());
  peer->msg_size = rpc_params_json.length();
  // set peer connection up for sending
  peer->msg_recv = false;
  return 0;
}

int JsonRpcServer::processMessage(char* msgbuf, unsigned int* msg_size, 
				  JsonrpcPeerConnection* peer) {
  DBG("parsing message ...\n");
  // const char* txt = "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
  AmArg rpc_params;
  if (!json2arg(msgbuf, rpc_params)) {
    INFO("Error parsing message '%.*s'\n", (int)msg_size, msgbuf);
    return -1;
  }

  if (!rpc_params.hasMember("jsonrpc") || strcmp(rpc_params["jsonrpc"].asCStr(), "2.0")) {
    INFO("wrong json-rpc version received; only 2.0 supported!\n");
    return -2; // todo: check value, reply with error?
  }

  bool is_request = (rpc_params.hasMember("method") && isArgCStr(rpc_params["method"]));
  if (!is_request) {
    // todo: move this to  peer->processReply(rpc_params);
    
    // process reply
    if (!rpc_params.hasMember("id") || !isArgCStr(rpc_params["id"]) 
	|| rpc_params["id"] == "") {
      INFO("Error parsing jsonrpc message: no id in reply!\n");
      return -2;// todo: check value, reply with error?
    }
    string id = rpc_params["id"].asCStr();
    
    std::map<std::string, std::string>::iterator rep_recv_q = 
      peer->replyReceivers.find(id);
    if (rep_recv_q == peer->replyReceivers.end()) {
      DBG("received repy for unknown request");
      *msg_size = 0;

      if (peer->flags && JsonrpcPeerConnection::FL_CLOSE_WRONG_REPLY) {
	INFO("closing connection after unknown reply id %s received\n", id.c_str());
	return -2;
      }
      return 0;
    }
    JsonRpcResponseEvent* resp_ev = NULL; 
    if (rpc_params.hasMember("result")) {
      resp_ev = new JsonRpcResponseEvent(false, id, rpc_params["result"]);
    } else {
      if (!rpc_params.hasMember("error")) {
	INFO("protocol error: reply does not have error nor result!\n");
	return -2;
      }
      resp_ev = new JsonRpcResponseEvent(true, id, rpc_params["error"]);
    }

    bool posted = AmEventDispatcher::instance()->
      post(rep_recv_q->second, resp_ev);
    if (!posted) {
      DBG("receiver event queue does not exist (any more)\n");
      peer->replyReceivers.erase(rep_recv_q);
      *msg_size = 0;
      if (peer->flags & JsonrpcPeerConnection::FL_CLOSE_NO_REPLYLINK) {
	INFO("closing connection where reply link missing");
	return -2;
      }
      return 0; 
    }	
    DBG("successfully posted reply to event queue\n");
    peer->replyReceivers.erase(rep_recv_q);
    // don't send a reply
    *msg_size = 0;
    return 0;
  }

  string id;
  if (rpc_params.hasMember("id") && isArgCStr(rpc_params["id"])) {
    id = rpc_params["id"].asCStr();
  } else {
    DBG("received notification\n");
  }

  // send directly to event queue
  if ((id.empty() && !peer->notificationReceiver.empty()) || 
      (!id.empty() && !peer->requestReceiver.empty())) {
    // don't send a reply
    *msg_size = 0;

    string dst_evqueue = id.empty() ? 
      peer->notificationReceiver.c_str() : peer->requestReceiver.c_str();

    DBG("directly passing %s to event queue '%s'\n",
	id.empty() ? "notification":"request",
	dst_evqueue.c_str());
    AmArg none_params;
    AmArg& params = none_params;
    if (rpc_params.hasMember("params")) {
      params = rpc_params["params"];
    } 
    JsonRpcRequestEvent* notification_ev = 
      new JsonRpcRequestEvent(rpc_params["method"].asCStr(), 
			      id, params);
    
    bool posted = AmEventDispatcher::instance()-> 
      post(dst_evqueue, notification_ev);
    
    if (!posted) {
      DBG("%s receiver event queue '%s' does not exist (any more)\n",
	   id.empty() ? "notification":"request",
	   dst_evqueue.c_str());
      delete notification_ev;

      if (id.empty() && (peer->flags & JsonrpcPeerConnection::FL_CLOSE_NO_NOTIF_RECV)) {
	INFO("closing connection on missing notification receiver queue\n");
	return -1; // todo: reply error? 
      } 

      if (!id.empty() && (peer->flags & JsonrpcPeerConnection::FL_CLOSE_NO_REQUEST_RECV)) {
	INFO("closing connection on missing request receiver queue\n");
	return -1; // todo: reply error? 
      }
    }	else {
      DBG("successfully posted %s to event queue '%s'\n", 
	  id.empty() ? "notification":"request",
	  dst_evqueue.c_str());
    }
    return 0;
  }

  AmArg rpc_res;

  execRpc(rpc_params, rpc_res);

  // rpc_res["error"] = AmArg(); // Undef/null
  // rpc_res["id"] = rpc_params["id"];

  string res_s = arg2json(rpc_res);
  if (res_s.length() > MAX_RPC_MSG_SIZE) {
    ERROR("internal error: reply exceeded MAX_RPC_MSG_SIZE (%d)\n", 
	  MAX_RPC_MSG_SIZE);
    return -3;
  }
  DBG("RPC result: >>%.*s<<\n", res_s.length(), res_s.c_str());
  memcpy(msgbuf, res_s.c_str(), res_s.length());
  *msg_size = res_s.length();

  return 0;
}

/** rpc_params must contain "method" member as string */
void JsonRpcServer::execRpc(const AmArg& rpc_params, AmArg& rpc_res) {
  try  {
    AmArg none_params;
    AmArg& params = none_params;
    if (rpc_params.hasMember("params")) {
      params = rpc_params["params"];
    } 

    string method = rpc_params["method"].asCStr();
    size_t dot_pos = method.find('.');
    if (dot_pos == string::npos || dot_pos == method.length()) {
      throw JsonRpcError(-32601, "Method not found", 
			 "use module.method as rpc method name");    
    }
    string factory = method.substr(0, method.find('.'));
    string fact_meth = method.substr(method.find('.')+1);

    try {
      if (factory == "core") {
	runCoreMethod(fact_meth, params, rpc_res["result"]);
	rpc_res["id"] = rpc_params["id"];
	rpc_res["error"] = AmArg(); // Undef/null
	rpc_res["jsonrpc"] = "2.0";
	return;
      }
      // todo: direct_export

      DBG("searching for factory '%s' method '%s'\n",
	  factory.c_str(), fact_meth.c_str());
      
      AmDynInvokeFactory* fact = 
	AmPlugIn::instance()->getFactory4Di(factory);
      if (fact==NULL) {
	throw JsonRpcError(-32601, "Method not found", 
			   "module not loaded");
      }
      AmDynInvoke* di_inst = fact->getInstance();
      if(!di_inst) {
	throw JsonRpcError(-32601, "Method not found", 
			   "failed to instanciate module");
      }

      di_inst->invoke(fact_meth, params, rpc_res["result"]);
    } catch (const AmDynInvoke::NotImplemented& ni) {
      INFO("not implemented DI function '%s'\n", 
	   ni.what.c_str());
      throw JsonRpcError(-32601, "Method not found", 
			 "function unknown in module");
    } catch (const AmArg::OutOfBoundsException& oob) {
      INFO("out of bounds in  RPC DI call\n");
      throw JsonRpcError(-32602, "Invalid params",
			 "out of bounds in function call");
    } catch (const AmArg::TypeMismatchException& oob) {
      INFO("type mismatch  in  RPC DI call\n"); 
     throw JsonRpcError(-32602, "Invalid params",
			 "parameters type mismatch in function call");
    } catch (...) {
      ERROR("unexpected Exception in  RPC DI call\n");
      throw JsonRpcError(-32000, "Server error",
			 "unexpected Exception");      
    }

    // todo: notification!
    rpc_res["id"] = rpc_params["id"];
    rpc_res["jsonrpc"] = "2.0";
  } catch (const JsonRpcError& e) {
    INFO("got JsonRpcError core %d message '%s'\n", 
	 e.code, e.message.c_str());
    rpc_res["error"] = AmArg(); 
    rpc_res["error"]["code"] = e.code;
    rpc_res["error"]["message"] = e.message;
    rpc_res["error"]["data"] = e.data;
    // todo: notification!
    rpc_res["id"] = rpc_params["id"];
    rpc_res["jsonrpc"] = "2.0";
    return;
  }
  
  rpc_res["error"] = AmArg(); // Undef/null
}

void JsonRpcServer::runCoreMethod(const string& method, const AmArg& params, AmArg& res) {
  if (method == "calls") {
    res[0] = (int)AmSession::getSessionNum();
  } else if (method == "set_loglevel") {
    assertArgArray(params);
    assertArgInt(params[0]);
    log_level = params[0].asInt();
    DBG("set log_level to %d\n", log_level);
  } else if (method == "get_loglevel") {
    res[0] = log_level;
    DBG("get_log_level returns %d\n", log_level);
  } else {
    throw JsonRpcError(-32601, "Method not found", 
		       "function unknown in core");
  }
}
