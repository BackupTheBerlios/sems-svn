/*
 * $Id: Statistics.h,v 1.3 2005/06/17 10:41:10 sayer Exp $
 *
 * Copyright (C) 2002-2005 Fhg Fokus
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

#ifndef _Statistics_h_
#define _Statistics_h_

#include "AmApi.h"
#include "AmInterfaceHandler.h"

#define MOD_NAME "stats"

class AmSessionContainer;

class StatsFactory: public AmSessionFactory,
		    public AmInterfaceHandler
{
    AmSessionContainer* sc;

public:
    StatsFactory(const std::string& _app_name);
    
    // AmSessionFactory interface
    int onLoad();
    AmSession* onInvite(const AmSipRequest& req);
    
    // AmInterfaceHandler interface
    int handleRequest(AmCtrlInterface* ctrl);
};

// template<class T> 
// class SharedVarInc: public AmSharedVar<T>
// {
// public:
//     SharedVarInc(const T& _t) : AmSharedVar<T>(_t) {}

//     void operator ++ () {
// 	lock();
// 	set(get()+1);
// 	unlock();
//     }
// };

// class Statistics
// {
// public:
//     Statistics();
//     ~Statistics();

//     void onSessionStart(AmRequest* req);
//     void onBye();
// };


#endif
// Local Variables:
// mode:C++
// End:
