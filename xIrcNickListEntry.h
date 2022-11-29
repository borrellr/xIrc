/***************************************************************************
**    xIrcNickListEntry.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for handling Nick names
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
#ifndef _xIRCNICKLISTENTRY_H
#define _xIRCNICKLISTENTRY_H

#include <qstring.h>

class xIrcNickListEntry;

class xIrcNickListEntry
{
public:
   xIrcNickListEntry(const char *pName, xIrcNickListEntry *entry = NULL);
   xIrcNickListEntry(xIrcNickListEntry *pNick, xIrcNickListEntry *entry = NULL);
   xIrcNickListEntry();
   ~xIrcNickListEntry();

   bool is(const char *pName, bool byNick = TRUE);
   const char *getNick(bool fullName = FALSE);
   const char *getAddr() { return(addr); };
   void setNick(const char *pNick);
   void setAddr(const char *pAddr);
   xIrcNickListEntry  *next() { return(pNext); };
   xIrcNickListEntry  *prev() { return(pPrev); };
   void link(xIrcNickListEntry  *entry);
   void unlink();

protected:
   xIrcNickListEntry  *next(xIrcNickListEntry  *pEntry);
   xIrcNickListEntry  *prev(xIrcNickListEntry  *pEntry);
   xIrcNickListEntry  *pNext;
   xIrcNickListEntry  *pPrev;

private:
   QString        nick;
   QString        addr;
};

#endif
