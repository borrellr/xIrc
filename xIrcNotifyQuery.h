/***************************************************************************
**    xIrcNotifyQuery.h  $Revision: 1.4 $ - $Name:  $ 
**    Dialog box to manage the Notification list
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
#ifndef _xIRCNOTIFYQUERY_H
#define _xIRCNOTIFYQUERY_H

#include <qmenubar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <xPshBtnFrame.h>
#include <xResources.h>
#include "xIrcConnectDialog.h"
#include "xIrcPeopleTable.h"
#include "xIrcSocket.h"

class xIrcNotifyQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcNotifyQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xIrcNotifyQuery();
   void initClass(const char *pName);
   
   xIrcPeopleList *list() { return(pTable->list()); };
   bool gotNotification(xIrcMessage *pMsg);
   void startUp();
   void shutDown();

signals:
   void msgToSocket(const char *pBuf);
   void sendNotify(const char *pNick);
   void updateList(xIrcPeopleList *pList);

public slots:
   void messageIn(xIrcMessage *pMsg)
   {
      gotNotification(pMsg);
   };
   void add(xIrcPeopleEntry &e);
   void newEntry();
   void newList();
   void saveList();
   void editEntry(int entry);
   void editEntry();
   void deleteEntry();
   void loadList();

protected slots:
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   void reject();
   void accept();
   void pollList();
   
private slots:
   void gotReturn();
   void gotoFirst();
   void gotoLast();

private:
   bool              whoSent;
   QString           getNick(xIrcMessage*);
   QString           Port;
   QAccel            *pAccel;
   QMenuBar          *pMenu;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pNotifyMenu;
   QTimer            *pTimer;
   QLabel            *pLabel;
   xIrcPeopleTable   *pTable;
   xIrcPeopleListIterator *pPeople;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
};

#endif
