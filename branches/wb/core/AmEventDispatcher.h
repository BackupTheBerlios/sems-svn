/*
 * $Id$
 *
 * Copyright (C) 2008 Raphael Coeffic
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
#ifndef _AMEVENTDISPATCHER_h_ 
#define _AMEVENTDISPATCHER_h_

#include "AmEventQueue.h"
#include "AmSipMsg.h"
#include <map>

class AmEventDispatcher
{
public:

    typedef std::map<string, AmEventQueueInterface*> EvQueueMap;
    typedef EvQueueMap::iterator                     EvQueueMapIter;
    
    typedef std::map<string,string>  Dictionnary;
    typedef Dictionnary::iterator    DictIter;


private:

    static AmEventDispatcher *_instance;

    /** 
     * Container for active sessions 
     * local tag -> event queue
     */
    EvQueueMap queues;

    /** 
     * Call ID + remote tag -> local tag 
     *  (needed for CANCELs and some provisionnal answers)
     *  (UAS sessions only)
     */
    Dictionnary id_lookup;
    
    // mutex for "queues" and "id_lookup"
    AmMutex m_queues;

public:

    static AmEventDispatcher* instance();

    bool postSipRequest(const string& callid, const string& remote_tag, 
			const AmSipRequest& req);

    bool post(const string& local_tag, AmEvent* ev);
    bool post(const string& callid, const string& remote_tag, AmEvent* ev);

    bool addEventQueue(const string& local_tag, 
		       AmEventQueueInterface* q,
		       const string& callid="", 
		       const string& remote_tag="");

    AmEventQueueInterface* delEventQueue(const string& local_tag,
					 const string& callid="", 
					 const string& remote_tag="");
};

#endif
