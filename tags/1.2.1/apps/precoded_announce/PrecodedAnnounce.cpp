/*
 * $Id$
 *
 * Copyright (C) 2008 iptego GmbH
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

#include "PrecodedAnnounce.h"

#include "log.h"
#include "AmConfigReader.h"
#include "AmUtils.h"

EXPORT_SESSION_FACTORY(PrecodedFactory,MOD_NAME);

PrecodedFactory::PrecodedFactory(const string& _app_name)
  : AmSessionFactory(_app_name)
{
}

int PrecodedFactory::onLoad()
{
    AmConfigReader cfg;
    if(cfg.loadFile(AmConfig::ModConfigPath + string(MOD_NAME ".conf")))
	return -1;

    if (precoded_file.open(cfg.getParameter("announcement_file")) < 0) {
      ERROR("loading precoded file");
      return -1;
    }

    precoded_file.initPlugin();
    return 0;
}

AmSession* PrecodedFactory::onInvite(const AmSipRequest& req)
{
    return new PrecodedDialog(&precoded_file);
}

PrecodedDialog::PrecodedDialog(AmPrecodedFile* file_def)
  : file_def(file_def)
{
}

PrecodedDialog::~PrecodedDialog()
{
}

AmPayloadProviderInterface* PrecodedDialog::getPayloadProvider() {
  return file_def;
}

AmAudioRtpFormat* PrecodedDialog::getNewRtpFormat() {
  if (m_payloads.empty()) {
    ERROR("can not create RTP format without payloads.\n");
    return NULL;
  }

  current_payload = m_payloads[0]->payload_type;
    
  precoded_payload_t *precoded_payload = 
    static_cast<precoded_payload_t*>(file_def->payload(current_payload));
  if (precoded_payload == NULL) {
    ERROR("new payload not provided\n");
    return NULL;
  }
  return new AmPrecodedRtpFormat(*precoded_payload, m_payloads);
}

void PrecodedDialog::onSessionStart(const AmSipRequest& req)
{
  AmPrecodedFileInstance* file = file_def->getFileInstance(current_payload, 
							   m_payloads);
  if (!file) {
    ERROR("no payload\n");
  }
  if (!file || file->open()) { 
    ERROR("PrecodedDialog::onSessionStart: Cannot open file\n");
    dlg.bye();
    setStopped();
    return;
  }
 
  setOutput(file);
  setReceiving(false);
}

void PrecodedDialog::onBye(const AmSipRequest& req)
{
    DBG("onBye: stopSession\n");
    setStopped();
}

