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
#include "DSM.h"
#include "AmConfig.h"
#include "AmUtils.h"
#include "AmPlugIn.h"
#include "log.h"
#include "AmConfigReader.h"
#include "DSMDialog.h"
#include "DSMChartReader.h"

#include <string>
#include <fstream>


#define MOD_NAME "dsm"

// session creator export 
extern "C" void* session_factory_create() {
  return DSMFactory::instance();
}

DSMFactory* DSMFactory::_instance=0;

DSMFactory* DSMFactory::instance()
{
  if(_instance == NULL)
    _instance = new DSMFactory(MOD_NAME); 
  return _instance;
}

string DSMFactory::InboundStartDiag;
string DSMFactory::OutboundStartDiag;
map<string, string> DSMFactory::config;
bool DSMFactory::RunInviteEvent;

DSMFactory::DSMFactory(const string& _app_name)
  : AmSessionFactory(_app_name),
    loaded(false)
{
}

DSMFactory::~DSMFactory() {
  for (map<string, AmPromptCollection*>::iterator it=
	 prompt_sets.begin(); it != prompt_sets.end(); it++)
    delete it->second;
}

int DSMFactory::onLoad()
{
  if (loaded)
    return 0;
  loaded = true;

  AmConfigReader cfg;
  if(cfg.loadFile(AmConfig::ModConfigPath + string(MOD_NAME ".conf")))
    return -1;
 
  // get application specific global parameters
  configureModule(cfg);
  
  vector<string> prompts_files = 
    explode(cfg.getParameter("load_prompts"), ",");
  for (vector<string>::iterator it=
	 prompts_files.begin(); it != prompts_files.end(); it++) {
    DBG("loading prompts from '%s'\n", it->c_str());
    std::ifstream ifs(it->c_str());
    string s;
    while (ifs.good() && !ifs.eof()) {
      getline(ifs, s);
      if (s.length() && s.find_first_not_of(" \t")!= string::npos &&
	  s[s.find_first_not_of(" \t")] != '#') {
        vector<string> p=explode(s, "=");
	if (p.size()==2) {
	  prompts.setPrompt(p[0], p[1], MOD_NAME);
	  DBG("added prompt '%s' as '%s'\n", 
	      p[0].c_str(), p[1].c_str());
	}
      }
    }
  }

  string prompt_sets_path = cfg.getParameter("prompts_sets_path");

  vector<string> prompt_sets_names = 
    explode(cfg.getParameter("load_prompts_sets"), ",");
  for (vector<string>::iterator it=
	 prompt_sets_names.begin(); it != prompt_sets_names.end(); it++) {
    string fname = prompt_sets_path.empty() ? "": prompt_sets_path + "/";
    fname += *it;
    DBG("loading prompts for '%s' (file '%s')\n", it->c_str(), fname.c_str());
    std::ifstream ifs(fname.c_str());
    string s;
    if (!ifs.good()) {
      WARN("prompts set file '%s' could not be read\n", fname.c_str());
    }
    AmPromptCollection* pc = new AmPromptCollection();
    while (ifs.good() && !ifs.eof()) {
      getline(ifs, s);
      if (s.length() && s.find_first_not_of(" \t")!= string::npos &&
	  s[s.find_first_not_of(" \t")] != '#') {
        vector<string> p=explode(s, "=");
	if (p.size()==2) {
	  pc->setPrompt(p[0], p[1], MOD_NAME);
	  DBG("set '%s' added prompt '%s' as '%s'\n", 
	      it->c_str(), p[0].c_str(), p[1].c_str());
	}
      }
    }
    prompt_sets[*it] = pc;
  }

  string DiagPath = cfg.getParameter("diag_path");
  if (DiagPath.length() && DiagPath[DiagPath.length()-1] != '/')
    DiagPath += '/';

  string ModPath = cfg.getParameter("mod_path");

  string preload_mods = cfg.getParameter("preload_mods");
  vector<string> preload_names = explode(preload_mods, ",");
  if (preload_names.size()) {
    DSMChartReader reader;
    for (vector<string>::iterator it=
	   preload_names.begin(); it != preload_names.end(); it++) {
      DBG("preloading '%s'...\n", it->c_str());
      if (!reader.importModule("import("+*it+")", ModPath)) {
	ERROR("importing module '%s' for preload\n", it->c_str());
	return -1;
      }
      DSMModule* last_loaded = reader.mods.back();
      if (last_loaded) {
 	if (last_loaded->preload()) {
 	  DBG("Error while preloading '%s'\n", it->c_str());
 	  return -1;
 	}
      }
    }
  }
  // TODO: pass preloaded mods to chart reader

  string LoadDiags = cfg.getParameter("load_diags");
  vector<string> diags_names = explode(LoadDiags, ",");
  for (vector<string>::iterator it=
	 diags_names.begin(); it != diags_names.end(); it++) {
    if (!diags.loadFile(DiagPath+*it+".dsm", *it, ModPath)) {
      ERROR("loading %s from %s\n", 
	    it->c_str(), (DiagPath+*it+".dsm").c_str());
      return -1;
    }
  }

  InboundStartDiag = cfg.getParameter("inbound_start_diag");
  if (InboundStartDiag.empty()) {
    INFO("no 'inbound_start_diag' set in config. inbound calls disabled.\n");
  }
  OutboundStartDiag = cfg.getParameter("outbound_start_diag");
  if (OutboundStartDiag.empty()) {
    INFO("no 'outbound_start_diag' set in config. outbound calls disabled.\n");
  }

  for (std::map<string,string>::const_iterator it = 
	 cfg.begin(); it != cfg.end(); it++)
    config[it->first] = it->second;

  RunInviteEvent = cfg.getParameter("run_invite_event")=="yes";

  return 0;
}

void DSMFactory::prepareSession(DSMDialog* s) {
  s->setPromptSets(prompt_sets);
  for (map<string, string>::iterator it = 
	 config.begin(); it != config.end(); it++)
    s->var["config."+it->first] = it->second;
}

AmSession* DSMFactory::onInvite(const AmSipRequest& req)
{
  if (InboundStartDiag.empty()) {
    ERROR("no inbound calls allowed\n");
    throw AmSession::Exception(488, "Not Acceptable Here");
  }
  DSMDialog* s = new DSMDialog(prompts, diags, InboundStartDiag, NULL);
  prepareSession(s);
  return s;
}

AmSession* DSMFactory::onInvite(const AmSipRequest& req,
				AmArg& session_params)
{
  if (OutboundStartDiag.empty()) {
    ERROR("no outbound calls allowed\n");
    throw AmSession::Exception(488, "Not Acceptable Here");
  }

  UACAuthCred* cred = NULL;
  if (session_params.getType() == AmArg::AObject) {
    ArgObject* cred_obj = session_params.asObject();
    if (cred_obj)
      cred = dynamic_cast<UACAuthCred*>(cred_obj);
  }

  DSMDialog* s = new DSMDialog(prompts, diags, OutboundStartDiag, cred); 
  prepareSession(s);

  if (NULL == cred) {
    WARN("discarding unknown session parameters.\n");
  } else {
    AmSessionEventHandlerFactory* uac_auth_f = 
      AmPlugIn::instance()->getFactory4Seh("uac_auth");
    if (uac_auth_f != NULL) {
      DBG("UAC Auth enabled for new DSM session.\n");
      AmSessionEventHandler* h = uac_auth_f->getHandler(s);
      if (h != NULL )
	s->addHandler(h);
    } else {
      ERROR("uac_auth interface not accessible. "
	    "Load uac_auth for authenticated dialout.\n");
    }		
  }

  return s;
}

