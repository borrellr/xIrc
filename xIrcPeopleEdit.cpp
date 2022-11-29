/***************************************************************************
**    xIrcPeopleEdit.cpp  $Revision: 1.3 $ - $Name:  $ 
**    People List Entry Edit Dialog
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
#include <qstrlist.h>
#include "xIrcPeopleEdit.h"

static int dbg = 0;

xIrcPeopleEdit::xIrcPeopleEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                                     const char *pName,
                                     xIrcPeopleEntry *entry = NULL) :
                   xDialog(wdtRes = new xWidgetResInfo(pPRes, QString(pName),
                                                              QString("PeopleEdit")),
                           pParent, pName, TRUE)
{
   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Enter\n");
   if (dbg) fflush(stdout);

   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   setMargins(0, 0);
   setWidgetSpacing(0);
   setCaption(pName);

   pAccel = new QAccel(this);
   pAccel->connectItem(pAccel->insertItem(Key_Tab), this, SLOT(gotTab()));

   pEntry = entry;   
   mainFrame->setAutoDelete(TRUE);
   setAlignment(xALIGN_Vert);

   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Creating Frame\n");
   if (dbg) fflush(stdout);
   pFrame = new xFrame(wdtRes, this);
   pFrame->setAutoDelete(TRUE);
   pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame->setAlignment(xALIGN_Vert);
   pFrame->setMargins(5, 5);
   pFrame->setWidgetSpacing(0);

   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Creating Nick Edit\n");
   if (dbg) fflush(stdout);
   pNick = new QLineEdit("Nick:", pFrame);
   pNick->setAlignment(xALIGN_Horz);
   //pNick->setEditWidth(15);
   //pNick->setMargins(0, 2);
   //pNick->setWidgetSpacing(0);
   pNick->setText(entry->nick());
   connect(pNick, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
   pFrame->addWidget(pNick);

   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Creating Mask Edit\n");
   if (dbg) fflush(stdout);
   pMask = new QLineEdit("Address Mask:", pFrame);
   pMask->setAlignment(xALIGN_Horz);
   //pMask->setEditWidth(25);
   //pMask->setMargins(0, 2);
   //pMask->setWidgetSpacing(0);
   pMask->setText(entry->mask());
   connect(pMask, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
   pFrame->addWidget(pMask);

   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Creating Message Edit\n");
   if (dbg) fflush(stdout);
   pMessage = new QLineEdit("Message:", pFrame);
   pMessage->setAlignment(xALIGN_Horz);
   //pMessage->setEditWidth(25);
   //pMessage->setMargins(0, 2);
   //pMessage->setWidgetSpacing(0);
   pMessage->setText(entry->message());
   connect(pMessage, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
   pFrame->addWidget(pMessage);

   pFlag = new xCheckBox(wdtRes, pFrame, NULL, pName);
   pFrame->addWidget(pFlag);
   if (entry->flag())
       pFlag->setChecked(TRUE);

   pByAddr = new xCheckBox(wdtRes, pFrame, NULL, "By Address");
   pFrame->addWidget(pByAddr);
   if (entry->flag() > 0)
       pByAddr->setChecked(TRUE);

   pFrame->fitFrame();

   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Adding buttons\n");
   if (dbg) fflush(stdout);
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Ok", Accepted);
   pButtons->addButton("Cancel", Rejected);
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));

   addWidget(pFrame);
   addWidget(pButtons);
   initFrame();
   if (dbg) fprintf(stdout, "xIrcPeopleEdit::xIrcPeopleEdit(): Enter\n");
   if (dbg) fflush(stdout);
}

xIrcPeopleEdit::~xIrcPeopleEdit()
{
}

void xIrcPeopleEdit::gotReturn()
{
   int f;
   pEntry->setNick(pNick->text());
   pEntry->setMask(pMask->text());
   pEntry->setMessage(pMessage->text());
   f = (pFlag->isChecked() == FALSE) ?
       0 : (pByAddr->isChecked() == TRUE) ? 1 : -1;
   pEntry->setFlag(f);
   done(Accepted);
}

void xIrcPeopleEdit::gotTab()
{
   focusNextPrevChild(TRUE);
}

void xIrcPeopleEdit::gotButton(int btn)
{
   int f;

   switch (btn)
   {
      case Rejected:
         done(Rejected);
         break;

      case Accepted:
         pEntry->setNick(pNick->text());
         pEntry->setMask(pMask->text());
         pEntry->setMessage(pMessage->text());
         f = (pFlag->isChecked() == FALSE) ? 0 : (pByAddr->isChecked() == TRUE) ? 1 : -1;
         pEntry->setFlag(f);
         done(Accepted);
         break;
   }
}
//#include "xIrcPeopleEdit.moc"
