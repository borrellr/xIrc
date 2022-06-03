/***************************************************************************
**    xIrcPeopleList.h  $Revision: 1.2 $ - $Name:  $ 
**    Maintains a List of People (Friends and Enimies)
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
#ifndef XIRCPEOPLELIST_H
#define XIRCPEOPLELIST_H

#include <qlist.h>
#include "xIrcPeopleEntry.h"

typedef QList<xIrcPeopleEntry>           xIrcPeopleListBase;
typedef QListIterator<xIrcPeopleEntry>   xIrcPeopleListIterator;

class xIrcPeopleList : public xIrcPeopleListBase
{
public:
   xIrcPeopleList(xIrcPeopleList &list);
   xIrcPeopleList();
   ~xIrcPeopleList();

   int readFile(const char *fn);
   int writeFile(const char *fn);

   bool matches(xIrcMessage *pMsg);
   xIrcPeopleEntry *entry(xIrcMessage *pMsg);

   void add(xIrcPeopleList &list);
   void add(xIrcPeopleEntry &entry);
};

#endif
