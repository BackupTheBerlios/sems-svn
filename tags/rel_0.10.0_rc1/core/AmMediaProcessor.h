/*
 * $Id: AmMediaProcessor.h,v 1.1.2.4 2005/06/01 12:00:24 rco Exp $
 *
 * Copyright (C) 2002-2003 Fhg Fokus
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

#ifndef _AmMediaProcessor_h_
#define _AmMediaProcessor_h_

#include "AmSession.h"
#include "AmEventQueue.h"

#include <set>
using std::set;
#include <map>
using std::map;
using std::multimap;

struct SchedRequest;

/**
 * \brief Media processing thread
 * 
 * This class implements a media processing thread.
 * It processes the media and triggers the sending of RTP
 * of all sessions added to it.
 */
class AmMediaProcessorThread :
  public AmThread,
  public AmEventHandler
{
    AmEventQueue    events;
    unsigned char   buffer[AUDIO_BUFFER_SIZE];
    set<AmSession*> sessions;
  
    void processAudio(unsigned int ts);
    /**
     * Process pending DTMF events
     */
    void processDtmfEvents();

    // AmThread interface
    void run();
    void on_stop();
    
    // AmEventHandler interface
    void process(AmEvent* e);
public:
  AmMediaProcessorThread();
  ~AmMediaProcessorThread();

  inline void postRequest(SchedRequest* sr);
  
  unsigned int getLoad();
};

/**
 * \brief Media processing thread manager
 * 
 * This class implements the manager that assigns and removes 
 * the Sessions to the various \ref MediaProcessorThreads, 
 * according to their call group. This class contains the API 
 * for the MediaProcessor.
 */
class AmMediaProcessor
{
  static AmMediaProcessor* _instance;

  unsigned int num_threads; 
  AmMediaProcessorThread**  threads;
  
  map<string, unsigned int> callgroup2thread;
  multimap<string, AmSession*> callgroupmembers;
  map<AmSession*, string> session2callgroup;
  AmMutex group_mut;

  AmMediaProcessor();
  ~AmMediaProcessor();
	
  void removeFromProcessor(AmSession* s, unsigned int r_type);
public:
  /** 
   * InsertSession : inserts the session to the processor
   * RemoveSession : remove the session from the processor
   * ClearSession  : remove the session from processor and clear audio
   */
  enum { InsertSession, RemoveSession, ClearSession };

  static AmMediaProcessor* instance();

  void init();
	/** Add session s to processor */
  void addSession(AmSession* s, const string& callgroup);
	/** Remove session s from processor */
  void removeSession(AmSession* s);
	/** Remove session s from processor and clear its audio */
  void clearSession(AmSession* s);

};


#endif

// Local Variables:
// mode:C++
// End:





