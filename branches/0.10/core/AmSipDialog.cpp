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
#include "AmServer.h"
#include "sems.h"

char* AmSipDialog::status2str[4]  = { 	
  "Disconnected",
  "Pending",
  "Connected",
  "Disconnecting" };


AmSipDialog::~AmSipDialog()
{
  DBG("callid = %s\n",callid.c_str());
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
  if(uas_trans.find(req.cseq) == uas_trans.end())
    uas_trans[req.cseq] = AmSipTransaction(req.method,req.cseq);

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

    setRoute(req.route);
    next_hop   = req.next_hop;
  }
}

/**
 *
 * update dialog status from UAC Request that we send (e.g. INVITE)
 */
void AmSipDialog::updateStatusFromLocalRequest(const AmSipRequest& req)
{
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

    // 	sip_ip       = AmConfig::req.dstip;
    // 	sip_port     = req.port;

    // 	setRoute(req.route);
    next_hop   = req.next_hop;
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

  //t->reply_code = code;
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
	    
      if((code < 300) && (code >= 200))
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

void AmSipDialog::updateStatus(const AmSipReply& reply)
{
  TransMap::iterator t_it = uac_trans.find(reply.cseq);
  if(t_it == uac_trans.end()){
    ERROR("could not find any transaction matching reply\n");
    return;
  }
  DBG("updateStatus(reply): transaction found!\n");

  AmSipTransaction& t = t_it->second;

  //t->reply_code = reply.code;

  // rfc3261 12.1
  // && method==INVITE
  // Dialog established only by 101-199 or 2xx 
  // responses to INVITE
  if ((reply.code >= 101) && (reply.code < 300) &&  
      (remote_tag.empty() && !reply.remote_tag.empty()))
    remote_tag = reply.remote_tag;

  // allow route overwritting
  if(status < Connected) {

    if(!reply.route.empty())
      setRoute(reply.route);

    next_hop = reply.next_hop;
  }

  remote_uri = reply.next_request_uri;

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

  if(reply.code >= 200)
    uac_trans.erase(t_it);
}

string AmSipDialog::getContactHdr()
{
  if(!contact_uri.empty())
    return contact_uri;

  string res = "Contact: <sip:";
    
  if(user.empty() || !AmConfig::PrefixSep.empty())
    res += CONTACT_USER_PREFIX;
    
  if(!AmConfig::PrefixSep.empty())
    res += AmConfig::PrefixSep;
    
  if(!user.empty())
    res += user;
    
  res += "@";
    
  if(sip_ip.empty())
    res += "!!"; // Ser will replace that...
  else {
#ifdef SUPPORT_IPV6
    if(sip_ip.find('.') != string::npos)
      res += sip_ip;
    else
      res += "[" + sip_ip + "]";
#else
    res += sip_ip;
#endif
  }

  if(!sip_port.empty())
    res += ":" + sip_port;

  res += ">\n";
    
  // save contact_uri for subsequent contact header
  // only if sip_ip is known
  if (!sip_ip.empty())
    contact_uri  = res;

  return res;
}

#ifdef OpenSER

/* Escape " chars of argument and return escaped string */
string escape(string s)
{
    string::size_type pos;

    pos = 0;
    while ((pos = s.find("\"", pos)) != string::npos) {
	s.insert(pos, "\\");
	pos = pos + 2;
    }
    return s;
}

/* Add CR before each LF if not already there */
string lf2crlf(string s)
{
    string::size_type pos;

    pos = 0;
    while ((pos = s.find("\n", pos)) != string::npos) {
	if ((pos > 0) && (s[pos - 1] == 13)) {
	    pos = pos + 1;
	} else {
	    s.insert(pos, "\r");
	    pos = pos + 2;
	}
    }
    return s;
}

int AmSipDialog::reply(const AmSipRequest& req,
		       unsigned int  code,
		       const string& reason,
		       const string& content_type,
		       const string& body,
		       const string& hdrs)
{
  string m_hdrs = hdrs;
  int headers;

  if(hdl)
    hdl->onSendReply(req,code,reason,
		     content_type,body,m_hdrs);

  string reply_sock = "/tmp/" + AmSession::getNewId();
  string code_str = int2str(code);

  string msg = ":t_reply:\n";

  msg += code_str;
  msg += "\n";

  msg += reason;
  msg += "\n";

  msg += req.key;
  msg += "\n";

  msg += local_tag;
  msg += "\n";

  headers = 0;

  if(!m_hdrs.empty()) {
      msg += "\"" + lf2crlf(escape(m_hdrs));
      headers = 1;
  }

  if ((req.method!="CANCEL")&&
      !((req.method=="BYE")&&(code<300))) {
      if (headers == 1) {
	  msg += lf2crlf(escape(getContactHdr()));
      } else {
	  msg += "\"" + lf2crlf(escape(getContactHdr()));
	  headers = 1;
      }
  }
	  
  if(!body.empty()) {
      if (headers == 1) {  
	  msg += "Content-Type: " + content_type + "\r\n";
      } else {
	  msg += "\"Content-Type: " + content_type + "\r\n";
	  headers = 1;
      }
  }

  if (AmConfig::Signature.length()) {
      if (headers == 1) {
	  msg += "Server: " + AmConfig::Signature + "\r\n";
      } else {
	  msg += "\"Server: " + AmConfig::Signature + "\r\n";
	  headers = 1;
      }
  }

  if (headers == 1) {
      msg += "\"\n";
  } else {
      msg += ".\n";
  }

  if(!body.empty())
      msg += "\"" + body + "\"\n";

  if(updateStatusReply(req,code))
    return -1;

  return AmServer::send_msg(msg,reply_sock, 500);
}

#else /* Ser */

int AmSipDialog::reply(const AmSipRequest& req,
		       unsigned int  code,
		       const string& reason,
		       const string& content_type,
		       const string& body,
		       const string& hdrs)
{
  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendReply(req,code,reason,
		     content_type,body,m_hdrs);

  string reply_sock = "/tmp/" + AmSession::getNewId();
  string code_str = int2str(code);

  string msg = ":t_reply:";

  msg += reply_sock;
  msg += "\n";

  msg += code_str;
  msg += "\n";

  msg += reason;
  msg += "\n";

  msg += req.key;
  msg += "\n";

  msg += local_tag;
  msg += "\n";

  if(!m_hdrs.empty())
    msg += m_hdrs;

  if ((req.method!="CANCEL")&&
      !((req.method=="BYE")&&(code<300)))
    msg += getContactHdr();

  if(!body.empty())
    msg += "Content-Type: " + content_type + "\n";
  if (AmConfig::Signature.length()) 
    msg += "Server: " + AmConfig::Signature + "\n";

  msg += ".\n";

  if(!body.empty())
    msg += body;

  msg += ".\n\n";

  if(updateStatusReply(req,code))
    return -1;

  return AmServer::send_msg(msg,reply_sock, 500);
}

#endif

#ifdef OpenSER

/* static */
int AmSipDialog::reply_error(const AmSipRequest& req, unsigned int code, 
			     const string& reason, const string& hdrs)
{
  string reply_sock = "/tmp/" + AmSession::getNewId();
  string code_str = int2str(code);
  int headers;

  string msg = 
    ":t_reply:\n" +
    code_str + "\n" + 
    reason + "\n" + 
    req.key + "\n" + 
    AmSession::getNewId() + "\n";

  headers = 0;

  if(!hdrs.empty()) {
    msg += "\"" + lf2crlf(escape(hdrs));
    headers = 1;
  }

  if (AmConfig::Signature.length()) {
    if (headers == 1) {
      msg += "Server: " + AmConfig::Signature + "\r\n";
    } else {
	msg += "\"Server: " + AmConfig::Signature + "\r\n";
	headers = 1;
    }
  }

  if (headers == 1) {
      msg += "\"\n";
  } else {
      msg += ".\n";
  }

  return AmServer::send_msg(msg,reply_sock, 500);
}

#else /* Ser */

/* static */
int AmSipDialog::reply_error(const AmSipRequest& req, unsigned int code, 
			     const string& reason, const string& hdrs)
{
  string reply_sock = "/tmp/" + AmSession::getNewId();
  string code_str = int2str(code);

  string msg = 
    ":t_reply:" + reply_sock + "\n" +
    code_str + "\n" + 
    reason + "\n" + 
    req.key + "\n" + 
    AmSession::getNewId() + "\n";
  if(!hdrs.empty())
    msg += hdrs;

  if (AmConfig::Signature.length()) 
    msg += "Server: " + AmConfig::Signature + "\n";

  msg += ".\n.\n\n";
    
  return AmServer::send_msg(msg,reply_sock, 500);
}

#endif

int AmSipDialog::bye()
{
  switch(status){
  case Disconnecting:
  case Connected:
    status = Disconnected;
    return sendRequest("BYE");
  case Pending:
    status = Disconnecting;
    if(getUACTransPending())
      return cancel();
    else {
      // missing AmSipRequest to be able
      // to send the reply on behalf of the app.
      ERROR("bye(): Dialog should have"
	    " been terminated by the app !!!\n");
    }
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
  case Connected:
    return sendRequest("INVITE", content_type, body, hdrs);
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

int AmSipDialog::refer(const string& refer_to)
{
  switch(status){
  case Connected:
    return sendRequest("REFER", "", "", "Refer-To: "+refer_to);
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
		
    tmp_d.setRoute("");
    tmp_d.contact_uri = "Contact: <" + tmp_d.remote_uri + ">\r\n";
    tmp_d.remote_uri = target;
		
    string r_set;
    if(!route.empty()){
			
      vector<string>::iterator it = route.begin();
      r_set ="Transfer-RR=\"" + *it;
			
      for(it++; it != route.end(); it++)
	r_set += "," + *it;
			
      r_set += "\"";
    }
				
    if (!(next_hop.empty() && route.empty())) {
      hdrs = PARAM_HDR ": ";
      if (!next_hop.empty()) 
	hdrs+="Transfer-NH=\"" + next_hop +"\";";
		  
      if (!r_set.empty()) 
	hdrs+=r_set;
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

#ifdef OpenSER

int AmSipDialog::cancel()
{
  int cancel_cseq = -1;
  TransMap::reverse_iterator t;

  for(t = uac_trans.rbegin();
      t != uac_trans.rend(); t++) {

    if(t->second.method == "INVITE"){
      cancel_cseq = t->second.cseq;
      break;
    }
  }
    
  if(t == uac_trans.rend()){
    ERROR("could not find INVITE transaction to cancel\n");
    return -1;
  }
    
  string reply_sock = "/tmp/" + AmSession::getNewId();
  string msg = ":t_uac_cancel:\n"
    + callid + "\n"
    + int2str(cancel_cseq) + "\n";

  return AmServer::send_msg(msg, reply_sock, 50000);
}

#else /* Ser */

int AmSipDialog::cancel()
{
  int cancel_cseq = -1;
  TransMap::reverse_iterator t;

  for(t = uac_trans.rbegin();
      t != uac_trans.rend(); t++) {

    if(t->second.method == "INVITE"){
      cancel_cseq = t->second.cseq;
      break;
    }
  }
    
  if(t == uac_trans.rend()){
    ERROR("could not find INVITE transaction to cancel\n");
    return -1;
  }
    
  string reply_sock = "/tmp/" + AmSession::getNewId();
  string msg = ":t_uac_cancel:" + reply_sock + "\n"
    + callid + "\n"
    + int2str(cancel_cseq) + "\n\n";

  return AmServer::send_msg(msg, reply_sock, 50000);
}

#endif

#ifdef OpenSER

int AmSipDialog::sendRequest(const string& method, 
			     const string& content_type,
			     const string& body,
			     const string& hdrs)
{
  string msg,ser_cmd;
  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendRequest(method,content_type,body,m_hdrs,cseq);

  msg = ":t_uac_dlg:\n"
    + method + "\n"
    + remote_uri + "\n";
    
  if(next_hop.empty())
    msg += ".";
  else
    msg += next_hop;
    
  msg += "\n";
    
  msg += ".\n"; /* socket */

  msg += "\"From: " + escape(local_party);
  if(!local_tag.empty())
    msg += ";tag=" + local_tag;
    
  msg += "\n";
  
  msg += "To: " + escape(remote_party);
  if(!remote_tag.empty()) 
    msg += ";tag=" + remote_tag;
    
  msg += "\n";
    
  msg += "CSeq: " + int2str(cseq) + " " + method + "\n"
    + "Call-ID: " + callid + "\n";
    
  if((method!="BYE")&&(method!="CANCEL"))
    msg += lf2crlf(escape(getContactHdr()));
    
  if(!m_hdrs.empty()){
    msg += lf2crlf(escape(m_hdrs));
    if(m_hdrs[m_hdrs.length()-1] != '\n')
      msg += "\r\n";
  }

  if(!route.empty())
    msg += lf2crlf(getRoute());
    
  if(!body.empty())
    msg += "Content-Type: " + content_type + "\r\n";
    
  msg += "Max-Forwards: "  MAX_FORWARDS "\r\n";

  if (AmConfig::Signature.length()) 
    msg += "User-Agent: " + AmConfig::Signature + "\r\n";

  msg += "\"\n";

  if (!body.empty())
    msg += "\"" + lf2crlf(body) + "\"\n";

  if (AmServer::send_msg_replyhandler(msg)) 
    return -1;
    
  uac_trans[cseq] = AmSipTransaction(method,cseq);

  // increment for next request
  cseq++;
    
  return 0;
}

#else /* Ser */

int AmSipDialog::sendRequest(const string& method, 
			     const string& content_type,
			     const string& body,
			     const string& hdrs)
{
  string msg,ser_cmd;
  string m_hdrs = hdrs;

  if(hdl)
    hdl->onSendRequest(method,content_type,body,m_hdrs,cseq);

  msg = ":t_uac_dlg:" + AmConfig::ReplySocketName + "\n"
    + method + "\n"
    + remote_uri + "\n";
    
  if(next_hop.empty())
    msg += ".";
  else
    msg += next_hop;
    
  msg += "\n";
    
  msg += "From: " + local_party;
  if(!local_tag.empty())
    msg += ";tag=" + local_tag;
    
  msg += "\n";
    
  msg += "To: " + remote_party;
  if(!remote_tag.empty()) 
    msg += ";tag=" + remote_tag;
    
  msg += "\n";
    
  msg += "CSeq: " + int2str(cseq) + " " + method + "\n"
    + "Call-ID: " + callid + "\n";
    
  if((method!="BYE")&&(method!="CANCEL"))
    msg += getContactHdr();
    
  if(!m_hdrs.empty()){
    msg += m_hdrs;
    if(m_hdrs[m_hdrs.length()-1] != '\n')
      msg += "\n";
  }

  if(!route.empty())
    msg += getRoute();
    
  if(!body.empty())
    msg += "Content-Type: " + content_type + "\n";
    
  msg += "Max-Forwards: "  MAX_FORWARDS "\n";

  if (AmConfig::Signature.length()) 
    msg += "User-Agent: " + AmConfig::Signature + "\n";

  msg += ".\n" // EoH
    + body + ".\n\n";

  if (AmServer::send_msg_replyhandler(msg)) 
    return -1;
    
  uac_trans[cseq] = AmSipTransaction(method,cseq);

  // increment for next request
  cseq++;
    
  return 0;
}

#endif

bool AmSipDialog::match_cancel(const AmSipRequest& cancel_req)
{
  TransMap::iterator t = uas_trans.find(cancel_req.cseq);

  if((t != uas_trans.end()) && (t->second.method == "INVITE"))
    return true;

  return false;
}

string AmSipDialog::get_uac_trans_method(unsigned int cseq)
{
  TransMap::iterator t = uac_trans.find(cseq);

  if (t != uac_trans.end())
    return t->second.method;

  return "";
}

string AmSipDialog::getRoute()
{
  string r_set("");
  for(vector<string>::iterator it = route.begin();
      it != route.end(); it++) {

    r_set += "Route: " + *it + "\r\n";
  }

  return r_set;
}

void AmSipDialog::setRoute(const string& n_route)
{
  string m_route = n_route;
  if(!m_route.empty() && (m_route.find("Route: ")!=string::npos))
    m_route = m_route.substr(7/*sizeof("Route: ")*/);
    
  route.clear();
  while(!m_route.empty()){
	
    string::size_type comma_pos;
	
    comma_pos = m_route.find(',');
    //route += "Route: " + m_route.substr(0,comma_pos) + "\n";
    route.push_back(m_route.substr(0,comma_pos));
	
    if(comma_pos != string::npos)
      m_route = m_route.substr(comma_pos+1);
    else
      m_route = "";
  }
}

int AmSipDialog::drop()
{	
  status = Disconnected;
  return 1;
}
