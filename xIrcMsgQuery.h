/***************************************************************************
**    xIrcMsgQuery.h  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box to ask about Private messages
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
#ifndef _xIRCMSGQUERY_H
#define _xIRCMSGQUERY_H

#include <qptrlist.h>
#include <qlabel.h>
#include <xPshBtnFrame.h>
#include <xDialog.h>
#include <xMultiLineFrame.h>
#include <xResources.h>
#include "xIrcSocket.h"

class xIrcMsgQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcMsgQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xIrcMsgQuery();
   
   xIrcMessageList *getList() { return(&msgList); };
   int results() { return(btnPressed); };
   void add(xIrcMessage *pMsg);
   bool is(xIrcMessage *pMsg) { return(msgList.is(pMsg)); };

signals:   
   void textSelected(xMultiLineTextSelection txt);
   void done(xIrcMsgQuery *p);
   
protected slots:
   virtual void buttonPressed(int results);
   void haveTextSelection(xMultiLineTextSelection msg);
   
private slots:

private:
   int               btnPressed;
   xIrcMessageList   msgList;
   xMultiLineFrame   *pMsg;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
   void initClass(QWidget *pParent, const char *pName);
};

typedef QPtrList<xIrcMsgQuery>           xIrcMsgQueryListBase;
typedef QPtrListIterator<xIrcMsgQuery>   xIrcMsgQueryListIterator;

class xIrcMsgQueryList : public xIrcMsgQueryListBase
{
public:
   xIrcMsgQueryList() { setAutoDelete(TRUE); };
   xIrcMsgQuery *find(xIrcMessage *pMsg);
};

#endif
