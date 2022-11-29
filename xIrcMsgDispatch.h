/*
** xIrcMsgDispatch.h Message Dispatch Entry class used for dispatching
** and thier responses going to the right widgets.
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
*/
#ifndef XIRCMSGDISPATCH_H
#define XIRCMSGDISPATCH_H

#include <qobject.h>
#include "xIrcSocket.h"
#include "xIrcMsgDispatchEntry.h"

class xIrcMsgDispatch
{
public:
   xIrcMsgDispatch();
   ~xIrcMsgDispatch();

   void dispatchMsg(QObject *pObj, const char *pSlot, xIrcMessage *pMsg);
   bool proccessResponse(xIrcMessage *pMsg);
   bool outstanding(xIrcMessage *pMsg);
   void setSocket(xIrcSocket *s);

private:
   xIrcMsgDispatchList  dspList;
   xIrcSocket           *pSocket;
};

#endif
