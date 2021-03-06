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
#ifndef _STATE_ENGINE_H
#define _STATE_ENGINE_H

#include "DSMElemContainer.h"
#include "AmSipMsg.h"

class AmSession;
class DSMSession;

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;

using std::pair;

#include "log.h"

class DSMElement {
 public: 
  DSMElement() { }
  virtual ~DSMElement() { }
  string name; // documentary only

};

class DSMCondition
  : public DSMElement {
 public:
  enum EventType {
    Any,
    Invite,
    SessionStart,
    Key,
    Timer,

    NoAudio,

    Hangup,
    Hold,
    UnHold,

    XmlrpcResponse,
    DSMEvent,
    PlaylistSeparator
  };

  DSMCondition() { }
  virtual ~DSMCondition() { }

  EventType type;
  map<string, string> params;

  virtual bool match(AmSession* sess, DSMCondition::EventType event,
		     map<string,string>* event_params);
};

class DSMAction 
: public DSMElement {
 public:
  /** modifies State Engine operation */
  enum SEAction {
    None,   // no modification
    Repost, // repost current event
    Jump,   // jump FSM
    Call,   // call FSM
    Return  // return from FSM call 
  };

  DSMAction() { /* DBG("const action\n"); */ }
  virtual ~DSMAction() { /* DBG("dest action\n"); */ }

  /** @return whether state engine is to be modified (via getSEAction) */
  virtual bool execute(AmSession* sess, DSMCondition::EventType event,	\
		       map<string,string>* event_params) = 0;

  /** @return state engine modification */
  virtual SEAction getSEAction(string& param) { return None; }
};

class DSMTransition;

class State 
: public DSMElement {
 public:
  State();
  ~State();
  vector<DSMAction*> pre_actions;
  vector<DSMAction*> post_actions;
  
  vector<DSMTransition> transitions;
};

class DSMTransition 
: public DSMElement {
 public:
  DSMTransition();
  ~DSMTransition();

  vector<DSMCondition*> precond;
  vector<DSMAction*> actions;
  string from_state;
  string to_state;
};

class DSMModule;

class DSMStateDiagram  {
  vector<State> states;
  string name;
  string initial_state;

 public:
  DSMStateDiagram(const string& name);
  ~DSMStateDiagram();

  State* getInitialState();
  State* getState(const string& s_name);

  void addState(const State& state, bool is_initial = false);
  bool addTransition(const DSMTransition& trans);
  const string& getName() { return name; }
};

class DSMStateEngine {
  State* current;
  DSMStateDiagram* current_diag;
  vector<DSMStateDiagram*> diags;

  vector<pair<DSMStateDiagram*, State*> > stack;

  bool callDiag(const string& diag_name, AmSession* sess, DSMCondition::EventType event,
			   map<string,string>* event_params);
  bool jumpDiag(const string& diag_name, AmSession* sess, DSMCondition::EventType event,
			   map<string,string>* event_params);
  bool returnDiag(AmSession* sess);
  bool runactions(vector<DSMAction*>::iterator from, 
		  vector<DSMAction*>::iterator to, 
		  AmSession* sess, DSMCondition::EventType event,
		  map<string,string>* event_params,  bool& is_consumed);

  vector<DSMModule*> mods;

 public: 
  DSMStateEngine();
  ~DSMStateEngine();

  void addDiagram(DSMStateDiagram* diag); 
  void addModules(vector<DSMModule*> modules);

  bool init(AmSession* sess, const string& startDiagram,
	    DSMCondition::EventType init_event);

  void runEvent(AmSession* sess,
		DSMCondition::EventType event,
		map<string,string>* event_params);

  /** @return whether call should be accepted */
  bool onInvite(const AmSipRequest& req, DSMSession* sess);
};


#endif
