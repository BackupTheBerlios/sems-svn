/*
 * $Id: $
 *
 * Copyright (C) 2007 Raphael Coeffic
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

#include "trans_layer.h"
#include "sip_parser.h"
#include "hash_table.h"
#include "parse_cseq.h"
#include "parse_from_to.h"
#include "sip_trans.h"
#include "msg_fline.h"
#include "msg_hdrs.h"
#include "udp_trsp.h"
#include "resolver.h"
#include "log.h"

#include "wheeltimer.h"
#include "sip_timers.h"

#include "SipCtrlInterface.h"
#include "AmUtils.h"
#include "AmSipMsg.h"
#include "AmConfig.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <assert.h>

/** 
 * Singleton pointer.
 * @see instance()
 */
trans_layer* trans_layer::_instance = NULL;


trans_layer* trans_layer::instance()
{
    if(!_instance)
	_instance = new trans_layer();

    return _instance;
}


trans_layer::trans_layer()
    : ua(NULL),
      transport(NULL)
{
}

trans_layer::~trans_layer()
{}


void trans_layer::register_ua(sip_ua* ua)
{
    this->ua = ua;
}

void trans_layer::register_transport(udp_trsp* trsp)
{
    transport = trsp;
}



int trans_layer::send_reply(trans_bucket* bucket, sip_trans* t,
			    int reply_code, const cstring& reason,
			    const cstring& to_tag, const cstring& hdrs, 
			    const cstring& body)
{
    // Ref.: RFC 3261 8.2.6, 12.1.1
    //
    // Fields to copy (from RFC 3261):
    //  - From
    //  - Call-ID
    //  - CSeq
    //  - Vias (same order)
    //  - To (+ tag if not yet present in request)
    //  - (if a dialog is created) Record-Route
    //
    // Fields to generate (if INVITE transaction):
    //    - Contact
    //    - Route: copied from 
    // 
    // SHOULD be contained:
    //  - Allow, Supported
    //
    // MAY be contained:
    //  - Accept


    bucket->lock();
    if(!bucket->exist(t)){
	bucket->unlock();
	ERROR("Invalid transaction key: transaction does not exist\n");
	return -1;
    }

    sip_msg* req = t->msg;

    bool have_to_tag = false;
    int  reply_len   = status_line_len(reason);

    for(list<sip_header*>::iterator it = req->hdrs.begin();
	it != req->hdrs.end(); ++it) {

	switch((*it)->type){

	case sip_header::H_TO:
	    if(! ((sip_from_to*)(*it)->p)->tag.len ) {

		reply_len += 5/* ';tag=' */
		    + to_tag.len; 
	    }
	    else {
		// To-tag present in request
		have_to_tag = true;

		t->to_tag = ((sip_from_to*)(*it)->p)->tag;
	    }
	    // fall-through-trap
	case sip_header::H_FROM:
	case sip_header::H_CALL_ID:
	case sip_header::H_CSEQ:
	case sip_header::H_VIA:
	case sip_header::H_RECORD_ROUTE:
	    reply_len += copy_hdr_len(*it);
	    break;
	}
    }

    reply_len += hdrs.len;

    string c_len = int2str(body.len);
    reply_len += content_length_len((char*)c_len.c_str());

    if(body.len){
	
	reply_len += body.len;
    }

    reply_len += 2/*CRLF*/;
    
    // Allocate buffer for the reply
    //
    char* reply_buf = new char[reply_len];
    char* c = reply_buf;

    status_line_wr(&c,reply_code,reason);

    for(list<sip_header*>::iterator it = req->hdrs.begin();
	it != req->hdrs.end(); ++it) {

	switch((*it)->type){

	case sip_header::H_TO:
	    memcpy(c,(*it)->name.s,(*it)->name.len);
	    c += (*it)->name.len;
	    
	    *(c++) = ':';
	    *(c++) = SP;
	    
	    memcpy(c,(*it)->value.s,(*it)->value.len);
	    c += (*it)->value.len;
	    
	    if(!have_to_tag){

		memcpy(c,";tag=",5);
		c += 5;

		t->to_tag.s = c;
		t->to_tag.len = to_tag.len;

		memcpy(c,to_tag.s,to_tag.len);
		c += to_tag.len;
	    }

	    *(c++) = CR;
	    *(c++) = LF;
	    break;

	case sip_header::H_FROM:
	case sip_header::H_CALL_ID:
	case sip_header::H_CSEQ:
	case sip_header::H_VIA:
	case sip_header::H_RECORD_ROUTE:
	    copy_hdr_wr(&c,*it);
	    break;
	}
    }

    if (hdrs.len) {
      memcpy(c,hdrs.s,hdrs.len);
      c += hdrs.len;
    }

    content_length_wr(&c,(char*)c_len.c_str());

    *c++ = CR;
    *c++ = LF;

    if(body.len){
	
	memcpy(c,body.s,body.len);
    }

    //    DBG("Sending: <%.*s>\n",reply_len,reply_buf);

    assert(transport);
    int err = transport->send(&req->remote_ip,reply_buf,reply_len);

    if(err < 0){
	delete [] reply_buf;
	goto end;
    }

    err = update_uas_reply(bucket,t,reply_code);
    if(err < 0){
	
	ERROR("Invalid state change\n");
	delete [] reply_buf;
    }
    else if(err != TS_TERMINATED) {
	if (t->retr_buf) 
		delete [] t->retr_buf;

	t->retr_buf = reply_buf;
	t->retr_len = reply_len;
	memcpy(&t->retr_addr,&req->remote_ip,sizeof(sockaddr_storage));

	err = 0;
    }
    else {
	// Transaction has been deleted
	// -> should not happen, as we 
	//    now wait for 200 ACK
	delete [] reply_buf;
	err = 0;
    }
    
 end:
    bucket->unlock();
    return err;
}

//
// Ref. RFC 3261 "12.2.1.1 Generating the Request"
//
int trans_layer::set_next_hop(list<sip_header*>& route_hdrs, 
			      cstring& r_uri, 
			      sockaddr_storage* remote_ip)
{
    string         next_hop;
    unsigned short next_port=0; 

    //assert(msg->type == SIP_REQUEST);

    int err=0;

    if(!route_hdrs.empty()){
	
	sip_header* fr = route_hdrs.front();
	
	sip_nameaddr na;
	const char* c = fr->value.s;
	if(parse_nameaddr(&na, &c, fr->value.len)<0) {
	    
	    DBG("Parsing name-addr failed\n");
	    return -1;
	}
	
	if(parse_uri(&na.uri,na.addr.s,na.addr.len) < 0) {
	    
	    DBG("Parsing route uri failed\n");
	    return -1;
	}

	bool is_lr = false;
	if(!na.uri.params.empty()){
	    
	    list<sip_avp*>::iterator it = na.uri.params.begin();
	    for(;it != na.uri.params.end(); it++){
		
		if( ((*it)->name.len == 2) && 
		    (!memcmp((*it)->name.s,"lr",2)) ) {

		    is_lr = true;
		    break;
		}
	    }

	}

	if (SipCtrlInterfaceFactory::outbound_host.empty()) {
	    next_hop  = c2stlstr(na.uri.host);
	    next_port = na.uri.port;
	} else {
	    next_hop = SipCtrlInterfaceFactory::outbound_host;
	    next_port = SipCtrlInterfaceFactory::outbound_port;
	}	    

	if(!is_lr){
	    
	    // detect beginning of next route

	    enum {
		RR_PARAMS=0,
		RR_QUOTED,
		RR_SEP_SWS,  // space(s) after ','
		RR_NXT_ROUTE
	    };

	    int st = RR_PARAMS;
	    const char* end = fr->value.s + fr->value.len;
	    for(;c<end;c++){
		
		switch(st){
		case RR_PARAMS:
		    switch(*c){
		    case SP:
		    case HTAB:
		    case CR:
		    case LF:
			break;
		    case COMMA:
			st = RR_SEP_SWS;
			break;
		    case DQUOTE:
			st = RR_QUOTED;
			break;
		    }
		    break;
		case RR_QUOTED:
		    switch(*c){
		    case BACKSLASH:
			c++;
			break;
		    case DQUOTE:
			st = RR_PARAMS;
			break;
		    }
		    break;
		case RR_SEP_SWS:
		    switch(*c){
		    case SP:
		    case HTAB:
		    case CR:
		    case LF:
			break;
		    default:
			st = RR_NXT_ROUTE;
			goto nxt_route;
		    }
		    break;
		}
	    }

	nxt_route:
	    
	    switch(st){
	    case RR_QUOTED:
	    case RR_SEP_SWS:
		DBG("Malformed first route header\n");
	    case RR_PARAMS:
		// remove current route header from message
		DBG("delete (fr=0x%p)\n",fr);
		delete fr; // route_hdrs.front();
		route_hdrs.pop_front();
		DBG("route_hdrs.length() = %i\n",(int)route_hdrs.size());
		break;
		
	    case RR_NXT_ROUTE:
		// remove current route from this header
		fr->value.s   = c;
		fr->value.len = end-c;
		break;
	    }

	    
	    // copy r_uri at the end of 
	    // the route set.
	    route_hdrs.push_back(new sip_header(0,"Route",r_uri));

	    r_uri = na.addr;
	}
	
    }
    else {

	if (SipCtrlInterfaceFactory::outbound_host.empty()) {
	    sip_uri parsed_r_uri;
	    err = parse_uri(&parsed_r_uri,r_uri.s,r_uri.len);
	    if(err < 0){
		ERROR("Invalid Request URI\n");
		return -1;
	    }
	    next_hop  = c2stlstr(parsed_r_uri.host);
	    next_port = parsed_r_uri.port;
	} else {
	    next_hop = SipCtrlInterfaceFactory::outbound_host;
	    next_port = SipCtrlInterfaceFactory::outbound_port;
	}
    }

    DBG("next_hop:next_port is <%s:%u>\n", next_hop.c_str(), next_port);
    
    err = resolver::instance()->resolve_name(next_hop.c_str(),
					     remote_ip,IPv4,UDP);
    if(err < 0){
	ERROR("Unresolvable Request URI\n");
	return -1;
    }

    ((sockaddr_in*)remote_ip)->sin_port = htons(next_port);
    
    return 0;
}

void trans_layer::timeout(trans_bucket* bucket, sip_trans* t)
{
    t->reset_all_timers();
    t->state = TS_TERMINATED;

    // send 408 to 'ua'
    sip_msg  msg;
    sip_msg* req = t->msg;

    msg.type = SIP_REPLY;
    msg.u.reply = new sip_reply();

    msg.u.reply->code = 408;
    msg.u.reply->reason = cstring("Timeout");

    msg.from = req->from;
    msg.to = req->to;
    msg.cseq = req->cseq;
    msg.callid = req->callid;

    ua->handle_sip_reply(&msg);

    bucket->remove_trans(t);
}

int trans_layer::send_request(sip_msg* msg, char* tid)
{
    // Request-URI
    // To
    // From
    // Call-ID
    // CSeq
    // Max-Forwards
    // Via
    // Contact
    // Supported / Require
    // Content-Length / Content-Type
    
    assert(transport);

    if(set_next_hop(msg->route,msg->u.request->ruri_str,
		    &msg->remote_ip) < 0){
	// TODO: error handling
	DBG("set_next_hop failed\n");
	//delete msg;
	return -1;
    }

    // assume that msg->route headers are not in msg->hdrs
    msg->hdrs.insert(msg->hdrs.begin(),msg->route.begin(),msg->route.end());

    int request_len = request_line_len(msg->u.request->method_str,
				       msg->u.request->ruri_str);

    char branch_buf[BRANCH_BUF_LEN];
    compute_branch(branch_buf,msg->callid->value,msg->cseq->value);
    cstring branch(branch_buf,BRANCH_BUF_LEN);
    
    string via(transport->get_local_ip());
    if(transport->get_local_port() != 5060)
	via += ":" + int2str(transport->get_local_port());

    request_len += via_len(stl2cstr(via),branch);

    request_len += copy_hdrs_len(msg->hdrs);

    string content_len = int2str(msg->body.len);

    request_len += content_length_len(stl2cstr(content_len));
    request_len += 2/* CRLF end-of-headers*/;

    if(msg->body.len){
	request_len += msg->body.len;
    }

    // Allocate new message
    sip_msg* p_msg = new sip_msg();
    p_msg->buf = new char[request_len];
    p_msg->len = request_len;

    // generate it
    char* c = p_msg->buf;
    request_line_wr(&c,msg->u.request->method_str,
		    msg->u.request->ruri_str);

    via_wr(&c,stl2cstr(via),branch);
    copy_hdrs_wr(&c,msg->hdrs);

    content_length_wr(&c,stl2cstr(content_len));

    *c++ = CR;
    *c++ = LF;

    if(msg->body.len){
	memcpy(c,msg->body.s,msg->body.len);

	// Not needed by now as the message is finished
	//c += body.len;
    }

    // and parse it
    if(parse_sip_msg(p_msg)){
	ERROR("Parser failed on generated request\n");
	ERROR("Message was: <%.*s>\n",p_msg->len,p_msg->buf);
	delete p_msg;
	return MALFORMED_SIP_MSG;
    }

    memcpy(&p_msg->remote_ip,&msg->remote_ip,sizeof(sockaddr_storage));

    DBG("Sending to %s:%i <%.*s>\n",
	get_addr_str(((sockaddr_in*)&p_msg->remote_ip)->sin_addr).c_str(),
	ntohs(((sockaddr_in*)&p_msg->remote_ip)->sin_port),
	p_msg->len,p_msg->buf);

    trans_bucket* bucket = get_trans_bucket(p_msg->callid->value,
					    get_cseq(p_msg)->num_str);
    bucket->lock();

    int send_err = transport->send(&p_msg->remote_ip,p_msg->buf,p_msg->len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
	delete p_msg;
    }
    else {

	sip_trans* t = bucket->add_trans(p_msg,TT_UAC);
	if(p_msg->u.request->method == sip_request::INVITE){
	    
	    // if transport == UDP
	    t->reset_timer(STIMER_A,A_TIMER,bucket->get_id());
	    // for any transport type
	    t->reset_timer(STIMER_B,B_TIMER,bucket->get_id());
	}
	else {
	    
	    // if transport == UDP
	    t->reset_timer(STIMER_E,E_TIMER,bucket->get_id());
	    // for any transport type
	    t->reset_timer(STIMER_F,F_TIMER,bucket->get_id());

	}

	string t_id = int2hex(bucket->get_id()).substr(5,string::npos) 
	    + ":" + long2hex((unsigned long)t);
	memcpy(tid,t_id.c_str(),12);
    }

    bucket->unlock();
    
    return send_err;
}

int trans_layer::cancel(trans_bucket* bucket, sip_trans* t)
{
    bucket->lock();
    if(!bucket->exist(t)){
	DBG("No transaction to cancel: wrong key or finally replied\n");
	bucket->unlock();
	return 0;
    }

    sip_msg* req = t->msg;
    
    // RFC 3261 says: SHOULD NOT be sent for other request
    // than INVITE.
    if(req->u.request->method != sip_request::INVITE){
	bucket->unlock();
	ERROR("Trying to cancel a non-INVITE request (we SHOULD NOT do that)\n");
	return -1;
    }
    
    switch(t->state){
    case TS_CALLING:
	// do not send a request:
	// just remove the transaction
	bucket->remove_trans(t);
	bucket->unlock();
	return 0;

    case TS_COMPLETED:
	// final reply has been sent:
	// do nothing!!!
	bucket->unlock();
	return 0;
	
    case TS_PROCEEDING:
	// continue with CANCEL request
	break;
    }

    cstring cancel_str("CANCEL");

    int request_len = request_line_len(cancel_str,
				       req->u.request->ruri_str);

    char branch_buf[BRANCH_BUF_LEN];
    compute_branch(branch_buf,req->callid->value,get_cseq(req)->num_str);
    cstring branch(branch_buf,BRANCH_BUF_LEN);
    
    string via(transport->get_local_ip());
    if(transport->get_local_port() != 5060)
	via += ":" + int2str(transport->get_local_port());

    request_len += copy_hdr_len(req->via1);

    request_len += copy_hdr_len(req->to)
	+ copy_hdr_len(req->from)
	+ copy_hdr_len(req->callid)
	+ cseq_len(get_cseq(req)->num_str,cancel_str)
	+ copy_hdrs_len(req->route)
	+ copy_hdrs_len(req->contacts);

    request_len += 2/* CRLF end-of-headers*/;

    // Allocate new message
    sip_msg* p_msg = new sip_msg();
    p_msg->buf = new char[request_len];
    p_msg->len = request_len;

    // generate it
    char* c = p_msg->buf;
    request_line_wr(&c,cancel_str,
		    req->u.request->ruri_str);

    copy_hdr_wr(&c,req->via1);
    copy_hdr_wr(&c,req->to);
    copy_hdr_wr(&c,req->from);
    copy_hdr_wr(&c,req->callid);
    cseq_wr(&c,get_cseq(req)->num_str,cancel_str);
    copy_hdrs_wr(&c,req->route);
    copy_hdrs_wr(&c,req->contacts);

    *c++ = CR;
    *c++ = LF;

    // and parse it
    if(parse_sip_msg(p_msg)){
	ERROR("Parser failed on generated request\n");
	ERROR("Message was: <%.*s>\n",p_msg->len,p_msg->buf);
	delete p_msg;
	return MALFORMED_SIP_MSG;
    }

    memcpy(&p_msg->remote_ip,&req->remote_ip,sizeof(sockaddr_storage));

    DBG("Sending to %s:%i <%.*s>\n",
	get_addr_str(((sockaddr_in*)&p_msg->remote_ip)->sin_addr).c_str(),
	ntohs(((sockaddr_in*)&p_msg->remote_ip)->sin_port),
	p_msg->len,p_msg->buf);

    int send_err = transport->send(&p_msg->remote_ip,p_msg->buf,p_msg->len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
	delete p_msg;
    }
    else {
	trans_bucket* n_bucket = get_trans_bucket(p_msg->callid->value,
						  get_cseq(p_msg)->num_str);

	if(bucket != n_bucket)
	    n_bucket->lock();

	sip_trans* t = n_bucket->add_trans(p_msg,TT_UAC);
	    
	// if transport == UDP
	t->reset_timer(STIMER_E,E_TIMER,bucket->get_id());
	// for any transport type
	t->reset_timer(STIMER_F,F_TIMER,bucket->get_id());

	if(bucket != n_bucket)
	    n_bucket->unlock();
    }
    
    bucket->unlock();
    return send_err;
}


void trans_layer::received_msg(sip_msg* msg)
{
#define DROP_MSG \
          delete msg;\
          return

    int err = parse_sip_msg(msg);
    DBG("parse_sip_msg returned %i\n",err);

    if(err){
	DBG("Message was: \"%.*s\"\n",msg->len,msg->buf);
	DBG("dropping message\n");
	DROP_MSG;
    }
    
    assert(msg->callid && get_cseq(msg));
    if(!msg->callid || !get_cseq(msg)){
	
	DBG("Call-ID or CSeq header missing: dropping message\n");
	DROP_MSG;
    }

    unsigned int  h = hash(msg->callid->value, get_cseq(msg)->num_str);
    trans_bucket* bucket = get_trans_bucket(h);
    sip_trans* t = NULL;

    bucket->lock();

    switch(msg->type){
    case SIP_REQUEST: 
	
	if((t = bucket->match_request(msg)) != NULL){
	    if(msg->u.request->method != t->msg->u.request->method){
		
		// ACK matched INVITE transaction
		DBG("ACK matched INVITE transaction\n");
		
		err = update_uas_request(bucket,t,msg);
		if(err<0){
		    DBG("trans_layer::update_uas_trans() failed!\n");
		    // Anyway, there is nothing we can do...
		}
		else if(err == TS_TERMINATED){
		
		    // do not touch the transaction anymore:
		    // it could have been deleted !!!
		       
		    // should we forward the ACK to SEMS-App upstream? Yes
		}
	    }
	    else {
		DBG("Found retransmission\n");
		retransmit(t);
	    }
	}
	else {

	    string t_id;
	    sip_trans* t = NULL;
	    if(msg->u.request->method != sip_request::ACK){
		
		// New transaction
		t = bucket->add_trans(msg, TT_UAS);

		t_id = int2hex(h).substr(5,string::npos) 
		    + ":" + long2hex((unsigned long)t);
	    }

	    bucket->unlock();
	    
	    //  let's pass the request to
	    //  the UA. 
	    assert(ua);
	    ua->handle_sip_request(t_id.c_str(),msg);

	    if(!t){
		DROP_MSG;
	    }
	    //Else:
	    // forget the msg: it will be
	    // owned by the new transaction
	    return;
	}
	break;
    
    case SIP_REPLY:

	if((t = bucket->match_reply(msg)) != NULL){

	    // Reply matched UAC transaction
	    
	    DBG("Reply matched an existing transaction\n");
	    if(update_uac_trans(bucket,t,msg) < 0){
		ERROR("update_uac_trans() failed, so what happens now???\n");
		break;
	    }
	    // do not touch the transaction anymore:
	    // it could have been deleted !!!
	}
	else {
	    DBG("Reply did NOT match any existing transaction...\n");
	    DBG("reply code = %i\n",msg->u.reply->code);
	    if( (msg->u.reply->code >= 200) &&
	        (msg->u.reply->code <  300) ) {
		
		bucket->unlock();
		
		// pass to UA
		assert(ua);
		ua->handle_sip_reply(msg);
		
		DROP_MSG;
	    }
	}
	break;

    default:
	ERROR("Got unknown message type: Bug?\n");
	break;
    }

    // unlock_drop:
    bucket->unlock();
    DROP_MSG;
}


int trans_layer::update_uac_trans(trans_bucket* bucket, sip_trans* t, sip_msg* msg)
{
    assert(msg->type == SIP_REPLY);

    cstring to_tag;
    int     reply_code = msg->u.reply->code;

    DBG("reply code = %i\n",msg->u.reply->code);

    if(reply_code < 200){

	// Provisional reply
	switch(t->state){

	case TS_CALLING:
	    t->clear_timer(STIMER_A);
	    t->clear_timer(STIMER_B);
	    // fall through trap

	case TS_TRYING:
	    t->state = TS_PROCEEDING;
	    // fall through trap

	case TS_PROCEEDING:
	    goto pass_reply;

	case TS_COMPLETED:
	default:
	    goto end;
	}
    }
    
    to_tag = ((sip_from_to*)msg->to->p)->tag;
    if((t->msg->u.request->method != sip_request::CANCEL) && !to_tag.len){
	DBG("To-tag missing in final reply\n");
	if (!SipCtrlInterfaceFactory::accept_fr_without_totag)
	    return -1;
    }
    
    if(t->msg->u.request->method == sip_request::INVITE){
    
	if(reply_code >= 300){
	    
	    // Final error reply
	    switch(t->state){
		
	    case TS_CALLING:

		t->clear_timer(STIMER_A);

	    case TS_PROCEEDING:
		
		t->state = TS_COMPLETED;
		send_non_200_ack(msg,t);
		
		// TODO: set D timer if UDP
		t->reset_timer(STIMER_D, D_TIMER, bucket->get_id());
		
		goto pass_reply;
		
	    case TS_COMPLETED:
		// retransmit non-200 ACK
		retransmit(t);
	    default:
		goto end;
	    }
	} 
	else {
	    
	    // Positive final reply to INVITE transaction
	    switch(t->state){
		
	    case TS_CALLING:
	    case TS_PROCEEDING:
		// TODO:
		//  we should take care of 200 ACKs
		//    - on first reply:
		//      - save to-tag.
		//      - use route-set included in the INV req (if applicable).
		//      - save ACK for retransmitions.
		//    - compare to-tag on subsequent replies.
		//      - (if different): 
		//        - (generate new 200 ACK based on reply).
		//        - (send BYE (check for existing UAC trans)).
		//      - else:
		//        - re-transmit ACK.

		t->state  = TS_TERMINATED_200;
		t->clear_timer(STIMER_A);
		t->clear_timer(STIMER_B);

		t->reset_timer(STIMER_L, L_TIMER, bucket->get_id());

		t->to_tag.s = new char[to_tag.len];
		t->to_tag.len = to_tag.len;
		memcpy((void*)t->to_tag.s,to_tag.s,to_tag.len);
		
		send_200_ack(msg,t);

		goto pass_reply;
		
	    case TS_TERMINATED_200:
		
		if( (to_tag.len != t->to_tag.len) ||
		    (memcmp(to_tag.s,t->to_tag.s,to_tag.len) != 0) ){

		    // TODO: 
		    //   (this should be implemented in the UA)
		    //   we should send 200 ACK also here,
		    //   but this would mean that we should
		    //   also be sending a BYE also to quit
		    //   this dialog.
		    //
		    goto end;
		}

		retransmit(t);
		goto end;

	    default:
		goto end;
	    }
	}
    }
    else { // non-INVITE transaction

	// Final reply
	switch(t->state){
	    
	case TS_TRYING:
	case TS_CALLING:
	case TS_PROCEEDING:
	    
	    t->state = TS_COMPLETED;
	
	    t->clear_timer(STIMER_E);
	    // TODO: timer should be 0 if reliable transport
	    t->reset_timer(STIMER_K, K_TIMER, bucket->get_id());
	    
	    if(t->msg->u.request->method != sip_request::CANCEL)
		goto pass_reply;
	    else
		goto end;

	case TS_COMPLETED:
	    // Absorb reply retransmission (only if UDP)
	    goto end;
	    
	default:
	    goto end;
	}
    }

 pass_reply:
    assert(ua);
    ua->handle_sip_reply(msg);
 end:
    return 0;
}

int trans_layer::update_uas_reply(trans_bucket* bucket, sip_trans* t, int reply_code)
{
    if(t->reply_status >= 200){
	ERROR("Trying to send a reply whereby reply_status >= 300\n");
	return -1;
    }

    t->reply_status = reply_code;

    if(t->reply_status >= 300){

	// error reply
	t->state = TS_COMPLETED;
	    
	if(t->msg->u.request->method == sip_request::INVITE){
	    t->reset_timer(STIMER_G,G_TIMER,bucket->get_id());
	    t->reset_timer(STIMER_H,H_TIMER,bucket->get_id());
	}
	else {
	    // 64*T1_TIMER if UDP / 0 if !UDP
	    t->reset_timer(STIMER_J,J_TIMER,bucket->get_id()); 
	}
    }
    else if(t->reply_status >= 200) {

	if(t->msg->u.request->method == sip_request::INVITE){

	    // final reply

	    //bucket->remove_trans(t);
	    //return TS_TERMINATED;

	    //
	    // In this stack, the transaction layer
	    // takes care of re-transmiting the 200 reply
	    // in a UAS INVITE transaction. The code above
	    // is commented out and shows the behavior as
	    // required by the RFC.
	    //
	    t->state = TS_TERMINATED_200;
	    t->reset_timer(STIMER_G,G_TIMER,bucket->get_id());
	    t->reset_timer(STIMER_H,H_TIMER,bucket->get_id());

	}
	else {
	    t->state = TS_COMPLETED;
	    // Only for unreliable transports.
	    t->reset_timer(STIMER_J,J_TIMER,bucket->get_id()); 
	}
    }
    else {
	// provisional reply
	t->state = TS_PROCEEDING;
    }
	
    return t->state;
}

int trans_layer::update_uas_request(trans_bucket* bucket, sip_trans* t, sip_msg* msg)
{
    if(msg->u.request->method != sip_request::ACK){
	ERROR("Bug? Recvd non-ACK for existing UAS transaction\n");
	return -1;
    }
	
    switch(t->state){
	    
    case TS_COMPLETED:
	t->state = TS_CONFIRMED;

	t->clear_timer(STIMER_G);
	t->clear_timer(STIMER_H);

	t->reset_timer(STIMER_I,I_TIMER,bucket->get_id());
	
	// drop through
    case TS_CONFIRMED:
	return t->state;
	    
    case TS_TERMINATED_200:
	// remove transaction
	bucket->remove_trans(t);
	return TS_REMOVED;
	    
    default:
	DBG("Bug? Unknown state at this point: %i\n",t->state);
    }

    return -1;
}

void trans_layer::send_non_200_ack(sip_msg* reply, sip_trans* t)
{
    sip_msg* inv = t->msg;
    
    cstring method("ACK",3);
    int ack_len = request_line_len(method,inv->u.request->ruri_str);
    
    ack_len += copy_hdr_len(inv->via1)
	+ copy_hdr_len(inv->from)
	+ copy_hdr_len(reply->to)
	+ copy_hdr_len(inv->callid);
    
    ack_len += cseq_len(get_cseq(inv)->num_str,method);
    ack_len += 2/* EoH CRLF */;

    if(!inv->route.empty())
 	ack_len += copy_hdrs_len(inv->route);
    
    char* ack_buf = new char [ack_len];
    char* c = ack_buf;

    request_line_wr(&c,method,inv->u.request->ruri_str);
    
    copy_hdr_wr(&c,inv->via1);

    if(!inv->route.empty())
	 copy_hdrs_wr(&c,inv->route);

    copy_hdr_wr(&c,inv->from);
    copy_hdr_wr(&c,reply->to);
    copy_hdr_wr(&c,inv->callid);
    
    cseq_wr(&c,get_cseq(inv)->num_str,method);
    
    *c++ = CR;
    *c++ = LF;

    DBG("About to send ACK\n");

    assert(transport);
    int send_err = transport->send(&inv->remote_ip,ack_buf,ack_len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
	delete ack_buf;
    }
    else {
	delete [] t->retr_buf;
	t->retr_buf = ack_buf;
	t->retr_len = ack_len;
	memcpy(&t->retr_addr,&inv->remote_ip,sizeof(sockaddr_storage));
    }
}

void trans_layer::send_200_ack(sip_msg* reply, sip_trans* t)
{
    // Set request URI
    // TODO: use correct R-URI instead of just 'Contact'
    if(!get_contact(reply)) {
	DBG("Sorry, reply has no Contact header: could not send ACK\n");
	return;
    }
    
    sip_nameaddr na;
    const char* c = get_contact(reply)->value.s;
    if(parse_nameaddr(&na,&c,get_contact(reply)->value.len) < 0){
	DBG("Sorry, reply's Contact parsing failed: could not send ACK\n");
	return;
    }
    
    cstring r_uri = na.addr;
    list<sip_header*> route_hdrs;

    if(t && !t->msg->route.empty()){
	for(list<sip_header*>::iterator it = t->msg->route.begin();
	    it != t->msg->route.end(); ++it) {
	    
	    route_hdrs.push_back(new sip_header(0,"Route",(*it)->value));
	}
    }
    else {
	for(list<sip_header*>::reverse_iterator it = reply->record_route.rbegin();
	    it != reply->record_route.rend(); ++it) {
	    
	    route_hdrs.push_back(new sip_header(0,"Route",(*it)->value));
	}
    }

    sockaddr_storage remote_ip;
    set_next_hop(route_hdrs,r_uri,&remote_ip);

    int request_len = request_line_len(cstring("ACK",3),r_uri);
   
    char branch_buf[BRANCH_BUF_LEN];
    compute_branch(branch_buf,reply->callid->value,reply->cseq->value);
    cstring branch(branch_buf,BRANCH_BUF_LEN);

    sip_header* max_forward = new sip_header(0,cstring("Max-Forwards"),cstring("10"));

    //cstring via((char*)transport->get_local_ip());
    //request_len += via_len(via,branch);

    string via(transport->get_local_ip());
    if(transport->get_local_port() != 5060)
	via += ":" + int2str(transport->get_local_port());

    request_len += via_len(stl2cstr(via),branch);

    request_len += copy_hdrs_len(route_hdrs);

    request_len += copy_hdr_len(reply->to);
    request_len += copy_hdr_len(reply->from);
    request_len += copy_hdr_len(reply->callid);
    request_len += copy_hdr_len(max_forward);
    request_len += cseq_len(get_cseq(reply)->num_str,cstring("ACK",3));
    request_len += 2/* CRLF end-of-headers*/;

    // Allocate new message
    char* ack_buf = new char[request_len];

    // generate it
    char* msg = ack_buf;

    request_line_wr(&msg,cstring("ACK",3),r_uri);
    via_wr(&msg,stl2cstr(via),branch);

    copy_hdrs_wr(&msg,route_hdrs);

    copy_hdr_wr(&msg,reply->from);
    copy_hdr_wr(&msg,reply->to);
    copy_hdr_wr(&msg,reply->callid);
    copy_hdr_wr(&msg,max_forward);
    delete max_forward;
    cseq_wr(&msg,get_cseq(reply)->num_str,cstring("ACK",3));

    *msg++ = CR;
    *msg++ = LF;

    DBG("About to send 200 ACK\n");
    //    DBG("About to send 200 ACK: \n<%.*s>\n",request_len,ack_buf);

    assert(transport);
    int send_err = transport->send(&remote_ip,ack_buf,request_len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
	delete [] ack_buf;
    }
    else if(t){
	delete [] t->retr_buf;
	t->retr_buf = ack_buf;
	t->retr_len = request_len;
	memcpy(&t->retr_addr,&remote_ip,sizeof(sockaddr_storage));
    }
}

void trans_layer::retransmit(sip_trans* t)
{
    assert(transport);
    if(!t->retr_buf || !t->retr_len){
	// there is nothing to re-transmit yet!!!
	return;
    }

    int send_err = transport->send(&t->retr_addr,t->retr_buf,t->retr_len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
    }
}

void trans_layer::retransmit(sip_msg* msg)
{
    assert(transport);
    int send_err = transport->send(&msg->remote_ip,msg->buf,msg->len);
    if(send_err < 0){
	ERROR("Error from transport layer\n");
    }
}

void trans_layer::timer_expired(timer* t, trans_bucket* bucket, sip_trans* tr)
{
    int n = t->type >> 16;
    int type = t->type & 0xFFFF;

    switch(type){

    case STIMER_A:  // Calling: (re-)send INV

	n++;
	retransmit(tr->msg);
	tr->reset_timer((n<<16) | type, T1_TIMER<<n, bucket->get_id());
	break;
	
    case STIMER_B:  // Calling: -> Terminated

	tr->clear_timer(STIMER_B);
	if(tr->state == TS_CALLING) {
	    DBG("Transaction timeout!\n");
	    timeout(bucket,tr);
	}
	else {
	    DBG("Transaction timeout timer hit while state=0x%x",tr->state);
	}
	break;

    case STIMER_F:  // Trying/Proceeding: terminate transaction
	
	tr->clear_timer(STIMER_F);

	switch(tr->state) {

	case TS_TRYING:
	case TS_PROCEEDING:
	    DBG("Transaction timeout!\n");
	    timeout(bucket,tr);
	    break;
// 	case TS_TERMINATED_200:
// 	    bucket->remove_trans(tr);
// 	    break;
	}
	break;

    case STIMER_D:  // Completed: -> Terminated
    case STIMER_K:  // Completed: terminate transaction
    case STIMER_J:  // Completed: -> Terminated
    case STIMER_H:  // Completed: -> Terminated
    case STIMER_I:  // Confirmed: -> Terminated
    case STIMER_L:  // Terminated_200 -> Terminated
	
	tr->clear_timer(type);
	tr->state = TS_TERMINATED;
	bucket->remove_trans(tr);
	break;


    case STIMER_E:  // Trying/Proceeding: (re-)send request
    case STIMER_G:  // Completed: (re-)send response

	n++; // re-transmission counter

	//
	// in this stack, the transaction layer
	// takes care of re-transmiting the 200 reply
	// in a UAS INVITE transaction.
	//
	if(tr->type == TT_UAS){
	    
	    // re-transmit reply to INV
	    retransmit(tr);
	}
	else {

	    // re-transmit request
	    retransmit(tr->msg);
	}

	if(T1_TIMER<<n > T2_TIMER) {
	    tr->reset_timer((n<<16) | type, T2_TIMER, bucket->get_id());
	}
	else {
	    tr->reset_timer((n<<16) | type, T1_TIMER<<n, bucket->get_id());
	}
	break;

    default:
	ERROR("Invalid timer type %i\n",t->type);
	break;
    }
}


/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 4
 * End:
 */
