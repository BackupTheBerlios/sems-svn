/*
 * $Id$
 *
 * Copyright (C) 2008 iptego GmbH
 *
 * This file is part of SEMS, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _MOD_SYS_H
#define _MOD_SYS_H
#include "DSMModule.h"

class URIModule 
: public DSMModule {

 public:
  URIModule();
  ~URIModule();
  
  DSMAction* getAction(const string& from_str);
  DSMCondition* getCondition(const string& from_str);

  void onInvite(const AmSipRequest& req, DSMSession* sess);
};

DEF_TwoParAction(URIParseAction);
DEF_TwoParAction(URIGetHeaderAction);

#endif
