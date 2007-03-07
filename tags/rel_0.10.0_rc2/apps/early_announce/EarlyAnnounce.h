/*
 * $Id$
 *
 * Copyright (C) 2006 iptel.org GmbH
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

#ifndef _EarlyAnnounce_h_
#define _EarlyAnnounce_h_

#include "AmSession.h"
#include "AmConfigReader.h"

#include <string>
using std::string;

class EarlyAnnounceFactory: public AmSessionFactory
{
public:
    static string AnnouncePath;
    static string AnnounceFile;

    EarlyAnnounceFactory(const string& _app_name);

    int onLoad();
    AmSession* onInvite(const AmSipRequest& req);
};

class EarlyAnnounceDialog : public AmSession
{
    AmAudioFile wav_file;
    string filename;
    AmSipRequest localreq;
    
 public:
    EarlyAnnounceDialog(const string& filename);
    ~EarlyAnnounceDialog();

    void onInvite(const AmSipRequest& req);
    void onSessionStart(const AmSipRequest& req);
    void onBye(const AmSipRequest& req);
    void onCancel();
    void onDtmf(int event, int duration_msec) {}

    void process(AmEvent* event);
};

#endif
// Local Variables:
// mode:C++
// End:

