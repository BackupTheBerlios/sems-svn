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

#ifndef _DSM_H_
#define _DSM_H_

#include "AmApi.h"

#include "AmPromptCollection.h"

#include "DSMStateEngine.h"
#include "DSMStateDiagramCollection.h"
#include "DSMSession.h"
#include "DSMChartReader.h"

#include <string>
using std::string;

#include <memory>

#include <set>

enum MonSelectType {
  MonSelect_NONE, 
  MonSelect_RURI, 
  MonSelect_TO, 
  MonSelect_FROM, 
  MonSelect_PAI
}; 

class DSMCall;
class DSMModule;
/** \brief Factory for announcement sessions */
class DSMFactory
  : public AmSessionFactory,
    public AmDynInvoke,
    public AmDynInvokeFactory,
    public AmEventQueueInterface
{
  AmPromptCollection prompts;
  AmMutex main_diags_mut;

  std::set<DSMStateDiagramCollection*> old_diags;

  static bool DebugDSM;

  static string InboundStartDiag;
  static string OutboundStartDiag;

  DSMScriptConfig MainScriptConfig;
  map<string, DSMScriptConfig> ScriptConfigs;
  AmMutex ScriptConfigs_mut;

#ifdef USE_MONITORING
  static MonSelectType MonSelectCaller;
  static MonSelectType MonSelectCallee;
#endif // USE_MONITORING

  static DSMFactory* _instance;
  DSMFactory(const string& _app_name);
  ~DSMFactory();
  bool loaded;

  int preloadModules(AmConfigReader& cfg, string& res, const string& ModPath);
  bool loadConfig(const string& conf_file_name, const string& conf_name, 
		  bool live_reload, DSMStateDiagramCollection* m_diags);
  bool loadDiags(AmConfigReader& cfg, DSMStateDiagramCollection* m_diags);
  bool registerApps(AmConfigReader& cfg, DSMStateDiagramCollection* m_diags, 
		    vector<string>& register_names /* out */);
  bool loadPromptSets(AmConfigReader& cfg);
  bool loadPrompts(AmConfigReader& cfg);
  bool hasDSM(const string& dsm_name, const string& conf_name);

  map<string, AmPromptCollection*> prompt_sets; 
  void prepareSession(DSMCall* s);
  void addVariables(DSMCall* s, const string& prefix,
		    map<string, string>& vars);
  void addParams(DSMCall* s, const string& hdrs);

  DSMChartReader preload_reader;

  void listDSMs(const AmArg& args, AmArg& ret);
  void hasDSM(const AmArg& args, AmArg& ret);
  void reloadDSMs(const AmArg& args, AmArg& ret);
  void preloadModules(const AmArg& args, AmArg& ret);
  void preloadModule(const AmArg& args, AmArg& ret);
  void loadDSM(const AmArg& args, AmArg& ret);
  void loadDSMWithPaths(const AmArg& args, AmArg& ret);
  void registerApplication(const AmArg& args, AmArg& ret);
  void loadConfig(const AmArg& args, AmArg& ret);

public:
  static DSMFactory* instance();

#ifdef USE_MONITORING
  static bool MonitoringFullCallgraph;
  static bool MonitoringFullTransitions;
#endif // USE_MONITORING


  int onLoad();
  AmSession* onInvite(const AmSipRequest& req);
  AmSession* onInvite(const AmSipRequest& req,
		      AmArg& session_params);
  // DI
  // DI factory
  AmDynInvoke* getInstance() { return instance(); }
  // DI API
  void invoke(const string& method, 
	      const AmArg& args, AmArg& ret);

  void postEvent(AmEvent* e);

};

#endif
// Local Variables:
// mode:C++
// End:

