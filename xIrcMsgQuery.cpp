/***************************************************************************
**    xIrcMsgQuery.cpp  $Revision: 1.3 $ - $Name:  $ 
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
#include <stdlib.h>
#include <qfont.h>
#include <xApp.h>
#include "xIrcMsgQuery.h"

extern xApplication *pApp;

static int dbg = 0;

static const char *pInitialResources[] =
{
   NULL
};
/*
MessageDialog.
MessageDialog.
MessageDialog.
MessageDialog.
*/

xIrcMsgQuery::xIrcMsgQuery(xWidgetResInfo *pPRes, QWidget *pParent, 
                           const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("messagedialog"),
                                                   QString("MessageDialog")), 
                       pParent, pName)
{
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   initClass(pParent, pName);
}

void xIrcMsgQuery::initClass(QWidget *pParent, const char *pName)
{
   const char *ccp;
   int lines, cols;
   if (dbg) fprintf(stdout, "xIrcMsgQuery::initClass():Enter\n");
   if (dbg) fflush(stdout);

   // Appease the compiler warnings
   pParent = pParent;
   pName = pName;
      
#ifdef QT2
   setFocusPolicy(StrongFocus);
#else
   setAcceptFocus(TRUE);   
#endif
   
   if ((ccp = Resources->get(wdtRes, "Columns", "Columns")) == NULL)
      cols = 40;
   else
      cols = atoi(ccp);
   if (cols < 10)
      cols = 40;

   if ((ccp = Resources->get(wdtRes, "lines", "Lines")) == NULL)
      lines = 7;
   else
      lines = atoi(ccp);

   if (lines < 3)
      lines = 7;

   pMsg = new xMultiLineFrame(wdtRes, this, NULL, cols, lines);
//   pMsg = new xMultiLineEdit(wdtRes, this);
//   pMsg->setWidth(80);
//   pMsg->setHeight(7);
//   pMsg->setEnabled(FALSE);

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->setMargins(2, 2);
   pButtons->addButton("Chat", QDialog::Accepted);
   pButtons->addButton("Close", QDialog::Rejected);

   addWidget(pMsg);
   addWidget(pButtons);
   setMargins(0, 0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   connect(pMsg, SIGNAL(textSelected(xMultiLineTextSelection)),
           this, SLOT(haveTextSelection(xMultiLineTextSelection)));
   if (dbg) fprintf(stdout, "xIrcMsgQuery::initClass():Exit\n");
   if (dbg) fflush(stdout);   
}

void xIrcMsgQuery::buttonPressed(int results)
{
   if (dbg) fprintf(stdout, "xIrcMsgQuery::button pressed():results = %d\n", results);
   if (dbg) fflush(stdout);
   
   btnPressed = results;
   if (dbg) fprintf(stdout, "xIrcMsgQuery::button pressed():signaling...\n");
   
   emit done(this);
}

void xIrcMsgQuery::add(xIrcMessage *pmsg)
{
   QString strTmp;
   const char *cp;
   
   if (dbg) fprintf(stdout, "xIrcMsgQuery::setupQuery():Enter\n");
   if (dbg) fflush(stdout);

   msgList.append(pmsg);

   QString caption("New Message From: ");
   caption += pmsg->srcNick;
   setCaption(caption);

   strTmp = "<";
   strTmp += pmsg->srcNick;
   strTmp += "> ";
   for (cp = pmsg->msgStr; *cp; cp++)
   {
      if (*cp != '\n' && *cp != '\r')
         strTmp += *cp;
   }

   pMsg->pWin->putString(strTmp);
   pApp->beep();

   if (dbg) fprintf(stdout, "xIrcMsgQuery::setupQuery():Exit\n");
   if (dbg) fflush(stdout);   
}

xIrcMsgQuery::~xIrcMsgQuery()
{
   if (dbg) fprintf(stdout, "xIrcMsgQuery::~xIrcMsgQuery(0x%x):Enter\n",
                            (unsigned int)this);
   if (dbg) fflush(stdout);
   if (pMsg)
      delete pMsg;
   if (pButtons)
      delete pButtons;
   if (dbg) fprintf(stdout, "xIrcMsgQuery::~xIrcMsgQuery():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcMsgQuery *xIrcMsgQueryList::find(xIrcMessage *pMsg)
{
   xIrcMsgQuery *p;
   if (dbg) fprintf(stdout, "xIrcMsgQueryList::find():Enter\n");
   if (dbg) fflush(stdout);
   for (p = first(); p != NULL; p = next())
   {
      if (dbg) fprintf(stdout, "xIrcMsgQueryList::find():Testing 0x%x\n",
                       (unsigned int)p);
      if (dbg) fflush(stdout);
      if (p->is(pMsg))
         break;
   }
   if (dbg) fprintf(stdout, "xIrcMsgQueryList::find():Exit(0x%x)\n",
                    (unsigned int)p);
   return(p);
};

void xIrcMsgQuery::haveTextSelection(xMultiLineTextSelection msg)
{
   if (dbg) fprintf(stdout, "xIrcMessageFrame::havetextSelection():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcMessageFrame::havetextSelection():winName = |%s|, text = |%s|\n",
                             (const char *)msg.winName, (const char *)msg.text);
   if (dbg) fflush(stdout);
   emit textSelected(msg);
}

//#include "xIrcMsgQuery.moc"
