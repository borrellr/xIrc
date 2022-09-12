/***************************************************************************
**    xTable.h    $Revision: 1.2 $ - $Name:  $ 
**    2 Dimensional Widget Table Frame
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
#ifndef _XTABLE_H
#define _XTABLE_H

#include <qframe.h>
#include <qptrlist.h>
#include <qlineedit.h>
#include <qintdict.h>
#include "xMouseEvent.h"
#include "xResources.h"

class xTable : public QFrame
{
   Q_OBJECT

public:
   xTable(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
          int cols, int rows);
   ~xTable();
   
   void setMargins(int horz, int vert)
   {
      horzMargin = horz;
      vertMargin = vert;
   }

   void setVertSpacing(int space)
   {
      vertSpace = space;
   }

   void setHorzSpacing(int space)
   {
      horzSpace = space;
   }

   int rows() { return(numRows); };
   int cols() { return(numCols); };
   void addWidget(QWidget *pWidget);
   void setColWidth(int width);
   void setColWidth(int col, int width);
   void setRowHeight(int height);
   void setRowHeight(int row, int height);
   int  getColWidth(int col);
   int  getRowHeight(int row);
   QWidget *getWidget(int num);
   QWidget *getWidget(int col, int row);
   QPtrListIterator<QWidget> widgetIt();
   QPtrList<QWidget> widgetList;
   void resizeWidget(int col, int row);
   void appendAt(int row);
   void insert(int row);
   void enable(QWidget *pWdt);
   void disable();
   void fitFrame();
   int getRow(QWidget *pWdt);
   void highlightRowOf(QWidget *pWdt);
   void clearHighlight();
   void clearAll();
   int  getSelectedRow()
   {
      return(curSelectedRow);
   };

protected:
/*
   void mousePressEvent(QMouseEvent *pEvt);
   void mouseReleaseEvent(QMouseEvent *pEvt);
   void mouseDoubleClickEvent(QMouseEvent *pEvt);
   void mouseMoveEvent(QMouseEvent *pEvt);
*/
protected slots:
   virtual void gotMousePressed(xMouseEvent *pEvt);
   virtual void gotMouseReleased(xMouseEvent *pEvt);
   virtual void gotMouseDoubleClicked(xMouseEvent *pEvt);
   virtual void gotMouseMoved(xMouseEvent *pEvt);

signals:
   void mousePressed(xMouseEvent *pEvt);
   void mouseReleased(xMouseEvent *pEvt);
   void mouseDoubleClicked(xMouseEvent *pEvt);
   void mouseMoved(xMouseEvent *pEvt);

private:
   int   numRows;
   int   numCols;
   int   totalWidth;
   int   totalHeight;
   int   horzMargin;
   int   vertMargin;
   int   horzSpace;
   int   vertSpace;
   int   curSelectedRow;
   int   recurse;
   QIntDict<int>  colWidths;
   QIntDict<int>  rowHeights;
   xWidgetResInfo    *wdtRes;
};

#endif
