Announcement then call transfer 

This application plays an announcement and then REFERs the caller
to either the uri configured in the Refer-To session parameter, 
or, if his is not set, the request URI of the first INVITE.

If refer is not accepted, succeeds, or fails, a BYE is sent.

Example ser configuration appends:

#appends: parameter for announce_transfer
modparam( "tm", "tw_append",
   "announce_transfer_headers:P-App-Param=avp[$refer_to]")

# appends for REFER NOTIFYs
modparam( "tm", "tw_append",
   "notify_headers:hdr[Content-Length];hdr[Content-Type];hdr[Event];msg[body]")

Example of invoking announce_transfer within route:

	if (method != "ACK" && method != "INVITE"  
		&& method != "CANCEL" && method != "INFO" 
		&& method != "NOTIFY" && method != "BYE" ){
			log("unsupported method\n");
			sl_send_reply("500","unsupported method");
			break;
	}
   
	if (method == "NOTIFY") {
		if (!t_write_unix("/tmp/sems_sock","announce_transfer/notify_headers")){
			log("could not contact announce_transfer\n");
			t_reply("500","could not contact media server");	
			break;
		}
		break;
	} 

	if (method == "INVITE") {
		avp_write("Refer-To=sip:callme@example.com","$refer_to");
		if (!t_write_unix("/tmp/sems_sock","announce_transfer/announce_transfer_headers")){
			log("could not contact announce_transfer\n");
			t_reply("500","could not contact media server");	
			break;
		}
		break;
	}

	if (!t_write_unix("/tmp/sems_sock","announce_transfer")){
		log("could not contact announce_transfer\n");
		t_reply("500","could not contact media server");	
		break;
	}

