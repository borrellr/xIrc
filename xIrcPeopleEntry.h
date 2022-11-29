/***************************************************************************
**    xIrcPeopleEntry.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Builds an entry for the People list
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
#ifndef XIRCPEOPLEENTRY_H
#define XIRCPEOPLEENTRY_H

#include <qstring.h>
#include "xIrcSocket.h"

class xIrcPeopleEntry
{
public:
   xIrcPeopleEntry(xIrcMessage *pMsg, bool atAround=TRUE);
   xIrcPeopleEntry(const char *pNick, const char *pMask, const char *pMsg, int flag);
   xIrcPeopleEntry(xIrcPeopleEntry &e);
   
   QString nick() { return(Nick); };
   QString realNick() { return(RealNick); };
   QString mask() { return(Mask); };
   QString message() { return(Message); };
   int flag() { return(Flag); };
   int state() { return(State); };
   void setFlag(int f) { Flag = f; };
   void setState(int f) { State = f; };
   void setMessage(const char *m) { Message = m; };
   void setNick(const char *m) { Nick = m; };
   void setRealNick(const char *m) { RealNick = ""; RealNick = m; };
   void setMask(const char *m) { Mask = m; };

   bool is(xIrcMessage *pMsg, int byAddr);
   int compare(xIrcPeopleEntry *e);
   QString buildMask(xIrcMessage *pMsg, bool atAround=TRUE);

private:
   QString     Nick;
   QString     RealNick;
   QString     Mask;
   QString     Message;
   int         Flag;
   int         State;
};

#endif
