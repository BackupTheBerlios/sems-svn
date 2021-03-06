************************************
* Early announcement Server Module *
************************************

Description:
------------

The annoucement server plays a wav file to the caller
in an early session: it replies to an invite with
183 Session Progress (with SDP), then sends the announcement
and finally replies the request with a response (by default
404 Not found ), or, if b2bua mode is set, continues the call
to request-URI in b2bua more.

No b2bua mode: Final Reply sent
-------------------------------
Session Parameters (no b2bua mode):
   Final-Reply-Code     integer (e.g. Final-Reply-Code=405 )
   Final-Reply-Reason   string  (e.g. Final-Reply-Reason=Prohibited)

e.g. 
 append_hf("P-App-Param: Final-Reply-Code=405;Final-Reply-Reason=Prohibited;\r\n");

b2bua mode
----------
If continue_b2b=yes is set in early_announce.conf, then the INVITE 
is sent in a b2bua session to the original request-URI. From then on, 
the call is continued as normal b2bua call, i.e. all requests and replies 
are relayed into the other leg.

Announcement file played from file system
-----------------------------------------
If early_announce application was compiled WITHOUT -DUSE_MYSQL option,
it searches for files to play following these rules:

 1) <announce_path>/[RURI-Domain]/[RURI-User].wav
 2) <announce_path>/[RURI-User].wav
 3) <announce_path>/<default_announce>

Example sems (early_announce.conf) configuration for file system audio:
announce_path=wav/
default_announce=default_en.wav

Announcement file from MySQL DB
-------------------------------
If early_announce application was compiled with -DUSE_MYSQL option, it
supports third Session Parameter

   Language	      two letter string or empty string (e.g. en)
  
It searches files to play first from user_audio, then from domain_audio,
and finally from default_audio tables (described below).  When an audio
file is fetched from database, it is stored in /tmp directory.

Example sems (early_announce.conf) configuration for MySQL audio:

# Host where MySQL server is running (optional, defaults to 'localhost')
mysql_host=localhost

# MySQL username (mandatory, no default)
mysql_user=sems

# MySQL password (mandatory, no default)
mysql_passwd=secret

# Database where audio is stored (optional, defaults to 'sems') 
mysql_db=sems

# Value of 'application' field in early_announce audio records (optional,
# defaults to 'early_announce')
application=early_announce

# Value of 'message' field in early_announce audio records (optional,
# defaults to 'greeting_msg')
message=greeting_msg

# Default language if Language Session Parameter does not exist
# (optional, defaults to 'en')
default_language=en

Database tables:

CREATE TABLE default_audio (
  id int(10) unsigned NOT NULL auto_increment,
  application varchar(32) NOT NULL,
  message varchar(32) NOT NULL,
  `language` char(2) NOT NULL default 'en',
  audio mediumblob NOT NULL,
  PRIMARY KEY  (id),
  UNIQUE KEY application (application,message,`language`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1;

CREATE TABLE domain_audio (
  id int(10) unsigned NOT NULL auto_increment,
  application varchar(32) NOT NULL,
  message varchar(32) NOT NULL,
  domain varchar(128) NOT NULL,
  `language` char(2) NOT NULL default 'en',
  audio mediumblob NOT NULL,
  PRIMARY KEY  (id),
  UNIQUE KEY application (application,message,domain,`language`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1;

CREATE TABLE user_audio (
  id int(10) unsigned NOT NULL auto_increment,
  application varchar(32) NOT NULL,
  message varchar(32) NOT NULL,
  domain varchar(128) NOT NULL,
  userid varchar(64) NOT NULL,
  audio mediumblob NOT NULL,
  PRIMARY KEY  (id),
  UNIQUE KEY application (application,message,domain,userid)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1;


Example ser configuration:
-------------------------

#
# simple single instance Ser config script with early media
# ----------- global configuration parameters ------------------------

debug=5         # debug level (cmd line: -dddddddddd)
fork=yes
log_stderror=yes        # (cmd line: -E)

check_via=no    # (cmd. line: -v)
dns=no           # (cmd. line: -r)
rev_dns=no      # (cmd. line: -R)
port=5060
#children=4
sip_warning=no

# Configure Ser to create a FIFO server
#fifo="/tmp/ser_fifo"

# Configure Ser to create an unix socket server
unix_sock="/tmp/ser_socket"

loadmodule "modules/sl/sl.so"
loadmodule "modules/tm/tm.so"
loadmodule "modules/rr/rr.so"
loadmodule "modules/maxfwd/maxfwd.so"
loadmodule "modules/avp/avp.so"
loadmodule "modules/avpops/avpops.so"

# -- rr params --
# add value to ;lr param to make some broken UAs happy
modparam("rr", "enable_full_lr", 1)

modparam( "avpops", "avp_aliases", "sess_params=i:66" )

# this is to be appended to the invite
modparam("tm", "tw_append","early_headers:P-App-Name=avp[$sess_params]")

# -------------------------  request routing logic -------------------
# main routing logic
route{

        # initial sanity checks -- messages with
        # max_forwards==0, or excessively long requests
        if (!mf_process_maxfwd_header("10")) {
                sl_send_reply("483","Too Many Hops");
                break;
        };
        if ( msg:len > max_len ) {
                sl_send_reply("513", "Message too big");
                break;
        };
        # we record-route all messages -- to make sure that
        # subsequent messages will go through our proxy; that's
        # particularly good if upstream and downstream entities
        # use different transport protocol
        record_route();
        # loose-route processing
        if (loose_route()) {
                t_relay();
                break;
        };

        if (uri==myself) {
        	# filter unsupported requests
           	if (!(method=="REGISTER" || method=="ACK" || method=="INVITE" ||
              	      method=="BYE" || method=="CANCEL" )) {
                	sl_send_reply("501", "method not understood here");
                	break;
           	}

           	if (method=="REGISTER") {
              		# make UAs which want to register unhappy
              		sl_send_reply("501","registration couldn't be accepted");
              		break;
           	};

            	if (!t_newtran()){
                	sl_send_reply("500","could not create transaction");
                	break;
            	};

           	if (method == "INVITE") {

           		avp_write("Final-Reply-Code=405;Final-Reply-Reason=Prohiboted", 
                                  "$sess_param");

            		if(!t_write_unix("/tmp/sems_sock","early_announce/early_headers")){
                		log("could not contact early_announce\n");
                		t_reply("404","not found");
            		};
            		break;

	        }  else if (method=="BYE" || method=="CANCEL") {
                	if(!t_write_unix("/tmp/sems_sock","bye")) {
                    		t_reply("500","error contacting sems");
                	};
                	break;
           	} else if (method=="ACK"){
                        # absorb ACKs
                        t_relay();
                        break;
           	};
           	break;
        }

        if (!t_relay()) {
                sl_reply_error();
        };
}

