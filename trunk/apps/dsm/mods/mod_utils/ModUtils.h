/*
 * $Id$
 *
 * Copyright (C) 2009 Teltech Systems Inc.
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
#ifndef _MOD_UTILS_H
#define _MOD_UTILS_H
#include "DSMModule.h"

#define MOD_CLS_NAME SCUtilsModule

DECLARE_MODULE(MOD_CLS_NAME);

DEF_ACTION_2P(SCUPlayCountRightAction);
DEF_ACTION_2P(SCUPlayCountLeftAction);
DEF_ACTION_1P(SCGetNewIdAction);
DEF_ACTION_2P(SCUSpellAction);
DEF_ACTION_2P(SCURandomAction);
DEF_ACTION_1P(SCUSRandomAction);
DEF_ACTION_2P(SCUSAddAction);
DEF_ACTION_2P(SCUSSubAction);

#endif
