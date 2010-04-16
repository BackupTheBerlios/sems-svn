/*
 * $Id: transport.h 1048 2008-07-15 18:48:07Z sayer $
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
#ifndef _transport_h_
#define _transport_h_

#include "../AmThread.h"
#include <sys/socket.h>

#include <string>
using std::string;

#define SAv4(addr) \
            ((struct sockaddr_in*)addr)

#define SAv6(addr) \
            ((struct sockaddr_in6*)addr)

class trsp_socket
{
protected:
    // socket descriptor
    int sd;

    // bound address
    sockaddr_storage addr;

    // bound IP
    string           ip;

    // bound port number
    unsigned short   port;

public:
    trsp_socket();
    virtual ~trsp_socket();

    /**
     * Binds the transport socket to an address
     * @return -1 if error(s) occured.
     */
    virtual int bind(const string& address, unsigned short port)=0;

    /**
     * Getter for IP address
     */
    const char* get_ip();
    
    /**
     * Getter for the port number
     */
    unsigned short get_port();

    /**
     *  Getter for the socket descriptor
     */
    int get_sd();

    /**
     * Copy the internal address into the given one (sa).
     */
    void copy_addr_to(sockaddr_storage* sa);

    /**
     * Sends a message.
     * @return -1 if error(s) occured.
     */
    virtual int send(const sockaddr_storage* sa, const char* msg, const int msg_len);
};

class transport: public AmThread
{
protected:
    trsp_socket* sock;

public:
    transport(trsp_socket* sock): sock(sock) {}
    virtual ~transport();
};

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 4
 * End:
 */
