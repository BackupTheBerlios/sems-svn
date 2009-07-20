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

#include "DSMStateEngine.h"
#include "DSMModule.h"

#include "AmUtils.h"
#include "AmSession.h"
#include "log.h"

#include "AmSessionContainer.h"
#include "ampi/MonitoringAPI.h"

DSMStateDiagram::DSMStateDiagram(const string& name) 
  : name(name) {
}

DSMStateDiagram::~DSMStateDiagram() {
}

void DSMStateDiagram::addState(const State& state, bool is_initial) {
  DBG("adding state '%s'\n", state.name.c_str());
  for (vector<DSMAction*>::const_iterator it=
	 state.pre_actions.begin(); it != state.pre_actions.end(); it++) {
    DBG("   pre-action '%s'\n", (*it)->name.c_str());
  }
  for (vector<DSMAction*>::const_iterator it=
	 state.post_actions.begin(); it != state.post_actions.end(); it++) {
    DBG("   post-action '%s'\n", (*it)->name.c_str());
  }

  states.push_back(state);
  if (is_initial) {
    if (!initial_state.empty()) {
      ERROR("trying to override initial state '%s' with '%s'\n",
	    initial_state.c_str(), state.name.c_str());
    } else {
      initial_state = state.name;
      DBG("set initial state '%s'\n", state.name.c_str());
    }
  }
}

bool DSMStateDiagram::addTransition(const DSMTransition& trans) {
  DBG("adding Transition '%s' %s -(...)-> %s\n",
      trans.name.c_str(), trans.from_state.c_str(), trans.to_state.c_str());
  for (vector<DSMCondition*>::const_iterator it=
	 trans.precond.begin(); it != trans.precond.end(); it++) {
    DBG("       DSMCondition  %s'%s'\n", 
	(*it)->invert?"not ":"", (*it)->name.c_str());
  }
  for (vector<DSMAction*>::const_iterator it=
	 trans.actions.begin(); it != trans.actions.end(); it++) {
    DBG("       Action     '%s'\n", (*it)->name.c_str());
  }

  vector<string> from_states;

  if (trans.from_state.find_first_of("(") != string::npos) {

    string states = trans.from_state;
    if (states.length() && states[0] == '(')
      states = states.substr(1);
    if (states.length() && states[states.length()-1] == ')')
      states = states.substr(0, states.length()-1);

    from_states = explode(states, ",");
    for (vector<string>::iterator it=from_states.begin(); 
	 it != from_states.end(); it++) {
      if (it->length() && (*it)[0] == ' ')
	*it = it->substr(1);
      if (it->length() && (*it)[it->length()-1] == ' ')
	*it = it->substr(0, it->length()-1);
    }

  } else  {
    from_states.push_back(trans.from_state);
  }

  for (vector<string>::iterator it=
	 from_states.begin(); it != from_states.end(); it++) {
    State* source_st = getState(*it);
    if (!source_st) {
      ERROR("state '%s' for transition '%s' not found\n",
	    it->c_str(), trans.name.c_str());
      return false;
    }
    
    source_st->transitions.push_back(trans);
  }

  return true;
}

State* DSMStateDiagram::getState(const string& s_name) {
  // find target state
  for (vector<State>::iterator target_st = 
	 states.begin(); target_st != states.end(); target_st++) {
    if (target_st->name == s_name) 
      return  &(*target_st);
  }

  return NULL;
}

State* DSMStateDiagram::getInitialState() {
  if (initial_state.empty()) {
    ERROR("diag '%s' doesn't have an initial state!\n",
	  name.c_str());
    return false;
  }
  return getState(initial_state);
}


DSMStateEngine::DSMStateEngine() 
  : current(NULL) {
}

DSMStateEngine::~DSMStateEngine() {
}

bool DSMStateEngine::onInvite(const AmSipRequest& req, DSMSession* sess) {
  bool res = true;
  for (vector<DSMModule*>::iterator it =
	 mods.begin(); it != mods.end(); it++)
    res &= (*it)->onInvite(req, sess);

  return res;
}

bool DSMStateEngine::runactions(vector<DSMAction*>::iterator from, 
			     vector<DSMAction*>::iterator to, 
			     AmSession* sess,  DSMCondition::EventType event,
			     map<string,string>* event_params,  bool& is_consumed) {
//   DBG("running %zd actions\n", to - from);
  for (vector<DSMAction*>::iterator it=from; it != to; it++) {
    DBG("executing '%s'\n", (*it)->name.c_str()); 
    if ((*it)->execute(sess, event, event_params)) {
      string se_modifier;
      switch ((*it)->getSEAction(se_modifier)) {
      case DSMAction::Repost: 
	is_consumed = false; 
	break;
      case DSMAction::Jump: 
	DBG("jumping %s\n", se_modifier.c_str());
	if (jumpDiag(se_modifier, sess, event, event_params)) {
	  // is_consumed = false; 
	  return true;  
	} break;
      case DSMAction::Call: 
	if (callDiag(se_modifier, sess, event, event_params))  {
	  // is_consumed = false; 
	  return true;   
	} break;
      case DSMAction::Return: 
	if (returnDiag(sess)) {  
	  //is_consumed = false;
	  return true; 
	} break;
      default: break;
      }
    }
  }
  
  return false;
} 

void DSMStateEngine::addDiagram(DSMStateDiagram* diag) {
  diags.push_back(diag);
}

void DSMStateEngine::addModules(vector<DSMModule*> modules) {
  for (vector<DSMModule*>::iterator it=
	 modules.begin(); it != modules.end(); it++)
    mods.push_back(*it);
}

bool DSMStateEngine::init(AmSession* sess, const string& startDiagram, 
			  DSMCondition::EventType init_event) {

  if (!jumpDiag(startDiagram, sess, init_event, NULL)) {
    ERROR("initializing with start diag '%s'\n",
	  startDiagram.c_str());
    return false;
  }  

  DBG("run init event...\n");
  runEvent(sess, init_event, NULL);
  return true;
}

bool DSMCondition::_match(AmSession* sess, 
		      DSMCondition::EventType event,
		      map<string,string>* event_params) {
  // or xor
  return invert?(!match(sess,event,event_params)):match(sess,event,event_params);
}

bool DSMCondition::match(AmSession* sess, 
		      DSMCondition::EventType event,
		      map<string,string>* event_params) {

  if ((type != Any) && (event != type))
    return false;

  if (!event_params)
    return true;

  for (map<string,string>::iterator it=params.begin();
       it!=params.end(); it++) {
    map<string,string>::iterator val = event_params->find(it->first);
    if (val == event_params->end() || val->second != it->second)
      return false;
  }

  DBG("condition matched: '%s'\n", name.c_str());
  return true;
}

void DSMStateEngine::runEvent(AmSession* sess,
			   DSMCondition::EventType event,
			   map<string,string>* event_params) {
  if (!current || !current_diag)
    return;

  bool is_consumed = true;
  do {
    is_consumed = true;

    for (vector<DSMTransition>::iterator tr = current->transitions.begin();
	 tr != current->transitions.end();tr++) {
      DBG("checking transition '%s'\n", tr->name.c_str());
      
      vector<DSMCondition*>::iterator con=tr->precond.begin();
      while (con!=tr->precond.end()) {
	if (!(*con)->_match(sess, event, event_params))
	  break;
	con++;
      }
      if (con == tr->precond.end()) {
	DBG("transition '%s' matched.\n", tr->name.c_str());
	
	//  matched all preconditions
	// find target state
	State* target_st = current_diag->getState(tr->to_state);
	if (!target_st) {
	  ERROR("script writer error: transition '%s' from "
		"state '%s' to unknown state '%s'\n",
		tr->name.c_str(),
		current->name.c_str(),
		tr->to_state.c_str());
	}
	
	// run post-actions
	if (current->post_actions.size()) {
	  DBG("running %zd post_actions of state '%s'\n",
	      current->post_actions.size(), current->name.c_str());
	  if (runactions(current->post_actions.begin(), 
			 current->post_actions.end(), 
			 sess, event, event_params, is_consumed)) {
	    break;
	  }
	}
	
	// run transition actions
	if (tr->actions.size()) {
	  DBG("running %zd actions of transition '%s'\n",
	      tr->actions.size(), tr->name.c_str());
	  if (runactions(tr->actions.begin(), 
			 tr->actions.end(), 
			 sess, event, event_params, is_consumed)) {
	    break;
	  }
	}
	
	// go into new state
	if (!target_st) {
	  break;
	}
	DBG("changing to new state '%s'\n", target_st->name.c_str());
	MONITORING_LOG(sess->getLocalTag().c_str(), "dsm_state", target_st->name.c_str());
	current = target_st;
	
	// execute pre-actions
	if (current->pre_actions.size()) {
	  DBG("running %zd pre_actions of state '%s'\n",
	      current->pre_actions.size(), current->name.c_str());
	  if (runactions(current->pre_actions.begin(), 
			 current->pre_actions.end(), 
			 sess, event, event_params, is_consumed)) {
	    break;
	  }
	}

	break;
      }
    }
  } while (!is_consumed);
}

bool DSMStateEngine::callDiag(const string& diag_name, AmSession* sess, 
			   DSMCondition::EventType event,
			   map<string,string>* event_params) {
  if (!current || !current_diag) {
    ERROR("no current diag to push\n");
    return false;
  }
  stack.push_back(std::make_pair(current_diag, current));
  return jumpDiag(diag_name, sess, event, event_params);
}

bool DSMStateEngine::jumpDiag(const string& diag_name, AmSession* sess,
			   DSMCondition::EventType event,
			   map<string,string>* event_params) {
  for (vector<DSMStateDiagram*>::iterator it=
	 diags.begin(); it != diags.end(); it++) {
    if ((*it)->getName() == diag_name) {
      current_diag = *it;
      current = current_diag->getInitialState();
      if (!current) {
	ERROR("diag '%s' does not have initial state.\n",  
	      diag_name.c_str());
	return false;
      }
      MONITORING_LOG2(sess->getLocalTag().c_str(), 
		      "dsm_diag", diag_name.c_str(),
		      "dsm_state", current->name.c_str());

      // execute pre-actions
      DBG("running %zd pre_actions of init state '%s'\n",
	  current->pre_actions.size(), current->name.c_str());
      
      bool is_finished;
      is_finished = true;
      runactions(current->pre_actions.begin(), 
		 current->pre_actions.end(), 
		 sess, event, event_params, is_finished); 

      return true;
    }
  }
  ERROR("diag '%s' not found.\n",  diag_name.c_str());
  return false;
}

bool DSMStateEngine::returnDiag(AmSession* sess) {
  if (stack.empty()) {
    ERROR("returning from empty stack\n");
    return false;
  }
  current_diag = stack.back().first;
  current = stack.back().second;
  stack.pop_back();

  MONITORING_LOG2(sess->getLocalTag().c_str(), 
		  "dsm_diag", current_diag->getName().c_str(),
		  "dsm_state", current->name.c_str());

  DBG("returned to diag '%s' state '%s'\n",
      current_diag->getName().c_str(), 
      current->name.c_str());

  return true;
}

State::State() {
}

State::~State() {
}

DSMTransition::DSMTransition(){
}

DSMTransition::~DSMTransition(){
}

