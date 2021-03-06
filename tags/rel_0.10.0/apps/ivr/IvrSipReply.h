/*
 * $Id: IvrSipReply.cpp,v 1.15.2.1 2005/09/02 13:47:46 sayer Exp $
 * Copyright (C) 2007 iptego GmbH
 *
 * This file is part of sems, a free SIP media server.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef IvrSipReply_h
#define IvrSipReply_h

// Python stuff
#include <Python.h>
#include "structmember.h"

#include "AmSipReply.h"

extern PyTypeObject IvrSipReplyType;
PyObject* IvrSipReply_FromPtr(AmSipReply* req);

#endif
