/*
 * $Id$
 *
 * Copyright (C) 2006 iptego GmbH
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

#include <string>
#include <map>
using std::map;
using std::string;

struct AmUriParser {
  string display_name;
  string uri;
	
  string uri_user; 
  string uri_host; 
  string uri_port; 
  string uri_param;

  map<string, string> params;

  bool isEqual(const AmUriParser& c) const;
  bool parse_contact(string& line, size_t pos, size_t& end);
  bool parse_uri();
  bool parse_params(string& line, int& pos);
  void dump();
  AmUriParser() { }
};
