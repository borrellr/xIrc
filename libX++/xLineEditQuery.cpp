/***************************************************************************
**    xLineEditQuery.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box w/ xLineEdit and xPshBtnFrame widgets
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
#include "xResources.h"
#include "xLineEditQuery.h"

static int dbg = 0;

#define xLQ_Clear   101
#define xLQ_Default 102

xLineEditQuery::xLineEditQuery(xWidgetResInfo *pPRes, QWidget *pParent, 
                               const char *pName,
                               const char *pTag, xDefaults *pDef) :
               xDialog((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                                    QString(""))),
                        pParent, pName, TRUE)
{
   if (dbg) fprintf(stdout, "xLineEditQuery::xLineEditQuery():Default tag to %s\n", pTag);
   initClass(NULL, NULL, pParent, pName, pTag, pDef);
}

xLineEditQuery::xLineEditQuery(xWidgetResInfo *pPRes,
                               const char *pText, const char *pEdtText, 
                               QWidget *pParent, const char *pName,
                               const char *pTag, xDefaults *pDef) :
               xDialog((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                                    QString(""))),
                       pParent, pName, TRUE)
{
   if (dbg) fprintf(stdout, "xLineEditQuery::xLineEditQuery():Default tag to %s\n", pTag);
   initClass(pText, pEdtText, pParent, pName, pTag, pDef);
}

void xLineEditQuery::initClass(const char *pText, const char *pEdtText, 
                                QWidget *pParent, const char *pName,
                                const char *pTag, xDefaults *pDef)
{
   // Appease the Compiler Warnings
   pParent = pParent;
   pName = pName;

   if (dbg) fprintf(stdout, "xLineEditQuery::initClass():Enter\n");
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setFocusPolicy(QWidget::StrongFocus);
   if (pName)
      setCaption(pName);
      
   pEditField = new xLineEdit(wdtRes, this);
   pEditField->setFrameStyle(QFrame::Panel | QFrame::Raised);
   if (pEdtText)
   {
      if (dbg) fprintf(stdout, "xLineEditQuery::initClass():Setting Edit text\n");
      pEditField->setText(pEdtText);
   }
   if (pText)
   {
      if (dbg) fprintf(stdout, "xLineEditQuery::initClass():Setting Label text\n");
      pEditField->setLabel(pText);
   }
   tag="";
   if (dbg) fprintf(stdout, "xLineEditQuery::initClass():Setting default tag to %s\n", pTag);
   pDefaults = pDef;
   for (tag = ""; pTag && *pTag; pTag++)
      tag += *pTag;
   setDefault();
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("OK", QDialog::Accepted);
   pButtons->addButton("Cancel", QDialog::Rejected);
   pButtons->addButton("Clear", xLQ_Clear);
   if (pDefaults)
      pButtons->addButton("Default", xLQ_Default);
   
   
   addWidget(pEditField);
   addWidget(pButtons);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
   connect(pEditField, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
   if (dbg) fprintf(stdout, "xLineEditQuery::initClass():Exit\n");
}

void xLineEditQuery::setDefault()
{
   if (dbg) fprintf(stdout, "xLineEditQuery::setDefault():Enter- tag:%s = %s\n",
                            (const char *)tag, pDefaults->get((const char *)tag));
   if (pDefaults && pDefaults->get((const char *)tag))
      setText(pDefaults->get((const char *)tag));
   else
      setText("");
   if (dbg) fprintf(stdout, "xLineEditQuery::setDefault():Exit\n");
}
   
void xLineEditQuery::accept()
{
   QDialog::accept();
}

void xLineEditQuery::reject()
{
   QDialog::reject();
}

void xLineEditQuery::buttonClicked(int iBtn)
{
   if (iBtn == QDialog::Accepted)
      accept();
   else if (iBtn == QDialog::Rejected)
      reject();
   else if (iBtn == xLQ_Clear)
      setText("");
   else if (iBtn == xLQ_Default)
      setDefault();
}

void xLineEditQuery::gotReturn()
{
   if (dbg > 1) fprintf(stdout, "xLineEditQuery::gotReturn():Enter\n");
   QDialog::accept();
   if (dbg > 1) fprintf(stdout, "xLineEditQuery::Exit():Enter\n");
}

xLineEditQuery::~xLineEditQuery()
{
   if (dbg) fprintf(stdout, "xLineEditQuery::~xLineEditQuery():Deleting |%s|\n",  (name() != NULL) ? name() : "");
   if (dbg) fprintf(stdout, "xLineEditQuery::~xLineEditQuery():Disconnecting signals\n");
   if (dbg) fflush(stdout);
   disconnect(this);
   if (pEditField)
   {
      if (dbg) fprintf(stdout, "xLineEditQuery::~xLineEditQuery():Deleting pEditField\n");
      if (dbg) fflush(stdout);
      delete pEditField;
   }
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xLineEditQuery::~xLineEditQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
   if (dbg) fprintf(stdout, "xLineEditQuery::~xLineEditQuery():Done!\n");
   if (dbg) fflush(stdout);
}

//#include "xLineEditQuery.moc"
