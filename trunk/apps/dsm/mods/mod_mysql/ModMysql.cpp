/*
 * $Id$
 *
 * Copyright (C) 2009 TelTech Systems Inc.
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

#include "ModMysql.h"
#include "log.h"
#include "AmUtils.h"

#include "DSMSession.h"
#include "AmSession.h"

SC_EXPORT(SCMysqlModule);

SCMysqlModule::SCMysqlModule() {
}

SCMysqlModule::~SCMysqlModule() {
}


DSMAction* SCMysqlModule::getAction(const string& from_str) {
  string cmd;
  string params;
  splitCmd(from_str, cmd, params);

  DEF_CMD("mysql.connect",            SCMyConnectAction);
  DEF_CMD("mysql.disconnect",         SCMyDisconnectAction);
  DEF_CMD("mysql.execute",            SCMyExecuteAction);
  DEF_CMD("mysql.query",              SCMyQueryAction);
  DEF_CMD("mysql.queryGetResult",     SCMyQueryGetResultAction);
  DEF_CMD("mysql.getResult",          SCMyGetResultAction);
  DEF_CMD("mysql.getClientVersion",   SCMyGetClientVersion);
  DEF_CMD("mysql.resolveQueryParams", SCMyResolveQueryParams);
  DEF_CMD("mysql.saveResult",         SCMySaveResultAction);
  DEF_CMD("mysql.useResult",          SCMyUseResultAction);

  return NULL;
}

DSMCondition* SCMysqlModule::getCondition(const string& from_str) {
  string cmd;
  string params;
  splitCmd(from_str, cmd, params);

  if (cmd == "mysql.hasResult") {
    return new MyHasResultCondition(params, false);
  }

  if (cmd == "mysql.connected") {
    return new MyConnectedCondition(params, true);
  }

  return NULL;
}

mysqlpp::Connection* getMyDSMSessionConnection(DSMSession* sc_sess) {
  if (sc_sess->avar.find(MY_AKEY_CONNECTION) == sc_sess->avar.end()) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);
    return NULL;
  }
  ArgObject* ao = NULL; mysqlpp::Connection* res = NULL;
  try {
    if (!isArgAObject(sc_sess->avar[MY_AKEY_CONNECTION])) {
      sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);
      return NULL;
    }
    ao = sc_sess->avar[MY_AKEY_CONNECTION].asObject();
  } catch (...){
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);
    return NULL;
  }

  if (NULL == ao || NULL == (res = dynamic_cast<mysqlpp::Connection*>(ao))) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);
    return NULL;
  }
  return res;
}

mysqlpp::StoreQueryResult* getMyDSMQueryResult(DSMSession* sc_sess) {
  if (sc_sess->avar.find(MY_AKEY_RESULT) == sc_sess->avar.end()) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_NORESULT);
    return NULL;
  }
  ArgObject* ao = NULL; mysqlpp::StoreQueryResult* res = NULL;
  try {
    assertArgAObject(sc_sess->avar[MY_AKEY_RESULT]);
    ao = sc_sess->avar[MY_AKEY_RESULT].asObject();
  } catch (...){
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_NORESULT);
    return NULL;
  }

  if (NULL == ao || NULL == (res = dynamic_cast<mysqlpp::StoreQueryResult*>(ao))) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_NORESULT);
    return NULL;
  }
  return res;
}

string replaceQueryParams(const string& q, DSMSession* sc_sess, 
			  map<string,string>* event_params) {
  string res = q;
  size_t repl_pos = 0;
  while (repl_pos<res.length()) {
    size_t rstart = res.find_first_of("$#", repl_pos);
    repl_pos = rstart+1;
    if (rstart == string::npos) 
      break;
    if (rstart && res[rstart-1] == '\\') // escaped
      continue;
    
    size_t rend = res.find_first_of(" ,()$#\t;'\"", rstart+1);
    if (rend==string::npos)
      rend = res.length();
    switch(res[rstart]) {
    case '$': 
      res.replace(rstart, rend-rstart, 
		  sc_sess->var[res.substr(rstart+1, rend-rstart-1)]); break;
    case '#':
      if (NULL!=event_params) {
	res.replace(rstart, rend-rstart, 
		    (*event_params)[res.substr(rstart+1, rend-rstart-1)]); break;
      }
    default: break;
    }
  }
  return res;
}

string str_between(const string s, char b, char e) {
  size_t pos1 = s.find(b);
  if (b == '\0' || pos1 == string::npos)
    pos1 = 0;
  else
    pos1++;
  size_t pos2 = s.find(e, pos1);
  if (e == '\0' || pos2 == string::npos)
    pos2 = s.length();
  return s.substr(pos1, pos2-pos1);
}

EXEC_ACTION_START(SCMyConnectAction) {
  string db_url = arg.length()?arg:sc_sess->var["config.db_url"];
  if (db_url.empty() || db_url.length() < 11 || db_url.substr(0, 8) != "mysql://") {
    ERROR("missing correct db_url config or connect parameter\n");
    sc_sess->SET_ERRNO(DSM_ERRNO_UNKNOWN_ARG);
    return false;
  }
  // split url
  db_url = db_url.substr(8); 
  string db_user = str_between(db_url, '\0', ':');
  string db_pwd  = str_between(db_url,  ':', '@');
  string db_host = str_between(db_url,  '@', '/');
  string db_db   = str_between(db_url,  '/', '\0');

  DSMMyConnection* conn = NULL;
  try {
    conn = new DSMMyConnection(db_db.c_str(), db_host.c_str(), db_user.c_str(), db_pwd.c_str());

    // save connection for later use
    AmArg c_arg;
    c_arg.setBorrowedPointer(conn);
    sc_sess->avar[MY_AKEY_CONNECTION] = c_arg;
    // for garbage collection
    sc_sess->transferOwnership(conn);
    sc_sess->SET_ERRNO(DSM_ERRNO_OK);    
  } catch (const mysqlpp::ConnectionFailed& e) {
    ERROR("DB connection failed with error %d: '%s'\n",e.errnum(), e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);    
    sc_sess->var["db.errno"] = int2str(e.errnum());
    sc_sess->var["db.ereason"] = e.what();
  } catch (const mysqlpp::Exception& e) {
    ERROR("DB connection failed: '%s'\n", e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);    
    sc_sess->var["db.ereason"] = e.what();
  }

} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyDisconnectAction) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;

  try {
    conn->disconnect();
    // connection object might be reused - but its safer to create a new one
    sc_sess->avar[MY_AKEY_CONNECTION] = AmArg();
    sc_sess->SET_ERRNO(DSM_ERRNO_OK);
  } catch (const mysqlpp::Exception& e) {
    ERROR("DB disconnect failed: '%s'\n", e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_CONNECTION);
    sc_sess->var["db.ereason"] = e.what();
  }
} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyResolveQueryParams) {
  sc_sess->var["db.qstr"] = 
    replaceQueryParams(arg, sc_sess, event_params);
} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyExecuteAction) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;
  string qstr = replaceQueryParams(arg, sc_sess, event_params);

  try {
    mysqlpp::Query query = conn->query(qstr.c_str());
    mysqlpp::SimpleResult res = query.execute();
    if (res) {
      sc_sess->SET_ERRNO(DSM_ERRNO_OK);
      sc_sess->var["db.rows"] = int2str(res.rows());
      sc_sess->var["db.info"] = res.info();
      sc_sess->var["db.insert_id"] = int2str(res.insert_id());
    } else {
      sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);
      sc_sess->var["db.info"] = res.info();
    }

  } catch (const mysqlpp::Exception& e) {
    ERROR("DB query '%s' failed: '%s'\n", 
	  qstr.c_str(), e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);    
    sc_sess->var["db.ereason"] = e.what();
  }
} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyQueryAction) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;
  string qstr = replaceQueryParams(arg, sc_sess, event_params);

  try {
    mysqlpp::Query query = conn->query(qstr.c_str());
    mysqlpp::StoreQueryResult res = query.store();    
    if (res) {
      // MySQL++ does not allow working with pointers here, so copy construct it
      DSMMyStoreQueryResult* m_res = new DSMMyStoreQueryResult(res);

      // save result for later use
      AmArg c_arg;
      c_arg.setBorrowedPointer(m_res);
      sc_sess->avar[MY_AKEY_RESULT] = c_arg;

      // for garbage collection
      sc_sess->transferOwnership(m_res);

      sc_sess->SET_ERRNO(DSM_ERRNO_OK);    
      sc_sess->var["db.rows"] = int2str(res.num_rows());
    } else {
      sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);
    }
  } catch (const mysqlpp::Exception& e) {
    ERROR("DB query '%s' failed: '%s'\n", 
	  qstr.c_str(), e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);    
    sc_sess->var["db.ereason"] = e.what();
  }
} EXEC_ACTION_END;

CONST_ACTION_2P(SCMyQueryGetResultAction, ',', true);
EXEC_ACTION_START(SCMyQueryGetResultAction) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;
  string qstr = replaceQueryParams(par1, sc_sess, event_params);

  try {
    mysqlpp::Query query = conn->query(qstr.c_str());
    mysqlpp::StoreQueryResult res = query.store();    
    if (res) {
      size_t rowindex_i = 0;
      string rowindex = resolveVars(par2, sess, sc_sess, event_params);
      if (rowindex.length()) {
	if (str2i(rowindex, rowindex_i)) {
	  ERROR("row index '%s' not understood\n", rowindex.c_str());
	  sc_sess->SET_ERRNO(DSM_ERRNO_UNKNOWN_ARG);
	  return false;
	}
      }
      if (res.size() <= rowindex_i) {
	sc_sess->SET_ERRNO(DSM_ERRNO_MY_NOROW);
	return false;
      }

      // get all columns
      for (size_t i = 0; i < res.field_names()->size(); i++) {
	sc_sess->var[res.field_name(i)] = 
	  (string)res[rowindex_i][res.field_name(i).c_str()];
      }

      sc_sess->SET_ERRNO(DSM_ERRNO_OK);    
      sc_sess->var["db.rows"] = int2str(res.num_rows());
    } else {
      sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);
    }
  } catch (const mysqlpp::Exception& e) {
    ERROR("DB query '%s' failed: '%s'\n", 
	  qstr.c_str(), e.what());
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_QUERY);    
    sc_sess->var["db.ereason"] = e.what();
  }
} EXEC_ACTION_END;

CONST_ACTION_2P(SCMyGetResultAction, ',', true);
EXEC_ACTION_START(SCMyGetResultAction) {
  mysqlpp::StoreQueryResult* res = getMyDSMQueryResult(sc_sess);
  if (NULL == res)
    return false;

  if (!res) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_NORESULT);
    return false;
  }

  size_t rowindex_i = 0;
  string rowindex = resolveVars(par1, sess, sc_sess, event_params);
  string colname  = resolveVars(par2, sess, sc_sess, event_params);

  if (rowindex.length()) {
    if (str2i(rowindex, rowindex_i)) {
      ERROR("row index '%s' not understood\n", rowindex.c_str());
      sc_sess->SET_ERRNO(DSM_ERRNO_UNKNOWN_ARG);
      return false;
    }
  }

  if (res->size() <= rowindex_i) {
    sc_sess->SET_ERRNO(DSM_ERRNO_MY_NOROW);
    return false;
  }
  DBG("rowindex_i = %d\n", rowindex_i);
  if (colname.length()) {
    // get only this column
    try {      
      sc_sess->var[colname] = 
	(string)(*res)[(int)rowindex_i][colname.c_str()];
    } catch (const mysqlpp::BadFieldName& e) {
      sc_sess->SET_ERRNO(DSM_ERRNO_MY_NOCOLUMN);
      return false;
    }
  } else {
    // get all columns
    for (size_t i = 0; i < res->field_names()->size(); i++) {
      sc_sess->var[res->field_name(i)] = 
	(string)(*res)[rowindex_i][res->field_name(i).c_str()];
    }
  }
} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyGetClientVersion) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;

  sc_sess->var[resolveVars(arg, sess, sc_sess, event_params)] = 
    conn->client_version();
} EXEC_ACTION_END;

MATCH_CONDITION_START(MyHasResultCondition) {
  mysqlpp::StoreQueryResult* res = getMyDSMQueryResult(sc_sess);
  if (NULL == res)
    return false;

  if (!res || !(res->size())) {
    return false;
  }

  return true;
} MATCH_CONDITION_END;

MATCH_CONDITION_START(MyConnectedCondition) {
  mysqlpp::Connection* conn = 
    getMyDSMSessionConnection(sc_sess);
  if (NULL == conn) 
    return false;

  return conn->connected();
} MATCH_CONDITION_END;


EXEC_ACTION_START(SCMySaveResultAction) {
  sc_sess->avar[resolveVars(arg, sess, sc_sess, event_params)] = sc_sess->avar[MY_AKEY_RESULT];
} EXEC_ACTION_END;

EXEC_ACTION_START(SCMyUseResultAction) {
  sc_sess->avar[MY_AKEY_RESULT] = sc_sess->avar[resolveVars(arg, sess, sc_sess, event_params)];
} EXEC_ACTION_END;
