/***************************************************************************
**    xTable.cpp  $Revision: 1.3 $ - $Name:  $ 
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
#include <stdio.h>
#include <qlineedit.h>
#include "xEdit.h"
#include "xLabel.h"
#include "xMisc.h"
#include "xFrame.h"
#include "xTable.h"

static int dbg = 0;

xTable::xTable(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
               int cols, int rows) :
      QFrame(pParent, pName)
{
   int x;
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   colWidths.setAutoDelete(TRUE);
   rowHeights.setAutoDelete(TRUE);
   numRows = rows;
   numCols = cols;
   curSelectedRow = -1;
   horzSpace = 0;
   vertSpace = 3;
   horzMargin = 5;
   vertMargin = 5;
   recurse = 0;
   for (x = 0; x < rows; x++)
      rowHeights.replace(x, new int(10));
   for (x = 0; x < cols; x++)
      colWidths.replace(x, new int(10));
}

xTable::~xTable()
{
}

void xTable::addWidget(QWidget *pWidget)
{
   if (dbg) fprintf(stdout, "xTable::addWidget():Enter\n");
   if (dbg) fflush(stdout);
   widgetList.append(pWidget);
   if (pWidget->inherits("xLabel"))
   {
      connect((xLabel*)pWidget, SIGNAL(mousePressed(xMouseEvent*)),
              this, SLOT(gotMousePressed(xMouseEvent*)));

      connect((xLabel*)pWidget, SIGNAL(mouseReleased(xMouseEvent*)),
              this, SLOT(gotMouseReleased(xMouseEvent*)));

      connect((xLabel*)pWidget, SIGNAL(mouseDoubleClicked(xMouseEvent*)),
              this, SLOT(gotMouseDoubleClicked(xMouseEvent*)));

      connect((xLabel*)pWidget, SIGNAL(mouseMoved(xMouseEvent*)),
              this, SLOT(gotMouseMoved(xMouseEvent*)));
   }
   else if (pWidget->inherits("xEdit"))
   {
      connect((xEdit*)pWidget, SIGNAL(mousePressed(xMouseEvent*)),
              this, SLOT(gotMousePressed(xMouseEvent*)));

      connect((xEdit*)pWidget, SIGNAL(mouseReleased(xMouseEvent*)),
              this, SLOT(gotMouseReleased(xMouseEvent*)));

      connect((xEdit*)pWidget, SIGNAL(mouseDoubleClicked(xMouseEvent*)),
              this, SLOT(gotMouseDoubleClicked(xMouseEvent*)));

      connect((xEdit*)pWidget, SIGNAL(mouseMoved(xMouseEvent*)),
              this, SLOT(gotMouseMoved(xMouseEvent*)));
   }
   if (dbg) fprintf(stdout, "xTable::addWidget():Exit\n");
   if (dbg) fflush(stdout);
}      

void xTable::setColWidth(int width)
{
   int x;

   for (x = 0; x < numCols; x++)
      colWidths.replace(x, new int(width));
}

void xTable::setColWidth(int col, int width)
{
   colWidths.replace(col, new int(width));
}

void xTable::setRowHeight(int height)
{
   int x;

   for (x = 0; x < numRows; x++)
      rowHeights.replace(x, new int(height));
}

int xTable::getRow(QWidget *pWdt)
{
   int wdt;

   if ((wdt = widgetList.findRef(pWdt)) >= 0)
      return(wdt / numCols);
   else
      return(-1);
}

void xTable::appendAt(int row)
{
   if (row == -1 || row >= (numRows - 1))
      insert(numRows - 1);
   else
      insert(row + 1);
}

void xTable::insert(int r)
{
   int row, col;
   QWidget *pWdtSrc = NULL, *pWdtDst = NULL;

   for (row = numRows - 1; row > r; row--)
   {
      for (col = 0; col < numCols; col++)
      {
         if ((pWdtDst = getWidget(col, row)) != NULL && 
              (pWdtSrc = getWidget(col, row - 1)) != NULL)
         {
            const char *pTxt;
            if (pWdtSrc->inherits("QLineEdit"))
               pTxt = ((QLineEdit*)pWdtSrc)->text();
            else if (pWdtSrc->inherits("QLabel"))
               pTxt = ((QLabel*)pWdtSrc)->text();
            else
               continue;

            if (pWdtDst->inherits("QLineEdit"))
               ((QLineEdit*)pWdtDst)->setText(pTxt);
            else if (pWdtSrc->inherits("QLabel"))
               ((QLabel*)pWdtDst)->setText(pTxt);
            else
               continue;
         }
      }
   }
   for (col = 0; col < numCols; col++)
   {
      if ((pWdtDst = getWidget(col, row)) != NULL)
      if (pWdtDst->inherits("QLineEdit"))
         ((QLineEdit*)pWdtDst)->setText("");
      else if (pWdtDst->inherits("QLabel"))
         ((QLabel*)pWdtDst)->setText("");
      else
         continue;
   }
}

void xTable::disable()
{
   int row, col;
   QFrame *pFrame;

   if ((row = curSelectedRow) >= 0)
   {
      for (col = 0; row < numRows && col < numCols; col++)
      {
         pFrame = (QFrame*)getWidget(col, row);
         if (getWidget(0, row)->inherits("xEdit")) 
            ((xEdit*)pFrame)->setEnabled(FALSE);
         else if (getWidget(0, row)->inherits("QLineEdit")) 
            pFrame->setEnabled(FALSE);
         else if (getWidget(0, row)->inherits("QFrame"))
            pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
         pFrame->update();
      }
   }
}
   
void xTable::enable(QWidget *pWdt)
{
   int wdt, row, col;
   QFrame *pFrame;
   
   if (dbg) fprintf(stdout, "xTable::enable():Enter\n");
   if (dbg) fflush(stdout);
   if ((wdt = widgetList.findRef(pWdt)) >= 0)
   {
      if ((row = curSelectedRow) >= 0)
      {
         if (dbg) fprintf(stdout, "xTable::enable():Clearing highlight row %d\n",
                                  row);
         if (dbg) fflush(stdout);
         for (col = 0; row < numRows && col < numCols; col++)
         {
            pFrame = (QFrame*)getWidget(col, row);
            if (pFrame->inherits("xEdit")) 
               ((xEdit*)pFrame)->setEnabled(FALSE);
            else if (pFrame->inherits("QLineEdit")) 
               ((QLineEdit*)pFrame)->setEnabled(FALSE);
            else if (pFrame->inherits("QFrame"))
               pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
            pFrame->update();
         }
      }
      row = (wdt / numCols);
      if (dbg) fprintf(stdout, "xTable::enable():Enabling row %d\n",
                               row);
      if (dbg) fflush(stdout);
      for (col = 0; row < numRows && col < numCols; col++)
      {
         pFrame = (QFrame*)getWidget(col, row);
         if (pFrame->inherits("xEdit")) 
            ((xEdit*)pFrame)->setEnabled(TRUE);
         if (pFrame->inherits("QLineEdit")) 
            ((QLineEdit*)pFrame)->setEnabled(TRUE);
         else if (pFrame->inherits("QFrame"))
            pFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
         pFrame->update();
      }
      curSelectedRow = row;
   }
   if (dbg) fprintf(stdout, "xTable::enable():Exit\n");
   if (dbg) fflush(stdout);
}

void xTable::clearHighlight()
{
   int row, col;
   QFrame *pFrame;

   if (dbg) fprintf(stdout, "xTable::clearHighlight():Enter\n");
   if (dbg) fflush(stdout);
   if ((row = curSelectedRow) >= 0)
   {
      if (dbg) fprintf(stdout, "xTable::clearHighlight():Row %d\n", row);
      if (dbg) fflush(stdout);
      for (col = 0; row < numRows && col < numCols; col++)
      {
         pFrame = (QFrame*)getWidget(col, row);
         if (getWidget(0, row)->inherits("xEdit")) 
            ((xEdit*)pFrame)->setHighlight(FALSE);
         else if (getWidget(0, row)->inherits("QLineEdit")) 
            ((QLineEdit*)pFrame)->setEnabled(FALSE);
         else if (getWidget(0, row)->inherits("QFrame"))
            pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
         pFrame->update();
      }
   }
   if (dbg) fprintf(stdout, "xTable::clearHighlight():Exit\n");
   if (dbg) fflush(stdout);
}
   
void xTable::highlightRowOf(QWidget *pWdt)
{
   int wdt, row, col;
   QFrame *pFrame;
   
   if (dbg) fprintf(stdout, "xTable::highlightRowOf():Enter\n");
   if (dbg) fflush(stdout);
   if ((wdt = widgetList.findRef(pWdt)) >= 0)
   {
      if ((row = curSelectedRow) >= 0)
      {
         if (dbg) fprintf(stdout, "xTable::highlightRowOf():Clearing highlight Row %d\n",
                                  row);
         if (dbg) fflush(stdout);
         for (col = 0; row < numRows && col < numCols; col++)
         {
            pFrame = (QFrame*)getWidget(col, row);
            if (pFrame->inherits("xEdit"))
               ((xEdit*)pFrame)->setHighlight(FALSE);
            else if (pFrame->inherits("QFrame"))
               pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
            pFrame->update();
         }
      }
      row = (wdt / numCols);
      if (dbg) fprintf(stdout, "xTable::highlightRowOf():Highlighting Row %d\n",
                                row);
      for (col = 0; row < numRows && col < numCols; col++)
      {
         pFrame = (QFrame*)getWidget(col, row);
         if (pFrame->inherits("xEdit"))
            ((xEdit*)pFrame)->setHighlight(TRUE);
         else if (pFrame->inherits("QFrame"))
            pFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
         pFrame->update();
      }
      curSelectedRow = row;
   }
   if (dbg) fprintf(stdout, "xTable::highlightRowOf():Exit\n");
   if (dbg) fflush(stdout);
}

void xTable::setRowHeight(int row, int height)
{
   rowHeights.replace(row, new int(height));
}

void xTable::resizeWidget(int col, int row)
{
   if (row >= numRows || col >= numCols)
      return;

   QWidget *pWidget = widgetList.at(row * numCols + col);
   if (pWidget != NULL)
      pWidget->resize(*colWidths.find((long)col), *rowHeights.find((long)row));
}

void xTable::clearAll()
{
   QWidget *pWidget;
   QListIterator<QWidget> it(widgetList);

   for (it.toFirst(); (pWidget = it.current()) != NULL; ++it)
   {
      if (pWidget->inherits("QLineEdit"))
         ((QLineEdit*)it.current())->setText("");
      else if (pWidget->inherits("QLabel"))
         ((QLabel*)it.current())->setText("");
   }
}

void xTable::fitFrame()
{
   int z, x, y, row, col, width, height;
   QWidget *pWidget;

   if (dbg) fprintf(stdout, "xTable::fitFrame():Enter, recurse = %d\n",
                    recurse);
   if (dbg) fflush(stdout);
   if (recurse > 1)
   {
      if (dbg) fprintf(stdout, "xTable::fitFrame():Exit (On Recursion)\n");
      if (dbg) fflush(stdout);
      return;
   }
   recurse++;
   width = height = frameWidth() * 2;
   x = horzMargin + frameWidth();
   y = vertMargin + frameWidth();
   if (dbg) fprintf(stdout, "xTable::fitFrame():Height = %d, width = %d, x = %d, y = %d\n",
                     width, height, x, y);
   if (dbg) fflush(stdout);
   
   row = col = 0;

   QListIterator<QWidget> it(widgetList);
   if (dbg) fprintf(stdout, "xTable::fitFrame():Fitting %d Widgets\n", it.count());
   if (dbg) fflush(stdout);

   for (z = row = col = 0, it.toFirst(); (pWidget = it.current()) != NULL; ++it, z++)
   {
      if (dbg) fprintf(stdout, "xTable::fitFrame():Fitting Widget %d, col %d, row %d\n", z, col, row);
      if (dbg) fflush(stdout);
      pWidget->setGeometry(x, y, getColWidth(col), getRowHeight(row));
      if (dbg) fprintf(stdout, "xTable::fitFrame():Calculating new x & y\n");
      if (dbg) fflush(stdout);
      x +=  getColWidth(col) + horzSpace;
      if (col < numCols - 1)
      {
         col++;
      }
      else
      {
         if (dbg) fprintf(stdout, "xTable::fitFrame():New Row!\n");
         if (dbg) fflush(stdout);
         y +=  getRowHeight(row) + vertSpace;
         x = horzMargin + frameWidth();
         col = 0;
         row++;
         if (dbg) fprintf(stdout, "xTable::fitFrame():row = %d, col = %d, x = %d\n",
                                    row, col, x);
         if (dbg) fflush(stdout);
      }
      if (dbg) fprintf(stdout, "xTable::fitFrame():Next Offset: x = %d, y = %d\n", x, y);
      if (dbg) fflush(stdout);
   }
   if (dbg) fprintf(stdout, "xTable::fitFrame():Calculating Total width\n");
   if (dbg) fflush(stdout);
   for (totalWidth = col = 0; col < numCols; col++)
   {
      if (dbg) fprintf(stdout, "xTable::fitFrame():col %d\n", col);
      if (dbg) fflush(stdout);
      totalWidth += getColWidth(col);
   }
   if (dbg) fprintf(stdout, "xTable::fitFrame():1:Calculating Total width = %d\n", totalWidth);
   if (dbg) fflush(stdout);
   totalWidth += (horzMargin * 2) + ((numCols - 1) * horzSpace) + 
                 (frameWidth() * 2);
   if (dbg) fprintf(stdout, "xTable::fitFrame():2:Calculating Total width = %d\n", totalWidth);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xTable::fitFrame():Calculating Total height\n");
   if (dbg) fflush(stdout);
   for (totalHeight = row = 0; row < numRows; row++)
   {
      if (dbg) fprintf(stdout, "xTable::fitFrame():row %d\n", row);
      if (dbg) fflush(stdout);
      totalHeight += getRowHeight(row);
   }
   totalHeight += (vertMargin * 2) + ((numRows - 1) * vertSpace) +
                  (frameWidth() * 2);
   if (dbg) fprintf(stdout, "xTable::fitFrame():Calculating Total height = %d\n", totalHeight);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xTable::fitFrame():Resizing Frame\n");
   if (dbg) fflush(stdout);
   resize(totalWidth, totalHeight);
   recurse--;
   if (dbg) fprintf(stdout, "xTable::fitFrame():Exit\n");
   if (dbg) fflush(stdout);
}

int  xTable::getColWidth(int col)
{
   return(*colWidths.find((long)col));
}

int  xTable::getRowHeight(int row)
{
   return(*rowHeights.find((long)row));
}

QWidget *xTable::getWidget(int num)
{
   return(widgetList.at(num));
}

QWidget *xTable::getWidget(int col, int row)
{
   return(widgetList.at(row * numCols + col));
}
   
QListIterator<QWidget> xTable::widgetIt()
{
   QListIterator<QWidget> it(widgetList);
   return(it);
}

void xTable::gotMousePressed(xMouseEvent *pEvt)
{
   emit mousePressed(pEvt);
}

void xTable::gotMouseReleased(xMouseEvent *pEvt)
{
   emit mouseReleased(pEvt);
}

void xTable::gotMouseDoubleClicked(xMouseEvent *pEvt)
{
   emit mouseDoubleClicked(pEvt);
}

void xTable::gotMouseMoved(xMouseEvent *pEvt)
{
   emit mouseMoved(pEvt);
}

/*
void xTable::mousePressEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xTable::mousePressEvent():Enter\n");
   emit mousePressed(&evt);
   if (dbg) fprintf(stdout, "xTable::mousePressEvent():Exit\n");
}

void xTable::mouseReleaseEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xTable::mouseReleaseEvent():Enter\n");
   emit mouseReleased(&evt);
   if (dbg) fprintf(stdout, "xTable::mouseReleaseEvent():Exit\n");
}

void xTable::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xTable::mouseDoubleClickEvent():Enter\n");
   emit mouseDoubleClicked(&evt);
   if (dbg) fprintf(stdout, "xTable::mouseDoubleClickEvent():Exit\n");
}

void xTable::mouseMoveEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xTable::mouseEvent():Enter\n");
   emit mouseMoved(&evt);
   if (dbg) fprintf(stdout, "xTable::mouseEvent():Exit\n");
}
*/
//#include "xTable.moc"
