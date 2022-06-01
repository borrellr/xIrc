/***************************************************************************
**    xConfirmation.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box to display error messages
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <stdio.h>
#include <qkeycode.h>
#include "xConfirmation.h"

static int dbg = 0;

xConfirmation::xConfirmation(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName, const char *pCaption,
                           const char *pMessage) :
            xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("confirmation"), 
                                                QString("Confirmation")),
                    pParent, pName, TRUE)
{  
   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Enter\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   if (pCaption != NULL)
      setCaption(pCaption);
   else
      setCaption("Confirm");
//   setFont(QFont("helvetica", 12, QFont::Bold));
   pLabel = new xLabel(wdtRes, this);
   pLabel->setAlignment(AlignCenter);
   pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pLabel->setText(pMessage);
   pLabel->adjustSize();

   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Creating buttonframe\n");
   if (dbg) fflush(stdout);
   pButton = new xPshBtnFrame(wdtRes, this);
   pButton->setAlignment(xALIGN_Horz);
   pButton->setFrameStyle(QFrame::Panel | QFrame::Raised);
   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Adding buttons to button frame\n");
   if (dbg) fflush(stdout);
   pButton->addButton("Yes", QDialog::Accepted);
   pButton->addButton("No", QDialog::Rejected);

   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Adding button frame to frame\n");
   if (dbg) fflush(stdout);
   addWidget(pLabel);
   addWidget(pButton);

   setMargins(0, 0);
   setWidgetSpacing(0);
   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Fitting frame\n");
   if (dbg) fflush(stdout);
   initFrame();

   connect(pButton, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));

   pAccel = new QAccel(this);
   pAccel->connectItem(pAccel->insertItem(Key_Enter), 
                       this, SLOT(gotEnter()));
   pAccel->connectItem(pAccel->insertItem(Key_Return), 
                       this, SLOT(gotEnter()));
   pAccel->connectItem(pAccel->insertItem(Key_Escape), 
                       this, SLOT(gotEscape()));

   if (dbg) fprintf(stdout, "xConfirmation::xConfirmation():Exit\n");
   if (dbg) fflush(stdout);

}


void xConfirmation::gotEscape()
{
   done(QDialog::Rejected);
}

void xConfirmation::gotEnter()
{
   done(QDialog::Accepted);
}

void xConfirmation::gotButton(int btn)
{
   if (dbg) fprintf(stdout, "xConfirmation::gotButton(%d):Enter\n", btn);
   if (dbg) fflush(stdout);
   done(btn);
};

int xConfirmation::confirm(xWidgetResInfo *pPRes, const char *pCaption,
                           const char *pMsg, 
                           QWidget *pParent, const char * pName)
{
   xConfirmation *p = new xConfirmation(pPRes, pParent, pName,
                                        pCaption, pMsg);
   int rv = p->exec();
   delete p;
   return(rv);
}

//#include "xConfirmation.moc"
