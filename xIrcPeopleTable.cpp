/***************************************************************************
**    xIrcPeopleTable.cpp  $Revision: 1.6 $ - $Name:  $ 
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
#include <stdio.h>
#include <qlist.h>
#include <qregexp.h>
#include <qlabel.h>
#include "xEdit.h"
#include "xIrcPeopleTable.h"

static int dbg = 0;

static const char *pInitialResources[] =
{
   NULL
};

xIrcPeopleBtns::xIrcPeopleBtns(xWidgetResInfo *pPRes, QWidget *pParent, 
                     const char *pName, const char *pFlagLabel) :
          xFrame((wdtRes = new xWidgetResInfo(pPRes, QString("peoplebtn"),
                                                     QString("PeopleBtns"))),
                  xALIGN_Horz, xSPACE_ToFit, 0, pParent, pName)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   setMargins(2, 3);
   setWidgetSpacing(1);
   setAutoDelete(TRUE);
   setAlignment(xALIGN_Horz);

   pFlagLabel++;
   pFlag   = new xCheckBox(wdtRes, this, NULL, "");
   connect(pFlag, SIGNAL(clicked()), this, SLOT(gotClicked()));
   pFlag->adjustSize();
   addWidget(pFlag);
   pByAddr = new xCheckBox(wdtRes, this, NULL, "By Addr.");
   connect(pByAddr, SIGNAL(clicked()), this, SLOT(gotClicked()));
   pByAddr->adjustSize();
   addWidget(pByAddr);
   fitFrame();
}

xIrcPeopleBtns::~xIrcPeopleBtns()
{
}
 
void xIrcPeopleBtns::gotClicked()
{
   emit clicked(this);
}

void xIrcPeopleBtns::setFlag(int f)
{
   if (f)
      pFlag->setChecked(TRUE);
   else
      pFlag->setChecked(FALSE);
   if (f > 0)
      pByAddr->setChecked(TRUE);
   else
      pByAddr->setChecked(FALSE);
}

int xIrcPeopleBtns::flag()
{
   return((pFlag->isChecked() == FALSE) ? 0 : (pByAddr->isChecked() == TRUE) ? 1 : -1);
}

void xIrcPeopleBtns::setEnabled(bool f)
{
   pFlag->setEnabled(f);
   pByAddr->setEnabled(f);
   xFrame::setEnabled(f);
}

xIrcPeopleTable::xIrcPeopleTable(xWidgetResInfo *pPRes, QWidget *pParent, 
                     const char *pName, const char *pFlagLabel, int rows) :
          xFrame((wdtRes = new xWidgetResInfo(pPRes, QString("people"),
                                                     QString("People"))),
                  xALIGN_Vert, xSPACE_Resize, 0, pParent, pName)
{
   int x;
   QLabel *pLabel;
   xIrcPeopleBtns *pBtnFrame;

   if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Enter\n");
   if (dbg) fflush(stdout);
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   QFontMetrics fm(fontMetrics());

   enbRow = -1;
   selRow = -1;
   curRow = 1;
   FlagLabel = pFlagLabel;
   connected = FALSE;

   setFrameStyle(QFrame::Panel | QFrame::Raised);
   setMargins(0, 0);

   if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Creating Field Table\n");
   if (dbg) fflush(stdout);
   pFrame = new xFrame(wdtRes, xALIGN_Horz, xSPACE_Resize, 0, this);
   pFrame->setMargins(0, 0);
   pFrame->setWidgetSpacing(0);

   pFrame1 = new xFrame(wdtRes, xALIGN_Vert, xSPACE_Resize, 0, pFrame);
   pFrame1->setMargins(0, 0);
   pFrame1->setWidgetSpacing(0);

   pTable1 = new xTable(wdtRes, pFrame1, "FieldEntries", 4, 1);
   pTable1->setMargins(0, 0);
   pTable1->setFont(font());
   pTable1->setRowHeight(fm.lineSpacing() + 10);
   pTable1->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->addWidget(pTable1);

   pTable = new xTable(wdtRes, pFrame1, "FieldEntries", 4, rows);
   pTable->setMargins(0, 0);
   pTable->setFont(font());
   pTable->setRowHeight(fm.lineSpacing() + 10);
   pTable->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->addWidget(pTable);

   if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Creating Fields\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Table Rows: %d\n",
                             pTable->rows());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Nick      ");
   //pLabel->setWidth(10);
   pTable1->setColWidth(0, pLabel->width());
   pTable->setColWidth(0, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Address Mask");
   //pLabel->setWidth(20);
   pTable1->setColWidth(1, pLabel->width());
   pTable->setColWidth(1, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText(FlagLabel);
   //pLabel->setWidth(9);
   pTable1->setColWidth(2, pLabel->width());
   pTable->setColWidth(2, pLabel->width());

   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("Message");
   //pLabel->setWidth(30);
   pTable1->setColWidth(3, pLabel->width());
   pTable->setColWidth(3, pLabel->width());

/*
   pTable1->addWidget((pLabel = new QLabel(pTable1)));
   pLabel->setText("By Addr");
   pLabel->adjustSize();
   pTable1->setColWidth(2, pLabel->width());
   pTable->setColWidth(2, pLabel->width());
*/
   for (x = 0; x < pTable->rows(); x++)
   {
      if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Row %d\n", x);
      if (dbg) fflush(stdout);
      pTable->addWidget((pLabel = new QLabel(pTable)));
      pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);

      pTable->addWidget((pLabel = new QLabel(pTable)));
      pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);

      pBtnFrame = new xIrcPeopleBtns(wdtRes, pTable, "", FlagLabel);
      pBtnFrame->fitFrame();
      pTable->addWidget(pBtnFrame);
      pBtnFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pBtnFrame->setEnabled(FALSE);
      connect(pBtnFrame, SIGNAL(clicked(xIrcPeopleBtns*)),
              this, SLOT(gotClicked(xIrcPeopleBtns*)));

      pLabel = new QLabel(pTable);
      pTable->addWidget(pLabel);
      pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
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

   pPeopleList = new xIrcPeopleList();

   setSB();
   showRows();

   if (dbg) fprintf(stdout, "xIrcPeopleTable::xIrcPeopleTable():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcPeopleTable::~xIrcPeopleTable()
{
   if (pTable)
      delete pTable;
   if (pTable)
      delete pTable;
}
   
void xIrcPeopleTable::gotClicked(xIrcPeopleBtns *pBtn)
{
   int x, row;
   xIrcPeopleListIterator si(*pPeopleList);

   if (dbg) fprintf(stdout, "xIrcPeopleTable::gotClicked():Enter\n");
   if (dbg) fflush(stdout);
   row = pTable->getRow(pBtn);
   if (dbg) fprintf(stdout, "xIrcPeopleTable::gotClicked():Row %d\n", row);
   if (dbg) fflush(stdout);
   for (x = 0; x < row; x++, ++si)
      ;
   if (dbg) fprintf(stdout, "xIrcPeopleTable::gotClicked():Setting Flag for nick: |%s|\n",
                           (const char *)si.current()->nick());
   if (dbg) fflush(stdout);
   si.current()->setFlag(pBtn->flag());
   if (dbg) fprintf(stdout, "xIrcPeopleTable::gotClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::clear()
{
   pPeopleList->clear();
   show();
}

void xIrcPeopleTable::readFile(const char *fn)
{
   pPeopleList->readFile(fn);

   setSB();
   showRows();
}

void xIrcPeopleTable::writeFile(const char *fn)
{
   if (dbg) fprintf(stdout, "xIrcPeopleTable::writeFile(%s):Enter\n", fn);
   if (dbg) fflush(stdout);
   pPeopleList->writeFile(fn);
   showRows();
   if (dbg) fprintf(stdout, "xIrcPeopleTable::writeFile(%s):Exit\n", fn);
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::add(xIrcPeopleEntry &entry)
{
   pPeopleList->add(entry);
}

void xIrcPeopleTable::remove(int r)
{
   if (r >= 0)
   {
      xIrcPeopleEntry e(*entry(r));
      pPeopleList->remove(&e);
      if ((unsigned int)curRow >= pPeopleList->count())
         curRow--;
      showRows(curRow);
   }
}

xIrcPeopleEntry *xIrcPeopleTable::entry(int r)
{
   return(pPeopleList->at(r));
}

xIrcPeopleEntry *xIrcPeopleTable::entry(xIrcMessage *pMsg)
{
   return(pPeopleList->entry(pMsg));
}

void xIrcPeopleTable::showRows(int row)
{
   int x, r;
   xIrcPeopleListIterator si(*pPeopleList);

   pTable->clearAll();
   curRow = row;
   if (dbg) fprintf(stdout, "xIrcPeopleTable::showRows():Enter\n");
   if (dbg) fflush(stdout);

   for (r = 0; r < pTable->rows(); r++)
      ((xIrcPeopleBtns*)pTable->getWidget(2, r))->setEnabled(FALSE);
   for (x = 0; x < row; x++, ++si)
      ;
   for (r = 0; r < pTable->rows() && si.current() != NULL; ++si)
   {
      ((QLabel*)pTable->getWidget(0, r))->setText(si.current()->nick());
      ((QLabel*)pTable->getWidget(1, r))->setText(si.current()->mask());
      ((xIrcPeopleBtns*)pTable->getWidget(2, r))->setFlag(si.current()->flag());
      ((xIrcPeopleBtns*)pTable->getWidget(2, r))->setEnabled(TRUE);
      ((QLabel*)pTable->getWidget(3, r))->setText(si.current()->message());
      if (selRow >= row && selRow < row + pTable->rows())
      {
         if (dbg) fprintf(stdout, "xIrcPeopleTable::showRows():selRow = %d\n", selRow);
         if (dbg) fprintf(stdout, "xIrcPeopleTable::showRows():r = %d\n", r);
         if (dbg) fprintf(stdout, "xIrcPeopleTable::showRows():row = %d\n", row);
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
   if (dbg) fprintf(stdout, "xIrcPeopleTable::showRows():Exit\n");
   if (dbg) fflush(stdout);

}
     
void xIrcPeopleTable::haveMousePressed(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMousePressed():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMouseClicked():r = %d\n",
                             r);
   highlightRow(r);
   emit rowClicked(r);
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMouseClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::haveMouseDoubleClicked(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMouseDoubleClicked():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMouseDoubleClicked():r = %d\n",
                             r);
   if (dbg) fflush(stdout);
   emit rowDoubleClicked(r);
   if (dbg) fprintf(stdout, "xIrcPeopleTable::haveMouseDoubleClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::highlightRow(int row)
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
      if ((unsigned int)row > pPeopleList->count())
         row = pPeopleList->count();
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
   if (r >= 0 && (unsigned int)r < pPeopleList->count())
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

void xIrcPeopleTable::highlightTopRow()
{
   highlightRow(curRow);
}

void xIrcPeopleTable::highlightPrev()
{
   int r;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():Enter\n");
   if (dbg) fflush(stdout);
   if ((selRow) > 0)
   {
      selRow--;
      if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      if (selRow < curRow)
      {
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():Showing selected row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         pSB->setValue(selRow);
      }
      r = selRow - curRow;
      if (r >= 0)
      {
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pTable->getWidget(0, r);
         pTable->highlightRowOf(pWdt);
      }
      else if (r < 0)
      {
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():Subracting Row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         scrollBack();
      }
   }
   if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightPrev():Exit\n");
}

void xIrcPeopleTable::highlightNext()
{
   int r = 1;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():selRow = %d\n", selRow);
   if (dbg) fflush(stdout);
   if ((unsigned int)selRow + 1 < pPeopleList->count())
   {
      selRow++;
      if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      r = selRow - curRow;
      if (r < pTable->rows())
      {
         if (r < 0)
         {
            if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Showing selected row\n");
            if (dbg) fflush(stdout);
            showRows(selRow);
            r = selRow - curRow;
         }
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pTable->getWidget(0, r);
         pTable->highlightRowOf(pWdt);
      }
      else
      {
         if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Scrolling 1 line forward\n");
         if (dbg) fflush(stdout);
         scrollFwd();
      }
   }
   if (dbg) fprintf(stdout, "xIrcPeopleTable::highlightNext():Exit\n");
}

void xIrcPeopleTable::scroll(int val)
{
   int row = 0;

   if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll(%d):Enter- selRow = %d, curRow = %d\n",
                             val, selRow, curRow);
   if (dbg) fflush(stdout);
   if (val != curRow && enbRow <= 0)
   {
      if (pPeopleList->count() > (unsigned int)pTable->rows()) // && curRow != val + 1)
      {
         row = val;
         if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll():1.row now = %d\n", row);
         if (dbg) fflush(stdout);
         if (row < 0)
            row = 0;
         else
         {
            if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll():Base.rows = %d, Table.rows = %d\n",
                                     pPeopleList->count(), pTable->rows());
            if (dbg) fflush(stdout);
            if ((unsigned int)row > (pPeopleList->count()) - 
                                    (unsigned int)(pTable->rows() - 1))
            {
               row = (pPeopleList->count()) - (unsigned int)(pTable->rows() - 1);
               if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll():2.row now = %d\n", row);
               if (dbg) fflush(stdout);
            }
         }
         if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll():calling showRows(%d)\n", row);
         if (dbg) fflush(stdout);
         showRows(row);
      }
   }
   if (dbg) fprintf(stdout, "xIrcPeopleTable::scroll():Exit- row = %d\n", row);
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::scrollBack()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
      pSB->setValue(curRow - 1);
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::scrollFwd()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Enter\n");
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
      pSB->setValue(curRow + 1);
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcPeopleTable::scrollPageBack()
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

void xIrcPeopleTable::scrollPageFwd()
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

void xIrcPeopleTable::setSB()
{
   xIrcPeopleListIterator si(*pPeopleList);
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

//#include "xIrcPeopleTable.moc"
