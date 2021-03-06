/*
 * $Id: CacheAnnounce.h 145 2006-11-26 00:01:18Z sayer $
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 * Copyright (C) 2007 iptego GmbH
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

#ifndef _CACHEANNOUNCE_H_
#define _CACHEANNOUNCE_H_

#include "AmSession.h"
#include "AmConfigReader.h"

#include "AmCachedAudioFile.h"

#include <memory>
using std::auto_ptr;

#include <string>
using std::string;

class CacheAnnounceFactory: public AmSessionFactory
{
	
    MemStream ann_cache;

public:
    static string AnnouncePath;
    static string AnnounceFile;

    CacheAnnounceFactory(const string& _app_name);

    int onLoad();
    AmSession* onInvite(const AmSipRequest& req);
};

class CacheAnnounceDialog : public AmSession
{
    auto_ptr<AmCachedAudioFile> wav_file;
    MemStream* announce;

 public:
    CacheAnnounceDialog(MemStream* announce);
    ~CacheAnnounceDialog();

    void onSessionStart(const AmSipRequest& req);
    void onSessionStart(const AmSipReply& rep);
    void startSession();
    void onBye(const AmSipRequest& req);
    void onDtmf(int event, int duration_msec) {}

    void process(AmEvent* event);
};

#endif
// Local Variables:
// mode:C++
// End:

