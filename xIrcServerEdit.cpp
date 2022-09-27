/***************************************************************************
**    xIrcServerEdit.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Server List Entry Edit Dialog
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
#include <qt.h>
#include <qstrlist.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include "xIrcServerEdit.h"

static int dbg = 0;

xIrcServerEdit::xIrcServerEdit(xWidgetResInfo *pPRes, QWidget *pParent,
                                     const char *pName,
                                     xIrcServerEntry *entry) :
                   xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("serveredit"),
                                                              QString("ServerEdit")),
                           pParent, pName, TRUE)
{
   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Enter\n");
   if (dbg) fflush(stdout);

   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   setMargins(0, 0);
   setWidgetSpacing(0);

   pEntry = entry;   
   mainFrame->setAutoDelete(TRUE);
   setAlignment(xALIGN_Vert);

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating Frame\n");
   if (dbg) fflush(stdout);
#if 1
   pFrame = new QGrid(2, this);
#else
   pFrame = new xFrame(wdtRes, this);
   pFrame->setAutoDelete(TRUE);
   pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame->setAlignment(xALIGN_Vert);
   pFrame->setMargins(5, 5);
   pFrame->setWidgetSpacing(0);
#endif

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating Group Edit\n");
   if (dbg) fflush(stdout);

//   hgroupbox = new QHGroupBox(this);
   new QLabel("Group:", pFrame);
   pGroup = new QLineEdit(pFrame);

   pGroup->setAlignment(xALIGN_Horz);
   //pGroup->setEditWidth(15);
   //pGroup->setMargins(0, 2);
   //pGroup->setWidgetSpacing(0);
   pGroup->setText(entry->group());
   connect(pGroup, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(hgroupbox);

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating Country Edit\n");
   if (dbg) fflush(stdout);

//   hbox = new QHBox(this);

   new QLabel("Country:", pFrame);
   pCountry = new QLineEdit(pFrame);
   pCountry->setAlignment(xALIGN_Horz);
   //pCountry->setEditWidth(10);
   //pCountry->setMargins(0, 2);
   //pCountry->setWidgetSpacing(0);
   pCountry->setText(entry->country());
   connect(pCountry, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(hbox);

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating State Edit\n");
   if (dbg) fflush(stdout);
//   pFrame->addWidget(new QLabel("State:", this));
   new QLabel("State:", pFrame);
   pState = new QLineEdit(pFrame);
   pState->setAlignment(xALIGN_Horz);
   //pState->setEditWidth(10);
   //pState->setMargins(0, 2);
   //pState->setWidgetSpacing(0);
   pState->setText(entry->state());
   connect(pState, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(pState);

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating City Edit\n");
   if (dbg) fflush(stdout);
//   pFrame->addWidget(new QLabel("City:", this));
   new QLabel("City:", pFrame);
   pCity = new QLineEdit(pFrame);
   pCity->setAlignment(xALIGN_Horz);
   //pCity->setEditWidth(25);
   //pCity->setMargins(0, 2);
   //pCity->setWidgetSpacing(0);
   pCity->setText(entry->city());
   connect(pCity, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(pCity);

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Creating Server Edit\n");
   if (dbg) fflush(stdout);
//   pFrame->addWidget(new QLabel("Server:", this));
   new QLabel("Server:", pFrame);
   pServer = new QLineEdit(pFrame);
   pServer->setAlignment(xALIGN_Horz);
   //pServer->setEditWidth(25);
   //pServer->setMargins(0, 2);
   //pServer->setWidgetSpacing(0);
   pServer->setText(entry->server());
   connect(pServer, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(pServer);

//   pFrame->addWidget(new QLabel("Ports:", this));
   new QLabel("Ports:", pFrame);
   pPort = new QLineEdit(pFrame);
   pPort->setAlignment(xALIGN_Horz);
   //pPort->setEditWidth(25);
   //pPort->setMargins(0, 2);
   //pPort->setWidgetSpacing(0);
   pPort->setText(entry->ports());
   connect(pPort, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));
//   pFrame->addWidget(pPort);

//   pFrame->fitFrame();

   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Adding buttons\n");
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
   if (dbg) fprintf(stdout, "xIrcServerEdit::xIrcServerEdit(): Enter\n");
   if (dbg) fflush(stdout);
}

xIrcServerEdit::~xIrcServerEdit()
{
}

void xIrcServerEdit::gotReturn()
{
   pEntry->setGroup(pGroup->text());
   pEntry->setCountry(pCountry->text());
   pEntry->setState(pState->text());
   pEntry->setCity(pCity->text());
   pEntry->setServer(pServer->text());
   pEntry->setPorts(pPort->text());
   done(Accepted);
}

void xIrcServerEdit::gotButton(int btn)
{
   switch (btn)
   {
      case Rejected:
         done(Rejected);
         break;

      case Accepted:
         gotReturn();
         break;
   }
}
//#include "xIrcServerEdit.moc"
