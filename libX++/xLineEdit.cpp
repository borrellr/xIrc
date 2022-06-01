/***************************************************************************
**    xLineEdit.cpp  $Revision: 1.2 $ - $Name:  $ 
**    xFrame w/ xEdit and QLabel widgets
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
#include <ctype.h>
#include "xLineEdit.h"

static int dbg = 0;

static const char *pInitialResources[] =
{
   "*Label.Font.Weight: Bold",
   NULL
};

xLineEdit::xLineEdit(xWidgetResInfo *pPRes, QWidget *pParent, 
                     const char *pName, int alignLabel) :
           xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                               QString(""))),
                  pParent, pName)
{
   if (dbg) fprintf(stdout, "xLineEdit::xLineEdit():Enter\n\r");
   wdtResPrv = new xWidgetResInfo(pPRes);
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   eWidth = 9;

   setResizeMode(xSPACE_Resize);
   setAlignment(alignLabel);
   setFocusPolicy(QWidget::StrongFocus);
//   setFont(QFont("helvetica", 10, QFont::Bold));

   pEdit = new xEdit(wdtResPrv, this, pName);
   pEdit->setWidth(eWidth);

   if (dbg) fprintf(stdout, "xLineEdit::xLineEdit():Edit Width = %d, height = %d\n",
                            pEdit->width(), pEdit->height());

   pLabel = new QLabel(pName, this);
   setDefFont(pLabel, wdtRes);
//   pLabel->setFont(font());
   addWidget(pLabel);
   addWidget(pEdit);
   setWidgetSpacing((alignLabel == xALIGN_Vert) ? 0 : 3);
   setMargins(5, 5);
   fitFrame();
   if (dbg) fprintf(stdout, "xLineEdit::xLineEdit():After fitting- Edit Width = %d, height = %d\n",
                            pEdit->width(), pEdit->height());
   connect(pEdit, SIGNAL(returnPressed()), this, SLOT(gotReturn()));
   connect(pEdit, SIGNAL(escapePressed()), this, SLOT(gotEscape()));
   if (dbg) fprintf(stdout, "xLineEdit::xLineEdit():Exit\n\r");
}

xLineEdit::~xLineEdit()
{
   if (pEdit)
      delete pEdit;
   if (pLabel)
      delete pLabel;
}

void xLineEdit::setLabel(const char *pText)
{
   if (dbg) fprintf(stdout, "xLineEdit::setLabel():Enter\n\r");
   pLabel->setText(pText);
   pLabel->adjustSize();
   if (dbg) fprintf(stdout, "xLineEdit::setLabel():size w=%d X h=%d\n\r",
                            pLabel->size().width(), pLabel->size().height());
   if (dbg) fprintf(stdout, "xLineEdit::setLabel():Exit\n\r");
}

void xLineEdit::fitFrame(QSize *frameSize)
{
   xFrame::fitFrame(frameSize);
}

void xLineEdit::setEnabled(bool f)
{
   xFrame::setEnabled(f);
   pEdit->setEnabled(f);
   pLabel->setEnabled(f);
}

void xLineEdit::setEditWidth(int width)
{
   eWidth = width;
   pEdit->setWidth(width);
   fitFrame();
}

int xLineEdit::editWidth()
{
   return(eWidth);
}

/*
void xLineEdit::keyPressEvent(QKeyEvent *pEvt)
{
   if (pEvt->ascii() == '\t')
      emit tabPressed();
   else
      xEdit::keyPressEvent(pEvt);
}
*/

//#include "xLineEdit.moc"
