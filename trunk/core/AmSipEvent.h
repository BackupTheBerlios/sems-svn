/*
 * $Id$
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
/** @file AmSipEvent.h */
#ifndef AmSipEvent_h
#define AmSipEvent_h

#include "AmEvent.h"
#include "AmSipMsg.h"

class AmSipDialog;

/** \brief base class for SIP events */
class AmSipEvent: public AmEvent
{
 public:
  AmSipEvent()
    : AmEvent(-1)
    {}

  AmSipEvent(const AmSipEvent& ev)
    : AmEvent(ev)
    {}

  virtual void operator() (AmSipDialog* dlg)=0;
};

/** \brief UAS reply re-transmission timeout event */
class AmSipTimeoutEvent: public AmSipEvent
{
 public:

  enum EvType {
    _noEv=0,
    no2xxACK,
    noErrorACK,
    noPRACK
  };

  EvType       type;

  unsigned int cseq;

  AmSipTimeoutEvent(EvType t, unsigned int cseq_num)
    : AmSipEvent(), type(t)
   {}

  virtual void operator() (AmSipDialog* dlg);
};

/** \brief SIP request event */
class AmSipRequestEvent: public AmSipEvent
{
 public:
  AmSipRequest req;
    
  AmSipRequestEvent(const AmSipRequest& r)
    : AmSipEvent(), req(r)
    {}

  virtual void operator() (AmSipDialog* dlg);
};

/** \brief SIP reply event */
class AmSipReplyEvent: public AmSipEvent
{
 public:
  AmSipReply reply;

  AmSipReplyEvent(const AmSipReply& r) 
    : AmSipEvent(),reply(r) {}

  virtual void operator() (AmSipDialog* dlg);
};


#endif
