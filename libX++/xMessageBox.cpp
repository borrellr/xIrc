/***************************************************************************
**    xMessageBox.cpp  $Revision: 1.2 $ - $Name:  $ 
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
#include "xApp.h"
#include "xMessageBox.h"

static int dbg = 0;

extern xApplication *pApp;
extern xWidgetResInfo appRes;

xMessageBox::xMessageBox(const char *pCaption, const char *pMessage,
                         const char *pBtnText,
                         QWidget *pParent, const char *pName) :
            xDialog(wdtRes = new xWidgetResInfo(&appRes, QString("messagebox"), 
                                                QString("MessageBox")),
                    pParent, pName, TRUE)
{  
   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Enter\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   if (pCaption != NULL)
      setCaption(pCaption);
   else
      setCaption("Message");
//   setFont(QFont("helvetica", 12, QFont::Bold));
   pLabel = new xLabel(wdtRes, this);
   pLabel->setAlignment(AlignCenter);
   pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pLabel->setText(pMessage);
   pLabel->adjustSize();

   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Creating buttonframe\n");
   if (dbg) fflush(stdout);
   pButton = new xPshBtnFrame(wdtRes, this);
   pButton->setAlignment(xALIGN_Horz);
   pButton->setFrameStyle(QFrame::Panel | QFrame::Raised);
   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Adding buttons to button frame\n");
   if (dbg) fflush(stdout);

   if (pBtnText == NULL)
      pButton->addButton("Yes", QDialog::Accepted);
   else
      pButton->addButton(pBtnText, QDialog::Rejected);

   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Adding button frame to frame\n");
   if (dbg) fflush(stdout);
   addWidget(pLabel);
   addWidget(pButton);

   setMargins(0, 0);
   setWidgetSpacing(0);
   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Fitting frame\n");
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

   pApp->beep();
   pApp->beep();
   if (dbg) fprintf(stdout, "xMessageBox::xMessageBox():Exit\n");
   if (dbg) fflush(stdout);

}


void xMessageBox::gotEscape()
{
   done(QDialog::Rejected);
}

void xMessageBox::gotEnter()
{
   done(QDialog::Accepted);
}

void xMessageBox::gotButton(int btn)
{
   if (dbg) fprintf(stdout, "xMessageBox::gotButton(%d):Enter\n", btn);
   if (dbg) fflush(stdout);
   done(QDialog::Accepted);
};

int xMessageBox::message(const char *pCaption, const char *pMsg,
                         const char *pBtnText,
                         QWidget *pParent, const char *pName)
{
   xMessageBox *p = new xMessageBox(pCaption, pMsg, pBtnText,
                                    pParent, pName);
   int rv = p->exec();
   delete p;
   return(rv);
}

//#include "xMessageBox.moc"
