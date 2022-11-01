/***************************************************************************
**    xIrcServerTable.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Irc Server Table
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
**
**    This library is free software; you can redistribute it and/or
**    modify it under the terms of the GNU Library General Public
**    License as published by the Free Software Foundation; either
**    version 2 of the License, or (at your option) any later version.
**
**    This library is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**    Library General Public License for more details.
**
**    You should have received a copy of the GNU Library General Public
**    License along with this library; if not, write to the Free
**    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#ifndef _XIRCSERVERTABLE_H
#define _XIRCSERVERTABLE_H

#include <qaccel.h>
#include <qdict.h>
#include <qscrollbar.h>
#include <qstring.h>
#include <qstring.h>
#include <qevent.h>
#include <xFrame.h>
#include <qlabel.h>
#include <xTable.h>
#include <xMouseEvent.h>
#include "xIrcServerList.h"

class xIrcServerTable : public xFrame
{
   Q_OBJECT

public:
   xIrcServerTable(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
                   int rows = 10);
   ~xIrcServerTable();
   int selectedRow() { return(selRow); };
   int currentRow() { return(curRow); };
   int rows() { return(pServerList1->count()); };
   xIrcServerEntry *entry(int r);
   void remove(int r);
   void clear();
   void readFile(const QString &fn);
   void writeFile(const QString &fn);
   void import(const QString &fn);
   void add(xIrcServerEntry &entry);

signals:
   void rowClicked(int row);
   void rowDoubleClicked(int row);
   void returnPressed();

public slots:
   virtual void setNewMask();
   virtual void newMask(bool force = FALSE);
   virtual void showRows(int rows = 0);
   virtual void haveMousePressed(xMouseEvent*);
   virtual void haveMouseDoubleClicked(xMouseEvent*);

   virtual void highlightTopRow();
   virtual void highlightRow(int idx);
   virtual void highlightNext();
   virtual void highlightPrev();

   virtual void scroll(int idx);
   virtual void scrollBack();
   virtual void scrollFwd();
   virtual void scrollPageBack();
   virtual void scrollPageFwd();

private:
   void setSB();
   void showMaskEntries();

   QString                 groupMask;
   QString                 countryMask;
   QString                 stateMask;
   QString                 cityMask;
   QString                 serverMask;
   QLabel                  *pLabel1;
   xTable                  *pTable;
   xTable                  *pTable1;
   xTable                  *pTable2;
   xFrame                  *pFrame;
   xFrame                  *pFrame1;
   QScrollBar              *pSB;
   QLabel                  *pTitle;
   xIrcServerList          *pServerList;
   xIrcServerList          *pServerList1;
   int                     enbRow;
   int                     curRow;
   int                     selRow;
   bool                    connected;
   xWidgetResInfo          *wdtRes;
};

#endif
