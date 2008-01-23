#include "MyCtrlInterface.h"

#include "AmUtils.h"
#include "../AmSipMsg.h"

#include "trans_layer.h"
#include "sip_parser.h"
#include "parse_header.h"
#include "parse_from_to.h"
#include "parse_cseq.h"
#include "hash_table.h"
#include "sip_trans.h"

#include "log.h"

#include <assert.h>

MyCtrlInterface* MyCtrlInterface::_instance = NULL;


MyCtrlInterface* MyCtrlInterface::instance()
{
    if(!_instance)
	_instance = new MyCtrlInterface();
    
    return _instance;
}

MyCtrlInterface::MyCtrlInterface()
{
    tl = trans_layer::instance();
    tl->register_ua(this);
}


int MyCtrlInterface::send(const AmSipRequest &req, string &serKey)
{
    
}

int MyCtrlInterface::send(const AmSipReply &rep)
{
    unsigned int h=0;
    sip_trans*   t=0;

    if((sscanf(rep.serKey.c_str(),"%x:%x",&h,(unsigned long)&t) != 2) ||
       (h >= H_TABLE_ENTRIES)){
	ERROR("Invalid transaction key: invalid bucket ID\n");
	return -1;
    }
    
    return tl->send_reply(get_trans_bucket(h),t,
			  rep.code,stl2cstr(rep.reason),
			  stl2cstr(rep.local_tag),stl2cstr(rep.hdrs), stl2cstr(rep.body));
}

#define DBG_PARAM(p)\
    DBG("%s = <%s>\n",#p,p.c_str());

void MyCtrlInterface::handleSipMsg(AmSipRequest &req)
{
    DBG("Received new request:\n");

    DBG_PARAM(req.cmd);
    DBG_PARAM(req.method);
    DBG_PARAM(req.user);
    DBG_PARAM(req.domain);
    DBG_PARAM(req.dstip);
    DBG_PARAM(req.port);
    DBG_PARAM(req.r_uri);
    DBG_PARAM(req.from_uri);
    DBG_PARAM(req.from);
    DBG_PARAM(req.to);
    DBG_PARAM(req.callid);
    DBG_PARAM(req.from_tag);
    DBG_PARAM(req.to_tag);
    DBG("cseq = <%i>\n",req.cseq);
    DBG_PARAM(req.serKey);
    DBG_PARAM(req.route);
    DBG_PARAM(req.next_hop);
    DBG("hdrs = <%s>\n",req.hdrs.c_str());
    DBG("body = <%s>\n",req.body.c_str());

    AmSipReply reply;
    
    reply.method = req.method;
    reply.code = 404;
    reply.reason = "User not found";
    reply.serKey = req.serKey;
    reply.local_tag = "12345";
    
    int err = send(reply);
    if(err){
	DBG("send failed with err code %i\n",err);
    }
}

void MyCtrlInterface::handleSipMsg(AmSipReply &rep)
{
    
}

void MyCtrlInterface::handle_sip_request(trans_bucket* bucket, sip_msg* msg)
{
    assert(msg->from && msg->from->p);
    assert(msg->to && msg->to->p);
    
    AmSipRequest req;
    
    req.cmd      = "sems";
    req.method   = c2stlstr(msg->u.request->method_str);
    req.user     = c2stlstr(msg->u.request->ruri.user);
    req.domain   = c2stlstr(msg->u.request->ruri.host);
    req.dstip    = get_addr_str(((sockaddr_in*)(&msg->local_ip))->sin_addr); //FIXME: IPv6
    req.port     = int2str(ntohs(((sockaddr_in*)(&msg->local_ip))->sin_port));
    req.r_uri    = c2stlstr(msg->u.request->ruri_str);
    req.from_uri = c2stlstr(((sip_from_to*)msg->from->p)->nameaddr.addr);
    req.from     = c2stlstr(msg->from->value);
    req.to       = c2stlstr(msg->to->value);
    req.callid   = c2stlstr(msg->callid->value);
    req.from_tag = c2stlstr(((sip_from_to*)msg->from->p)->tag);
    req.to_tag   = c2stlstr(((sip_from_to*)msg->to->p)->tag);
    req.cseq     = get_cseq(msg)->num;
    req.body     = c2stlstr(msg->body);

    bucket->lock();

    sip_trans* t = bucket->add_trans(msg, TT_UAS);

    req.serKey = int2hex(hash(msg->callid->value, get_cseq(msg)->str)) 
	+ ":" + long2hex((unsigned long)t);

    bucket->unlock();

    handleSipMsg(req);
}

void MyCtrlInterface::handle_sip_reply(sip_msg* msg)
{
    
}
