/***************************************************************************
**    xIrcNickActionQuery.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Dialog box to ask about Private messages
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
#include <string.h>
#include <qfont.h>
#include "xIrcNickActionQuery.h"

static int dbg = 0;

xIrcNickActionQuery::xIrcNickActionQuery(xWidgetResInfo *pPRes, 
                                         QWidget *pParent, const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("nickactiondialog"),
                                                   QString("NickActionDialog")),
                       pParent, pName)
{
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::xIrcNickActionQuery():Enter\n");
   if (dbg) fflush(stdout);

   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

#ifdef QT2
   setFocusPolicy(StrongFocus);
#else
   setAcceptFocus(TRUE);   
#endif
   pNickEdit = new QLineEdit(this);
   pNickEdit->setText("Nick:");
   //pNickEdit->setMargins(0, 10);
   pChanEdit = new QLineEdit(this);
   pChanEdit->setText("Channel:");
   //pChanEdit->setMargins(0, 10);
   pButtons = new xPshBtnTable(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->addButton("Dcc Chat", DccChat);
   pButtons->addButton("Who", Who);
   pButtons->addButton("Give Ops", GiveOps);
   pButtons->addButton("Ban", Ban);
   pButtons->addButton("Ping", Ping);
   pButtons->addButton("Notify", Notify);

   pButtons->addButton("Priv Chat", PrivChat);
   pButtons->addButton("WhoIs", WhoIs);
   pButtons->addButton("Take Ops", TakeOps);
   pButtons->addButton("Kick", Kick);
   pButtons->addButton("Finger", Finger);
   pButtons->addButton("Ignore", Ignore);

   pButtons->addButton("File Send", FileSend);
   pButtons->addButton("WhoWas", WhoWas);
   pButtons->addButton("User Info", UserInfo);
   pButtons->addButton("Time", Time);
   pButtons->addButton("Version", Version);
   pButtons->addButton("Close", Close);

   pButtons->arrangeButtons();
   addWidget(pNickEdit);
   addWidget(pChanEdit);
   addWidget(pButtons);
   setMargins(3, 3);
   setWidgetSpacing(10);
   initFrame();
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::xIrcNickActionQuery():Exit\n");
   if (dbg) fflush(stdout);   
}

xIrcNickActionQuery::~xIrcNickActionQuery()
{
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::~xIrcNickActionQuery():Enter\n");
   if (dbg) fflush(stdout);
   if (pNickEdit)
      delete pNickEdit;
   if (pChanEdit)
      delete pChanEdit;
   if (pButtons)
      delete pButtons;
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::~xIrcNickActionQuery():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickActionQuery::wakeUp(xMultiLineTextSelection text)
{
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Enter\n");
   if (dbg) fflush(stdout);
   txtSel = text;
   if (txtSel.text != NULL && strlen(txtSel.text) > 0)
   {
      if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Nick = |%s|\n", (const char*)text.text);
      if (dbg) fflush(stdout);
      pNickEdit->setText(text.text);
   }
   if (txtSel.winName != NULL)  // && strlen(txtSel.winName) > 0)
   {
      if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Channel = |%s|\n", (const char*)text.winName);
      if (dbg) fflush(stdout);
      pChanEdit->setText(text.winName);
   }
   raise();
   show();
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickActionQuery::wakeUp(const char *pNick)
{
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Enter\n");
   if (dbg) fflush(stdout);
   txtSel.text = "";
   txtSel.winName = "";
   for (; *pNick != '\0' && *pNick != ' '; pNick++)
      txtSel.text += *pNick;
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Have Nick |%s|\n", (const char*)txtSel.text);
   if (dbg) fflush(stdout);
   txtSel.winName = "";
   if (strlen(txtSel.text) > 0)
      pNickEdit->setText(txtSel.text);
   if (strlen(txtSel.winName) > 0)
      pChanEdit->setText(txtSel.winName);
   raise();
   show();
   if (dbg) fprintf(stdout, "xIrcNickActionQuery::wakeUp():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickActionQuery::buttonPressed(int id)
{
   txtSel.text = pNickEdit->text();
   txtSel.winName = pChanEdit->text();
   txtSel.iData = id;
   emit done(txtSel);
}

//#include "xIrcNickActionQuery.moc"

