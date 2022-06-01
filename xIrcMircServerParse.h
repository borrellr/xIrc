/***************************************************************************
**    xIrcMircServerParse.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Parses and builds a list of Servers derived from a mIrc servers.ini file
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#ifndef XIRCSERVERPARSE
#define XIRCSERVERPARSE
#include "xIrcServerList.h"

bool xIrcMircServerParse(const char *fn, xIrcServerList &ServerList);

#endif
