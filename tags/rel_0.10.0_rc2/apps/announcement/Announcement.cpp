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

#include "Announcement.h"
#include "AmConfig.h"
#include "AmUtils.h"

#include "sems.h"
#include "log.h"

#define MOD_NAME "announcement"

EXPORT_SESSION_FACTORY(AnnouncementFactory,MOD_NAME);

string AnnouncementFactory::AnnouncePath;
string AnnouncementFactory::AnnounceFile;

AnnouncementFactory::AnnouncementFactory(const string& _app_name)
  : AmSessionFactory(_app_name)
{
}

int AnnouncementFactory::onLoad()
{
    AmConfigReader cfg;
    if(cfg.loadFile(AmConfig::ModConfigPath + string(MOD_NAME ".conf")))
	return -1;

    // get application specific global parameters
    configureModule(cfg);

    AnnouncePath = cfg.getParameter("announce_path",ANNOUNCE_PATH);
    if( !AnnouncePath.empty() 
	&& AnnouncePath[AnnouncePath.length()-1] != '/' )
	AnnouncePath += "/";

    AnnounceFile = cfg.getParameter("default_announce",ANNOUNCE_FILE);

    string announce_file = AnnouncePath + AnnounceFile;
    if(!file_exists(announce_file)){
	ERROR("default file for ann_b2b module does not exist ('%s').\n",
	      announce_file.c_str());
	return -1;
    }

    return 0;
}

AmSession* AnnouncementFactory::onInvite(const AmSipRequest& req)
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
    return new AnnouncementDialog(announce_file);
}

AnnouncementDialog::AnnouncementDialog(const string& filename)
    : filename(filename)
{
}

AnnouncementDialog::~AnnouncementDialog()
{
}

void AnnouncementDialog::onSessionStart(const AmSipRequest& req)
{
    DBG("AnnouncementDialog::onSessionStart\n");
    startSession();
}

void AnnouncementDialog::onSessionStart(const AmSipReply& rep)
{
    DBG("AnnouncementDialog::onSessionStart (SEMS originator mode)\n");
    startSession();
}

void AnnouncementDialog::startSession(){
    // we can drop all received packets
    // this disables DTMF detection as well
    setReceiving(false);

    if(wav_file.open(filename,AmAudioFile::Read))
	throw string("AnnouncementDialog::onSessionStart: Cannot open file\n");
    
    setOutput(&wav_file);
}

void AnnouncementDialog::onBye(const AmSipRequest& req)
{
    DBG("onBye: stopSession\n");
    setStopped();
}


void AnnouncementDialog::process(AmEvent* event)
{

    AmAudioEvent* audio_event = dynamic_cast<AmAudioEvent*>(event);
    if(audio_event && (audio_event->event_id == AmAudioEvent::cleared)){
	dlg.bye();
	setStopped();
	return;
    }

    AmSession::process(event);
}
