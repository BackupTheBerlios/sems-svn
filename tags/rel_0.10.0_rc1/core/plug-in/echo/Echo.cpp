/*
 * $Id: Echo.cpp,v 1.7 2003/11/25 16:19:18 rco Exp $
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * For a license to use the sems software under conditions
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

#include "AmApi.h"
#include "AmSession.h"
#include "AmConfig.h"
#include "AmMail.h"
#include "AmAudio.h"
#include "log.h"

#include "Echo.h"
#include "AmAudioEcho.h"

//#include "AmSessionTimer.h"
#include "AmPlugIn.h"


EXPORT_SESSION_FACTORY(EchoFactory,"echo");


EchoFactory::EchoFactory(const string& _app_name)
    : AmSessionFactory(_app_name),
      session_timer_f(0)
{}

int EchoFactory::onLoad()
{
    session_timer_f = AmPlugIn::instance()->getFactory4Seh("session_timer");
    DBG("session_timer_f == 0x%.16lX\n",(unsigned long)session_timer_f);
    return (session_timer_f == NULL);
}

AmSession* EchoFactory::onInvite(const AmSipRequest& req)
{
    AmSession* s = new EchoDialog();
    s->addHandler(session_timer_f->getHandler(s));

    return s;
}

EchoDialog::EchoDialog() 
{
}

EchoDialog::~EchoDialog()
{
}

void EchoDialog::onSessionStart(const AmSipRequest& req)
{
    setInOut(&echo,&echo);
}

void EchoDialog::onBye(const AmSipRequest& req)
{
    setStopped();
}
