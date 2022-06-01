/***************************************************************************
**    xSocketDialog.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Dialog Box to reflect a sockets status of connection
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
#include <qfont.h>
#include "xSocketDialog.h"

static int dbg = 0;

xSocketDialog::xSocketDialog(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                                    QString("")),
                       pParent, pName)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setCaption("Connect");
   pText = new xLabel(wdtRes, this);
   pText->setAlignment(AlignCenter);
   pText->setText(QString("Attempting Connection to: X.X.X.X"));
   pText->adjustSize();

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Cancel", QDialog::Rejected);
   pButtons->fitFrame();

   addWidget(pText);
   addWidget(pButtons);
   setMargins(0, 0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
}

xSocketDialog::~xSocketDialog()
{
   disconnect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   disconnect(this);
   if (pText != NULL)
      delete pText;
   if (pButtons != NULL)
      delete pButtons;
}

void xSocketDialog::buttonPressed(int btn)
{
   if (btn == QDialog::Rejected)
   {
      hide();
      emit stopConnection();
   }
}

void xSocketDialog::connectMessage(const char *pStr)
{
   QString strTmp;

   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Enter\n");
   strTmp = "Attempting Connection to: ";
   strTmp += pStr;
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Setting text to %s\n",
                            (const char *)strTmp);
   pText->setText(strTmp);
   pText->adjustSize();
   pButtons->fitFrame();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Initializing the frame\n");
//   pFrame->fitFrame();
   mainFrame->fitFrame(NULL, TRUE);
   initFrame();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():calling Show\n");
   show();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Exit!!!\n");
}

void xSocketDialog::connected(const char *pStr)
{
   QString strTmp;

   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Enter\n");
   strTmp = pStr;
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Setting text to %s\n",
                            (const char *)strTmp);
   pText->setText(strTmp);
   pText->adjustSize();
   pButtons->fitFrame();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Initializing the frame\n");
//   pFrame->fitFrame();
   mainFrame->fitFrame(NULL, TRUE);
   initFrame();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():calling Show\n");
//   show();
   if (dbg) fprintf(stdout, "xSocketDialog::connectMessage():Exit!!!\n");
}

//#include "xSocketDialog.moc"
