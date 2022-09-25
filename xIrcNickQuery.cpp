/***************************************************************************
**    xIrcNickQuery.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Dialog box to get server name
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
#include "xIrcNickQuery.h"
#include "xResources.h"
#include "xDefaults.h"

static int dbg = 0;
                    
extern xDefaults Defaults;
extern xResources *Resources;
extern QPixmap *AppPixMap;

xIrcNickQuery::xIrcNickQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("nickdialog"),
                                                   QString("NickDialog")),
                       pParent, pName, TRUE)
{
   char *cp;

   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (AppPixMap != NULL)
      setIcon(*AppPixMap);

   if (dbg) fprintf(stdout, "xIrcNickQuery::xIrcNickQuery():Enter\n\r");
   
   if (pName)
      setCaption(pName);

   setFocusPolicy(StrongFocus);
   pNick = new xEditList(wdtRes, this);
   pNick->setMargins(5, 5);
   pNick->setFrameStyle(QFrame::Panel | QFrame::Raised);
   if (dbg) fprintf(stdout, "xIrcNickQuery::xIrcNickQuery():Setting Label text\n\r");
   pNick->setLabel("Enter Nick");
   if ((cp = (char *)Defaults.get("NICKS")) == NULL || strlen(cp) == 0)
      if ((cp = (char *)Defaults.get("NICK")) == NULL || strlen(cp) == 0)
         cp = "Who-Am-I";
      else
         QMessageBox::warning(this, "Warning:", "Obsolete Definition used: New Definition is NICKS");
   pNick->insertItems(cp);
   pNick->setCurrentItem(0);

   pNickBtnFrame = new xFrame(wdtRes, this);
   pAutoNickBtn = new QCheckBox(pNickBtnFrame);
   pAutoNickBtn->setFont(font());
   pAutoNickBtn->setAutoResize(TRUE);
   pAutoNickBtn->setText("Auto Nick Selection");
//   pAutoNickBtn->adjustSize();
   pAutoNickBtn->setChecked(TRUE);
   pNickBtnFrame->addWidget(pAutoNickBtn);
   setMargins(0, 5);
   setWidgetSpacing(0);
   pNickBtnFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pNickBtnFrame->setAlignment(xALIGN_Horz);
   pNickBtnFrame->setResizeMode(xSPACE_Resize);
   pNickBtnFrame->fitFrame();

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("OK",     xIrcNickQuery::Accepted);
   pButtons->addButton("Clear",  xIrcNickQuery::Clear);
   pButtons->addButton("Cancel", xIrcNickQuery::Rejected);

   addWidget(pNick);
   addWidget(pNickBtnFrame);
   addWidget(pButtons);
   setMargins(0, 0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pNick, SIGNAL(gotEntry(const char *)),
           this, SLOT(gotReturn(const char *)));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
   if (dbg) fprintf(stdout, "xIrcNickQuery::xIrcNickQuery():Exit\n\r");
}

void xIrcNickQuery::gotButton(int btn)
{
   QString tmpStr;
   int idx;

   if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton(%d):Enter\n\r", btn);
   switch((QryResults)btn)
   {
      case Disconnect:
         if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton():Disconnecting??\n\r");
         done(btn);
         break;

      case Clear:
         if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton():Clearing??\n\r");
         pNick->setText("");                                               
         break;                                                              

      case Accepted:                                                         
         if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton():Accepting??\n\r");
         if (strlen(pNick->text()) == 0)
         {
            if ((idx = pNick->currentItem()) == -1)
            {
               QMessageBox::warning(this, "Error:", "No Nick Specified");
               return;
            }
            pNick->setText(pNick->string(idx));
         }
         accept();                                                           
         break;                                                              

      case Rejected:                                                         
         if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton():Rejecting??\n\r");
         reject();
         break;
   }
   if (dbg) fprintf(stdout, "xIrcNickQuery::gotButton():Exit\n\r");
}

const char *xIrcNickQuery::nextNick()
{
   int idx;
   const char *rv;

   if ((idx = pNick->currentItem()) == pNick->count() - 1)
   {
      pNick->setCurrentItem(0);
      rv = NULL;
   }
   else
   {
      pNick->setCurrentItem(++idx);
      rv = pNick->string(idx);
   }
   return(rv);
}

void xIrcNickQuery::accept()
{
   QDialog::accept();
}

void xIrcNickQuery::reject()
{
   QDialog::reject();
}

void xIrcNickQuery::gotReturn(const char *)
{
   int idx;

   if (dbg) fprintf(stdout, "xIrcNickQuery::gotReturn():Enter\n\r");
   if (strlen(pNick->text()) == 0)
   {
      if ((idx = pNick->currentItem()) == -1)
      {
         QMessageBox::warning(this, "Error:", "No Nick Specified");
         return;
      }
      pNick->setText(pNick->string(idx));
   }
   QDialog::accept();
   if (dbg) fprintf(stdout, "xIrcNickQuery::gotReturn():Exit\n\r");
}

xIrcNickQuery::~xIrcNickQuery()
{
   if (pNick)
   {
      if (dbg) fprintf(stdout, "xIrcNickQuery::~xIrcNickQuery():Deleting pNick\n");
      if (dbg) fflush(stdout);
      delete pNick;
   }
   if (pNickBtnFrame)
   {
      if (dbg) fprintf(stdout, "xIrcNickQuery::~xIrcNickQuery():Deleting pNickBtnFrame\n");
      if (dbg) fflush(stdout);
      delete pNickBtnFrame;
   }
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xIrcNickQuery::~xIrcNickQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
   if (dbg) fprintf(stdout, "xIrcNickQuery::~xIrcNickQuery():Done!\n");
   if (dbg) fflush(stdout);
//   disconnect(this);
}

//#include "xIrcNickQuery.moc"
