/*
 * $Id$
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 * Copyright (C) 2006 iptego GmbH
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

#include "AmSessionContainer.h"
#include "AmPlugIn.h"
#include "AmApi.h"
#include "AmConfig.h"
#include "AmUtils.h"
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include "sems.h"

// AmSessionContainer methods

AmSessionContainer* AmSessionContainer::_SessionContainer=0;

AmSessionContainer::AmSessionContainer()
  : _run_cond(false)
{
}

AmSessionContainer* AmSessionContainer::instance()
{
  if(!_SessionContainer)
    _SessionContainer = new AmSessionContainer();

  return _SessionContainer;
}

void AmSessionContainer::on_stop() 
{ 
}

void AmSessionContainer::run()
{

  while(1){

    _run_cond.wait_for();

    // Let some time for the Sessions 
    // to stop by themselves
    sleep(5);

    ds_mut.lock();
    DBG("Session cleaner starting its work\n");
	
    try {
      SessionQueue n_sessions;

      while(!d_sessions.empty()){

	AmSession* cur_session = d_sessions.front();
	d_sessions.pop();

	ds_mut.unlock();

	if(cur_session->is_stopped() && cur_session->detached.get()){
		    
	    DBG("session %p has been destroyed'\n",(void*)cur_session->_pid);
	    delete cur_session;
	}
	else {
	    DBG("session %p still running\n",(void*)cur_session->_pid);
	    n_sessions.push(cur_session);
	}

	ds_mut.lock();
      }

      swap(d_sessions,n_sessions);

    }catch(std::exception& e){
      ERROR("exception caught in session cleaner: %s\n", e.what());
      throw; /* throw again as this is fatal (because unlocking the mutex fails!! */
    }catch(...){
      ERROR("unknown exception caught in session cleaner!\n");
      throw; /* throw again as this is fatal (because unlocking the mutex fails!! */
    }

    bool more = !d_sessions.empty();
    ds_mut.unlock();

    DBG("Session cleaner finished\n");
    if(!more)
      _run_cond.set(false);
  }
}

void AmSessionContainer::stopAndQueue(AmSession* s)
{
  ds_mut.lock();

  if (AmConfig::LogSessions) {    
    INFO("session cleaner about to stop %s\n",
	 s->getLocalTag().c_str());
  }

  s->stop();
  d_sessions.push(s);
  _run_cond.set(true);
    
  ds_mut.unlock();
}

void AmSessionContainer::destroySession(AmSession* s)
{
  destroySession(s->getLocalTag());
}

void AmSessionContainer::destroySession(const string& local_tag)
{
  as_mut.lock();

  SessionMapIter sess_it = a_sessions.find(local_tag);
  if(sess_it != a_sessions.end()){
	
    AmSession* sess = sess_it->second;
    as_id_lookup.erase(sess->getCallID() + sess->getRemoteTag());
    a_sessions.erase(sess_it);
	
    stopAndQueue(sess);
    DBG("session stopped and queued for deletion (#sessions=%u)\n",
	(unsigned int)a_sessions.size());
  }
  else {
    DBG("could not remove session: id not found\n");
  }

  as_mut.unlock();
}

AmSession* AmSessionContainer::getSession(const string& callid, const string& remote_tag)
{
  DictIter it = as_id_lookup.find(callid+remote_tag);
  if(it == as_id_lookup.end()){
    //ERROR("could not find session (callid='%s'; remote_tag='%s')\n",
    //      callid.c_str(),remote_tag.c_str());
    return NULL;
  }

  return getSession(it->second);
}

AmSession* AmSessionContainer::getSession(const string& local_tag)
{
  SessionMapIter it = a_sessions.find(local_tag);
  if(it == a_sessions.end()){
      //ERROR("could not find session (local_tag='%s')\n",local_tag.c_str());
    return NULL;
  }

  return it->second;
}

AmSession* AmSessionContainer::startSessionUAC(AmSipRequest& req, AmArg* session_params) {
  AmSession* session = NULL;
  as_mut.lock();
  try {
    if((session = createSession(req, session_params)) != 0){
      session->dlg.updateStatusFromLocalRequest(req); // sets local tag as well
      session->setCallgroup(req.from_tag);

      session->setNegotiateOnReply(true);
      if (int err = session->sendInvite(req.hdrs)) {
	ERROR("INVITE could not be sent: error code = %d.\n", 
	      err);
	delete session;
	as_mut.unlock();
	return NULL;
      }

      if (AmConfig::LogSessions) {      
	INFO("Starting UAC session %s app %s\n",
	     session->getLocalTag().c_str(), req.cmd.c_str());
      }

      session->start();

      addSession_unsafe(req.callid,"",req.from_tag,session);
      // session does not get its own INVITE
      //      session->postEvent(new AmSipRequestEvent(req)); 
    }
  } 
  catch(const AmSession::Exception& e){
    ERROR("%i %s\n",e.code,e.reason.c_str());
    AmSipDialog::reply_error(req,e.code,e.reason);
  }
  catch(const string& err){
    ERROR("startSession: %s\n",err.c_str());
    AmSipDialog::reply_error(req,500,err);
  }
  catch(...){
    ERROR("unexpected exception\n");
    AmSipDialog::reply_error(req,500,"unexpected exception");
  }
  as_mut.unlock();
  return session;
}

void AmSessionContainer::startSessionUAS(AmSipRequest& req)
{
  as_mut.lock();
  try {
	
    AmSession* session = getSession(req.callid,req.from_tag);
    if( session ){
	    
      // it's a forked-and-merged INVITE
      // reply 482 Loop detected
      throw AmSession::Exception(482, "Loop detected");
    }
    else {
      // Call-ID and From-Tag are unknown: it's a new session
      AmSession* session;
      if((session = createSession(req)) != 0){

	// update session's local tag (ID) if not already set
	session->setLocalTag();
	const string& local_tag = session->getLocalTag();
	// by default each session is in its own callgroup
	session->setCallgroup(local_tag);

	if (AmConfig::LogSessions) {
	  INFO("Starting UAS session %s app %s\n",
	       session->getLocalTag().c_str(), req.cmd.c_str());
	}

	session->start();

	addSession_unsafe(req.callid,req.from_tag,local_tag,session);
	session->postEvent(new AmSipRequestEvent(req));
      }
    }
  } 
  catch(const AmSession::Exception& e){
    ERROR("%i %s\n",e.code,e.reason.c_str());
    AmSipDialog::reply_error(req,e.code,e.reason);
  }
  catch(const string& err){
    ERROR("startSession: %s\n",err.c_str());
    AmSipDialog::reply_error(req,500,err);
  }
  catch(...){
    ERROR("unexpected exception\n");
    AmSipDialog::reply_error(req,500,"unexpected exception");
  }
  as_mut.unlock();
}


bool AmSessionContainer::postEvent(const string& callid, 
				   const string& remote_tag,
				   AmEvent* event)
{
  //     DBG("postEvent: callid = %s; remote_tag = %s\n",
  // 	callid.c_str(),remote_tag.c_str());

  as_mut.lock();
  AmSession* s = getSession(callid,remote_tag);
    
  if(!s){
    as_mut.unlock();
    delete event;
    return false;
  }
    
  s->postEvent(event);
  as_mut.unlock();

  return true;
}

bool AmSessionContainer::postEvent(const string& local_tag,
				   AmEvent* event) 
{
  //     DBG("postEvent: local_tag = %s\n",local_tag.c_str());
  as_mut.lock();
  AmSession* s = getSession(local_tag);

  if (s  != NULL) {
    s->postEvent(event);
    as_mut.unlock();
    return true;
  }    
  as_mut.unlock();
	
  // try session factories
  AmSessionFactory* sf = AmPlugIn::instance()->getFactory4App(local_tag);
  if (sf != NULL) {
    sf->postEvent(event);
    return true;
  }

  delete event;
  return false;
}

AmSession* AmSessionContainer::createSession(AmSipRequest& req, 
					     AmArg* session_params)
{
  
  if (AmConfig::SessionLimit &&                   
      AmConfig::SessionLimit <= a_sessions.size()) {
    DBG("session_limit %d reached. Not creating session.\n", 
	AmConfig::SessionLimit);
    throw AmSession::Exception(AmConfig::SessionLimitErrCode, AmConfig::SessionLimitErrReason);
  }

  if(req.cmd.empty()){
    throw string("AmSessionContainer::createSession: req.cmd is empty!\n");
  } 
  else if(req.cmd == "sems"){
    switch (AmConfig::AppSelect) {
    case AmConfig::App_RURIUSER:
      req.cmd = req.user; 
      break;
    case AmConfig::App_APPHDR: 
      req.cmd = getHeader(req.hdrs, APPNAME_HDR); 
      break;      
    case AmConfig::App_RURIPARAM: 
      req.cmd = get_header_param(req.r_uri, "app");
      break;
    case AmConfig::App_MAPPING: 
      {
	req.cmd = "";	
	for (AmConfig::AppMappingVector::iterator it = 
	       AmConfig::AppMapping.begin(); 
	     it != AmConfig::AppMapping.end(); it++){
	  if (!regexec(&it->first, req.r_uri.c_str(), 0, NULL, 0)) {
	    DBG("match of r_uri '%s' to application %s\n", 
	      req.r_uri.c_str(), it->second.c_str());
	    req.cmd = it->second;
	    break;
	  }
	}
      } break;
    case AmConfig::App_SPECIFIED: 
      req.cmd = AmConfig::Application; 
      break;
    }
    if (req.cmd.empty()) {
      string ex = "Unknown Application";
      throw ex; 
    }
  }

  AmSessionFactory* state_factory = AmPlugIn::instance()->getFactory4App(req.cmd);
  if(!state_factory) {
    ERROR("application '%s' not found !\n", req.cmd.c_str());
    throw string("application '" + req.cmd + "' not found !");
  }
	    
  AmSession* session = NULL;
  if (req.method == "INVITE") {
    if (NULL != session_params) 
      session = state_factory->onInvite(req, *session_params);
    else 
      session = state_factory->onInvite(req);
  } else if (req.method == "REFER") {
    if (NULL != session_params) 
      session = state_factory->onRefer(req, *session_params);
    else 
      session = state_factory->onRefer(req);
  }

  if(!session) {
    //  State creation failed:
    //   application denied session creation
    //   or there was an error.
    //
    //  let's hope the createState function has replied...
    //  ... and do nothing !

    DBG("onInvite returned NULL\n");
    return 0;
  }
    
  //state_factory->configureSession(session);
  //session->checkSessionExpires(req);

  return session;
}

bool AmSessionContainer::addSession_unsafe(const string& callid, 
					   const string& remote_tag,
					   const string& local_tag,
					   AmSession* session)
{
  if(getSession(callid,remote_tag))
    return false;

  if (!remote_tag.empty())
    as_id_lookup[callid+remote_tag] = local_tag;

  return addSession_unsafe(local_tag,session);
}

bool AmSessionContainer::addSession_unsafe(const string& local_tag,
					   AmSession* session)
{
  if(getSession(local_tag))
    return false;
  
  //DBG("a_sessions['%s'] = session\n",local_tag.c_str());
  a_sessions[local_tag] = session;
  return true;
}

bool AmSessionContainer::addSession(const string& callid,
				    const string& remote_tag,
				    const string& local_tag,
				    AmSession* session)
{
  as_mut.lock();
  bool ret = addSession_unsafe(callid,remote_tag,local_tag,session);
  as_mut.unlock();
  return ret;
}

bool AmSessionContainer::addSession(const string& local_tag,
				    AmSession* session)
{
  as_mut.lock();
  bool ret = addSession_unsafe(local_tag,session);
  as_mut.unlock();
  return ret;
}

int AmSessionContainer::getSize()
{
  int res=0;
  as_mut.lock();
  res = a_sessions.size();
  as_mut.unlock();

  return res;
}
