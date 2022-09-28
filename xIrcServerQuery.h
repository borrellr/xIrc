/***************************************************************************
**    xIrcServerQuery.h  $Revision: 1.4 $ - $Name:  $ 
**    Dialog box to get server name
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
#ifndef _xIRCSERVERQUERY_H
#define _xIRCSERVERQUERY_H

#include <qmenubar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <xPshBtnFrame.h>
#include "xIrcConnectDialog.h"
#include "xIrcServerTable.h"
#include "xIrcSocket.h"

class xServerQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xServerQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xServerQuery();
   void initClass(const char *pName);
   
   const char *server() { 
         return((const char *)pTable->entry(pTable->selectedRow())->server()); 
      };
   const char *port() { return((const char *)Port); };

   enum QryResults { Rejected, Accepted, Disconnect };

signals:
   void msgToSocket(const char *pBuf);

public slots:
   void newList();
   void importList();
   void saveList();
   void newEntry();
   void editEntry();
   void deleteEntry();
   void loadList();
   void connectServer();
   void disconnectServer();

protected slots:
   void gotButton(int btn);
   void doConnect(int status);
   virtual void done(int results) { QDialog::done(results); };
   void reject();
   void accept();
   
private slots:
   void gotReturn();
   void serverSelected(int r);
   void gotoFirst();
   void gotoLast();

private:
   QString           Port;
   QAccel            *pAccel;
   QMenuBar          *pMenu;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pServerMenu;
   QLabel            *pLabel;
   xIrcConnectDialog *pConnect;
   xIrcServerTable   *pTable;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
};

#endif
