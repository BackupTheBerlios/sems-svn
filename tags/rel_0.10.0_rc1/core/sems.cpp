/*
 * $Id: sems.cpp,v 1.6.2.4 2005/04/13 10:57:09 rco Exp $
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

#include "sems.h"
#include "AmUtils.h"
#include "AmConfig.h"
//#include "SemsConfiguration.h"
#include "AmPlugIn.h"
#include "AmSessionContainer.h"
#include "AmMail.h"
#include "AmServer.h"
#include "AmCtrlInterface.h"
#include "AmInterfaceHandler.h"
#include "AmMediaProcessor.h"
#include "AmIcmpWatcher.h"
#include "AmRtpReceiver.h"
//#include "AmSessionTimer.h"

#include "log.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include <grp.h>
#include <pwd.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
using std::string;
using std::pair;
using std::make_pair;

#ifndef sighandler_t
typedef void (*sighandler_t) (int);
#endif

const char* progname;
string pid_file;
int    main_pid=0;
int    child_pid=0;
int    is_main=1;


static int parse_args(int argc, char* argv[], const string& flags,
		      const string& options, map<char,string>& args);

static void print_usage(char* progname);

static string getLocalIP(const string& dev_name);

int sig_flag;
int deamon_mode=1;

static void sig_usr_un(int signo)
{
    WARN("signal %d received\n", signo);
    
    if(!main_pid || (main_pid == getpid())){

	unlink(pid_file.c_str());
	INFO("finished\n");
	exit(0);
    }

    return;
}

int set_sighandler(sighandler_t sig_usr)
{
    if (signal(SIGINT, sig_usr) == SIG_ERR ) {
	ERROR("no SIGINT signal handler can be installed\n");
	return -1;
    }
    
    if (signal(SIGPIPE, sig_usr) == SIG_ERR ) {
	ERROR("no SIGPIPE signal handler can be installed\n");
	return -1;
    }

    if (signal(SIGCHLD , sig_usr)  == SIG_ERR ) {
	ERROR("no SIGCHLD signal handler can be installed\n");
	return -1;
    }

    if (signal(SIGTERM , sig_usr)  == SIG_ERR ) {
	ERROR("no SIGTERM signal handler can be installed\n");
	return -1;
    }

    if (signal(SIGHUP , sig_usr)  == SIG_ERR ) {
	ERROR("no SIGHUP signal handler can be installed\n");
	return -1;
    }

    return 0;
}

int write_pid_file()
{
    FILE* fpid = fopen(pid_file.c_str(),"w");

    if(fpid){
	string spid = int2str((int)getpid());
	fwrite(spid.c_str(),spid.length(),1,fpid);
	fclose(fpid);
	return 0;
    }
    else {
	ERROR("could not write pid file '%s': %s\n",
	      pid_file.c_str(),strerror(errno));
    }

    return -1;
}

// returns 0 if OK
static int use_args(char* progname, map<char,string>& args)
{
     for(map<char,string>::iterator it = args.begin(); 
	 it != args.end(); ++it){
	 
	 if(it->second.empty())
	     continue;
	 
         switch( it->first ){

	 case 'h':
	     print_usage(progname);
	     exit(0);
	     break;

	 case 'E':
	     AmConfig::setStderr("yes");
	     continue;

	 case 'd':
	     //if(AmConfig::LocalIP.empty())
	     // AmConfig::LocalIP = getLocalIP(it->second);
	     AmConfig::LocalIP = it->second;
	     break;
	     
	 case 'x':
	     AmConfig::PlugInPath = it->second;
	     break;
	     
	 case 'D':
	     if(sscanf(it->second.c_str(),"%u",&log_level) != 1){
		 fprintf(stderr,"%s: bad log level number: %s\n",progname,it->second.c_str());
		 return -1;
	     }
	     break;

	 case 'f':
	 case 'P':
	 case 'u':
	 case 'g':
	     // already processed, ignore it here
	     break;
	     
	 default:
	     ERROR("%s: bad parameter '-%c'\n",progname,it->first);
	     return -1;
	 }
     }
     return 0;
}

AmFifoCtrlInterface* init_fifo(const string& path)
{
    AmFifoCtrlInterface* ctrl = new AmFifoCtrlInterface();
    
    if(ctrl->createFifo(path))
	goto fifo_error;

    // avoid the FIFO reaches EOF...
    if((child_pid=fork())>0){
	int fifo_write=0;
        if((fifo_write=open(path.c_str(), O_WRONLY)) == -1){
	    ERROR("while opening fifo `%s': %s\n",
		  path.c_str(),strerror(errno));
	    kill(child_pid,SIGTERM);
	}
	waitpid(child_pid,0,0);
	if(fifo_write != -1)
	    ::close(fifo_write);

	exit(0);
    }
    // the main process is waiting 
    // for that child to terminate
    is_main = 0; 
	
    if(ctrl->init(path))
	goto fifo_error;

    goto fifo_end;

 fifo_error:
    delete ctrl;
    ctrl = 0;

 fifo_end:
    return ctrl;
}



int main(int argc, char* argv[])
{
    map<char,string> args;

    if(parse_args(argc, argv, "hE","ugPfiodxD", args)){
	print_usage(argv[0]);
	return -1;
    }

    if(args.find('h') != args.end()){
	print_usage(argv[0]);
	return 0;
    }

    init_log();

    AmConfig::setStderr("yes");
    AmConfig::setLoglevel("1");

    map<char,string>::iterator cfg_arg;
    if( (cfg_arg = args.find('f')) != args.end() )
	AmConfig::ConfigurationFile = cfg_arg->second;

//     if(!semsConfig.reloadFile(AmConfig::ConfigurationFile.c_str()))
// 	return -1;

    AmConfig::readConfiguration();
//     semsConfig.warnUnknownParams();
    
    if(use_args(argv[0],args)){
	print_usage(argv[0]);
	return -1;
    }

    //if(AmConfig::LocalIP.empty())
    AmConfig::LocalIP = getLocalIP(AmConfig::LocalIP);

    printf( "\n\nConfiguration:\n"
	    "       configuration file:  %s\n"
	    "       Ser's unix socket:   %s\n"
	    "       our unix socket:     %s\n"
	    "       reply unix socket:   %s\n"
	    "       plug-in path:        %s\n"
	    "       daemon mode:         %i\n"
	    "       local IP:            %s\n"
	    "\n",
	    AmConfig::ConfigurationFile.c_str(),
	    AmConfig::SerSocketName.c_str(),
	    AmConfig::SocketName.c_str(),
	    AmConfig::ReplySocketName.c_str(),
	    AmConfig::PlugInPath.c_str(),
	    AmConfig::DaemonMode,
	    AmConfig::LocalIP.c_str()
	    );

    if(AmConfig::DaemonMode){

	if( (cfg_arg = args.find('g')) != args.end() ){
	    unsigned int gid;
	    if(str2i(cfg_arg->second,gid)){
	        struct group* grnam = getgrnam(cfg_arg->second.c_str());
	        if(grnam != NULL){
		    gid = grnam->gr_gid;
	        }
	        else{
		    ERROR("could not find group '%s' in the group database\n",
		    	    cfg_arg->second.c_str());
		    return -1;
	        }
	    }
	    if(setgid(gid)<0){
		ERROR("cannot change gid to %i: %s",
		      gid,
		      strerror(errno));
		return -1;
	    }
	}

	if( (cfg_arg = args.find('u')) != args.end() ){
	    unsigned int uid;
	    if(str2i(cfg_arg->second,uid)){
	    	struct passwd* pwnam = getpwnam(cfg_arg->second.c_str());
	    	if(pwnam != NULL){
		    uid = pwnam->pw_uid;
	    	}
	    	else{
		    ERROR("could not find user '%s' in the user database.\n",
		            cfg_arg->second.c_str());
		    return -1;
	        }
	    }
	    if(setuid(uid)<0){
		ERROR("cannot change uid to %i: %s",
		      uid,
		      strerror(errno));
		return -1;
	    }
	}

        /* fork to become!= group leader*/
	int pid;
        if ((pid=fork())<0){
                ERROR("Cannot fork:%s\n", strerror(errno));
                return -1;
        }else if (pid!=0){
                /* parent process => exit*/
                return 0;
        }
        /* become session leader to drop the ctrl. terminal */
        if (setsid()<0){
                ERROR("setsid failed: %s\n",strerror(errno));
        }
        /* fork again to drop group  leadership */
        if ((pid=fork())<0){
                ERROR("Cannot  fork:%s\n", strerror(errno));
                return -1;
        }else if (pid!=0){
                /*parent process => exit */
                return 0;
        }
	
	if( (cfg_arg = args.find('P')) != args.end() ){
	    pid_file = cfg_arg->second;
	    if(write_pid_file()<0)
		return -1;
	}

	/* try to replace stdin, stdout & stderr with /dev/null */
	if (freopen("/dev/null", "r", stdin)==0){
		ERROR("unable to replace stdin with /dev/null: %s\n",
				strerror(errno));
		/* continue, leave it open */
	};
	if (freopen("/dev/null", "w", stdout)==0){
		ERROR("unable to replace stdout with /dev/null: %s\n",
				strerror(errno));
		/* continue, leave it open */
	};
	/* close stderr only if log_stderr=0 */
	if ((!log_stderr) &&(freopen("/dev/null", "w", stderr)==0)){
		ERROR("unable to replace stderr with /dev/null: %s\n",
				strerror(errno));
		/* continue, leave it open */
	};
    }

    main_pid = getpid();

    if(set_sighandler(sig_usr_un))
	return -1;

    if(AmConfig::init())
	return -1;

    DBG("Loading plug-ins\n");
    if(AmPlugIn::instance()->load(AmConfig::PlugInPath))
	return -1;

    DBG("Starting session container\n");
    AmSessionContainer::instance()->start();

    DBG("Starting media processor\n");
    AmMediaProcessor::instance()->init();

    DBG("Starting mailer\n");
    AmMailDeamon::instance()->start();

    DBG("Starting RTP receiver\n");
    AmRtpReceiver::instance()->start();

    //DBG("Starting Session Timer\n");
    //AmSessionTimer::instance()->start();

    //DBG("Starting ICMP watcher\n");
    //AmIcmpWatcher::instance()->start();

    AmUnixCtrlInterface* un_ctrl=0;
    if(!AmConfig::SocketName.empty()){
	un_ctrl = new AmUnixCtrlInterface();
	if(un_ctrl->init(AmConfig::SocketName.c_str())){
	    delete un_ctrl;
	    un_ctrl = 0;
	}
	else {
	    AmServer::instance()->regIface(IfaceDesc(un_ctrl,
						     AmRequestHandler::get()
						     ));

	    AmReplyHandler* rh = AmReplyHandler::get();
	    if(!rh) return -1;
	    
	    AmServer::instance()->regIface(IfaceDesc(rh->getCtrl(),rh));
	}
    }

    if(// fifo_ctrl || 
       un_ctrl){

	AmServer::instance()->run();
    }
    else {
	ERROR("Sems cannot start without a working link to Ser.\n"
	      "Please set in the config file 'socket_name' \n"
	      " parameter and check the log file"
	      " for errors or warnings.\nExiting.\n");
	return -1;
    }

    return 0;
}

static void print_usage(char* progname)
{
    printf(
	   "USAGE: %s [options]\n"
	   "   Options:\n"
	   "       -f config_filename:  sets configuration file to use\n"
	   "       -i fifo_name:        path and file name of our fifo file.\n"
	   "       -o ser_fifo_name:    path and file name of Ser's fifo file.\n"
	   "       -d device:           sets network device for media advertising\n"
	   "       -P pid_file:         write a pid file.\n"
	   "       -u uid:              set user id.\n"
	   "       -g gid:              set group id.\n"
	   "       -x plugin_path:      root path for plugins\n"
	   "       -D log_level:        sets log level (error=0, warning=1, info=2, debug=3).\n"
	   "       -E :                 debug mode: do not fork and log to stderr.\n"
	   "       -h :                 this help screen.\n"
	   "\n"
	   "   Notes:\n"
	   "       * plug-ins are searched in plugin_path/{apps,audio}/*.so.\n",
	   progname
	   );
}

static void getInterfaceList(int sd, vector<pair<string,string> >& if_list)
{
    struct ifconf ifc;
    struct ifreq ifrs[MAX_NET_DEVICES];

    ifc.ifc_len = sizeof(struct ifreq) * MAX_NET_DEVICES;
    ifc.ifc_req = ifrs;
    memset(ifrs,0,ifc.ifc_len);
    
    if(ioctl(sd,SIOCGIFCONF,&ifc)!=0){
	ERROR("getInterfaceList: ioctl: %s",strerror(errno));
	exit(-1);
    }

    int n_dev = ifc.ifc_len / sizeof(struct ifreq);
    for(int i=0; i<n_dev; i++){
	if(ifrs[i].ifr_addr.sa_family==PF_INET){
	    struct sockaddr_in* sa = (struct sockaddr_in*)&ifrs[i].ifr_addr;
	    if_list.push_back(make_pair((char*)ifrs[i].ifr_name,
					inet_ntoa(sa->sin_addr)));
	}
    }
}

static string getLocalIP(const string& dev_name)
{
    //DBG("getLocalIP(%s)\n",dev_name.c_str());

#ifdef SUPPORT_IPV6
    struct sockaddr_storage ss;
    if(inet_aton_v6(dev_name.c_str(),&ss)){
#else
    struct in_addr inp;
    if(inet_aton(dev_name.c_str(),&inp)){
#endif    
	return dev_name;
    }

    int sd = socket(PF_INET,SOCK_DGRAM,0);
    if(sd == -1){
	ERROR("setLocalIP: socket: %s\n",strerror(errno));
	exit(-1);
    }	

    struct ifreq ifr;
    vector<pair<string,string> > if_list;

    if(dev_name.empty())
	getInterfaceList(sd,if_list);
    else {
	memset(&ifr,0,sizeof(struct ifreq));
	strncpy(ifr.ifr_name,dev_name.c_str(),IFNAMSIZ-1);
	
	if(ioctl(sd,SIOCGIFADDR,&ifr)!=0){
	    ERROR("setLocalIP: ioctl: %s\n",strerror(errno));
	    exit(-1);
	}

	if(ifr.ifr_addr.sa_family==PF_INET){
	    struct sockaddr_in* sa = (struct sockaddr_in*)&ifr.ifr_addr;
	    if_list.push_back(make_pair((char*)ifr.ifr_name,
					inet_ntoa(sa->sin_addr)));
	}
    }

    string local_ip;
    for( vector<pair<string,string> >::iterator it = if_list.begin();
	 it != if_list.end(); ++it) {

	memset(&ifr,0,sizeof(struct ifreq));
	strncpy(ifr.ifr_name,it->first.c_str(),IFNAMSIZ-1);

	if(ioctl(sd,SIOCGIFFLAGS,&ifr)!=0){
	    ERROR("setLocalIP: ioctl: %s\n",strerror(errno));
	    exit(-1);
	}

	if( (ifr.ifr_flags & IFF_UP) &&
	    (!dev_name.empty() || !(ifr.ifr_flags & IFF_LOOPBACK)) ) {
	   
	    local_ip = it->second;
	    break;
	}
    }

    close(sd);

    if(local_ip.empty()){
	ERROR("Could not determine proper local address for media advertising !\n");
	ERROR("Try using 'ifconfig -a' to find a proper interface and configure\n");
	ERROR("Sems to use it.\n");
	exit(-1);
    }

    if(ifr.ifr_flags & IFF_LOOPBACK){
	WARN("Media advertising using loopback address !\n");
	WARN("Try to use another network interface if your Sems\n");
	WARN("should be joinable from the rest of the world.\n");
    }

    return local_ip;
}

static int parse_args(int argc, char* argv[],
		      const string& flags,
		      const string& options,
		      map<char,string>& args)
{
    for(int i=1; i<argc; i++){

	char* arg = argv[i];

	if( (*arg != '-') || !*(++arg) ) { 
	    fprintf(stderr,"%s: invalid parameter: '%s'\n",argv[0],argv[i]);
	    return -1;
	}    

	if( flags.find(*arg) != string::npos ) {
	    
	    args[*arg] = "yes";
	}
	else if(options.find(*arg) != string::npos) {

	    if(!argv[++i]){
		fprintf(stderr,"%s: missing argument for parameter '-%c'\n",argv[0],*arg);
		return -1;
	    }
	    
	    args[*arg] = argv[i];
	}
	else {
	    fprintf(stderr,"%s: unknown parameter '-%c'\n",argv[0],arg[1]);
	    return -1;
	}
    }
    return 0;
}
