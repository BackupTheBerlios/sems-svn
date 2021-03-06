/*
 * $Id: AmConfig.cpp,v 1.18 2004/08/13 13:50:38 rco Exp $
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include "AmConfig.h"
#include "sems.h"
#include "log.h"
#include "AmConfigReader.h"

string       AmConfig::ConfigurationFile       = CONFIG_FILE;
string       AmConfig::ModConfigPath           = MOD_CFG_PATH;
string       AmConfig::SocketName              = "";
string       AmConfig::ReplySocketName         = "";
string       AmConfig::SerSocketName           = "";
string       AmConfig::SendMethod              = SEND_METHOD;
string       AmConfig::SmtpServerAddress       = SMTP_ADDRESS_IP;
unsigned int AmConfig::SmtpServerPort          = SMTP_PORT;
string       AmConfig::PlugInPath              = PLUG_IN_PATH;
int          AmConfig::DaemonMode              = DEFAULT_DAEMON_MODE;
string       AmConfig::LocalIP                 = "";
string       AmConfig::PrefixSep               = PREFIX_SEPARATOR;
int          AmConfig::RtpLowPort              = RTP_LOWPORT;
int          AmConfig::RtpHighPort             = RTP_HIGHPORT;
int          AmConfig::MediaProcessorThreads   = NUM_MEDIA_PROCESSORS;

AmSessionTimerConfig AmConfig::defaultSessionTimerConfig;

int AmConfig::setSmtpPort(const string& port) 
{
    if(sscanf(port.c_str(),"%u",&AmConfig::SmtpServerPort) != 1) {
	return 0;
    }
    return 1;
}

int AmConfig::setRtpLowPort(const string& port)
{
    if(sscanf(port.c_str(),"%i",&AmConfig::RtpLowPort) != 1) {
	return 0;
    }
    return 1;
}

int AmConfig::setRtpHighPort(const string& port)
{
    if(sscanf(port.c_str(),"%i",&AmConfig::RtpHighPort) != 1) {
	return 0;
    }
    return 1;
}

int AmConfig::setLoglevel(const string& ll) {
    
    if(sscanf(ll.c_str(),"%u",&log_level) != 1) {
	return 0;
    }
    return 1;
}

int AmConfig::setFork(const string& fork) {
	if ( strcasecmp(fork.c_str(), "yes") == 0 ) {
		DaemonMode = 1;
	} else if ( strcasecmp(fork.c_str(), "no") == 0 ) {
		DaemonMode = 0;
	} else {
		return 0;
	}	
	return 1;
}		

int AmConfig::setStderr(const string& s) {
	if ( strcasecmp(s.c_str(), "yes") == 0 ) {
		log_stderr = 1;
		AmConfig::DaemonMode = 0;
	} else if ( strcasecmp(s.c_str(), "no") == 0 ) {
		log_stderr = 0;
	} else {
		return 0;
	}	
	return 1;
}		

int AmConfig::setMediaProcessorThreads(const string& th) {
    if(sscanf(th.c_str(),"%u",&MediaProcessorThreads) != 1) {
	return 0;
    }
    return 1;
}

int AmConfig::readConfiguration()
{
    AmConfigReader cfg;

    if(cfg.loadFile(ConfigurationFile.c_str())){
	ERROR("while loading main configuration file\n");
	return -1;
    }
       
    // take values from global configuration file
    // they will be overwritten by command line args

    // plugin_config_path
    ModConfigPath = cfg.getParameter("plugin_config_path",ModConfigPath);


    // smtp_server
    SmtpServerAddress = cfg.getParameter("smtp_server",SmtpServerAddress);

    // smtp_port
    if(cfg.hasParameter("smtp_port")){
	if(!setSmtpPort(cfg.getParameter("smtp_port").c_str())){
	    ERROR("invalid smtp port specified\n");
	    return -1;
	}
    }

    // local_ip
    LocalIP = cfg.getParameter("listen");
DBG("RvR :- Local IP set to %s\n", LocalIP.c_str());

    // socket_name
    SocketName = cfg.getParameter("socket_name");

    // reply socket_name
    ReplySocketName = cfg.getParameter("reply_socket_name");

    // ser_fifo_name
    SerSocketName = cfg.getParameter("ser_socket_name");

    // plugin_path
    PlugInPath = cfg.getParameter("plugin_path");

    // log_level
    if(cfg.hasParameter("loglevel")){
	if(!setLoglevel(cfg.getParameter("loglevel"))){
	    ERROR("invalid log level specified\n");
	    return -1;
	}
    }

    // fork 
    if(cfg.hasParameter("fork")){
	if(!setFork(cfg.getParameter("fork"))){
	    ERROR("invalid fork value specified,"
		  " valid are only yes or no\n");
	    return -1;
	}
    }

    // stderr 
    if(cfg.hasParameter("stderr")){
	if(!setStderr(cfg.getParameter("stderr"))){
	    ERROR("invalid stderr value specified,"
		  " valid are only yes or no\n");
	    return -1;
	}
    }

    // user_prefix_separator
    PrefixSep = cfg.getParameter("user_prefix_separator",PrefixSep);

    // rtp_low_port
    if(cfg.hasParameter("rtp_low_port")){
	if(!setRtpLowPort(cfg.getParameter("rtp_low_port"))){
	    ERROR("invalid rtp low port specified\n");
	    return -1;
	}
    }

    // rtp_high_port
    if(cfg.hasParameter("rtp_high_port")){
	if(!setRtpHighPort(cfg.getParameter("rtp_high_port"))){
	    ERROR("invalid rtp high port specified\n");
	    return -1;
	}
    }

    if(cfg.hasParameter("media_processor_threads")){
	if(!setMediaProcessorThreads(cfg.getParameter("media_processor_threads"))){
	    ERROR("invalid media_processor_threads value specified");
	    return -1;
	}
    }

    return defaultSessionTimerConfig.readFromConfig(cfg);
}	

int AmConfig::init()
{
    return 0;
}

AmSessionTimerConfig::AmSessionTimerConfig()
: EnableSessionTimer(DEFAULT_ENABLE_SESSION_TIMER), 
  SessionExpires(SESSION_EXPIRES), 
  MinimumTimer(MINIMUM_TIMER)
{

}
AmSessionTimerConfig::~AmSessionTimerConfig() 
{
}

int AmSessionTimerConfig::readFromConfig(AmConfigReader& cfg)
{
    /* Session Timer: -ssa */
    // enable_session_timer
    if(cfg.hasParameter("enable_session_timer")){
	if(!setEnableSessionTimer(cfg.getParameter("enable_session_timer"))){
	    ERROR("invalid enable_session_timer specified\n");
	    return -1;
	}
    }

    // session_expires
    if(cfg.hasParameter("session_expires")){
	if(!setSessionExpires(cfg.getParameter("session_expires"))){
	    ERROR("invalid session_expires specified\n");
	    return -1;
	}
    }

    // minimum_timer
    if(cfg.hasParameter("minimum_timer")){
	if(!setMinimumTimer(cfg.getParameter("minimum_timer"))){
	    ERROR("invalid minimum_timer specified\n");
	    return -1;
	}
    }
    /* end Session Timer */
    return 0;
}

int AmSessionTimerConfig::setEnableSessionTimer(const string& enable) {
	if ( strcasecmp(enable.c_str(), "yes") == 0 ) {
		EnableSessionTimer = 1;
	} else if ( strcasecmp(enable.c_str(), "no") == 0 ) {
		EnableSessionTimer = 0;
	} else {
		return 0;
	}	
	return 1;
}		

int AmSessionTimerConfig::setSessionExpires(const string& se) {
  if(sscanf(se.c_str(),"%u",&SessionExpires) != 1) {
    return 0;
  }
  DBG("setSessionExpires(%i)\n",SessionExpires);
  return 1;
} 

int AmSessionTimerConfig::setMinimumTimer(const string& minse) {
  if(sscanf(minse.c_str(),"%u",&MinimumTimer) != 1) {
    return 0;
  }
  DBG("setMinimumTimer(%i)\n",MinimumTimer);
  return 1;
}
/* end Session Timer: -ssa */


