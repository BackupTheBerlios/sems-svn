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

#ifndef _PRECODEDANNOUNCEAPP_H_
#define _PRECODEDANNOUNCEAPP_H_

#include "AmSession.h"
#include "AmPrecodedFile.h"

#include <string>
using std::string;

class PrecodedFactory: public AmSessionFactory
{
public:
    AmPrecodedFile precoded_file;  

    PrecodedFactory(const string& _app_name);

    int onLoad();
    AmSession* onInvite(const AmSipRequest& req);
};

class PrecodedDialog : public AmSession
{
    
  AmPrecodedFile* file_def;

 public:
  PrecodedDialog(AmPrecodedFile* file_def);
  ~PrecodedDialog();
  
  void onSessionStart(const AmSipRequest& req);
  void onBye(const AmSipRequest& req);

  AmPayloadProviderInterface* getPayloadProvider();
};

#endif

