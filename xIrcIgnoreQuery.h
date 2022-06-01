/***************************************************************************
**    xIrcIgnoreQuery.h  $Revision: 1.4 $ - $Name:  $ 
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
#ifndef _xIRCIGNOREQUERY_H
#define _xIRCIGNOREQUERY_H

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

class xIrcIgnoreQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcIgnoreQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xIrcIgnoreQuery();
   void initClass(const char *pName);
   
   xIrcPeopleList *list() { return(pTable->list()); };

signals:
   void msgToSocket(const char *pBuf);

public slots:
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
   
private slots:
   void gotReturn();
   void gotoFirst();
   void gotoLast();

private:
   QString           Port;
   QAccel            *pAccel;
   QMenuBar          *pMenu;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pIgnoreMenu;
   QLabel            *pLabel;
   xIrcPeopleTable   *pTable;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
};

#endif
