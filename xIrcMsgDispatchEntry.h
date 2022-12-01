/***************************************************************************
** xIrcDispatchEntry.h Message Dispatch Entry class used for keeping commands
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
#ifndef XIRCMSGDISPATCHENTRY_H
#define XIRCMSGDISPATCHENTRY_H

#include <qobject.h>
#include "xIrcSocket.h"

class xIrcMsgDispatchEntry : public QObject
{
   Q_OBJECT

public:
   xIrcMsgDispatchEntry(QObject *pObj, const char *p_Slot,
                         xIrcMessage *pMsg, int &accepted);
   ~xIrcMsgDispatchEntry();
   bool is(xIrcMessage *pMsg);
   bool dispatchResponse(xIrcMessage *pMsg);
   xIrcMessage *message() { return(&msg); };
   void setSent(bool f) { sentFlag = f; };
   bool sent() { return(sentFlag); };
   bool dispatch() { return(pObject != NULL); };

signals:
   void gotResponse(xIrcMessage *pMsg);

private:
   bool        sentFlag;
   QString     slot;
   xIrcMessage msg;
   QObject     *pObject;
   int         *pResponseList;
};

typedef QList<xIrcMsgDispatchEntry>            xIrcMsgDispatchListBase;
typedef QListIterator<xIrcMsgDispatchEntry>    xIrcMsgDispatchListIterator;

class xIrcMsgDispatchList : public xIrcMsgDispatchListBase
{
public:
   xIrcMsgDispatchList() { setAutoDelete(TRUE); };
};

#endif
