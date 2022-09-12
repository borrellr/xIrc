/***************************************************************************
**    xIrcBanDialog.cpp  $Revision: 1.5 $ - $Name:  $ 
**    Dialog box for Banning
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
#include <qmessagebox.h>
#include "xIrcBanDialog.h"
#include "xDefaults.h"

static bool dbg = FALSE;
                    
extern xDefaults Defaults;

xIrcBanDialog::xIrcBanDialog(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("bandialog"),
                                                   QString("BanDialog")),
                       pParent, pName, TRUE)
{
   char *cp;

   if (dbg) fprintf(stdout, "xIrcBanDialog::xIrcBanDialog():Enter\n\r");
   
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   if (pName)
      setCaption(pName);
   else
      setCaption("Ban");

   setFocusPolicy(StrongFocus);
   pEditFrame = new xFrame(wdtRes, this);
   pEditFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);

   pNick = new QLineEdit(pEditFrame);
   //pNick->setMargins(5, 5);
   pNick->setText("Nick");

   pChan = new QLineEdit(pEditFrame);
   //pChan->setMargins(5, 5);
   pChan->setText("Channel");

   pMask = new QLineEdit(pEditFrame);
   //pMask->setMargins(5, 5);
   pMask->setText("Ban Mask");

   pText = new QLineEdit(pEditFrame);
   //pText->setMargins(5, 5);
   pText->setText("Message");

   pEditFrame->addWidget(pText);
   pEditFrame->addWidget(pMask);
   pEditFrame->addWidget(pNick);
   pEditFrame->addWidget(pChan);
   pEditFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pEditFrame->setAlignment(xALIGN_Horz);
// pEditFrame->setResizeMode(xSPACE_Resize);
   pEditFrame->setMargins(0, 5);
   pEditFrame->setWidgetSpacing(0);
   pEditFrame->fitFrame();

   if ((cp = (char *)Defaults.get("BANMESSAGE")) != NULL && strlen(cp) != 0)
      pText->setText(cp);

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Ban", Accepted);
   pButtons->addButton("Kick", Kick);
   pButtons->addButton("Unban", UnBan);
   pButtons->addButton("Clear Kick Mesg", Clear);
   pButtons->addButton("Default Kick Mesg", Default);
   pButtons->addButton("Cancel", Rejected);

   addWidget(pEditFrame);
   addWidget(pButtons);
   setMargins(0, 0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pNick, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
   if (dbg) fprintf(stdout, "xIrcBanDialog::xIrcBanDialog():Exit\n\r");
}

void xIrcBanDialog::gotButton(int btn)
{
   QString tmpStr;
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcBanDialog::gotButton(%d):Enter\n\r", btn);
   switch((QryResults)btn)
   {
      case Clear:
         if (dbg) fprintf(stdout, "xIrcBanDialog::gotButton():Clearing??\n\r");
         pNick->setText("");                                               
         break;                                                              

      case UnBan:
      case Kick:
      case Accepted:
         if (dbg) fprintf(stdout, "xIrcBanDialog::gotButton():Accepting??\n\r");
         if (strlen(pMask->text()) == 0)
         {
            QMessageBox::warning(this, "Error", "No Ban Mask Specified");
            return;
         }
         done(btn);
         break;                                                              

      case Rejected:
         if (dbg) fprintf(stdout, "xIrcBanDialog::gotButton():Rejecting??\n\r");
         reject();
         break;

      case Default:                                                         
         if ((cp = (char *)Defaults.get("BANMESSAGE")) != NULL && strlen(cp) != 0)
            pText->setText(cp);
         break;
   }
   if (dbg) fprintf(stdout, "xIrcBanDialog::gotButton():Exit\n\r");
}

void xIrcBanDialog::accept()
{
   QDialog::accept();
}

void xIrcBanDialog::reject()
{
   QDialog::reject();
}

void xIrcBanDialog::gotReturn()
{
   if (dbg) fprintf(stdout, "xIrcBanDialog::gotReturn():Enter\n\r");
   QDialog::accept();
   if (dbg) fprintf(stdout, "xIrcBanDialog::gotReturn():Exit\n\r");
}

xIrcBanDialog::~xIrcBanDialog()
{
/*
   if (pNick)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pNick\n");
      if (dbg) fflush(stdout);
      delete pNick;
   }
   if (pMask)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pMask\n");
      if (dbg) fflush(stdout);
      delete pMask;
   }
   if (pChan)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pChan\n");
      if (dbg) fflush(stdout);
      delete pChan;
   }
   if (pText)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pText\n");
      if (dbg) fflush(stdout);
      delete pText;
   }
*/
   if (pEditFrame)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pEditFrame\n");
      if (dbg) fflush(stdout);
      delete pEditFrame;
   }

   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }

   if (dbg) fprintf(stdout, "xIrcBanDialog::~xIrcBanDialog():Done!\n");
   if (dbg) fflush(stdout);
//   disconnect(this);
}

//#include "xIrcBanDialog.moc"
