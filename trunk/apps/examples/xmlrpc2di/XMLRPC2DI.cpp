/*
 * $Id: XMLRPC2DI.cpp 145 2006-11-26 00:01:18Z sayer $
 *
 * Copyright (C) 2007 iptego GmbH
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * For a license to use the sems software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * sems is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "XMLRPC2DI.h"

#include "AmSessionContainer.h"
#include "AmPlugIn.h"
#include "log.h"
#include "AmConfigReader.h"
#include "AmUtils.h"
#include "AmArg.h"

#define MOD_NAME "xmlrpc2di"

#define XMLRPC_PORT   "8090" // default port
EXPORT_PLUGIN_CLASS_FACTORY(XMLRPC2DI, MOD_NAME)

  XMLRPC2DI::XMLRPC2DI(string mod_name) 
    : AmDynInvokeFactory(mod_name)
{
}

int XMLRPC2DI::onLoad() {

  AmConfigReader cfg;
  if(cfg.loadFile(AmConfig::ModConfigPath + string(MOD_NAME ".conf")))
    return -1;

  string conf_xmlrpc_port = cfg.getParameter("xmlrpc_port",XMLRPC_PORT);
  if (conf_xmlrpc_port.empty()) {
    ERROR("configuration: xmlrpc_port must be defined!\n");
    return -1;
  } 
  
  if (str2i(conf_xmlrpc_port, XMLRPCPort)) {
    ERROR("configuration: unable to decode xmlrpc_port value '%s'!\n", 
	  conf_xmlrpc_port.c_str());
    return -1;
  }

  bool export_di = false;
  string direct_export = cfg.getParameter("direct_export","");
  if (direct_export.length()) {
    DBG("direct_export interfaces: %s\n", direct_export.c_str());
  } else {
    DBG("No direct_export interfaces.\n");
  }

  string export_di_s = cfg.getParameter("export_di","yes");
  if (export_di_s == "yes") {
    export_di = true;
  } 
  
  DBG("XMLRPC Server: %snabling builtin method 'di'.\n", export_di?"E":"Not e");


  server = new XMLRPC2DIServer(XMLRPCPort, export_di, direct_export);

  server->start();
  return 0;
}

// XMLRPC server functions

XMLRPC2DIServer::XMLRPC2DIServer(unsigned int port, 
				 bool di_export, 
				 string direct_export) 
  : port(port),
    // register method 'calls'
    calls_method(&s),
    // register method 'get_loglevel'
    setloglevel_method(&s),
    // register method 'set_loglevel'
    getloglevel_method(&s)
{	
  DBG(" XMLRPC Server: enabled builtin method 'calls'\n");
  DBG("XMLRPC Server: enabled builtin method 'get_loglevel'\n");
  DBG("XMLRPC Server: enabled builtin method 'set_loglevel'\n");

  // export all methods via 'di' function? 
  if (di_export) {
    // register method 'di'
    di_method = new XMLRPC2DIServerDIMethod(&s);
  }
  
  vector<string> export_ifaces = explode(direct_export, ";");
  for(vector<string>::iterator it=export_ifaces.begin(); 
      it != export_ifaces.end(); it++) {
    registerMethods(*it);
  }

  DBG("Initialized XMLRPC2DIServer with: \n");
  DBG("                          port = %u\n", port);
}

/** register all methods on xmlrpc server listed by the iface 
 *    in _list function 
 */
void XMLRPC2DIServer::registerMethods(const std::string& iface) {
  try {
    AmDynInvokeFactory* di_f = AmPlugIn::instance()->getFactory4Di(iface);
    if(NULL == di_f){
      ERROR("DI interface '%s' could not be found. Missing load_plugins?\n", 
	    iface.c_str());
      return;
    } 
    
    AmDynInvoke* di = di_f->getInstance();
    if(NULL == di){
      ERROR("could not get DI instance from '%s'.\n", 
	    iface.c_str());
      return;
    } 
    AmArgArray dummy, fct_list;
    di->invoke("_list", dummy, fct_list);
    
    for (unsigned int i=0;i<fct_list.size();i++) {
      string method = fct_list.get(i).asCStr();
      // see whether method already registered
      bool has_method = (NULL != s.findMethod(method));
      if (has_method) {
	ERROR("name conflict for method '%s' from interface '%s', "
	      "method already exported!\n",
	      method.c_str(), iface.c_str());
	ERROR("This method will be exported only as '%s.%s'\n",
	      iface.c_str(), method.c_str());
      }
      
      if (!has_method) {
	DBG("XMLRPC Server: adding method '%s'\n",
	    method.c_str());
	DIMethodProxy* mp = new DIMethodProxy(method, method, di_f);
	s.addMethod(mp);
      }
      
      DBG("XMLRPC Server: adding method '%s.%s'\n",
	  iface.c_str(), method.c_str());
      DIMethodProxy* mp = new DIMethodProxy(iface + "." + method, 
					    method, di_f);
      s.addMethod(mp);
    }
  } catch (AmDynInvoke::NotImplemented& e) {
    ERROR("Not implemented in interface '%s': '%s'\n", 
	  iface.c_str(), e.what.c_str());
  } catch (const AmArgArray::OutOfBoundsException& e) {
    ERROR("Out of bounds exception occured while exporting interface '%s'\n", 
	  iface.c_str());
  } catch (...) {
    ERROR("Unknown exception occured while exporting interface '%s'\n", 
	  iface.c_str());
  }
}
  

void XMLRPC2DIServer::run() {
  DBG("Binding XMLRPC2DIServer to port %u \n", port);
  s.bindAndListen(port);
  DBG("starting XMLRPC2DIServer...\n");
  s.work(-1.0);
}
void XMLRPC2DIServer::on_stop() {
  DBG("sorry, don't know how to stop the server.\n");
}
void XMLRPC2DIServerCallsMethod::execute(XmlRpcValue& params, XmlRpcValue& result) {
  int res = AmSessionContainer::instance()->getSize();
  DBG("XMLRPC2DI: calls = %d\n", res);
  result = res;
}

void XMLRPC2DIServerGetLoglevelMethod::execute(XmlRpcValue& params, XmlRpcValue& result) {
  int res = log_level;
  DBG("XMLRPC2DI: get_loglevel returns %d\n", res);
  result = res;
}

void XMLRPC2DIServerSetLoglevelMethod::execute(XmlRpcValue& params, XmlRpcValue& result) {
  log_level = params[0];
  DBG("XMLRPC2DI: set log level to %d.\n", (int)params[0]);
  result = "200 OK";
}

void XMLRPC2DIServerDIMethod::execute(XmlRpcValue& params, XmlRpcValue& result) {
  try {
    if (params.size() < 2) {
      DBG("XMLRPC2DI: ERROR: need at least factory name"
	  " and function name to call\n");
      throw XmlRpcException("need at least factory name"
			    " and function name to call", 400);
    }
    
    string fact_name = params[0];
    string fct_name = params[1];

    DBG("XMLRPC2DI: factory '%s' function '%s'\n", 
	fact_name.c_str(), fct_name.c_str());

    // get args
    AmArgArray args;
    XMLRPC2DIServer::xmlrpcval2amarg(params, args, 2);
  
    AmDynInvokeFactory* di_f = AmPlugIn::instance()->getFactory4Di(fact_name);
    if(!di_f){
      throw XmlRpcException("could not get factory", 500);
    }
    AmDynInvoke* di = di_f->getInstance();
    if(!di){
      throw XmlRpcException("could not get instance from factory", 500);
    }
    AmArgArray ret;
    di->invoke(fct_name, args, ret);
  
    XMLRPC2DIServer::amarg2xmlrpcval(ret, result);


  } catch (const XmlRpcException& e) {
    throw;
  } catch (const AmDynInvoke::NotImplemented& e) {
    throw XmlRpcException("Exception: AmDynInvoke::NotImplemented: "
			  + e.what, 504);
  } catch (const AmArgArray::OutOfBoundsException& e) {
    throw XmlRpcException("Exception: AmArgArray out of bounds - paramter number mismatch.", 300);
  } catch (const string& e) {
    throw XmlRpcException("Exception: "+e, 500);
  } catch (...) {
    throw XmlRpcException("Exception occured.", 500);
  }
}

void XMLRPC2DIServer::xmlrpcval2amarg(XmlRpcValue& v, AmArgArray& a, 
				      unsigned int start_index) {
  if (v.valid()) {
    for (int i=start_index; i<v.size();i++) {
      switch (v[i].getType()) {
      case XmlRpcValue::TypeInt:   { a.push(AmArg((int)v[i]));    }  break;
      case XmlRpcValue::TypeDouble:{ a.push(AmArg((double)v[i])); }  break;
      case XmlRpcValue::TypeString:{ a.push(AmArg(((string)v[i]).c_str())); }  break;
	// TODO: support more types (datetime, struct, ...)
      default:     throw XmlRpcException("unsupported parameter type", 400);
      };
    }
  } 
}

void XMLRPC2DIServer::amarg2xmlrpcval(AmArgArray& a, 
				      XmlRpcValue& result) {
  if (a.size()) {
    result.setSize(a.size());
    
    for (unsigned int i=0;i<a.size();i++) {
	const AmArg& r = a.get(i);
	amarg2xmlrpcval(r, result, i);
    }
  }
}

// WARNING: AmArgArrays are deleted by this function!
void XMLRPC2DIServer::amarg2xmlrpcval(const AmArg& a, XmlRpcValue& result, 
				      unsigned int pos) {
  switch (a.getType()) {
  case AmArg::CStr:  
    result[pos]= string(a.asCStr()); break;

  case AmArg::Int:  
    result[pos]=a.asInt(); break;

  case AmArg::Double: 
    result[pos]=a.asDouble(); break;

  case AmArg::AObject: {
    AmArgArray* arr = dynamic_cast<AmArgArray*>(a.asObject());
    if (NULL != arr) {
      result[pos].setSize(arr->size());
    
      for (unsigned int i=0;i<arr->size();i++) {
	const AmArg& r = arr->get(i);
	amarg2xmlrpcval(r, result[pos], i);
      }
      delete arr; 
    } else {
      WARN("unsupported return value type of parameter %d (not AmArgArray)\n", pos);
    }
  } break;
  default: { WARN("unsupported return value type %d\n", a.getType()); } break;
    // TODO: do sth with the data here ?
  }
}

DIMethodProxy::DIMethodProxy(std::string const &server_method_name, 
			     std::string const &di_method_name, 
			     AmDynInvokeFactory* di_factory)
  : server_method_name(server_method_name),
    di_method_name(di_method_name),
    di_factory(di_factory),
    XmlRpcServerMethod(server_method_name)
{ }    
  
void DIMethodProxy::execute(XmlRpcValue& params, 
			    XmlRpcValue& result) {

  try {
    if (NULL == di_factory) {
      throw XmlRpcException("could not get DI factory", 500);
    }
  
    AmDynInvoke* di = di_factory->getInstance();
    if(NULL == di){
      throw XmlRpcException("could not get instance from factory", 500);
    }
    
    AmArgArray args, ret;
    XMLRPC2DIServer::xmlrpcval2amarg(params, args);
    
    DBG("XMLRPC2DI '%s': function '%s'\n", 
	server_method_name.c_str(),
	di_method_name.c_str());

    di->invoke(di_method_name, args, ret);
    
    XMLRPC2DIServer::amarg2xmlrpcval(ret, result);

  } catch (const XmlRpcException& e) {
    throw;
  } catch (const AmDynInvoke::NotImplemented& e) {
    throw XmlRpcException("Exception: AmDynInvoke::NotImplemented: "
			  + e.what, 504);
  } catch (const AmArgArray::OutOfBoundsException& e) {
    throw XmlRpcException("Exception: AmArgArray out of bounds - paramter number mismatch.", 300);
  } catch (const string& e) {
    throw XmlRpcException("Exception: "+e, 500);
  } catch (...) {
    throw XmlRpcException("Exception occured.", 500);
  }
}
