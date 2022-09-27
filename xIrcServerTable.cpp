/***************************************************************************
**    xIrcServerTable.cpp  $Revision: 1.7 $ - $Name:  $ 
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
#include <stdio.h>
#include <qt.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qlabel.h>
#include "xLabel.h"
#include "xEdit.h"
#include "xIrcMircServerParse.h"
#include "xIrcServerTable.h"

static bool dbg = false;

static const char *pInitialResources[] =
{
   NULL
};

xIrcServerTable::xIrcServerTable(xWidgetResInfo *pPRes, QWidget *pParent, 
                     const char *pName, int rows) :
          xFrame((wdtRes = new xWidgetResInfo(pPRes, QString("servers"),
                                                     QString("Servers"))),
                  xALIGN_Vert, xSPACE_Resize, 0, pParent, pName)
{
   int x;
   QLabel *pLabel;
   xLabel *pLabel2;
   xEdit *pEdit;

   if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Enter\n");
   if (dbg) fflush(stdout);
   setAutoDelete(TRUE);
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   QFontMetrics fm(fontMetrics());

   enbRow = -1;
   selRow = -1;
   curRow = 1;
   connected = FALSE;
   setFrameStyle(QFrame::Panel | QFrame::Raised);
   setMargins(0, 0);

   groupMask = Resources->get(wdtRes, "mask.group", "Mask.Group");
   countryMask = Resources->get(wdtRes, "mask.country", "Mask.Country");
   stateMask = Resources->get(wdtRes, "mask.state", "Mask.State");
   cityMask = Resources->get(wdtRes, "mask.city", "Mask.City");
   serverMask = Resources->get(wdtRes, "mask.server", "Mask.Server");

   if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Creating Field Table\n");
   if (dbg) fflush(stdout);
   pFrame = new xFrame(wdtRes, xALIGN_Horz, xSPACE_Resize, 0, this);
   pFrame->setMargins(0, 0);
   pFrame->setWidgetSpacing(0);

   pFrame1 = new xFrame(wdtRes, xALIGN_Vert, xSPACE_Resize, 0, pFrame);
   pFrame1->setMargins(0, 0);
   pFrame1->setWidgetSpacing(0);

   pLabel1 = new QLabel("Server Masks", pFrame1);
   pLabel1->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pLabel1->setAlignment(AlignCenter);
   QFont f = pLabel1->font();
   f.setBold(TRUE);
   pLabel1->setFont(f);
   pFrame1->addWidget(pLabel1);

   pTable1 = new xTable(wdtRes, pFrame1, "FieldEntries", 5, 1);
   pTable1->setMargins(0, 0);
   pTable1->setFont(font());
   pTable1->setRowHeight(fm.lineSpacing() + 10);
   pTable1->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->addWidget(pTable1);

   pTable2 = new xTable(wdtRes, pFrame1, "FieldEntries", 5, 1);
   pTable2->setMargins(0, 0);
   pTable2->setFont(font());
   pTable2->setRowHeight(fm.lineSpacing() + 10);
   pTable2->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->addWidget(pTable2);

   pTable = new xTable(wdtRes, pFrame1, "FieldEntries", 5, rows);
   pTable->setMargins(0, 0);
   pTable->setFont(font());
   pTable->setRowHeight(fm.lineSpacing() + 10);
   pTable->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->addWidget(pTable);

   if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Creating Fields\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Table Rows: %d\n",
                             pTable->rows());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Group         ");
   pLabel->adjustSize();
   pTable1->setColWidth(0, pLabel->width());
   pTable2->setColWidth(0, pLabel->width());
   pTable->setColWidth(0, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Country      ");
   pLabel->adjustSize();
   pTable1->setColWidth(1, pLabel->width());
   pTable2->setColWidth(1, pLabel->width());
   pTable->setColWidth(1, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("State                      ");
   pLabel->adjustSize();
   pTable1->setColWidth(2, pLabel->width());
   pTable2->setColWidth(2, pLabel->width());
   pTable->setColWidth(2, pLabel->width());


   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("City                       ");
   pLabel->adjustSize();
   pTable1->setColWidth(3, pLabel->width());
   pTable2->setColWidth(3, pLabel->width());
   pTable->setColWidth(3, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Server                                       ");
   pLabel->adjustSize();
   pTable1->setColWidth(4, pLabel->width());
   pTable2->setColWidth(4, pLabel->width());
   pTable->setColWidth(4, pLabel->width());

   pTable2->addWidget((pEdit = new xEdit(wdtRes, pTable2, "")));
   pEdit->setFont(font());
   pEdit->setFocus();
   pEdit->setText(groupMask);
   connect(pEdit, SIGNAL(returnPressed()),
           this, SLOT(setNewMask()));

   pTable2->addWidget((pEdit = new xEdit(wdtRes, pTable2, "")));
   pEdit->setFont(font());
   pEdit->setText(countryMask);
   connect(pEdit, SIGNAL(returnPressed()),
           this, SLOT(setNewMask()));

   pTable2->addWidget((pEdit = new xEdit(wdtRes, pTable2, "")));
   pEdit->setFont(font());
   pEdit->setText(stateMask);
   connect(pEdit, SIGNAL(returnPressed()),
           this, SLOT(setNewMask()));

   pTable2->addWidget((pEdit = new xEdit(wdtRes, pTable2, "")));
   pEdit->setFont(font());
   pEdit->setText(cityMask);
   connect(pEdit, SIGNAL(returnPressed()),
           this, SLOT(setNewMask()));

   pTable2->addWidget((pEdit = new xEdit(wdtRes, pTable2, "")));
   pEdit->setFont(font());
   pEdit->setText(serverMask);
   connect(pEdit, SIGNAL(returnPressed()),
           this, SLOT(setNewMask()));

   for (x = 0; x < pTable->rows(); x++)
   {
      if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Row %d\n", x);
      if (dbg) fflush(stdout);
      pTable->addWidget((pLabel2 = new xLabel(wdtRes, pTable)));
      pLabel2->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTable->addWidget((pLabel2 = new xLabel(wdtRes, pTable)));
      pLabel2->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTable->addWidget((pLabel2 = new xLabel(wdtRes, pTable)));
      pLabel2->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTable->addWidget((pLabel2 = new xLabel(wdtRes, pTable)));
      pLabel2->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTable->addWidget((pLabel2 = new xLabel(wdtRes, pTable)));
      pLabel2->setFrameStyle(QFrame::Panel | QFrame::Raised);
   }
   pTable->setRowHeight(fm.lineSpacing() + 10);

   pSB = new QScrollBar(QScrollBar::Vertical, pFrame);
   QFontMetrics fmWin = fontMetrics();
   int widthSB = (int)((float)fmWin.maxWidth() * 1.5);
   pSB->resize(widthSB, (pFrame->geometry()).height());
   pSB->setValue(0);
   pSB->setRange(0, 0);

   pFrame->addWidget(pSB);
   connect(pTable, SIGNAL(mousePressed(xMouseEvent*)),
           this, SLOT(haveMousePressed(xMouseEvent*)));
   connect(pTable, SIGNAL(mouseDoubleClicked(xMouseEvent*)),
           this, SLOT(haveMouseDoubleClicked(xMouseEvent*)));

   pFrame->addWidget(pFrame1);
   pFrame->fitFrame();

   addWidget(pFrame);
   fitFrame();
   setFocusPolicy(StrongFocus);

   pServerList = new xIrcServerList();
   pServerList1 = new xIrcServerList();

   setSB();
   showRows();

   if (dbg) fprintf(stdout, "xIrcServerTable::xIrcServerTable():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcServerTable::~xIrcServerTable()
{
//   if (pTable)
//      delete pTable;
//   if (pTable)
//      delete pTable;
}
   
void xIrcServerTable::clear()
{
   pServerList->clear();
   newMask(TRUE);
}

void xIrcServerTable::readFile(const char *fn)
{
   pServerList->readFile(fn);

   newMask(TRUE);
   setSB();
   showRows();
}

void xIrcServerTable::writeFile(const char *fn)
{
   if (dbg) fprintf(stdout, "xIrcServerTable::writeFile(%s):Enter\n", fn);
   if (dbg) fflush(stdout);
   pServerList->writeFile(fn);
   showRows();
   if (dbg) fprintf(stdout, "xIrcServerTable::writeFile(%s):Exit\n", fn);
   if (dbg) fflush(stdout);
}

void xIrcServerTable::import(const char *fn)
{
   xIrcMircServerParse(fn, *pServerList);

   newMask(TRUE);
   setSB();
   showRows();
}

void xIrcServerTable::add(xIrcServerEntry &entry)
{
   int x = curRow;
   pServerList->add(entry);
   newMask(TRUE);
   curRow = x;
}

void xIrcServerTable::remove(int r)
{
   xIrcServerEntry e(*entry(r));
   pServerList->remove(&e);
   pServerList1->remove(&e);
   if ((unsigned int)curRow >= pServerList1->count())
      curRow--;
   showRows(curRow);
}
xIrcServerEntry *xIrcServerTable::entry(int r)
{
   return(pServerList1->at(r));
}

void xIrcServerTable::showRows(int row)
{
   int x, r;
   xIrcServerListIterator si(*pServerList1);

   pTable->clearAll();
   curRow = row;
   if (dbg) fprintf(stdout, "xIrcServerTable::showRows():Enter\n");
   if (dbg) fflush(stdout);

   for (x = 0; x < row; x++, ++si)
      ;
   for (r = 0; r < pTable->rows() && si.current() != NULL; ++si)
   {
      ((QLabel*)pTable->getWidget(0, r))->setText(si.current()->group());
      ((QLabel*)pTable->getWidget(1, r))->setText(si.current()->country());
      ((QLabel*)pTable->getWidget(2, r))->setText(si.current()->state());
      ((QLabel*)pTable->getWidget(3, r))->setText(si.current()->city());
      ((QLabel*)pTable->getWidget(4, r))->setText(si.current()->server());
      if (selRow >= row && selRow < row + pTable->rows())
      {
         if (dbg) fprintf(stdout, "xIrcServerTable::showRows():selRow = %d\n", selRow);
         if (dbg) fprintf(stdout, "xIrcServerTable::showRows():r = %d\n", r);
         if (dbg) fprintf(stdout, "xIrcServerTable::showRows():row = %d\n", row);
         if (dbg) fflush(stdout);
         if (r + row == selRow)
         {
            QWidget *pWdt = pTable->getWidget(0, r);
            pTable->highlightRowOf(pWdt);
         }
      }
      else
         pTable->clearHighlight();
      r++;
   }
   if (dbg) fprintf(stdout, "xIrcServerTable::showRows():Exit\n");
   if (dbg) fflush(stdout);

}
     
void xIrcServerTable::haveMousePressed(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMousePressed():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMouseClicked():r = %d\n",
                             r);
   highlightRow(r);
   emit rowClicked(r);
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMouseClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::haveMouseDoubleClicked(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMouseDoubleClicked():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMouseDoubleClicked():r = %d\n",
                             r);
   if (dbg) fflush(stdout);
   emit rowDoubleClicked(r);
   if (dbg) fprintf(stdout, "xIrcServerTable::haveMouseDoubleClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::highlightRow(int row)
{
   int r;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xSQLTable::highlightRow(%d):Enter- selRow = %d\n",
                             row, selRow);
   if (dbg) fflush(stdout);

   //
   // If the row to be highlighted is not in view, redisplay the data
   // with the highlighted row at the top
   //
   if (row < curRow || row >= curRow + pTable->rows())
   {
      if (row < 1) 
         row = 1;
      if ((unsigned int)row > pServerList->count())
         row = pServerList->count();
      if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Scrolling Line into View, row = %d, selRow = %d\n",
                               row, selRow);
      if (dbg) fflush(stdout);
      pSB->setValue(row - 1);
   }

   //
   // Calculate the row offset into the visible table to highlight
   //
   r = row - curRow;
   selRow = row;
   if (dbg) fprintf(stdout, "xSQLTable::highlightRow():selRow = %d, Row offset = %d\n", 
                            selRow, r);
   if (dbg) fflush(stdout);

   //
   // Only allow rows with valid data to be highlighted
   //
   if (r >= 0 && (unsigned int)r < pServerList->count())
   {
      if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Highlighting row %d\n",
                                r + 1);
      if (dbg) fflush(stdout);
      pWdt = pTable->getWidget(0, r);
      pTable->highlightRowOf(pWdt);
   }
   else
      pTable->clearHighlight();

   if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Exit\n");
}

void xIrcServerTable::highlightTopRow()
{
   highlightRow(curRow);
}

void xIrcServerTable::highlightPrev()
{
   int r;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():Enter\n");
   if (dbg) fflush(stdout);
   if ((selRow) > 0)
   {
      selRow--;
      if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      if (selRow < curRow)
      {
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():Showing selected row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         pSB->setValue(selRow);
      }
      r = selRow - curRow;
      if (r >= 0)
      {
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pTable->getWidget(0, r);
         pTable->highlightRowOf(pWdt);
      }
      else if (r < 0)
      {
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():Subracting Row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         scrollBack();
      }
   }
   if (dbg) fprintf(stdout, "xIrcServerTable::highlightPrev():Exit\n");
}

void xIrcServerTable::highlightNext()
{
   int r = 1;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():selRow = %d\n", selRow);
   if (dbg) fflush(stdout);
   if ((unsigned int)selRow + 1 < pServerList1->count())
   {
      selRow++;
      if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      r = selRow - curRow;
      if (r < pTable->rows())
      {
         if (r < 0)
         {
            if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Showing selected row\n");
            if (dbg) fflush(stdout);
            showRows(selRow);
            r = selRow - curRow;
         }
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pTable->getWidget(0, r);
         pTable->highlightRowOf(pWdt);
      }
      else
      {
         if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Scrolling 1 line forward\n");
         if (dbg) fflush(stdout);
         scrollFwd();
      }
   }
   if (dbg) fprintf(stdout, "xIrcServerTable::highlightNext():Exit\n");
}

void xIrcServerTable::scroll(int val)
{
   int row = 0;

   if (dbg) fprintf(stdout, "xIrcServerTable::scroll(%d):Enter- selRow = %d, curRow = %d\n",
                             val, selRow, curRow);
   if (dbg) fflush(stdout);
   if (val != curRow && enbRow <= 0)
   {
      if (pServerList->count() > (unsigned int)pTable->rows()) // && curRow != val + 1)
      {
         row = val;
         if (dbg) fprintf(stdout, "xIrcServerTable::scroll():1.row now = %d\n", row);
         if (dbg) fflush(stdout);
         if (row < 0)
            row = 0;
         else
         {
            if (dbg) fprintf(stdout, "xIrcServerTable::scroll():Base.rows = %d, Table.rows = %d\n",
                                     pServerList->count(), pTable->rows());
            if (dbg) fflush(stdout);
            if ((unsigned int)row > (pServerList->count()) - 
                                    (unsigned int)(pTable->rows() - 1))
            {
               row = (pServerList->count()) - (unsigned int)(pTable->rows() - 1);
               if (dbg) fprintf(stdout, "xIrcServerTable::scroll():2.row now = %d\n", row);
               if (dbg) fflush(stdout);
            }
         }
         if (dbg) fprintf(stdout, "xIrcServerTable::scroll():calling showRows(%d)\n", row);
         if (dbg) fflush(stdout);
         showRows(row);
      }
   }
   if (dbg) fprintf(stdout, "xIrcServerTable::scroll():Exit- row = %d\n", row);
   if (dbg) fflush(stdout);
}

void xIrcServerTable::scrollBack()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
      pSB->setValue(curRow - 1);
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::scrollFwd()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
      pSB->setValue(curRow + 1);
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::scrollPageBack()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageBack():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
   {
      pSB->setValue((curRow) - pTable->rows());
      selRow = curRow;
      highlightNext();
      highlightPrev();
   }
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageBack():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::scrollPageFwd()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageFwd():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
   {
      pSB->setValue((curRow) + pTable->rows());
      selRow = curRow;
      highlightNext();
      highlightPrev();
   }
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageFwd():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::setNewMask()
{
   newMask(FALSE);
}

void xIrcServerTable::newMask(bool force)
{
   if (dbg) fprintf(stdout, "xSqlTable::newMask():Enter\n");
   if (dbg) fflush(stdout);
   if (force == TRUE ||
       (groupMask != ((xEdit*)pTable2->getWidget(0, 0))->text()) ||
       (countryMask != ((xEdit*)pTable2->getWidget(1, 0))->text()) ||
       (stateMask != ((xEdit*)pTable2->getWidget(2, 0))->text()) ||
       (cityMask != ((xEdit*)pTable2->getWidget(3, 0))->text()) ||
       (serverMask != ((xEdit*)pTable2->getWidget(4, 0))->text()))
   {
      groupMask = ((xEdit*)pTable2->getWidget(0, 0))->text();
      countryMask = ((xEdit*)pTable2->getWidget(1, 0))->text();
      stateMask = ((xEdit*)pTable2->getWidget(2, 0))->text();
      cityMask = ((xEdit*)pTable2->getWidget(3, 0))->text();
      serverMask = ((xEdit*)pTable2->getWidget(4, 0))->text();

      xIrcServerEntry mask(
         ((xEdit*)pTable2->getWidget(0, 0))->text(),
         ((xEdit*)pTable2->getWidget(1, 0))->text(),
         ((xEdit*)pTable2->getWidget(2, 0))->text(),
         ((xEdit*)pTable2->getWidget(3, 0))->text(),
         ((xEdit*)pTable2->getWidget(4, 0))->text(),
         "*");
      xIrcServerList *list = new xIrcServerList(*pServerList, &mask);
      delete pServerList1;
      pServerList1 = list;
      setSB();
      showRows();
   }
   else
      emit returnPressed();
   if (dbg) fprintf(stdout, "xSqlTable::newMask():Enter\n");
   if (dbg) fflush(stdout);
}

void xIrcServerTable::setSB()
{
   xIrcServerListIterator si(*pServerList1);
   unsigned int r;

   for (r = 0; si.current() != NULL; ++si)
      r++;
   if (r > (unsigned int)pTable->rows())
   {
      pSB->setValue(0);
      pSB->setRange(0, r - pTable->rows());
      pSB->setSteps(1, pTable->rows());
      if (!connected)
      {
         connect(pSB, SIGNAL(valueChanged(int)),
                 this, SLOT(scroll(int)));
         connected = TRUE;
      }
   }
   else
   {
      disconnect(pSB, SIGNAL(valueChanged(int)),
                 this, SLOT(scroll(int)));
      connected = FALSE;
   }
   selRow = 0;
   curRow = 0;
}
