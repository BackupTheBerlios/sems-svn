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

#include "AmSipDialog.h"
#include "AmConfig.h"
#include "AmSession.h"
#include "AmUtils.h"
#include "AmSipHeaders.h"
#include "SipCtrlInterface.h"
#include "sems.h"

const char* AmSipDialog::status2str[4]  = { 	
  "Disconnected",
  "Pending",
  "Connected",
  "Disconnecting" };


AmSipDialog::AmSipDialog(AmSipDialogEventHandler* h)
  : status(Disconnected),cseq(10),hdl(h),
    outbound_proxy(AmConfig::OutboundProxy),
    force_outbound_proxy(AmConfig::ForceOutboundProxy)
{
}

AmSipDialog::~AmSipDialog()
{
  DBG("callid = %s\n",callid.c_str());
  DBG("local_tag = %s\n",local_tag.c_str());
  DBG("uac_trans.size() = %u\n",(unsigned int)uac_trans.size());
  if(uac_trans.size()){
    for(TransMap::iterator it = uac_trans.begin();
	it != uac_trans.end(); it++){
	    
      DBG("    cseq = %i; method = %s\n",it->first,it->second.method.c_str());
    }
  }
  DBG("uas_trans.size() = %u\n",(unsigned int)uas_trans.size());
  if(uas_trans.size()){
    for(TransMap::iterator it = uas_trans.begin();
	it != uas_trans.end(); it++){
	    
      DBG("    cseq = %i; method = %s\n",it->first,it->second.method.c_str());
    }
  }
}

void AmSipDialog::updateStatus(const AmSipRequest& req)
{
  if ((req.method == "ACK") || (req.method == "CANCEL")) {
    return;
  }

  if(uas_trans.find(req.cseq) == uas_trans.end()){
      DBG("req.tt = {%p,%p}\n",req.tt._bucket, req.tt._t);
      uas_trans[req.cseq] = AmSipTransaction(req.method,req.cseq,req.tt);
  }
  // else {
  //   // shouldn't we drop those requests?
  //   // (CANCEL requests should have been handled before)
  // }

  // target refresh requests
  if (req.from_uri.length() && 
      (req.method == "INVITE" || 
       req.method == "UPDATE" ||
       req.method == "SUBSCRIBE" ||
       req.method == "NOTIFY"))
    remote_uri = req.from_uri;

  sip_ip       = req.dstip;
  sip_port     = req.port;

  if(callid.empty()){
    callid       = req.callid;
    remote_tag   = req.from_tag;
    user         = req.user;
    domain       = req.domain;
    local_uri    = req.r_uri;
    remote_party = req.from;
    local_party  = req.to;
    route        = req.route;
  }
}

/**
 *
 * update dialog status from UAC Request that we send (e.g. INVITE)
 */
void AmSipDialog::updateStatusFromLocalRequest(const AmSipRequest& req)
{
  if (req.r_uri.length())
    remote_uri = req.r_uri;

  if(callid.empty()){
    DBG("dialog callid is empty, updating from UACRequest\n");
    callid       = req.callid;
    local_tag    = req.from_tag;
    DBG("local_tag = %s\n",local_tag.c_str());
    user         = req.user;
    domain       = req.domain;
    local_uri    = req.from_uri;
    remote_party = req.to;
    local_party  = req.from;

    //next_hop   = req.next_hop;
  }
}

int AmSipDialog::updateStatusReply(const AmSipRequest& req, unsigned int code)
{
  TransMap::iterator t_it = uas_trans.find(req.cseq);
  if(t_it == uas_trans.end()){
    ERROR("could not find any transaction matching request\n");
    ERROR("method=%s; callid=%s; local_tag=%s; remote_tag=%s; cseq=%i\n",
	  req.method.c_str(),callid.c_str(),local_tag.c_str(),
	  remote_tag.c_str(),req.cseq);
    return -1;
  }
  DBG("reply: transaction found!\n");
    
  AmSipTransaction& t = t_it->second;
  switch(status){

  case Disconnected:
  case Pending:
    if(t.method == "INVITE"){
	
      if(req.method == "CANCEL"){
		
	// wait for somebody
	// to answer 487
	return 0;
      }

      if(code < 200)
	status = Pending;
      else if(code < 300)
	status = Connected;
      else
	status = Disconnected;
    }
	
    break;
  case Connected:
  case Disconnecting:
    if(t.method == "BYE"){
	    
	if(code >= 200)
	    status = Disconnected;
    }
    break;
  }

  if(code >= 200){
    DBG("req.method = %s; t.method = %s\n",
	req.method.c_str(),t.method.c_str());

    uas_trans.erase(t_it);
  }

  return 0;
}

void AmSipDialog::updateStatus(const AmSipReply& reply, bool do_200_ack)
{
  TransMap::iterator t_it = uac_trans.find(reply.cseq);
  if(t_it == uac_trans.end()){
    ERROR("could not find any transaction matching reply: %s\n", 
        ((AmSipReply)reply).print().c_str());
    return;
  }
  DBG("updateStatus(reply): transaction found!\n");

  AmSipTransaction& t = t_it->second;

  // rfc3261 12.1
  // Dialog established only by 101-199 or 2xx 
  // responses to INVITE

  if ( (reply.code > 100) 
       && (reply.code < 300) 
       && !reply.remote_tag.empty() 
       && (remote_tag.empty() ||
	   ((status < Connected) && (reply.code >= 200))) ) {  

    remote_tag = reply.remote_tag;
  }

  // allow route overwritting
  if ((status < Connected) && !reply.route.empty()) {
      route = reply.route;
  }

  if (reply.next_request_uri.length())
    remote_uri = reply.next_request_uri;

  if(!reply.dstip.empty()){
      sip_ip     = reply.dstip;
      sip_port   = reply.port;
  }

  switch(status){
  case Disconnecting:
    if( t.method == "INVITE" ){

      if(reply.code == 487){
	// CANCEL accepted
	status = Disconnected;
      }
      else {
	// CANCEL rejected
	sendRequest("BYE");
      }
    }
    break;

  case Pending:
  case Disconnected:
    // only change status of dialog if reply 
    // to INVITE received
    if(t.method == "INVITE"){ 
      if(reply.code < 200)
	status = Pending;
      else if(reply.code >= 300)
	status = Disconnected;
      else
	status = Connected;
    }
    break;
  default:
    break;
  }

  // TODO: remove the transaction only after the dedicated timer has hit
  //       this would help taking care of multiple 2xx replies.
  if(reply.code >= 200){
    // TODO: 
    // - place this somewhere else.
    //   (probably in AmSession...)
    if((reply.code < 300) && (t.method == "INVITE") && do_200_ack) {
      send_200_ack(t);
    }

    uac_trans.erase(t_it);
  }
}

bool AmSipDialog::getUACTransPending() { 
  return !uac_trans.empty(); 
}

bool AmSipDialog::getUACInvTransPending() {
  for (TransMap::iterator it=uac_trans.begin(); 
       it != uac_trans.end(); it++) {
    if (it->second.method == "INVITE")
      return true;
  }
  return false;
}

string AmSipDialog::getContactHdr()
{
  if(contact_uri.empty()) {

    contact_uri = SIP_HDR_COLSP(SIP_HDR_CONTACT) "<sip:";

    if(!user.empty()) {
      contact_uri += user + "@";
    }
    
    contact_uri += AmConfig::LocalSIPIP + ":";
    contact_uri += int2str(AmConfig::LocalSIPPort);
    contact_uri += ">";

    contact_uri += CRLF;
    
  }

  return contact_uri;
}

int AmSipDialog::reply(const AmSipRequest& req,
		       unsigned int  code,
		       const string& reason,
		       const string& content_type,
		       const string& body,
		       const string& hdrs,
		       int flags)
{
  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendReply(req,code,reason,
		     content_type,body,m_hdrs,flags);

  AmSipReply reply;

  reply.method = req.method;
  reply.code = code;
  reply.reason = reason;
  reply.tt = req.tt;
  reply.local_tag = local_tag;
  reply.hdrs = m_hdrs;

  if (!flags&SIP_FLAGS_VERBATIM) {
    // add Signature
    if (AmConfig::Signature.length())
      reply.hdrs += SIP_HDR_COLSP(SIP_HDR_SERVER) + AmConfig::Signature + CRLF;
  }

  if ((req.method!="CANCEL")&&
      !((req.method=="BYE")&&(code<300)))
    reply.contact = getContactHdr();

  reply.content_type = content_type;
  reply.body = body;

  if(updateStatusReply(req,code))
    return -1;

  int ret = SipCtrlInterface::send(reply);
  if(ret){
    ERROR("Could not send reply: code=%i; reason='%s'; method=%s; call-id=%s; cseq=%i\n",
	  reply.code,reply.reason.c_str(),req.method.c_str(),req.callid.c_str(),req.cseq);
  }
  return ret;
}

/* static */
int AmSipDialog::reply_error(const AmSipRequest& req, unsigned int code, 
			     const string& reason, const string& hdrs)
{
  AmSipReply reply;

  reply.method = req.method;
  reply.code = code;
  reply.reason = reason;
  reply.tt = req.tt;
  reply.hdrs = hdrs;
  reply.local_tag = AmSession::getNewId();

  if (AmConfig::Signature.length())
    reply.hdrs += SIP_HDR_COLSP(SIP_HDR_SERVER) + AmConfig::Signature + CRLF;

  int ret = SipCtrlInterface::send(reply);
  if(ret){
    ERROR("Could not send reply: code=%i; reason='%s'; method=%s; call-id=%s; cseq=%i\n",
	  reply.code,reply.reason.c_str(),req.method.c_str(),req.callid.c_str(),req.cseq);
  }
  return ret;
}


int AmSipDialog::bye(const string& hdrs)
{
    switch(status){
    case Disconnecting:
    case Connected:
	status = Disconnected;
	return sendRequest("BYE", "", "", hdrs);
    case Pending:
	status = Disconnecting;
	if(getUACTransPending())
	    return cancel();
	else {
	    // missing AmSipRequest to be able
	    // to send the reply on behalf of the app.
	    DBG("ignoring bye() in Pending state: "
		"no UAC transaction to cancel.\n");
	}
	return 0;
    default:
	if(getUACTransPending())
	    return cancel();
	else {
	    DBG("bye(): we are not connected "
		"(status=%i). do nothing!\n",status);
	}
	return 0;
    }	
}

int AmSipDialog::reinvite(const string& hdrs,  
			  const string& content_type,
			  const string& body)
{
  switch(status){
  case Connected: {
    if (getUACInvTransPending())
      return E_IN_PROGRESS;
    return sendRequest("INVITE", content_type, body, hdrs);
  } break;
  case Disconnecting:
  case Pending:
    DBG("reinvite(): we are not yet connected."
	"(status=%i). do nothing!\n",status);

    return 0;
  default:
    DBG("reinvite(): we are not connected "
	"(status=%i). do nothing!\n",status);
    return 0;
  }	
}

int AmSipDialog::invite(const string& hdrs,  
			const string& content_type,
			const string& body)
{
  switch(status){
  case Disconnected: {
    int res = sendRequest("INVITE", content_type, body, hdrs);
    status = Pending;
    return res;
  }; break;

  case Disconnecting:
  case Connected:
  case Pending:
  default:
    DBG("invite(): we are already connected."
	"(status=%i). do nothing!\n",status);

    return 0;
  }	
}

int AmSipDialog::update(const string& hdrs)
{
  switch(status){
  case Connected:
    return sendRequest("UPDATE", "", "", hdrs);
  case Disconnecting:
  case Pending:
    DBG("update(): we are not yet connected."
	"(status=%i). do nothing!\n",status);

    return 0;
  default:
    DBG("update(): we are not connected "
	"(status=%i). do nothing!\n",status);
    return 0;
  }	
}

int AmSipDialog::refer(const string& refer_to,
		       int expires)
{
  switch(status){
  case Connected: {
    string hdrs = SIP_HDR_COLSP(SIP_HDR_REFER_TO) + refer_to + CRLF;
    if (expires>=0) 
      hdrs+= SIP_HDR_COLSP(SIP_HDR_EXPIRES) + int2str(expires) + CRLF;
    return sendRequest("REFER", "", "", hdrs);
  }
  case Disconnecting:
  case Pending:
    DBG("refer(): we are not yet connected."
	"(status=%i). do nothing!\n",status);

    return 0;
  default:
    DBG("refer(): we are not connected "
	"(status=%i). do nothing!\n",status);
    return 0;
  }	

}

int AmSipDialog::transfer(const string& target)
{
  if(status == Connected){

    status = Disconnecting;
		
    string      hdrs = "";
    AmSipDialog tmp_d(*this);
		
    tmp_d.route = "";
    tmp_d.contact_uri = SIP_HDR_COLSP(SIP_HDR_CONTACT) 
      "<" + tmp_d.remote_uri + ">" CRLF;
    tmp_d.remote_uri = target;
		
    string r_set;
    if(!route.empty()){
			
      hdrs = PARAM_HDR ": " "Transfer-RR=\"" + route + "\"";
    }
				
    int ret = tmp_d.sendRequest("REFER","","",hdrs);
    if(!ret){
      uac_trans.insert(tmp_d.uac_trans.begin(),
		       tmp_d.uac_trans.end());
      cseq = tmp_d.cseq;
    }
		
    return ret;
  }
	
  DBG("transfer(): we are not connected "
      "(status=%i). do nothing!\n",status);
    
  return 0;
}

int AmSipDialog::cancel()
{
    for(TransMap::reverse_iterator t = uac_trans.rbegin();
	t != uac_trans.rend(); t++) {
	
	if(t->second.method == "INVITE"){
	  
	  return SipCtrlInterface::cancel(&t->second.tt);
	}
    }
    
    ERROR("could not find INVITE transaction to cancel\n");
    return -1;
}

int AmSipDialog::sendRequest(const string& method, 
			     const string& content_type,
			     const string& body,
			     const string& hdrs,
			     int flags)
{
  string msg,ser_cmd;
  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendRequest(method,content_type,body,m_hdrs,flags,cseq);

  AmSipRequest req;

  req.method = method;
  req.r_uri = remote_uri;

  req.from = SIP_HDR_COLSP(SIP_HDR_FROM) + local_party;
  if(!local_tag.empty())
    req.from += ";tag=" + local_tag;
    
  req.to = SIP_HDR_COLSP(SIP_HDR_TO) + remote_party;
  if(!remote_tag.empty()) 
    req.to += ";tag=" + remote_tag;
    
  req.cseq = cseq;
  req.callid = callid;
    
  if((method!="BYE")&&(method!="CANCEL"))
    req.contact = getContactHdr();
    
  if(!m_hdrs.empty())
    req.hdrs = m_hdrs;
  
  if (!(flags&SIP_FLAGS_VERBATIM)) {
    // add Signature
    if (AmConfig::Signature.length())
      req.hdrs += SIP_HDR_COLSP(SIP_HDR_USER_AGENT) + AmConfig::Signature + CRLF;
    
    req.hdrs += SIP_HDR_COLSP(SIP_HDR_MAX_FORWARDS) /*TODO: configurable?!*/ MAX_FORWARDS CRLF;

  }

  if(!route.empty()) {

    req.route = SIP_HDR_COLSP(SIP_HDR_ROUTE);
    if(force_outbound_proxy && !outbound_proxy.empty()){
      req.route += "<" + outbound_proxy + ";lr>, ";
    }
    req.route += route + CRLF;
  }
  else if (remote_tag.empty() && !outbound_proxy.empty()) {
    req.route = SIP_HDR_COLSP(SIP_HDR_ROUTE) "<" + outbound_proxy + ";lr>" CRLF;
  }

  if(!body.empty()) {
    req.content_type = content_type;
    req.body = body;
  }

  if (SipCtrlInterface::send(req))
    return E_FAILED;
 
  uac_trans[cseq] = AmSipTransaction(method,cseq,req.tt);

  // increment for next request
  cseq++;

  return E_OK;
}

string AmSipDialog::get_uac_trans_method(unsigned int cseq)
{
  TransMap::iterator t = uac_trans.find(cseq);

  if (t != uac_trans.end())
    return t->second.method;

  return "";
}

int AmSipDialog::drop()
{	
  status = Disconnected;
  return 1;
}

int AmSipDialog::send_200_ack(const AmSipTransaction& t,
			      const string& content_type,
			      const string& body,
			      const string& hdrs,
			      int flags)
{
  // TODO: implement missing pieces from RFC 3261:
  // "The ACK MUST contain the same credentials as the INVITE.  If
  // the 2xx contains an offer (based on the rules above), the ACK MUST
  // carry an answer in its body.  If the offer in the 2xx response is not
  // acceptable, the UAC core MUST generate a valid answer in the ACK and
  // then send a BYE immediately."

  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendRequest("ACK",content_type,body,m_hdrs,flags,t.cseq);

  AmSipRequest req;

  req.method = "ACK";
  req.r_uri = remote_uri;
  //req.next_hop = next_hop;

  req.from = SIP_HDR_COLSP(SIP_HDR_FROM) + local_party;
  if(!local_tag.empty())
    req.from += ";tag=" + local_tag;
    
  req.to = SIP_HDR_COLSP(SIP_HDR_TO) + remote_party;
  if(!remote_tag.empty()) 
    req.to += ";tag=" + remote_tag;
    
  req.cseq = t.cseq;// should be the same as the INVITE
  req.callid = callid;
  req.contact = getContactHdr();
    
  if(!m_hdrs.empty())
    req.hdrs = m_hdrs;
  
  if (!(flags&SIP_FLAGS_VERBATIM)) {
    // add Signature
    if (AmConfig::Signature.length())
      req.hdrs += SIP_HDR_COLSP(SIP_HDR_USER_AGENT) + AmConfig::Signature + CRLF;
    
    req.hdrs += SIP_HDR_COLSP(SIP_HDR_MAX_FORWARDS) /*TODO: configurable?!*/MAX_FORWARDS CRLF;

  }

  if(!route.empty()) {
    // r1853
    req.route = SIP_HDR_COLSP(SIP_HDR_ROUTE) + route + CRLF;
  }
    
  if(!body.empty()) {
    req.content_type = content_type;
    req.body = body;
  }

  if (SipCtrlInterface::send(req))
    return -1;

  return 0;
}
