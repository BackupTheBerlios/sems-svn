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

#ifndef _ANNOUNCEB2B_H_
#define _ANNOUNCEB2B_H_

#include "AmSession.h"
#include "AmConfigReader.h"
#include "AmSipRequest.h"
#include "AmAudio.h"
#include "AmB2BSession.h"

#include <string>
using std::string;

class AnnounceB2BFactory: public AmSessionFactory
{
public:
    static string AnnouncePath;
    static string AnnounceFile;

    AnnounceB2BFactory(const string& _app_name);

    int onLoad();
    AmSession* onInvite(const AmSipRequest& req);
};

class AnnounceCallerDialog: public AmB2BCallerSession
{
    AmAudioFile wav_file;
    string filename;

    string callee_addr;
    string callee_uri;
    
public:
    AnnounceCallerDialog(const string& filename);
    
    void process(AmEvent* event);
    void onSessionStart(const AmSipRequest& req);
};

#endif
// Local Variables:
// mode:C++
// End:

