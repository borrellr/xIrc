/***************************************************************************
**    xIrcDccQuery.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Dialog box to ask about DCC  Messages
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
#include <qfont.h>
#include <qt.h>
#include <xApp.h>
#include "xIrcDccQuery.h"

static bool dbg = false;

extern xApplication *pApp;

xIrcDccQuery::xIrcDccQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName, bool autoDelete) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("dccquerydialog"),
                                                   QString("DccQueryDialog")), 
                       pParent, pName)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   initClass(pParent, pName, autoDelete);
}

void xIrcDccQuery::initClass(QWidget *pParent, const char *pName, bool autoDelete)
{
   if (dbg) fprintf(stdout, "xIrcDccQuery::initClass():Enter\n\r");
   if (dbg) fflush(stdout);

   // Appease the compiler warnings
   pParent = pParent;
   pName = pName;
      
   setCaption("DCC Request");
   setFocusPolicy(StrongFocus);
   deleteFlag = autoDelete;
   
   pHeader = new QLabel(this);
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setResizeMode(xSPACE_ToFit);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("OK", Accepted);
   pButtons->addButton("Message", Message);
   pButtons->addButton("Cancel", Rejected);
   pButtons->fitFrame();

   pHeader->setAlignment(AlignCenter);

   addWidget(pHeader);
   addWidget(pButtons);
   setMargins(0, 3);
   setWidgetSpacing(5);
   
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   if (dbg) fprintf(stdout, "xIrcDccQuery::initClass():Exit\n\r");
   if (dbg) fflush(stdout);   
}

void xIrcDccQuery::buttonPressed(int results)
{
   
   if (dbg) fprintf(stdout, "xIrcDccQuery::button pressed():results = %d\n\r", results);
   if (dbg) fflush(stdout);
   
   msg.rspCode = results;
   emit done(&msg);

   if (deleteFlag)
   {
      disconnect(this);
      delete this;
   }
}

void xIrcDccQuery::setupQuery(const char *pType, xIrcMessage *pMsg)
{
   QString strTmp;

   if (dbg) fprintf(stdout, "xIrcDccQuery::setupQuery():Enter\n\r");
   if (dbg) fflush(stdout);
   msg = *pMsg;
   strTmp = "DCC ";
   strTmp += pType;
   strTmp += " Request from: ";
   strTmp += pMsg->srcNick;
   
   strTmp += "\n\nAccept?";
   if (dbg) fprintf(stdout, "xIrcDccQuery::setupQuery():Setting Query Text to |%s|\n", (const char *)strTmp);
   if (dbg) fflush(stdout);
   pHeader->setText(strTmp);
   pHeader->adjustSize();
//   QSize s = pHeader->size();
//   s.setHeight(pButtons->height());
//   pButtons->fitFrame(&s);
   initFrame();
   pApp->beep();
   show();
   if (dbg) fprintf(stdout, "xIrcDccQuery::setupQuery():Exit\n\r");
   if (dbg) fflush(stdout);   
}

xIrcDccQuery::~xIrcDccQuery()
{
   if (dbg) fprintf(stdout, "xIrcDccQuery::~xIrcDccQuery():Enter\n\r");
   if (dbg) fflush(stdout);
   if (pHeader)
      delete pHeader;
   if (pButtons)
      delete pButtons;
   if (dbg) fprintf(stdout, "xIrcDccQuery::~xIrcDccQuery():Exit\n\r");
   if (dbg) fflush(stdout);
}

//#include "xIrcDccQuery.moc"
