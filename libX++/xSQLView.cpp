/***************************************************************************
**    xSQLView.cpp  $Revision: 1.3 $ - $Name:  $ 
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
#include <qmessagebox.h>
#include "xLabel.h"
#include "xMessageBox.h"
#include "xSQLView.h"

static int dbg = 0;

xSQLView::xSQLView(xWidgetResInfo *pPRes, QWidget *pParent, 
                   const char *pName, int db, xSQLFieldDef *pFields,
                   const char *pTitleStr) :
          xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                             QString(""))),
                 xALIGN_Vert, xSPACE_Resize, 0, pParent, pName),
          xSQLBase(pFields)
{
   if (dbg) fprintf(stdout, "xSQLView::xSQLView():Enter\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xSQLView::xSQLView():Frame enabled = %d\n",
                            isEnabled());
   if (dbg) fflush(stdout);

   curRow = 1;
   setFrameStyle(QFrame::Panel | QFrame::Raised);
   setMargins(5, 5);

   pFrame = new xFrame(wdtRes, xALIGN_Horz, xSPACE_ToFit, 0, this, "FieldFrame");
   pLabelFrame = new xFrame(wdtRes, xALIGN_Vert, xSPACE_ToFit, 0, pFrame, "FieldLabels");
   pLabelFrame->setMargins(2, 5);
   pLabelFrame->setAutoDelete(TRUE);

   pFieldFrame = new xFrame(wdtRes, xALIGN_Vert, xSPACE_ToFit, 0, pFrame, "FieldEntries");
   pFieldFrame->setWidgetEqualize(FALSE);
   pFieldFrame->setMargins(2, 5);
   pFieldFrame->setAutoDelete(TRUE);
   pFieldFrame->setEnabled(isEnabled());
   pFrame->addWidget(pLabelFrame);
   pFrame->addWidget(pFieldFrame);

   if (pTitleStr != NULL)
   {
      pTitle = new QLabel(pTitleStr, this, "titleLabel");
      pTitle->setAlignment(AlignHCenter | AlignVCenter);
      pTitle->setPalette(palette());
      QFont f(font());
      f.setWeight(QFont::Bold);
      pTitle->setFont(f);
      pTitle->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTitle->adjustSize();
      addWidget(pTitle);
   }
   addWidget(pFrame);
   for (xSQLFieldDef *pf = pFields; pf->name != NULL; pf++)
   {
      xLabel *pLabel;
      xSQLField *pField;

      if (dbg) fprintf(stdout, "xSQLView::xSQLView():Have Field |%s|, type %d:%d\n",
                       pf->name, pf->type, pf->type & sqlHidden);
      if (dbg) fflush(stdout);
      if ((pf->type & sqlHidden) != 0)
      {
         if (dbg) fprintf(stdout, "xSQLView::xSQLView():Skipping field\n");
         if (dbg) fflush(stdout);
         continue;
      }

      if (dbg) fprintf(stdout, "xSQLView::xSQLView():Creating field\n");
      if (dbg) fflush(stdout);
      pField = new xSQLField(pPRes, pFieldFrame, this, db, pf);
      pFieldFrame->addWidget(pField);

      connect(pField, SIGNAL(newData(xSQLField*)),
              this, SLOT(haveNewData(xSQLField*)));

      connect(pField, SIGNAL(invalidEntry(xSQLField*)),
              this, SLOT(haveInvalidEntry(xSQLField*)));

      connect(pField, SIGNAL(entryAbandoned(xSQLField*)),
              this, SLOT(abandonEntry(xSQLField*)));

      connect(pField, SIGNAL(mousePressed(xMouseEvent*)),
              this, SLOT(haveMousePressed(xMouseEvent*)));

      connect(pField, SIGNAL(mouseDoubleClicked(xMouseEvent*)),
              this, SLOT(haveMouseDoubleClicked(xMouseEvent*)));

      pLabelFrame->addWidget((pLabel = new xLabel(pPRes, pLabelFrame)));
      pLabel->setAlignment(AlignRight | AlignVCenter);
      pLabel->setText(pf->label);
//      QFont f(font());
//      f.setWeight(QFont::Bold);
//      pLabel->setFont(f);
      pLabel->adjustSize();
      pLabel->resize(pLabel->width(), pField->height());
   }
   setEnabled(TRUE);
   fitFrame();
   setFocusPolicy(QWidget::StrongFocus);
   if (dbg) fprintf(stdout, "xSQLView::xSQLView():Exit\n");
   if (dbg) fflush(stdout);
}

xSQLView::~xSQLView()
{
   if (pLabelFrame)
      delete pLabelFrame;
   if (pFieldFrame)
      delete pFieldFrame;
}

void xSQLView::setEnabled(bool f)
{
   int x;
   xSQLField *pWdt;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);

   if (dbg) fprintf(stdout, "xSQLView::setEnabled():Enter\n");
   if (dbg) fflush(stdout);
   xFrame::setEnabled(f);
   for (x = 0; (pWdt = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
   {
      if (dbg) fprintf(stdout, "xSQLView::setEnabled():Field %d:|%s| is type %d\n",
                              x, pWdt->name(), pWdt->fieldType());
      if (dbg) fflush(stdout);
      if (f == FALSE || pWdt->isComputation() == FALSE)
      {
         if (dbg) fprintf(stdout, "xSQLView::setEnabled():%s field\n",
                          (f == TRUE) ? "Enabling" : "Disabling");
         if (dbg) fflush(stdout);
         pWdt->setEnabled(f);
         if (x == 0)
         {
            if (dbg) fprintf(stdout, "xSQLView::setEnabled():Setting Focus\n");
            if (dbg) fflush(stdout);
            pWdt->setFocus();
         }
         x++;
      }
   }
   if (dbg) fprintf(stdout, "xSQLView::setEnabled():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLView::setEnabled(const char *pFieldName, bool f)
{
   xSQLField *pWdt;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);

   if (f == TRUE)
      xFrame::setEnabled(f);
   for (; (pWdt = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
   {
      if (strcmp(pWdt->name(), pFieldName) == 0)
         if (f == FALSE || !pWdt->isComputation())
            pWdt->setEnabled(f);
   }
}

xSQLField *xSQLView::getWidget(const char *pFieldName)
{
   xSQLField *rv;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);

   if (dbg) fprintf(stdout, "xSQLView::getWidget(\"%s\"):Enter\n",
                            pFieldName);
   if (dbg) fflush(stdout);
   for (; (rv = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
   {
      if (dbg) fprintf(stdout, "xSQLView::getWidget():Testing Widget: |%s|\n",
                               rv->name());
      if (dbg) fflush(stdout);
      if (strcmp(rv->name(), pFieldName) == 0)
      {
         if (dbg) fprintf(stdout, "xSQLView::getWidget():Found Widget\n");
         if (dbg) fflush(stdout);
         break;
      }
   }
   if (dbg) fprintf(stdout, "xSQLView::getWidget(\"%s\"):Exit(%d)\n",
                            pFieldName, (int)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QDict<QString> xSQLView::getEditFields(bool blankFields)
{
   QDict<QString> rv(71);
   xSQLField *pField;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);

   if (dbg) fprintf(stdout, "xSQLView::getEditFields():Enter\n");
   if (dbg) fflush(stdout);
   for (; (pField = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
   {
      if (dbg) fprintf(stdout, "xSQLView::getEditFields():Testing field:%s\n",
                                pField->name());
      if (dbg) fflush(stdout);
      if (blankFields == TRUE ||
          (pField->text() != NULL && strlen(pField->text()) > 0))
      {
         QString *pStr = new QString(pField->text());
         if (dbg) fprintf(stdout, "xSQLView::getEditFields():Adding field:%s |%s|\n",
                                  pField->name(),
                                  (const char *)*pStr);
         if (dbg) fflush(stdout);
         rv.insert(pField->name(), pStr);
         if (dbg) fprintf(stdout, "xSQLView::getEditFields():Done w/ field\n");
         if (dbg) fflush(stdout);
      }
   }
   if (dbg) fprintf(stdout, "xSQLView::getEditFields():Exit\n");
   if (dbg) fflush(stdout);
   return(rv);
}

void xSQLView::clearEditFields()
{
   xSQLField *pField;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);

   for (; (pField = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
      pField->setText("");
}

QString xSQLView::getField(const char *pFieldName, bool editFirst)
{
   QString rv("");
   QWidget *pWdt;
   const QObjectList *fields = pFieldFrame->queryList("xSQLField",
                                                      pFieldName);
   QObjectListIt fieldIt(*fields);

   if (dbg) fprintf(stdout, "xSQLView::getField(\"%s\"):Enter\n", pFieldName);
   if (dbg) fflush(stdout);
   if ( editFirst == TRUE && (pWdt = (QWidget*)fieldIt.current()) != NULL)
   {
      if (dbg) fprintf(stdout, "xSQLView::getField():Found Widget\n");
      if (dbg) fflush(stdout);
      rv = ((xSQLField*)pWdt)->text();
      if (dbg) fprintf(stdout, "xSQLView::getField():Widget = |%s|\n",
                               (const char *)rv);
      if (dbg) fflush(stdout);
   }
   if (strlen(rv) == 0)
   {
      QDict<QString> dbData(71);
      if (dbg) fprintf(stdout, "xSQLView::getField():Getting Row Data\n");
      if (dbg) fflush(stdout);
      dbData = getRow(curRow);
      if (dbg) fprintf(stdout, "xSQLView::getField():Have Row Data\n");
      if (dbg) fflush(stdout);
      if (dbData.find(pFieldName) != NULL)
      {
         if (dbg) fprintf(stdout, "xSQLView::getField():Found Fieldname\n");
         if (dbg) fflush(stdout);
         rv = *(dbData.find(pFieldName));
      }
   }
   if (dbg) fprintf(stdout, "xSQLView::getField():Exit(\"%s\")\n",
                            (const char *)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

void xSQLView::showRow(int row)
{
   xSQLField *pObj;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);
   QDict<QString> results;
   QString *pData, strData;

   if (dbg) fprintf(stdout, "xSQLView::showRow():Enter\n");
   if (dbg) fflush(stdout);

   curRow = row;
   results = getRow(row);
   if (results.count())
   {
      for (; (pObj = (xSQLField *)fieldIt.current()) != NULL; ++fieldIt)
      {
         if (dbg) fprintf(stdout, "xSQLView::showRow():Trying to match |%s|\n",
                                  (const char *)pObj->name());
         if (dbg) fflush(stdout);
         strData = "";
         if ((pData = results.find(pObj->name())) != NULL)
         {
            strData = *pData;
            if (dbg) fprintf(stdout, "xSQLView::showRow():Showing %d type data |%s|\n",
                                     pObj->fieldType(), (const char *)*pData);
            if (dbg) fflush(stdout);
            if (pObj->fieldType() == sqlComputation ||
                pObj->fieldType() == sqlFloat ||
                pObj->fieldType() == sqlCurrency)
            {
               if (dbg) fprintf(stdout, "xSQLView::showRow():Right Justifying Data for %d columns!\n",
                                         pObj->editWidth());
               if (dbg) fflush(stdout);
               strData = pData->rightJustify(pObj->editWidth());
               if (dbg) fprintf(stdout, "xSQLView::showRow():String is now |%s|\n",
                                         (const char *)strData);
               if (dbg) fflush(stdout);
            }
         }
         if (dbg) fprintf(stdout, "xSQLView::showRow():Placing text into field |%s|\n",
                                   (const char *)strData);
         if (dbg) fflush(stdout);
         pObj->setText(strData);
//         ((xEdit*)pObj)->setText(strData);
      }
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLView::showRow():No Data!!\n");
      if (dbg) fflush(stdout);
   }
   if (dbg) fprintf(stdout, "xSQLView::showRow():Exit\n");
   if (dbg) fflush(stdout);
}

bool xSQLView::validate(QString &err)
{
   xSQLField *pField;
   const QObjectList *children = pFieldFrame->children();
   QObjectListIt fieldIt(*children);
   bool rv = TRUE;

   for (; (pField = (xSQLField*)fieldIt.current()) != NULL; ++fieldIt)
   {
      if (pField->valid() != TRUE)
      {
         rv = FALSE;
         break;
      }
   }
   if (rv != TRUE && pField != NULL)
      err = pField->name();
   return(rv);
}

bool xSQLView::validate(const char *pFieldName)
{
   xSQLField *pField;
   const QObjectList *fields = pFieldFrame->queryList("xSQLField",
                                                      pFieldName);
   QObjectListIt fieldIt(*fields);
   bool rv;

   if (dbg) fprintf(stdout, "xSQLView::validate(\"%s\"):Enter\n", pFieldName);
   if (dbg) fflush(stdout);
   if ((pField = (xSQLField*)fieldIt.current()) != NULL)
      rv = pField->valid();
   else
      rv = TRUE;
   if (dbg) fprintf(stdout, "xSQLView::validate(\"%s\"):Exit\n", pFieldName);
   if (dbg) fflush(stdout);
   return(rv);
}

bool xSQLView::validate(const char *pFieldName, const char *pData)
{
   xSQLField *pField;
   const QObjectList *fields = pFieldFrame->queryList("xSQLField",
                                                      pFieldName);
   QObjectListIt fieldIt(*fields);
   bool rv;

   if (dbg) fprintf(stdout, "xSQLView::validate(\"%s\", \"%s\"):Enter\n",
                            pFieldName, pData);
   if (dbg) fflush(stdout);
   if ((pField = (xSQLField*)fieldIt.current()) != NULL)
      rv = pField->validate(pData);
   else
      rv = TRUE;
   if (dbg) fprintf(stdout, "xSQLView::validate(\"%s\", \"%s\"):Exit\n",
                            pFieldName, pData);
   if (dbg) fflush(stdout);
   return(rv);
}

void xSQLView::haveNewData(xSQLField*)
{
   QString errField;

   if (dbg) fprintf(stdout, "xSQLView::haveNewData():Enter\n");
   if (dbg) fflush(stdout);
   if (validate(errField) == TRUE)
   {
      if (dbg) fprintf(stdout, "xSQLView::haveNewData():Emitting entryComplete()\n");
      if (dbg) fflush(stdout);
      emit entryComplete();
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLView::haveNewData():have error from field |%s|\n",
                                (const char*)errField);
      if (dbg) fflush(stdout);
//      getWidget(errField)->setFocus();
      if (dbg) fprintf(stdout, "xSQLView::haveNewData():Emitting invalidEntry()\n");
      if (dbg) fflush(stdout);
      emit invalidEntry(getWidget(errField));
   }
   if (dbg) fprintf(stdout, "xSQLView::haveNewData():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLView::haveInvalidEntry(xSQLField *pField)
{
   QString errStr;

   if (dbg) fprintf(stdout, "xSQLView::haveInvalidEntry():Enter\n");
   if (dbg) fflush(stdout);
//   pField->setFocus();
   errStr = "Invalid Entry in Field \"" + pField->label() + "\"";
   xMessageBox::message("Error", errStr, "Ok");
   if (dbg) fprintf(stdout, "xSQLView::haveInvalidEntry():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLView::abandonEntry(xSQLField*)
{
   if (dbg) fprintf(stdout, "xSQLView::abandonEntry():Enter\n");
   if (dbg) fflush(stdout);
   emit abortEntry();
   if (dbg) fprintf(stdout, "xSQLView::abandonEntry():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLView::haveMousePressed(xMouseEvent*)
{
   if (dbg) fprintf(stdout, "xSQLView::haveMouseDoubleClicked():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLView::haveMouseDoubleClicked():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLView::haveMouseDoubleClicked(xMouseEvent*)
{
   if (dbg) fprintf(stdout, "xSQLView::haveMouseDoubleClicked():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLView::haveMouseDoubleClicked():Exit\n");
   if (dbg) fflush(stdout);
}

//#include "xSQLView.moc"
