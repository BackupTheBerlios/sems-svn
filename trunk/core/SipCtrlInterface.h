/*
 * $Id: SipCtrlInterface.h 1048 2008-07-15 18:48:07Z sayer $
 *
 * Copyright (C) 2007 Raphael Coeffic
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
#ifndef _SipCtrlInterface_h_
#define _SipCtrlInterface_h_

#include "sip/sip_ua.h"
#include "AmThread.h"

#include <string>
#include <list>
using std::string;
using std::list;

class AmSipRequest;
class AmSipReply;

struct sip_msg;
struct sip_header;
class trans_ticket;


class udp_trsp_socket;
class udp_trsp;

class SipCtrlInterface:
    public sip_ua
{
    void prepare_routes_uac(const list<sip_header*>& routes, string& route_field);
    void prepare_routes_uas(const list<sip_header*>& routes, string& route_field);

    friend class udp_trsp;

    AmCondition<bool> stopped;
    
    udp_trsp_socket* udp_socket;
    udp_trsp** udp_servers;

public:

    static string outbound_host;
    static unsigned int outbound_port;
    static bool log_parsed_messages;
    static int log_raw_messages;
    static int udp_rcvbuf;

    SipCtrlInterface();
    ~SipCtrlInterface(){}

    int load();

    void run(const string& bind_addr, unsigned short bind_port);
    void stop();
    void cleanup();

    /**
     * Sends a SIP request.
     *
     * @param req The request to send. If the request creates a transaction, 
     *            its ticket is written into req.tt.
     */
    static int send(AmSipRequest &req);

    /**
     * Sends a SIP reply. 
     *
     * @param rep The reply to be sent. 'rep.tt' should be set to transaction 
     *            ticket included in the SIP request.
     */
    static int send(const AmSipReply &rep);

    /**
     * CANCELs an INVITE transaction.
     *
     * @param tt transaction ticket of the request to cancel.
     */
    static int cancel(trans_ticket* tt);

    /**
     * From sip_ua
     */
    void handle_sip_request(const trans_ticket& tt, sip_msg* msg);
    void handle_sip_reply(sip_msg* msg);
    void timer_expired(sip_trans* trans, sip_timer_type tt);
};


#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 4
 * End:
 */
