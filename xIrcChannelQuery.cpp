/***************************************************************************
**    xIrcChannelQuery.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Dialog Box to get channel name to join
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
#include <ctype.h>
#include <qmessagebox.h>
#include "xIrcChannelQuery.h"
#include "xIrcCommands.h"
#include "xDefaults.h"
#include "xIrcSocket.h"

static bool dbg = FALSE;
                    
extern xIrcCommands ircResponses;
extern xDefaults Defaults;

xChannelQuery::xChannelQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("channeldialog"),
                                                   QString("ChannelDialog")),
                      pParent, pName)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   initClass(pName);
}

void xChannelQuery::initClass(const char *pName)
{
   char *cp;

   if (dbg) fprintf(stdout, "xChannelQuery::xinitClass():Enter\n\r");
   
   if (pName)
      setCaption(pName);
      
   setFocusPolicy(StrongFocus);
   setMargins(0, 0);
   setWidgetSpacing(0);
   if ((pChannel = new xEditList(wdtRes, this)) != NULL)
   {
      if (dbg) fprintf(stdout, "xChannelQuery::xinitClass():ListBox formed\n\r");
      pChannel->setMargins(5, 5);
      pChannel->setFrameStyle(QFrame::Panel | QFrame::Raised);
      if ((cp = (char *)Defaults.get("CHANNELS")) == NULL || strlen(cp) == 0)
         cp = "#chatzone";
      pChannel->insertItems(cp);
      pChannel->setLabel("Enter Channel or Nick Name");
      pChannel->setCurrentItem(0);
   }
   if ((pButtons = new xPshBtnFrame(wdtRes, this)) != NULL)
   {
      pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pButtons->setAlignment(xALIGN_Horz);
      pButtons->addButton("OK", Accepted);
      pButtons->addButton("Join", Join);
      pButtons->addButton("DCC Chat", DccChat);
      pButtons->addButton("Chat", Join);
      pButtons->addButton("Names/Whois", Names);
      pButtons->addButton("Clear", Clear);
      pButtons->addButton("Close", Rejected);
   }

   addWidget(pChannel);
   addWidget(pButtons);
   initFrame();
   
   connect(pChannel, SIGNAL(gotEntry(const char *)),
           this, SLOT(gotReturnFromChannel(const char *)));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   if (dbg) fprintf(stdout, "xChannelQuery::xinitClass():Exit\n\r");
}

void xChannelQuery::accept()
{
   QDialog::accept();
}

void xChannelQuery::reject()
{
   QDialog::reject();
}

void xChannelQuery::gotReturn()
{
   if (dbg) fprintf(stdout, "xChannelQuery::gotReturn():Enter\n\r");
   emit hasResult(Accepted);
   accept();
   if (dbg) fprintf(stdout, "xChannelQuery::gotReturn():Exit\n\r");
}

void xChannelQuery::gotReturnFromChannel(const char *)
{
   if (dbg) fprintf(stdout, "xChannelQuery::gotReturnFromChannel():Enter\n\r");
   emit hasResult(Accepted);
   accept();
   if (dbg) fprintf(stdout, "xChannelQuery::gotReturnFromChannel():Exit\n\r");
}

void xChannelQuery::buttonPressed(int results)
{
   xIrcMessage msg;
   const char *cp;
   int x;

   if (results == Rejected)
   {
      emit hasResult(results);
      done(results);
   }
   else
   {
      switch(results)
      {
         case Clear:
            pChannel->setText("");
            break;

         case Names:
            if (strlen(cp = pChannel->text()) == 0)
               if ((x = pChannel->currentItem()) >= 0)
                  cp = pChannel->string(x);
               else
                  break;
            while (isspace(*cp)) cp++;
            if (*cp == '#')
               msg.rspCode = ircResponses.code("NAMES");
            else
               msg.rspCode = ircResponses.code("WHOIS");
            msg.dstStr = cp;
            msg.msgStr = "";
            emit ircMessageOut(&msg);
            
            break;

         case DccChat:
            if (strlen(cp = pChannel->text()) == 0)
               if ((x = pChannel->currentItem()) >= 0)
               {
                  pChannel->setText(pChannel->string(x));
                  cp = pChannel->text();
               }
               else
                  break;
            while (isspace(*cp)) cp++;
            if (*cp == '#')
               QMessageBox::warning(this, "Error", "Cannot DCC Chat to a channel");
            else
               emit hasResult(results);
            break;

         case Accepted:
         case Join:
            if (strlen(pChannel->text()) == 0)
            {
               if ((x = pChannel->currentItem()) >= 0)
               {
                  pChannel->setText(pChannel->string(x));
                  emit hasResult(Accepted);
               }
            }
            else
               emit hasResult(Accepted);
            break;
      }
   }
}

xChannelQuery::~xChannelQuery()
{
   if (dbg) fprintf(stdout, "xChannelQuery::~xChannelQuery():Disconnecting Signals\n");
   if (dbg) fflush(stdout);
   disconnect(this);
   if (pChannel)
   {
      if (dbg) fprintf(stdout, "xChannelQuery::~xChannelQuery():Deleting pChannel\n");
      if (dbg) fflush(stdout);
      delete pChannel;
   }
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xChannelQuery::~xChannelQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
   if (dbg) fprintf(stdout, "xChannelQuery::~xChannel():Done!\n");
   if (dbg) fflush(stdout);
}

//#include "xIrcChannelQuery.moc"
