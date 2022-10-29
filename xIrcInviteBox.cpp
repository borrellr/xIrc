/***************************************************************************
**    xIrcInvite.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box to display and handle channel invitations 
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
#include <ctype.h>
#include <qt.h>
#include "xIrcInviteBox.h"

static bool dbg = false;

xIrcInviteBox::xIrcInviteBox(xWidgetResInfo *pPRes, QWidget *pParent, 
                             const char *pName, QObject *pReceiver, 
                             char *pSlot, xIrcMessage *pMsg) :
            xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("invitedialog"),
                                                QString("InviteDialog")),
                    pParent, pName)
{  
   char buff[512];
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Enter\n\r");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   for (cp = pMsg->msgStr.latin1(); *cp && !isspace(*cp) &&
                           *cp != '\r' && *cp != '\n'; cp++)
      strChannel += *cp;

   strNick = pMsg->srcNick;

   setCaption("Invitation");
//   setFont(QFont("helvetica", 12, QFont::Bold));

   pLabel = new QLabel(this);
   pLabel->setFont(font());
   pLabel->setAlignment(AlignCenter);
   pLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
   sprintf(buff, "%s cordially invites you to channel %s",
           (const char *)pMsg->srcNick.latin1(),
           (const char *)pMsg->msgStr.latin1());
   pLabel->setText(buff);
   pLabel->adjustSize();

   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Creating buttonframe\n\r");
   if (dbg) fflush(stdout);
   pButton = new xPshBtnFrame(wdtRes, this);
   pButton->setAlignment(xALIGN_Horz);
   pButton->setFrameStyle(QFrame::Panel | QFrame::Raised);
   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Adding button to button frame\n\r");
   if (dbg) fflush(stdout);
   pButton->addButton("Accept", 1);
   pButton->addButton("Chat", 2);
   pButton->addButton("Decline", 0);

   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Adding button frame to frame\n\r");
   if (dbg) fflush(stdout);
   addWidget(pLabel);
   addWidget(pButton);
   setMargins(0, 0);
   setWidgetSpacing(0);

   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Fitting frame\n\r");
   if (dbg) fflush(stdout);
   initFrame();

   connect(this, SIGNAL(acknowledged(QString)), pReceiver, pSlot);
   connect(pButton, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
   if (dbg) fprintf(stdout, "xIrcInviteBox::xIrcInviteBox():Exit\n\r");
   if (dbg) fflush(stdout);
}

void xIrcInviteBox::buttonClicked(int resp)
{
   if (dbg) fprintf(stdout, "xIrcInviteBox::~xIrcInviteBox():Enter\n\r");
   if (dbg) fflush(stdout);
   if (resp != 0)
      emit acknowledged((resp == 1) ? strChannel : strNick);
   delete this;
};

//#include "xIrcInviteBox.moc"

