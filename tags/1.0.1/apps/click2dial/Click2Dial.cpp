/*
 * $Id: $
 *
 * Copyright (C) 2007 Sipwise GmbH
 * Based on the concept of "announcement", Copyright (C) 2002-2003 Fhg Fokus
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

#include "Click2Dial.h"
#include "AmSessionContainer.h"
#include "AmConfig.h"
#include "AmUtils.h"
#include "AmApi.h"
#include "AmPlugIn.h"

#include "AmMediaProcessor.h"

#include "sems.h"
#include "log.h"

#define MOD_NAME "click2dial"

EXPORT_SESSION_FACTORY(Click2DialFactory, MOD_NAME);

string Click2DialFactory::AnnouncePath;
string Click2DialFactory::AnnounceFile;

Click2DialFactory::Click2DialFactory(const string& _app_name)
: AmSessionFactory(_app_name)
{
}


int Click2DialFactory::onLoad()
{
  AmConfigReader cfg;
  if(cfg.loadFile(AmConfig::ModConfigPath + string(MOD_NAME ".conf")))
    return -1;

  // get application specific global parameters
  configureModule(cfg);

  AnnouncePath = cfg.getParameter("announce_path",ANNOUNCE_PATH);
  if(!AnnouncePath.empty() && AnnouncePath[AnnouncePath.length()-1] != '/')
    AnnouncePath += "/";

  AnnounceFile = cfg.getParameter("default_announce",ANNOUNCE_FILE);
  DBG("AnnounceFile = %s\n",AnnounceFile.c_str());

  string announce_file = AnnouncePath + AnnounceFile;
  if(!file_exists(announce_file)) {
    ERROR("default file for ann_b2b module does not exist ('%s').\n",
      announce_file.c_str());
    return -1;
  }

  return 0;
}


string Click2DialFactory::getAnnounceFile(const AmSipRequest& req)
{
  string announce_path = AnnouncePath;
  string announce_file = announce_path + req.domain
    + "/" + req.user + ".wav";

  DBG("trying '%s'\n",announce_file.c_str());
  if(file_exists(announce_file))
    goto end;

  announce_file = announce_path + req.user + ".wav";
  DBG("trying '%s'\n",announce_file.c_str());
  if(file_exists(announce_file))
    goto end;
  announce_file = AnnouncePath + AnnounceFile;

  end:
  return announce_file;
}


AmSession* Click2DialFactory::onInvite(const AmSipRequest& req, AmArg& session_params)
{
  UACAuthCred* cred = NULL;
  string callee_uri, a_realm, a_user, a_pwd;

  if(session_params.size() != 4) {
    ERROR("Need 4 parameters, got %lu\n", 
	  (long unsigned int)session_params.size());
    return NULL;
  }

  if(session_params.get(0).getType() == AmArg::CStr) {
    a_realm = string(session_params.get(0).asCStr());
  }
  else {
    ERROR("All arguments have to be CStr\n");
    return NULL;
  }
  if(session_params.get(1).getType() == AmArg::CStr) {
    a_user = string(session_params.get(1).asCStr());
  }
  else {
    ERROR("All arguments have to be CStr\n");
    return NULL;
  }
  if (session_params.get(2).getType() == AmArg::CStr) {
    a_pwd = string(session_params.get(2).asCStr());
  }
  else {
    ERROR("All arguments have to be CStr\n");
    return NULL;
  }

  cred = new UACAuthCred(a_realm, a_user, a_pwd);
  if(cred == NULL) {
    ERROR("Failed to create authentication handle\n");
    return NULL;
  }

  if (session_params.get(3).getType() == AmArg::CStr) {
    callee_uri = string(session_params.get(3).asCStr());
  }
  else {
    ERROR("All arguments have to be CStr\n");
    return NULL;
  }

  AmSession* s = new C2DCallerDialog(req, getAnnounceFile(req), callee_uri, cred);
  if(s == NULL) {
    ERROR("Failed to create a click2dial dialog");
    return NULL;
  }

  AmSessionEventHandlerFactory* uac_auth_f =
    AmPlugIn::instance()->getFactory4Seh("uac_auth");
  if(uac_auth_f != NULL) {
    DBG("UAC Auth enabled for new announcement session.\n");
    AmSessionEventHandler *h = uac_auth_f->getHandler(s);
    if (h != NULL) {
      s->addHandler(h);
    }
    else {
      ERROR("Failed to get authentication event handler");
      delete s;
      return NULL;
    }
  }
  else {
    ERROR("uac_auth interface not accessible. "
      "Load uac_auth for authenticated dialout.\n");

    return NULL;
  }

  return s;
}


AmSession* Click2DialFactory::onInvite(const AmSipRequest& req)
{
  return new C2DCallerDialog(req, getAnnounceFile(req), NULL, NULL);
}


C2DCallerDialog::C2DCallerDialog(const AmSipRequest& req,
const string& filename, const string& c_uri, UACAuthCred* credentials)
: filename(filename), callee_uri(c_uri), cred(credentials),
AmB2BCallerSession()
{
  set_sip_relay_only(false);
}


void C2DCallerDialog::onSessionStart(const AmSipReply& rep)
{
  setReceiving(false);
  invite_req.body = rep.body;

  if(wav_file.open(filename,AmAudioFile::Read))
    throw string("AnnouncementDialog::onSessionStart: Cannot open file\n");
  setOutput(&wav_file);
}


void C2DCallerDialog::onSessionStart(const AmSipRequest& req)
{
}


void C2DCallerDialog::process(AmEvent* event)
{
  AmAudioEvent* audio_event = dynamic_cast<AmAudioEvent*>(event);
  if(audio_event && audio_event->event_id == AmAudioEvent::cleared) {

    if(getCalleeStatus() != None)
      return;
    AmMediaProcessor::instance()->removeSession(this);

    connectCallee(string("<") + callee_uri + ">", callee_uri);
    return;
  }

  AmB2BCallerSession::process(event);
}


void C2DCallerDialog::createCalleeSession()
{
  UACAuthCred* c = new UACAuthCred(cred.get()->realm,
    cred.get()->user, cred.get()->pwd);

  AmB2BCalleeSession* callee_session = new C2DCalleeDialog(this, c);
  AmSipDialog& callee_dlg = callee_session->dlg;

  other_id = AmSession::getNewId();

  callee_dlg.local_tag    = other_id;
  callee_dlg.callid       = AmSession::getNewId() + "@" + AmConfig::LocalIP;
  callee_dlg.local_party  = dlg.local_party;
  callee_dlg.remote_party = dlg.remote_party;
  callee_dlg.remote_uri   = dlg.remote_uri;

  callee_session->start();

  AmSessionContainer* sess_cont = AmSessionContainer::instance();
  sess_cont->addSession(other_id,callee_session);
}


void C2DCallerDialog::onB2BEvent(B2BEvent* ev)
{
  if(ev->event_id == B2BSipReply) {
    AmSipReply& reply = ((B2BSipReplyEvent*)ev)->reply;

    if(((reply.code == 407)||(reply.code == 401))  && cred.get() != NULL) {
      AmB2BSession::onB2BEvent(ev);
      return;
    }
  }
  AmB2BCallerSession::onB2BEvent(ev);
}


C2DCalleeDialog::C2DCalleeDialog(const AmB2BCallerSession* caller, UACAuthCred* credentials)
: AmB2BCalleeSession(caller), cred(credentials)
{
  setAuthHandler();
}


void C2DCalleeDialog::setAuthHandler()
{
  if(cred.get() != NULL) {
    AmSessionEventHandlerFactory* uac_auth_f =
      AmPlugIn::instance()->getFactory4Seh("uac_auth");
    if (uac_auth_f != NULL) {
      AmSessionEventHandler *h = uac_auth_f->getHandler(this);
      if (h != NULL ) {
        DBG("uac-auth enabled for new callee session.\n");
        addHandler(h);
      }
      else {
        ERROR("uac_auth interface not accessible. "
          "Load uac_auth for authenticated dialout.\n");
      }
    }
  }
}
