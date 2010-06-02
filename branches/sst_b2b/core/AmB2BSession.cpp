/*
 * $Id$
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
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
#include "AmB2BSession.h"
#include "AmSessionContainer.h"
#include "AmConfig.h"
#include "ampi/MonitoringAPI.h"

#include <assert.h>

//
// AmB2BSession methods
//

AmB2BSession::AmB2BSession()
  : sip_relay_only(true), other_leg_inv_pending(false)
{
}

AmB2BSession::AmB2BSession(const string& other_local_tag)
  : other_id(other_local_tag),
    sip_relay_only(true)
{
}


AmB2BSession::~AmB2BSession()
{
  DBG("relayed_req.size() = %u\n",(unsigned int)relayed_req.size());
  DBG("recvd_req.size() = %u\n",(unsigned int)recvd_req.size());
}

void AmB2BSession::set_sip_relay_only(bool r) { 
  sip_relay_only = r; 
}

void AmB2BSession::clear_other()
{
#if __GNUC__ < 3
  string cleared ("");
  other_id.assign (cleared, 0, 0);
#else
  other_id.clear();
#endif
}

void AmB2BSession::process(AmEvent* event)
{
  B2BEvent* b2b_e = dynamic_cast<B2BEvent*>(event);
  if(b2b_e){

    onB2BEvent(b2b_e);
    return;
  }

  AmSession::process(event);
}

void AmB2BSession::onB2BEvent(B2BEvent* ev)
{
  switch(ev->event_id){

  case B2BSipRequest:
    {   
      B2BSipRequestEvent* req_ev = dynamic_cast<B2BSipRequestEvent*>(ev);
      assert(req_ev);

      if(req_ev->forward){
	relaySip(req_ev->req);
      }
      else if( (req_ev->req.method == "BYE") ||
	       (req_ev->req.method == "CANCEL") ) {
		
	onOtherBye(req_ev->req);
      }
    }
    return;

  case B2BSipReply:
    {
      B2BSipReplyEvent* reply_ev = dynamic_cast<B2BSipReplyEvent*>(ev);
      assert(reply_ev);

      DBG("B2BSipReply: %i %s (fwd=%i)\n",reply_ev->reply.code,
	  reply_ev->reply.reason.c_str(),reply_ev->forward);
      DBG("B2BSipReply: content-type = %s\n",reply_ev->reply.content_type.c_str());

      if(reply_ev->forward){

        std::map<int,AmSipRequest>::iterator t_req = 
	  recvd_req.find(reply_ev->reply.cseq);
	if (t_req != recvd_req.end()) {
	  relaySip(t_req->second,reply_ev->reply);
		
	  if(reply_ev->reply.code >= 200){

	    if ((t_req->second.method == "INVITE") &&	      
		(reply_ev->reply.code == 487)) {
	      // 487 reply to INVITE received - INVITE terminated
		terminateLeg();
	    }
	    
	    recvd_req.erase(t_req);
	  } 
	} else {
	  ERROR("Request with CSeq %u not found in recvd_req.\n",
	      reply_ev->reply.cseq);
	}
      }
    }
    return;

  case B2BTerminateLeg:
    terminateLeg();
    break;

  case B2BMsgBody:
    {
      if (!sip_relay_only) {
	ERROR("relayed message body received but not in sip_relay_only mode\n");
	return;
      }

      B2BMsgBodyEvent* body_ev = dynamic_cast<B2BMsgBodyEvent*>(ev);
      assert(body_ev);

      DBG("received B2B Msg body event; is_offer=%s, r_cseq=%d\n",
	  body_ev->is_offer?"true":"false", body_ev->r_cseq);
      
      if (body_ev->is_offer) {
	if (dlg.getUACInvTransPending()) {
	  // INVITE UAC transaction already in progress - save body
	  DBG("INVITE UAC transaction already in progress..."
	      "saving event with r_cseq %d\n", body_ev->r_cseq);
	  pending_relayed_msgbodies.push(new B2BMsgBodyEvent(*body_ev));
	  DBG("Now %zd relayed bodies pending\n", pending_relayed_msgbodies.size());
	  return;
	}

	// send INVITE with SDP
	int reinv_result = sendRelayedBody(body_ev);
	if (reinv_result) {
	  ERROR("sending reinvite with relayed body\n");
	  // tear down: (TODO?: relay error back instead?)
	  DBG("error sending reinvite - terminating this and the other leg\n");
	  terminateOtherLeg();	   
	  terminateLeg();
	}
      } else {
	// is_answer - send 200 ACK
	// todo: use that from uas_trans? 
	trans_ticket tt; // not used for ACK
	AmSipTransaction trans("INVITE", body_ev->r_cseq, tt);
	if (body_ev->body.empty()) {
	  // todo (?): save last SDP and reuse it here
	  DBG("relayed body INVITE failed - sending empty ACK (in the hope the call will continue)\n");
	}
	if (dlg.send_200_ack(trans, body_ev->content_type, body_ev->body, 
			     "" /* hdrs - todo */, SIP_FLAGS_VERBATIM)) {
	  ERROR("sending ACK with SDP\n");
	}
      }
      return; 
    }; break;

  case B2BInvTransPending: {
    DBG("INVITE transaction on other leg is pending\n");
    other_leg_inv_pending = true;
  } break;

  case B2BInvTransFinished: {
    DBG("INVITE transaction on other leg is finished\n");
    other_leg_inv_pending = false;
  } break;

  }

  //ERROR("unknown event caught\n");
}

void AmB2BSession::onSipRequest(const AmSipRequest& req)
{
  if (sip_relay_only && other_leg_inv_pending &&
      req.method == "INVITE") {
    DBG("received INVITE while Inv transaction is pending on other leg\n");
    dlg.updateStatus(req);
    // reply with 491 so other side will retry later 
    dlg.reply(req, 491, "Request Pending");
  }

  bool fwd = sip_relay_only &&
    (req.method != "BYE") &&
    (req.method != "CANCEL");

  if(!fwd)
    AmSession::onSipRequest(req);
  else {
    dlg.updateStatus(req);
    recvd_req.insert(std::make_pair(req.cseq,req));
  }

  relayEvent(new B2BSipRequestEvent(req,fwd));
}

void AmB2BSession::onSipReply(const AmSipReply& reply)
{
  TransMap::iterator t = relayed_req.find(reply.cseq);
  bool fwd = t != relayed_req.end();

  DBG("onSipReply: %i %s (fwd=%i)\n",reply.code,reply.reason.c_str(),fwd);
  DBG("onSipReply: content-type = %s\n",reply.content_type.c_str());
  if(fwd) {
    // request was relayed from the other dialog
    AmSipReply n_reply = reply;
    n_reply.cseq = t->second.cseq;
    
    dlg.updateStatus(reply, false);
    relayEvent(new B2BSipReplyEvent(n_reply,true));

    if(reply.code >= 200) {
      if ((reply.code < 300) && (t->second.method == "INVITE")) {
	DBG("not removing relayed INVITE transaction yet...\n");
      } else 
	relayed_req.erase(t);
    }
  } else {
    // it's a reply to request we originated
    bool is_inv_trans = dlg.get_uac_trans_method(reply.cseq) == "INVITE";
    if (sip_relay_only) {
      // even though we are in sip_relay_only mode

      if (is_inv_trans && 
	  reply.code >= 200) {
	DBG("notifying the other leg of finished INVITE transaction\n");
	relayEvent(new B2BEvent(B2BInvTransFinished));
      }

      bool relay_body = 
	(// in reply to INVITE
	 is_inv_trans && 
	 // positive reply
	 (200 <= reply.code) && (reply.code < 300) 
	 // with body
	 && !reply.body.empty());
      
      if (relay_body) {
	// is it an answer to a relayed body, or an answer to empty re-INVITE? 
	TransMap::iterator rel_body_it = relayed_body_req.find(reply.cseq);
	bool is_offer =  (rel_body_it == relayed_body_req.end());
	relayEvent(new B2BMsgBodyEvent(reply.content_type, reply.body, is_offer, 
				       is_offer ? reply.cseq : rel_body_it->second.cseq));

	if (is_offer) {
	  // it's a reply to empty re-INVITE we have sent

	  // --  onSipReply from AmSession -- 
	  // without do_200_ack in dlg.updateStatus(reply)
	  // todo (?): add do_200_ack flag to AmSession::onSipReply and call it
	  CALL_EVENT_H(onSipReply,reply);
	  
	  int status = dlg.getStatus();
	  dlg.updateStatus(reply, false);
	  
	  if (status != dlg.getStatus())
	    DBG("Dialog status changed %s -> %s (stopped=%s) \n", 
		AmSipDialog::status2str[status], 
		AmSipDialog::status2str[dlg.getStatus()],
		getStopped() ? "true" : "false");
	  else 
	    DBG("Dialog status stays %s (stopped=%s)\n", 
	      AmSipDialog::status2str[status], 
		getStopped() ? "true" : "false");
	  // -- end onSipReply from AmSession --
	} else {
	  // it's an answer to a re-INVITE with SDP
	  relayed_body_req.erase(rel_body_it);
	  AmSession::onSipReply(reply);
	}

      } else {
	AmSession::onSipReply(reply);
      }

      // check for failed reinvites
      if (is_inv_trans && reply.code >= 300) {
	
	// is it an failed answer to a relayed body?
	TransMap::iterator rel_body_it = relayed_body_req.find(reply.cseq);
	bool is_offer =  (rel_body_it == relayed_body_req.end());
	if (!is_offer) {
	  DBG("relayed body INVITE failed\n");
	  // todo (?): save last SDP and reuse it here
	  relayEvent(new B2BMsgBodyEvent("", "", is_offer, 
					 rel_body_it->second.cseq));
	}

	onFailedReinvite(reply);
      }
    }

    relayEvent(new B2BSipReplyEvent(reply,false));
  }

  // check for pending bodies
  if (reply.code >= 200 && // check only when a transaction finished
      pending_relayed_msgbodies.size() && !dlg.getUACInvTransPending()) {
    if (!sendRelayedBody(pending_relayed_msgbodies.front())) {
      DBG("successfully relayed pending msg body with "
	  "r_cseq %d (%zd pending)\n",
	  pending_relayed_msgbodies.front()->r_cseq, 
	  pending_relayed_msgbodies.size()-1);
      delete pending_relayed_msgbodies.front();
      pending_relayed_msgbodies.pop();
    } else {
      DBG("relayed pending msg body with r_cseq %zd "
	  "could not (yet) be sent (%zd pending)\n",
	  pending_relayed_msgbodies.front()->r_cseq, 
	  pending_relayed_msgbodies.size());
    }
  }
}

void AmB2BSession::sendReinvite(bool updateSDP, const string& headers) {
  if (sip_relay_only) {
    if (other_leg_inv_pending) {
      DBG("not sending empty reinvite - Invite on other leg pending\n");
      return;
    }

    // we send empty reinvite 
    DBG("sending empty reinvite\n");
    if (dlg.reinvite(headers, "", "") == E_OK) {
      DBG("notifying the other leg of INVITE transaction in progress\n");
      relayEvent(new B2BEvent(B2BInvTransPending));
    } else {
      DBG("sending empty re-Invite failed\n");
    }
  } else {
    AmSession::sendReinvite(updateSDP, headers);
  }
}

int AmB2BSession::sendRelayedBody(const B2BMsgBodyEvent* body_ev) {
  trans_ticket tt; // empty transaction ticket
  relayed_body_req[dlg.cseq] = AmSipTransaction("INVITE", body_ev->r_cseq, tt);
  int reinv_result = dlg.reinvite("", body_ev->content_type, body_ev->body);

  if (reinv_result) // failed
    relayed_body_req.erase(dlg.cseq);

  return reinv_result;
}

void AmB2BSession::relayEvent(AmEvent* ev)
{
  DBG("AmB2BSession::relayEvent: id=%s\n",
      other_id.c_str());

  if(!other_id.empty())
    AmSessionContainer::instance()->postEvent(other_id,ev);
  else 
    delete ev;
}

void AmB2BSession::onOtherBye(const AmSipRequest& req)
{
  DBG("onOtherBye()\n");
  terminateLeg();
}

bool AmB2BSession::onOtherReply(const AmSipReply& reply)
{
  if(reply.code >= 300) 
    setStopped();
  
  return false;
}

/** re-invite failed while in established B2B call */
void AmB2BSession::onFailedReinvite(const AmSipReply& reply) {
  if (reply.code == 481 || reply.code == 408) { // 3261 14.1
    DBG("re-INVITE failed with code %d - terminating this and the other leg\n", 
	reply.code);
    terminateOtherLeg();	   
    terminateLeg();
  } else {
    DBG("re-INVITE failed with code %d\n", reply.code);
  }
}

void AmB2BSession::terminateLeg()
{
  setStopped();
  if ((dlg.getStatus() == AmSipDialog::Pending) 
      || (dlg.getStatus() == AmSipDialog::Connected))
    dlg.bye();
}

void AmB2BSession::terminateOtherLeg()
{
  relayEvent(new B2BEvent(B2BTerminateLeg));
  clear_other();
}

void AmB2BSession::relaySip(const AmSipRequest& req)
{
  if (req.method != "ACK") {
    relayed_req[dlg.cseq] = AmSipTransaction(req.method,req.cseq,req.tt);
    dlg.sendRequest(req.method, "application/sdp", 
		    req.body, req.hdrs, SIP_FLAGS_VERBATIM);
  } else {
    // its a (200) ACK 
    TransMap::iterator t = relayed_req.begin(); 

    while (t != relayed_req.end()) {
      if (t->second.cseq == req.cseq)
	break;
      t++;
    } 
    if (t == relayed_req.end()) {
      ERROR("transaction for ACK not found in relayed requests\n");
      return;
    }
    DBG("sending relayed ACK\n");
    dlg.send_200_ack(AmSipTransaction(t->second.method, t->first,t->second.tt), 
		     req.content_type, req.body, req.hdrs, SIP_FLAGS_VERBATIM);
    relayed_req.erase(t);
  }
}

void AmB2BSession::relaySip(const AmSipRequest& orig, const AmSipReply& reply)
{
  dlg.reply(orig,reply.code,reply.reason,
	    reply.content_type,
	    reply.body,reply.hdrs,SIP_FLAGS_VERBATIM);
}

// 
// AmB2BCallerSession methods
//

AmB2BCallerSession::AmB2BCallerSession()
  : AmB2BSession(),
    callee_status(None), sip_relay_early_media_sdp(false)
{
}

AmB2BCallerSession::~AmB2BCallerSession()
{
}

void AmB2BCallerSession::set_sip_relay_early_media_sdp(bool r)
{
  sip_relay_early_media_sdp = r; 
}

void AmB2BCallerSession::terminateLeg()
{
  AmB2BSession::terminateLeg();
}

void AmB2BCallerSession::terminateOtherLeg()
{
  AmB2BSession::terminateOtherLeg();
  callee_status = None;
}

void AmB2BCallerSession::onB2BEvent(B2BEvent* ev)
{
  bool processed = false;

  if(ev->event_id == B2BSipReply){

    AmSipReply& reply = ((B2BSipReplyEvent*)ev)->reply;

    if(other_id != reply.local_tag){
      DBG("Dialog mismatch!\n");
      return;
    }

    DBG("reply received from other leg\n");

    switch(callee_status){
    case NoReply:
    case Ringing:
	
      if(reply.code < 200){
	if ((!sip_relay_only) && sip_relay_early_media_sdp && 
	    reply.code>=180 && reply.code<=183 && (!reply.body.empty())) {
	  if (reinviteCaller(reply)) {
	    ERROR("re-INVITEing caller for early session - stopping "
		  "this and other leg\n");
	    terminateOtherLeg();
	    terminateLeg();
	  }
	}
	  
	callee_status = Ringing;
      }
      else if(reply.code < 300){
	  
	callee_status  = Connected;
	  
	if (!sip_relay_only) {
	  sip_relay_only = true;
	  if (reinviteCaller(reply)) {
	    ERROR("re-INVITEing caller - stopping this and other leg\n");
	    terminateOtherLeg();
	    terminateLeg();
	  }
	}
      }
      else {
	// 	DBG("received %i from other leg: other_id=%s; reply.local_tag=%s\n",
	// 	    reply.code,other_id.c_str(),reply.local_tag.c_str());
	  
	terminateOtherLeg();
      }
	
      processed = onOtherReply(reply);
      break;
	
    default:
      DBG("reply from callee: %i %s\n",reply.code,reply.reason.c_str());
      break;
    }
  }
   
  if (!processed)
    AmB2BSession::onB2BEvent(ev);
}

void AmB2BCallerSession::relayEvent(AmEvent* ev)
{
  if(other_id.empty()){

    if(dynamic_cast<B2BEvent*>(ev)){

      B2BSipEvent*     sip_ev = dynamic_cast<B2BSipEvent*>(ev);
      B2BConnectEvent* co_ev  = dynamic_cast<B2BConnectEvent*>(ev);
	    
      if( (sip_ev && sip_ev->forward) || co_ev ) {
	createCalleeSession();
	if (other_id.length()) {
	  MONITORING_LOG(getLocalTag().c_str(), "b2b_leg", other_id.c_str());
	}
      }
    }
  }

  AmB2BSession::relayEvent(ev);
}

void AmB2BCallerSession::onSessionStart(const AmSipRequest& req)
{
  invite_req = req;
  AmB2BSession::onSessionStart(req);
}

void AmB2BCallerSession::connectCallee(const string& remote_party,
				       const string& remote_uri,
				       bool relayed_invite)
{
  if(callee_status != None)
    terminateOtherLeg();

  B2BConnectEvent* ev = new B2BConnectEvent(remote_party,remote_uri);

  ev->content_type = "application/sdp"; // FIXME
  ev->body         = invite_req.body;
  ev->hdrs         = invite_req.hdrs;
  ev->relayed_invite = relayed_invite;
  ev->r_cseq       = invite_req.cseq;

  relayEvent(ev);
  callee_status = NoReply;
}

int AmB2BCallerSession::reinviteCaller(const AmSipReply& callee_reply)
{
  return dlg.sendRequest("INVITE", callee_reply.content_type, 
			 callee_reply.body, "", SIP_FLAGS_VERBATIM);
}

void AmB2BCallerSession::createCalleeSession() {
  AmB2BCalleeSession* callee_session = newCalleeSession();  
  if (NULL == callee_session) 
    return;

  AmSipDialog& callee_dlg = callee_session->dlg;

  other_id = AmSession::getNewId();
  
  callee_dlg.local_tag    = other_id;
  callee_dlg.callid       = AmSession::getNewId() + "@" + AmConfig::LocalIP;

  callee_dlg.local_party  = dlg.remote_party;
  callee_dlg.remote_party = dlg.local_party;
  callee_dlg.remote_uri   = dlg.local_uri;

  if (AmConfig::LogSessions) {
    INFO("Starting B2B callee session %s app %s\n",
	 callee_session->getLocalTag().c_str(), invite_req.cmd.c_str());
  }

  MONITORING_LOG5(other_id.c_str(), 
		  "app",  invite_req.cmd.c_str(),
		  "dir",  "out",
		  "from", callee_dlg.local_party.c_str(),
		  "to",   callee_dlg.remote_party.c_str(),
		  "ruri", callee_dlg.remote_uri.c_str());

  callee_session->start();

  AmSessionContainer* sess_cont = AmSessionContainer::instance();
  sess_cont->addSession(other_id,callee_session);
}

AmB2BCalleeSession* AmB2BCallerSession::newCalleeSession()
{
  return new AmB2BCalleeSession(this);
}

AmB2BCalleeSession::AmB2BCalleeSession(const string& other_local_tag)
  : AmB2BSession(other_local_tag)
{
}

AmB2BCalleeSession::AmB2BCalleeSession(const AmB2BCallerSession* caller)
  : AmB2BSession(caller->getLocalTag())
{
}

AmB2BCalleeSession::~AmB2BCalleeSession() {
}

void AmB2BCalleeSession::onB2BEvent(B2BEvent* ev)
{
  if(ev->event_id == B2BConnectLeg){
    B2BConnectEvent* co_ev = dynamic_cast<B2BConnectEvent*>(ev);
    if (!co_ev)
      return;

    MONITORING_LOG3(getLocalTag().c_str(), 
		    "b2b_leg", other_id.c_str(),
		    "to", co_ev->remote_party.c_str(),
		    "ruri", co_ev->remote_uri.c_str());


    dlg.remote_party = co_ev->remote_party;
    dlg.remote_uri   = co_ev->remote_uri;

    if (co_ev->relayed_invite) {
	relayed_req[dlg.cseq] = 
	  AmSipTransaction("INVITE", co_ev->r_cseq, trans_ticket());
    }

    dlg.sendRequest("INVITE", co_ev->content_type, co_ev->body, 
		    co_ev->hdrs, SIP_FLAGS_VERBATIM);

    return;
  }    

  AmB2BSession::onB2BEvent(ev);
}
