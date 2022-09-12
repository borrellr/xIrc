/***************************************************************************
**    xSQLTable.cpp  $Revision: 1.3 $ - $Name:  $
**    SQL View frame
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
#include <qlist.h>
//#include <qobjcoll.h>
#include <qobjectlist.h>
#include "xEdit.h"
#include "xLabel.h"
#include "xMessageBox.h"
#include "xSQLTable.h"

static int dbg = 0;

xSQLTable::xSQLTable(xWidgetResInfo *pPRes, QWidget *pParent,
                     const char *pName, int db, xSQLFieldDef *pFields,
                     const char *pTitleStr, int rows) :
          xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                              QString(""))),
                  xALIGN_Vert, xSPACE_Resize, 0, pParent, pName),
          xSQLBase(pFields)
{
   QFontMetrics fm(fontMetrics());
   int x, cols = 1;
   xSQLFieldDef *pf;

   if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Enter\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   enableMargin = 0;
   selRow = -1;
   enbRow = -1;
   curRow = 1;
   insMode = FALSE;
   insRow = -1;
   connected = FALSE;
   setFrameStyle(QFrame::Panel | QFrame::Raised);
   setMargins(0, 0);

   pFrame = new xFrame(wdtRes, xALIGN_Vert, xSPACE_Resize, 0, this, "FieldFrame");
   pFrame->setMargins(0, 0);

   for (pf = pFields, cols = 0; pf->name != NULL; pf++)
   {
      if ((pf->type & sqlHidden) == 0)
         cols++;
   }

   pLabelTable = new xTable(wdtRes, pFrame, "FieldLabels", cols, 1);
   pLabelTable->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pLabelTable->setMargins(0, 0);
   pLabelTable->setFont(font());
   pLabelTable->setRowHeight(fm.lineSpacing() + 10);
   if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():pLabelTable inherits xTable: %d\n",
                             pLabelTable->inherits("xTable"));
   if (dbg) fflush(stdout);

   pFrame->addWidget(pLabelTable);

   if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Creating Field Table\n");
   if (dbg) fflush(stdout);
   pFieldFrame = new xFrame(wdtRes, xALIGN_Horz, xSPACE_Resize, 0, pFrame);
   pFieldFrame->setMargins(0, 0);
   pFieldFrame->setWidgetSpacing(0);

   pFieldTable = new xTable(wdtRes, pFieldFrame, "FieldEntries", cols, rows);
   pFieldTable->setMargins(0, 0);
   pFieldTable->setFont(font());
   pFieldTable->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame->addWidget(pFieldFrame);
   pFieldFrame->addWidget(pFieldTable);

   if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Creating Fields\n");
   if (dbg) fflush(stdout);
   for (x = 0; x < rows; x++)
   {
      if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Row %d\n", x);
      if (dbg) fflush(stdout);
      for (pf = pFields; pf->name != NULL; pf++)
      {
         xLabel *pLabel = NULL;
         xSQLField *pField;

         if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Field |%s|\n",
                                  pf->name);
         if (dbg) fflush(stdout);

         if ((pf->type & sqlHidden) != 0)
            continue;

         pField = new xSQLField(pPRes, pFieldTable, this, db, pf);
         pField->setEnabled(FALSE);
         pFieldTable->addWidget(pField);
         connect(pField, SIGNAL(newData(xSQLField*)),
                 this, SLOT(haveNewData(xSQLField*)));
         connect(pField, SIGNAL(invalidEntry(xSQLField*)),
                 this, SLOT(haveInvalidEntry(xSQLField*)));
         connect(pField, SIGNAL(entryAbandoned(xSQLField*)),
                 this, SLOT(abandonEntry(xSQLField*)));

         if (x == 0)
         {
            if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Label |%s|\n",
                                     pf->label);
            pLabelTable->addWidget((pLabel = new xLabel(wdtRes, pLabelTable,
                                                        pField->name())));
            pLabel->setAlignment(AlignLeft | AlignVCenter);
            pLabel->setText(pf->label);
            pLabelTable->setColWidth(pLabelTable->children()->count() - 1,
                                     pField->width());
            if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Setting col width %d\n",
                                     width());
            pFieldTable->setColWidth(pLabelTable->children()->count() - 1,
                                     pField->width());
         }
      }
   }
   pFieldTable->setRowHeight(fm.lineSpacing() + 10);

   pSB = new QScrollBar(QScrollBar::Vertical, pFieldFrame);
   QFontMetrics fmWin = fontMetrics();
   int widthSB = (int)((float)fmWin.maxWidth() * 1.5);
   pSB->resize(widthSB, (pFieldFrame->geometry()).height());
   pSB->setValue(0);
   pSB->setRange(0, 0);

   pFieldFrame->addWidget(pSB);
   connect(pFieldTable, SIGNAL(mousePressed(xMouseEvent*)),
           this, SLOT(haveMousePressed(xMouseEvent*)));
   connect(pFieldTable, SIGNAL(mouseDoubleClicked(xMouseEvent*)),
           this, SLOT(haveMouseDoubleClicked(xMouseEvent*)));

   if (pTitleStr != NULL)
   {
      if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Creating Title\n");
      if (dbg) fflush(stdout);
      pTitle = new QLabel(pTitleStr, this, "titleLabel");
      pTitle->setPalette(palette());
      pTitle->setAlignment(AlignHCenter | AlignVCenter);
      QFont f(font());
      f.setWeight(QFont::Bold);
      pTitle->setFont(f);
      pTitle->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTitle->adjustSize();
      if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():pTitle inherits xTable: %d\n",
                                pTitle->inherits("xTable"));
      if (dbg) fflush(stdout);
      addWidget(pTitle);
   }
   else
      pTitle = NULL;
   addWidget(pFrame);
   fitFrame();
   setFocusPolicy(QWidget::StrongFocus);

   if (dbg) fprintf(stdout, "xSQLTable::xSQLTable():Exit\n");
   if (dbg) fflush(stdout);
}

xSQLTable::~xSQLTable()
{
   if (pLabelTable)
      delete pLabelTable;
   if (pFieldTable)
      delete pFieldTable;
}

bool xSQLTable::query(int db, QList<QString> sort)
{
   bool rv = xSQLBase::query(db, sort);
   setSB();
   return(rv);
}

bool xSQLTable::query(int db, QString index, QList<QString> sort)
{
   bool rv = xSQLBase::query(db, index, sort);
   setSB();
   return(rv);
}

bool xSQLTable::query(int db, long index, QList<QString> sort)
{
   bool rv = xSQLBase::query(db, index, sort);
   setSB();
   return(rv);
}

bool xSQLTable::query(int db, QDict<QString> &match, QDict<QString> &links,
                      QList<QString> sort)
{
   bool rv = xSQLBase::query(db, match, links, sort);
   setSB();
   return(rv);
}

bool xSQLTable::query(int db, QDict<QString> &match, QDict<QString> &links,
                      QDict<QString> &skip, QList<QString> sort)
{
   bool rv = xSQLBase::query(db, match, links, skip, sort);
   setSB();
   return(rv);
}

QDict<QString> xSQLTable::getRow()
{
   if (dbg) fprintf(stdout, "xSQLTable::getSelectedRow():Enter/Exit():selRow = %d\n",
                            selRow);
   if (dbg) fflush(stdout);
   return(xSQLBase::getRow(selRow));
}

void xSQLTable::clearEditFields()
{
   clearEditFields(selRow);
}

void xSQLTable::clearEditFields(int row)
{
   xSQLField *pField;

   if (dbg) fprintf(stdout, "xSQLTable::clearEditFields():Enter\n");
   if (dbg) fflush(stdout);
   for (int c = 0; c < pFieldTable->cols(); c++)
   {
      if (dbg) fprintf(stdout, "xSQLTable::clearEditFields():Getting widget at c = %d, r = %d\n",
                               c, row);
      if (dbg) fflush(stdout);
      pField = (xSQLField *)pFieldTable->getWidget(c, row - 1);
      if (pField != NULL &&
          pField->text() != NULL &&
          strlen(pField->text()) > 0)
         pField->setText("");
   }
}

void xSQLTable::clearEntryFields()
{
   selRow = -1;
   pFieldTable->clearHighlight();
   pFieldTable->clearAll();
}

/*
** This method will show the row of data in the results
** specified by row.
*/
void xSQLTable::showRows()
{
   if (dbg) fprintf(stdout, "xSQLTable::showRows():Enter, curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   showRows(curRow);
   if (dbg) fprintf(stdout, "xSQLTable::showRows():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLTable::showRows(int row)
{
   QObject *pObj;
   const QObjectList *children = pLabelTable->children();
   QObjectListIt LabelIt(*children);
   QDict<QString> results;
   QString *pData = NULL, strData;
   QWidget *pWdt;
   xSQLField *pField;
   int r, c;

   if (dbg) fprintf(stdout, "xSQLTable::showRows(%d):Enter\n", row);
   if (dbg) fflush(stdout);

   curRow = row;
   pFieldTable->clearAll();
   if (dbg) fprintf(stdout, "xSQLTable::showRows():Table has %d rows\n",
                            pFieldTable->rows());
   if (dbg) fflush(stdout);
   for (r = 0; r < pFieldTable->rows(); r++, row++)
   {
      if (insMode && row == insRow)
      {
         for (c = 0; c < pFieldTable->cols(); c++)
         {
            ((xSQLField *)pFieldTable->getWidget(c, r))->setText("");
            ((xSQLField *)pFieldTable->getWidget(c, r))->setEnabled(TRUE);
         }
      }
      else
      {
         results = xSQLBase::getRow(row);
         if (dbg) fprintf(stdout, "xSQLTable::showRows():Have %d results from xSQLBase::getRow()\n",
                                  results.count());
         if (dbg) fflush(stdout);
         if (results.count())
         {
            LabelIt.toFirst();
            for (c = 0; (pObj = LabelIt.current()) != NULL; ++LabelIt, c++)
            {
               if (dbg > 2) fprintf(stdout, "xSQLTable::showRows():Trying to match |%s|\n",
                                            (const char *)pObj->name());
               if (dbg) fflush(stdout);
               if (pObj->name() != NULL)
               {
                  pField = (xSQLField*)pFieldTable->getWidget(c, r);
                  strData = "";
                  if ((pData = results.find(pObj->name())) != NULL)
                  {
                     strData = *pData;
                     if (dbg) fprintf(stdout, "xSQLTable::showRows():Found Field for col |%s| = |%s|, r = %d, c = %d\n",
                                                  (const char *)pObj->name(),
                                                  (const char *)*pData,
                                                  r, c);
                     if (dbg) fflush(stdout);
                     if (pField->fieldType() == sqlComputation ||
                         pField->fieldType() == sqlFloat ||
                         pField->fieldType() == sqlCurrency)
                     {
                        if (dbg) fprintf(stdout, "xSQLTable::showRow():Right Justifying Data for %d columns!\n",
                                                  pField->editWidth());
                        if (dbg) fflush(stdout);
                        strData = pData->rightJustify(pField->editWidth());
                        if (dbg) fprintf(stdout, "xSQLTable::showRow():String is now |%s|\n",
                                                  (const char *)strData);
                        if (dbg) fflush(stdout);
                     }
                  }
                  if (pField != NULL)
                  {
                     if (dbg) fprintf(stdout, "xSQLTable::showRow():Setting field to |%s|\n",
                                               (const char *)strData);
                     if (dbg) fflush(stdout);
                     pField->setText(strData);
                  }
               }
            }
         }
         else
         {
            if (dbg) fprintf(stdout, "xSQLTable::showRows():No More Data!!\n");
            break;
         }
      }
   }
   if (selRow > 0 &&
       selRow >= curRow &&
       selRow < curRow + pFieldTable->rows())
   {
      if (dbg) fprintf(stdout, "xSQLTable::showRows():Highlighting Row %d\n",
                       selRow);
      if (dbg) fflush(stdout);
      pWdt = pFieldTable->getWidget(0, selRow - curRow);
      pFieldTable->highlightRowOf(pWdt);
   }
   else
      pFieldTable->clearHighlight();

   if (enbRow > 0 &&
       enbRow >= curRow &&
       enbRow < curRow + pFieldTable->rows())
   {
      if (dbg) fprintf(stdout, "xSQLTable::showRows():Enabling Row %d\n",
                       enbRow);
      enableRow(enbRow, TRUE);
   }
   if (dbg) fprintf(stdout, "xSQLTable::showRows():Exit\n");
   if (dbg) fflush(stdout);
}

long xSQLTable::insertBlank(int db, int row)
{
   long rv;
   if (insMode != TRUE)
   {
      selRow = row;
      insMode = TRUE;
      insRow = row;
      showRows(curRow);
      rv = -1;
   }
   else
   {
      QDict<QString> fields = getEditFields();
      rv = insert(db, fields);
      insMode = FALSE;
   }
   return(rv);
}

bool xSQLTable::enableRow(int row, bool f)
{
   if (dbg) fprintf(stdout, "xSQLTable::enableRow(%d):Enter\n", f);
   if (dbg) fflush(stdout);
   if (f == FALSE || (row > 0 && row <= xSQLBase::rows() + enableMargin))
   {
      if (dbg) fprintf(stdout, "xSQLTable::enableRow():curRow = %d\n", curRow);
      if (dbg) fflush(stdout);
      int r = row - curRow;
      if (f == TRUE && enbRow > 0)
         enableRow(enbRow, FALSE);

      if (f == TRUE)
      {
         selRow = row;
         enbRow = row;
      }
      else
         enbRow = -1;
      if (dbg) fprintf(stdout, "xSQLTable::enableRow():Enabling/Disabling Row %d:%d\n", row, r);
      if (dbg) fflush(stdout);
      if (r >= pFieldTable->rows())
      {
         if (dbg) fprintf(stdout, "xSQLTable::enableRow():Setting ScrollBar to %d\n",
                                  curRow + (r - pFieldTable->rows()));
         if (dbg) fflush(stdout);
         pSB->setValue(curRow + (r - pFieldTable->rows()));
         r = row - curRow;
         if (dbg) fprintf(stdout, "xSQLTable::enableRow():Retry- Enabling/Disabling Row %d:%d\n",
                                  row, r);
         if (dbg) fflush(stdout);
      }
      for (int c = 0; c < pFieldTable->cols(); c++)
      {
         xSQLField *pField;
         pField = ((xSQLField*)pFieldTable->getWidget(c, r));
         if (!pField->isComputation())
         {
            pField->setEnabled(f);
            if (c == 0)
               pField->setFocus();
         }
      }
      if (dbg) fprintf(stdout, "xSQLTable::enableRow():Exit(%d)\n", TRUE);
      if (dbg) fflush(stdout);
      return(TRUE);
   }
   if (dbg) fprintf(stdout, "xSQLTable::enableRow():Exit(%d)\n", TRUE);
   if (dbg) fflush(stdout);
   return(FALSE);
}

void xSQLTable::setFrameStyles(int enabled, int disabled)
{
   xEdit *pWdt;
   const QObjectList *children = pFieldTable->children();
   QObjectListIt fieldIt(*children);

   for (; (pWdt = (xEdit*)fieldIt.current()) != NULL; ++fieldIt)
      pWdt->setFrameStyles(enabled, disabled);
}

xSQLField *xSQLTable::getWidget(const char *pFieldName)
{
   xSQLField *rv = NULL;
   xLabel *pLabel;
   const QObjectList *children = pLabelTable->children();
   QObjectListIt fieldIt(*children);
   int x, r;

   if (dbg) fprintf(stdout, "xSQLTable::getWidget(\"%s\"):Enter\n",
                            pFieldName);
   if (dbg) fflush(stdout);
   for (x = 0;
        (pLabel = (xLabel*)fieldIt.current()) != NULL; ++fieldIt, x++)
   {
      if (dbg) fprintf(stdout, "xSQLTable::getField():Testing Widget: |%s|\n",
                               pLabel->name());
      if (dbg) fflush(stdout);
      if (strcmp(pLabel->name(), pFieldName) == 0)
      {
         if (dbg) fprintf(stdout, "xSQLTable::getField():Found Widget\n");
         if (dbg) fflush(stdout);
         if ((r = selRow) <= 0)
         {
            r = 1;
            fprintf(stderr, "No row selected for table %s, assuming row 1\n",
                            name());
         }
         r = selRow - curRow;
         rv = (xSQLField*)pFieldTable->xTable::getWidget(x, r);
         break;
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::getWidget(\"%s\"):Exit(%d)\n",
                            pFieldName, (int)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QString xSQLTable::getField(const char *pFieldName, bool editFirst)
{
   QString rv("");
   xSQLField *pField;

   if (dbg) fprintf(stdout, "xSQLTable::getField(\"%s\"):Enter\n", pFieldName);
   if (dbg) fflush(stdout);

   if ( editFirst == TRUE && (pField = getWidget(pFieldName)) != NULL)
   {
      if (dbg) fprintf(stdout, "xSQLTable::getField():Found Widget\n");
      if (dbg) fflush(stdout);
      rv = pField->text();
      if (dbg) fprintf(stdout, "xSQLTable::getField():Widget = |%s|\n",
                               (const char *)rv);
      if (dbg) fflush(stdout);
   }
   if (strlen(rv) == 0)
   {
      QDict<QString> dbData(71);
      if (dbg) fprintf(stdout, "xSQLTable::getField():Getting Row Data\n");
      if (dbg) fflush(stdout);
      dbData = getRow();
      if (dbg) fprintf(stdout, "xSQLTable::getField():Have Row Data\n");
      if (dbg) fflush(stdout);
      if (dbData.find(pFieldName) != NULL)
      {
         if (dbg) fprintf(stdout, "xSQLTable::getField():Found Fieldname\n");
         if (dbg) fflush(stdout);
         rv = *(dbData.find(pFieldName));
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::getField():Exit(\"%s\")\n",
                            (const char *)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QDict<QString> xSQLTable::getEditFields(bool blankFields)
{
   QDict<QString> rv(71);
   xSQLField *pField;
   int row = selRow - curRow;
   QString *pStr;

   if (dbg) fprintf(stdout, "xSQLTable::getEditFields():Enter\n");
   if (dbg) fflush(stdout);
   for (int c = 0; c < pFieldTable->cols(); c++)
   {
      if (dbg) fprintf(stdout, "xSQLTable::getEditFields():Getting widget at c = %d, r = %d\n",
                               c, row);
      if (dbg) fflush(stdout);
      pField = (xSQLField *)pFieldTable->getWidget(c, row);
      if (pField != NULL &&
          (blankFields == TRUE ||
          (pField->text() != NULL && strlen(pField->text()) > 0)))
      {
         pStr = new QString(pField->text());
         if (dbg) fprintf(stdout, "xSQLTable::getEditFields():Adding field:%s |%s|\n",
                                  pField->name(),
                                  (const char *)*pStr);
         if (dbg) fflush(stdout);
         rv.insert(pField->name(), pStr);
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::getEditFields():Exit()\n");
   if (dbg) fflush(stdout);
   return(rv);
}

bool xSQLTable::validate(QString &err)
{
   int row = selRow - curRow;
   xSQLField *pField = NULL;
   bool rv = TRUE;

   for (int c = 0; c < pFieldTable->cols(); c++)
   {
      pField = (xSQLField*)pFieldTable->getWidget(c, row);
      if (pField && pField->isEnabled())
      {
         if (pField->valid() != TRUE)
         {
            rv = FALSE;
            break;
         }
      }
   }
   if (rv != TRUE && pField != NULL)
      err = pField->name();
   return(rv);
}

bool xSQLTable::validate(const char *pFieldName)
{
   int row = selRow - curRow;
   xSQLField *pField;
   bool rv = TRUE;

   for (int c = 0; c < pFieldTable->cols(); c++)
   {
      pField = (xSQLField*)pFieldTable->getWidget(c, row);
      if (pField && pField->isEnabled() &&
          strcmp(pField->name(), pFieldName) == 0)
      {
         if ((rv = pField->valid()) != TRUE)
            break;
      }
   }
   return(rv);
}

bool xSQLTable::validate(const char *pFieldName, const char *pData)
{
   xSQLField *pField;
   const QObjectList *fields = pFieldTable->queryList("xSQLField",
                                                      pFieldName);
   QObjectListIt fieldIt(*fields);
   bool rv;

   if (dbg) fprintf(stdout, "xSQLTable::validate(\"%s\", \"%s\"):Enter\n",
                            pFieldName, pData);
   if (dbg) fflush(stdout);
   if ((pField = (xSQLField*)fieldIt.current()) != NULL)
      rv = pField->validate(pData);
   else
      rv = TRUE;
   if (dbg) fprintf(stdout, "xSQLTable::validate(\"%s\", \"%s\"):Exit\n",
                            pFieldName, pData);
   if (dbg) fflush(stdout);
   return(rv);
}

void xSQLTable::haveMousePressed(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xSQLTable::haveMousePressed():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pFieldTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xSQLTable::haveMouseClicked():r = %d\n",
                             r);
   highlightRow(r);
   emit rowClicked(r);
   if (dbg) fprintf(stdout, "xSQLTable::haveMouseClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLTable::haveMouseDoubleClicked(xMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xSQLTable::haveMouseDoubleClicked():Enter- curRow = %d\n",
                             curRow);
   if (dbg) fflush(stdout);
   int r = pFieldTable->getRow(pEvt->widget()) + curRow;
   if (dbg) fprintf(stdout, "xSQLTable::haveMouseDoubleClicked():r = %d\n",
                             r);
   if (dbg) fflush(stdout);
   emit rowDoubleClicked(r);
   if (dbg) fprintf(stdout, "xSQLTable::haveMouseDoubleClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLTable::highlightRow(int row)
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
   if (row < curRow || row >= curRow + pFieldTable->rows())
   {
      if (row < 1)
         row = 1;
      if (row > xSQLBase::rows() + enableMargin)
         row = xSQLBase::rows() + enableMargin;
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
   if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Row offset = %d\n", r);
   if (dbg) fflush(stdout);

   //
   // Only allow rows with valid data to be highlighted
   //
   if (r >= 0 && r < xSQLBase::rows() + enableMargin)
   {
      if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Highlighting row %d\n",
                                r + 1);
      if (dbg) fflush(stdout);
      pWdt = pFieldTable->getWidget(0, r);
      pFieldTable->highlightRowOf(pWdt);
   }
   else
      pFieldTable->clearHighlight();

   if (dbg) fprintf(stdout, "xSQLTable::highlightRow():Exit\n");
}

void xSQLTable::highlightTopRow()
{
   highlightRow(curRow);
}

void xSQLTable::highlightPrev()
{
   int r;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():Enter\n");
   if (dbg) fflush(stdout);
   if ((selRow - 1) > 0)
   {
      selRow--;
      if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      if (selRow < curRow)
      {
         if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():Showing selected row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         pSB->setValue(selRow - 1);
      }
      r = selRow - curRow;
      if (r >= 0)
      {
         if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pFieldTable->getWidget(0, r);
         pFieldTable->highlightRowOf(pWdt);
      }
      else if (r < 0)
      {
         if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():Subracting Row (call to scroll()!!!)\n");
         if (dbg) fflush(stdout);
         scrollBack();
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::highlightPrev():Exit\n");
}

void xSQLTable::highlightNext()
{
   int r = 1;
   QWidget *pWdt;

   if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLTable::highlightNext():selRow = %d, fieldTable.rows() = %d\n",
                            selRow, xSQLBase::rows());
   if (dbg) fflush(stdout);
   if (selRow + 1 <= xSQLBase::rows() + enableMargin)
   {
      selRow++;
      if (dbg) fprintf(stdout, "xSQLTable::highlightNext():curRow = %d, selRow = %d\n",
                                curRow, selRow);
      if (dbg) fflush(stdout);
      r = selRow - curRow;
      if (r < pFieldTable->rows())
      {
         if (r < 0)
         {
            if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Showing selected row\n");
            if (dbg) fflush(stdout);
            showRows(selRow);
            r = selRow - curRow;
         }
         if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Highlighting row %d\n",
                                   r);
         if (dbg) fflush(stdout);
         pWdt = pFieldTable->getWidget(0, r);
         pFieldTable->highlightRowOf(pWdt);
      }
      else
      {
         if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Scrolling 1 line forward\n");
         if (dbg) fflush(stdout);
         scrollFwd();
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::highlightNext():Exit\n");
}

void xSQLTable::scroll(int val)
{
   int row = 1;

   if (dbg) fprintf(stdout, "xSQLTable::scroll(%d):Enter- selRow = %d, curRow = %d\n",
                             val, selRow, curRow);
   if (dbg) fflush(stdout);
   if (enbRow <= 0)
   {
      if (xSQLBase::rows() + enableMargin > pFieldTable->rows() &&
          curRow != val + 1)
      {
         row = val + 1;
         if (dbg) fprintf(stdout, "xSQLTable::scroll():1.row now = %d\n", row);
         if (dbg) fflush(stdout);
         if (row < 1)
            row = 1;
         else
         {
            if (dbg) fprintf(stdout, "xSQLTable::scroll():Base.rows = %d, Table.rows = %d\n",
                                     xSQLBase::rows(), pFieldTable->rows());
            if (dbg) fflush(stdout);
            if (row > (xSQLBase::rows() + enableMargin) -
                      (pFieldTable->rows() - 1))
            {
               row = (xSQLBase::rows() + enableMargin) -
                     (pFieldTable->rows() - 1);
               if (dbg) fprintf(stdout, "xSQLTable::scroll():2.row now = %d\n", row);
               if (dbg) fflush(stdout);
            }
         }
         if (dbg) fprintf(stdout, "xSQLTable::scroll():calling showRows(%d)\n", row);
         if (dbg) fflush(stdout);
         showRows(row);
      }
   }
   if (dbg) fprintf(stdout, "xSQLTable::scroll():Exit- row = %d\n", row);
   if (dbg) fflush(stdout);
}

void xSQLTable::scrollBack()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Enter\n");
   if (enbRow <= 0)
      pSB->setValue(curRow - 2);
   if (dbg) fprintf(stdout, "xSqlTable::scrollBack():Exit\n");
}

void xSQLTable::scrollFwd()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Enter, enbRow = %d\n", enbRow);
   if (enbRow <= 0)
      pSB->setValue(curRow);
   if (dbg) fprintf(stdout, "xSqlTable::scrollFwd():Exit\n");
}

void xSQLTable::scrollPageBack()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageBack():Enter\n");
   if (enbRow <= 0)
      pSB->setValue((curRow - 1) - pFieldTable->rows());
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageBack():Exit\n");
}

void xSQLTable::scrollPageFwd()
{
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageFwd():Enter\n");
   if (enbRow <= 0)
      pSB->setValue((curRow - 1) + pFieldTable->rows());
   if (dbg) fprintf(stdout, "xSqlTable::scrollPageFwd():Exit\n");
}

void xSQLTable::setSB()
{
   if (dbg) fprintf(stdout, "xSqlTable::setSB():Enter\n");
   if (dbg) fflush(stdout);
   if (connected || xSQLBase::rows() + enableMargin > pFieldTable->rows())
   {
      pSB->setValue(0);
      pSB->setRange(0, (xSQLBase::rows() + enableMargin) -
                       pFieldTable->rows());
      pSB->setSteps(1, pFieldTable->rows());
      if (!connected)
      {
         if (dbg) fprintf(stdout, "xSqlTable::setSB():Connecting scrollbar\n");
         if (dbg) fflush(stdout);
         connect(pSB, SIGNAL(valueChanged(int)),
                 this, SLOT(scroll(int)));
      }
      connected = TRUE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSqlTable::setSB():Disconnecting scrollbar\n");
      if (dbg) fflush(stdout);
      disconnect(pSB, SIGNAL(valueChanged(int)),
                 this, SLOT(scroll(int)));
      connected = FALSE;
   }
   selRow = 1;
   if (dbg) fprintf(stdout, "xSqlTable::setSB():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLTable::abandonEntry(xSQLField* pField)
{
   int r = pFieldTable->getRow(pField) + curRow;

   if (dbg) fprintf(stdout, "xSQLTable::abandonEntry(%s):Enter, row = %d, curRow = %d\n",
                            name(), r, curRow);
   if (dbg) fflush(stdout);
   emit abortEntry(r);
   if (dbg) fprintf(stdout, "xSQLTable::abandonEntry():Exit, row = %d\n",
                            r);
   if (dbg) fflush(stdout);
}

void xSQLTable::haveNewData(xSQLField* pField)
{
   int r = pFieldTable->getRow(pField) + curRow;
   QString errField;

   if (dbg) fprintf(stdout, "xSQLTable::haveNewData():Enter\n");
   if (dbg) fflush(stdout);
   if (validate(errField) == TRUE)
   {
      if (dbg) fprintf(stdout, "xSQLTable::haveNewData():Emitting entryComplete()\n");
      if (dbg) fflush(stdout);
      emit entryComplete(r);
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLTable::haveNewData():have error from field |%s|\n",
                                (const char*)errField);
      if (dbg) fflush(stdout);
//      getWidget(errField)->setFocus();
      if (dbg) fprintf(stdout, "xSQLTable::haveNewData():Emitting invalidEntry()\n");
      if (dbg) fflush(stdout);
      emit invalidEntry(getWidget(errField));
   }
   if (dbg) fprintf(stdout, "xSQLTable::haveNewData():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLTable::haveInvalidEntry(xSQLField *pField)
{
   QString errStr;

   if (dbg) fprintf(stdout, "xSQLTable::haveInvalidEntry():Enter\n");
   if (dbg) fflush(stdout);
//   pField->setFocus();
   errStr = "Invalid Entry in Field \"" + pField->label() + "\"";
   xMessageBox::message("Error", errStr, "Ok");
   if (dbg) fprintf(stdout, "xSQLTable::haveInvalidEntry():Exit\n");
   if (dbg) fflush(stdout);
}

//#include "xSQLTable.moc"
