/***************************************************************************
**    xIrcPeopleTable.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Irc People Table
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
#ifndef _XIRCPEOPLETABLE_H
#define _XIRCPEOPLETABLE_H

#include <qaccel.h>
#include <qdict.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qstring.h>
#include <qstring.h>
#include <xCheckBox.h>
#include <xFrame.h>
#include <xResources.h>
#include <xTable.h>
#include "xIrcPeopleList.h"

class xIrcPeopleBtns : public xFrame
{
   Q_OBJECT

public:
   xIrcPeopleBtns(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
                  const char *pFlagLabel);
   ~xIrcPeopleBtns();

   void setFlag(int f);
   void setEnabled(bool f);
   int flag();

signals:
   void clicked(xIrcPeopleBtns *pBtn);

protected slots:
   void gotClicked();

private:
   xCheckBox         *pFlag;
   xCheckBox         *pByAddr;
   xWidgetResInfo    *wdtRes;
};

class xIrcPeopleTable : public xFrame
{
   Q_OBJECT

public:
   xIrcPeopleTable(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
                   const char *pFlagLabel, int rows = 10);
   ~xIrcPeopleTable();
   QString flagLabel() { return(FlagLabel); };
   int selectedRow() { return(selRow); };
   int currentRow() { return(curRow); };
   int rows() { return(pPeopleList->count()); };
   xIrcPeopleEntry *entry(int r);
   xIrcPeopleEntry *entry(xIrcMessage *pMsg);
   void remove(int r);
   void clear();
   xIrcPeopleList *list() { return(pPeopleList); };
   void readFile(const char *fn);
   void writeFile(const char *fn);
   void import(const char *fn);
   void add(xIrcPeopleEntry &entry);

signals:
   void rowClicked(int row);
   void rowDoubleClicked(int row);
   void returnPressed();

public slots:
   void gotClicked(xIrcPeopleBtns *pBtn);
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

   xTable                  *pTable;
   xTable                  *pTable1;
   xFrame                  *pFrame;
   xFrame                  *pFrame1;
   QScrollBar              *pSB;
   QString                 FlagLabel;
   QLabel                  *pTitle;
   xIrcPeopleList          *pPeopleList;
   xIrcPeopleList          *pPeopleList1;
   int                     enbRow;
   int                     curRow;
   int                     selRow;
   bool                    connected;
   xWidgetResInfo          *wdtRes;
};

#endif
