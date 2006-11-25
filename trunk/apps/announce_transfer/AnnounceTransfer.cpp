/*
 * $Id: Announcement.cpp,v 1.7.8.4 2005/08/31 13:54:29 rco Exp $
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 * Copyright (C) 2006 iptego GmbH
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

#include "AnnounceTransfer.h"
#include "AmConfig.h"
#include "AmUtils.h"

#include "sems.h"
#include "log.h"

#define MOD_NAME "announce_transfer"

EXPORT_SESSION_FACTORY(AnnounceTransferFactory,MOD_NAME);

string AnnounceTransferFactory::AnnouncePath;
string AnnounceTransferFactory::AnnounceFile;

AnnounceTransferFactory::AnnounceTransferFactory(const string& _app_name)
  : AmSessionFactory(_app_name)
{
}

int AnnounceTransferFactory::onLoad()
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

AmSession* AnnounceTransferFactory::onInvite(const AmSipRequest& req)
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
    return new AnnounceTransferDialog(announce_file);
}

AnnounceTransferDialog::AnnounceTransferDialog(const string& filename)
    : filename(filename),
	  status(Disconnected)
{
}

AnnounceTransferDialog::~AnnounceTransferDialog()
{
}

void AnnounceTransferDialog::onSessionStart(const AmSipRequest& req)
{
	DBG("AnnounceTransferDialog::onSessionStart\n");
	if (status == Disconnected) {
		status = Announcing;
		callee_uri = getHeader(req.hdrs, "P-Refer-To");
		if (!callee_uri.length())
		  callee_uri = req.r_uri;
		DBG("transfer uri set to '%s'\n", callee_uri.c_str());
		startSession();
	}
}

void AnnounceTransferDialog::startSession(){
    if(wav_file.open(filename,AmAudioFile::Read))
	throw string("AnnounceTransferDialog::onSessionStart: Cannot open file\n");
    
    setOutput(&wav_file);
}

void AnnounceTransferDialog::onSipRequest(const AmSipRequest& req)
{
    AmSession::onSipRequest(req);

    if((status == Transfering) && 
	   (req.method == "NOTIFY")) {
		try {

			if (getHeader(req.hdrs,"Event") != "refer") 
				throw AmSession::Exception(481, "Subscription does not exist");

			if ((strip_header_params(getHeader(req.hdrs,"Content-Type")) 
				 != "message/sipfrag"))
				throw AmSession::Exception(415, "Unsupported Media Type");
			
			string sipfrag_sline = req.body.substr(8, req.body.find("\n") - 8);
			DBG("extracted start line from sipfrag '%s'\n", sipfrag_sline.c_str());
			unsigned int code;
			string res_msg;

			
			if ((req.body.length() < 11)
				 || (parse_return_code(sipfrag_sline.c_str(), code, res_msg))) {
				throw AmSession::Exception(400, "Bad Request");				
			}

			if ((code >= 200)&&(code < 300)) {
				status = Hangup;
				DBG("refer succeeded... stopSession\n");
				dlg.bye();
				setStopped();
			} else if (code > 300) {
				DBG("refer failed...\n");
				dlg.bye();
				setStopped();
			}
			dlg.reply(req, 200, "OK", "", "");
		} catch (const AmSession::Exception& e) {
			dlg.reply(req, e.code, e.reason, "", "");
		}
	}

}

void AnnounceTransferDialog::onSipReply(const AmSipReply& rep) {
	if (status==Transfering && 
		dlg.get_uac_trans_method(rep.cseq) == "REFER") {
		if (rep.code >= 300) {
			DBG("refer not accepted, stop session.\n");
				dlg.bye();
				setStopped();
		}
	}

    AmSession::onSipReply(rep);
}

void AnnounceTransferDialog::onBye(const AmSipRequest& req)
{
    DBG("onBye: stopSession\n");
    setStopped();
}

void AnnounceTransferDialog::process(AmEvent* event)
{

    AmAudioEvent* audio_event = dynamic_cast<AmAudioEvent*>(event);
	
    if(audio_event && (audio_event->event_id == AmAudioEvent::cleared) 
	   && (status == Announcing)){
		dlg.refer(callee_uri);
		status = Transfering;
		return;
    }

    AmSession::process(event);
}
