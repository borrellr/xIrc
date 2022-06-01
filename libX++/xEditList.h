/***************************************************************************
**    xEditList.h $Revision: 1.2 $ - $Name:  $ 
**    QListBox w/ Edit field
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1996 Joseph Croft <jcroft@unicomp.net>
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
#ifndef _XEDITLIST_H
#define _XEDITLIST_H

#include <qlineedit.h>
#include "xResources.h"
#include "xLineEdit.h"
#include "xListBox.h"
#include "xDialog.h"

class xEditList : public xFrame
{
   Q_OBJECT
   
public:
   xEditList(xWidgetResInfo *pPRes, QWidget *pParent = NULL, const char *pName = NULL,
              int cols = 10, int rows = 20, bool _unique = TRUE);
   ~xEditList();
   
   void insertItemsSort(const char *pItems, char delim = ' ');
   void insertItems(const char *pItems, char delim = ' ', int index = -1);
   void setText(const char *pText) { pEdit->setText(pText); };
   void setLabel(const char *pLabel) { pEdit->setLabel(pLabel); };
   void removeItem(int x) { pList->removeItem(x); };
   int getIndex(const char *pString);
   void clear() { pList->clear(); };
   const char *text() { return(pEdit->text()); };
   const char *string(int x);
   int currentItem() { return(pList->currentItem()); };
   void setCurrentItem(int idx) { pList->setCurrentItem(idx); }
   int count() { return(pList->count()); };   

signals:
   void gotEntry(const char *);

protected slots:
   void recvdReturn();
   void recvdSelect(int index);
   void recvdHighlighted(int index) { index++; setText(""); };

protected:
   virtual void keyPressEvent(QKeyEvent *pEvt);
   virtual void resizeEvent(QResizeEvent *pEvt);
   xListBox       *pList;
   xLineEdit      *pEdit;
   bool           unique;
   xWidgetResInfo *wdtRes;
};      
   
#endif
