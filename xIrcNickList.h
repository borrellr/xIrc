/***************************************************************************
**    xIrcNickList.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for handling lists of Nick names
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
#ifndef _xIRCNICKLIST_H
#define _xIRCNICKLIST_H

#include "xIrcNickListEntry.h"

class xIrcNickList
{
public:
   xIrcNickList();
   virtual ~xIrcNickList();

   bool is(const char *pName, bool byNick = TRUE);
   xIrcNickListEntry *find(const char *pName, bool byNick = TRUE);
   virtual void merge(const char *pNames, char delim = ' ');
   virtual void merge(xIrcNickListEntry *pList);
   virtual void remove(const char *pNames, bool delflag = TRUE, char delim = ' ');
   virtual void remove(xIrcNickListEntry *pEntry, bool delflag = TRUE);
   virtual xIrcNickListEntry *getNext(xIrcNickListEntry *pEntry);
   virtual const char *getNicks(char delim = ' ', bool fullName = FALSE);
   virtual const char *getAddrs(char delim = ' ');
   int count();

protected:
   xIrcNickListEntry *pList;
   QString strTmp;
};
#endif
