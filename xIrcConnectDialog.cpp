/***************************************************************************
**    xIrcConnectDialog.cpp  $Revision: 1.7 $ - $Name:  $ 
**    Dialog Box to Complete Connection Parameter Entry
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
#include "xIrcConnectDialog.h"

static int dbg = 0;

xIrcConnectDialog::xIrcConnectDialog(xWidgetResInfo *pPRes, QWidget *pParent,
                                     const char *pName,
                                     xIrcServerEntry *entry) :
                   xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("connectdialog"),
                                                              QString("ConnectDialog")),
                           pParent, pName, TRUE)
{
   QStrList ports(TRUE);
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Enter\n");
   if (dbg) fflush(stdout);

   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   setMargins(0, 0);
   setWidgetSpacing(0);
   
   mainFrame->setAutoDelete(TRUE);
   setAlignment(xALIGN_Vert);
   ports.setAutoDelete(TRUE);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating Frame\n");
   if (dbg) fflush(stdout);
#if 0
   pFrame = new xFrame(wdtRes, this);
   pFrame->setAutoDelete(TRUE);
   pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame->setAlignment(xALIGN_Vert);
   pFrame->setMargins(5, 5);
   pFrame->setWidgetSpacing(0);
#else
   pFrame = new QGrid(2, this);
   pFrame->setGeometry(QRect(10, 10, 150, 200));
#endif

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating Group Edit\n");
   if (dbg) fflush(stdout);
   new QLabel("Group:", pFrame);
   pGroup = new QLineEdit(pFrame);
   pGroup->setAlignment(xALIGN_Horz);
   //pGroup->setEditWidth(15);
   //pGroup->setMargins(0, 2);
   //pGroup->setWidgetSpacing(0);
   if (entry != NULL)
      pGroup->setText(entry->group());
   pGroup->setEnabled(FALSE);
//   pFrame->addWidget(pGroup);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating Country Edit\n");
   if (dbg) fflush(stdout);
   new QLabel("Country:", pFrame);
   pCountry = new QLineEdit(pFrame);
   pCountry->setAlignment(xALIGN_Horz);
   //pCountry->setEditWidth(10);
   //pCountry->setMargins(0, 2);
   //pCountry->setWidgetSpacing(0);
   if (entry != NULL)
      pCountry->setText(entry->country());
   pCountry->setEnabled(FALSE);
//   pFrame->addWidget(pCountry);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating State Edit\n");
   if (dbg) fflush(stdout);
   new QLabel("State:", pFrame);
   pState = new QLineEdit("State:", pFrame);
   pState->setAlignment(xALIGN_Horz);
   //pState->setEditWidth(10);
   //pState->setMargins(0, 2);
   //pState->setWidgetSpacing(0);
   if (entry != NULL)
      pState->setText(entry->state());
   pState->setEnabled(FALSE);
//   pFrame->addWidget(pState);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating City Edit\n");
   if (dbg) fflush(stdout);
   new QLabel("City:", pFrame);
   pCity = new QLineEdit("City:", pFrame);
   pCity->setAlignment(xALIGN_Horz);
   //pCity->setEditWidth(25);
   //pCity->setMargins(0, 2);
   //pCity->setWidgetSpacing(0);
   if (entry != NULL)
      pCity->setText(entry->city());
   pCity->setEnabled(FALSE);
//   pFrame->addWidget(pCity);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating Server Edit\n");
   if (dbg) fflush(stdout);
   new QLabel("Server:", pFrame);
   pServer = new QLineEdit("Server:", pFrame);
   pServer->setAlignment(xALIGN_Horz);
   //pServer->setEditWidth(25);
   //pServer->setMargins(0, 2);
   //pServer->setWidgetSpacing(0);
   if (entry != NULL)
      pServer->setText(entry->server());
//   pFrame->addWidget(pServer);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Creating Port Combo and Frame\n");
   if (dbg) fflush(stdout);
#if 0
   pPortFrame = new xFrame(wdtRes, pFrame);
   pPortFrame->setAutoDelete(TRUE);
   pPortFrame->setResizeMode(xSPACE_Resize);
   pPortFrame->setMargins(0, 2);
//   pPortFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pPortFrame->setAlignment(xALIGN_Horz);
//   pPortFrame->setMargins(0, 0);
   pPortFrame->setWidgetSpacing(0);

//   pPort->setAutoResize(TRUE);
#else
   new QLabel("Port:", pFrame);
#endif

   pPort = new xComboBox(wdtRes, TRUE, pFrame, "ports");
   setDefPallet(pPort, wdtRes);
   setDefFont(pPort, wdtRes);

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Parsing Ports...\n");
   if (dbg) fflush(stdout);
   if (entry != NULL)
   {
      for (cp = entry->ports().latin1(); *cp;)
      {
         QString s;
         if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): cp = |%s|\n", cp);
         if (dbg) fflush(stdout);
         while (*cp != ',' && *cp != '\0')
            s += *(cp++);
         ports.inSort(s.latin1());
         if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): cp remaining length = %lu\n", 
                                 strlen(cp));
         if (dbg) fflush(stdout);
         if (*cp != '\0')
            cp++;
      }
      if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Adding Ports...\n");
      if (dbg) fflush(stdout);
      pPort->insertStrList(&ports);
      pPort->setCurrentItem(0);
//      pPort->setCurrentItem(1);
//      pPort->setCurrentItem(0);
   }

//   pPortLabel = new QLabel("Port:", pPortFrame);
//   pPortFrame->addWidget(pPortLabel);

//   pPortFrame->addWidget(pPort);
//   pPortFrame->fitFrame();
//   pFrame->addWidget(pPortFrame);

//   pFrame->fitFrame();

   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Adding buttons\n");
   if (dbg) fflush(stdout);

   QPushButton *qbutton1 = new QPushButton(this, "QPushButton1");
   qbutton1->setGeometry(QRect(20, 210, 70, 30));
   qbutton1->setText(tr("Connect"));
   connect(qbutton1, SIGNAL(clicked()), this, SLOT(accept()));

   QPushButton *qbutton2 = new QPushButton(this, "QPushButton2");
   qbutton2->setGeometry(QRect(100, 210, 70, 30));
   qbutton2->setText(tr("Cancel"));
   connect(qbutton2, SIGNAL(clicked()), this, SLOT(reject()));

   setMinimumSize(200, 250);
//   addWidget(pFrame);
//   addWidget(pButtons);
   initFrame();
   if (dbg) fprintf(stdout, "xIrcConnectDialog::xIrcConnectDialog(): Enter\n");
   if (dbg) fflush(stdout);
}

xIrcConnectDialog::~xIrcConnectDialog()
{
}

QString xIrcConnectDialog::port()
{
   QString rv;

   if (strlen(pPort->currentText().latin1()) > 0)
      rv = pPort->currentText();
   else
      rv = pPort->text(pPort->currentItem());
   return(rv);
}

void xIrcConnectDialog::accept()
{
    emit completed(1);
    done(Accepted);
}

void xIrcConnectDialog::reject()
{
    emit completed(0);
    done(Rejected);
}

void xIrcConnectDialog::gotButton(int btn)
{
   switch (btn)
   {
      case Rejected:
         emit completed(0);
         done(Rejected);
         break;
      case Accepted:
         emit completed(1);
         done(Accepted);
         break;
   }
}
