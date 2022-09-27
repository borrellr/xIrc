/***************************************************************************
**    xIrcChanNickBox.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Dialog Box for Disp./Manip. channel members
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
#include "xIrcChanNickBox.h"

static int dbg = 0;
   
xIrcChanNickBox::xIrcChanNickBox(xWidgetResInfo *pPRes, QWidget *pParent,
                                 const char *pName) : 
                 xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("channickdialog"), 
                                                     QString("ChanNickDialog")),
                         pParent, pName)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   if (pName)
      setCaption(pName);
      
   setAlignment(xALIGN_Horz);
   
   pBtns = new xPshBtnFrame(wdtRes, this);
   pBtns->addButton("Who is", xCN_Whois);
   pBtns->addButton("Chat", xCN_Query);
   pBtns->addButton("Ping", xCN_Ping);
   pBtns->addButton("Invite", xCN_Invite);
   pBtns->addButton("Kick", xCN_Kick);
   pBtns->addButton("Clear", xCN_Clear);
   pBtns->addButton("Close", xCN_Close);
   pBtns->setFrameStyle(QFrame::Panel | QFrame::Raised);
   
   pNicks = new xIrcNickCombo(wdtRes, this);
   pNicks->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pNicks->setLabel("Select Nick:");
   pNicks->setMargins(10, 10);
   
   QFontMetrics fm = pNicks->QWidget::fontMetrics();
   pNicks->resize(fm.maxWidth() * 20, (fm.lineSpacing() * 20) + 5);
   
   addWidget(pBtns);
   addWidget(pNicks);
   
   setMargins(0,0);
   setWidgetSpacing(0);
   
   setMargins(0,0);
   initFrame();
   
   connect(pNicks, SIGNAL(nickSelected(const char *)), 
           this, SLOT(gotSelected(const char *)));
   connect(pBtns, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
}

xIrcChanNickBox::~xIrcChanNickBox()
{
   disconnect(this);
   if (dbg) fprintf(stdout, "xIrcChanNickBox::~xIrcChanNickBox():Enter\n");
   if (dbg) fflush(stdout);
   if (pNicks)
      delete pNicks;
   if (dbg) fprintf(stdout, "xIrcChanNickBox::~xIrcChanNickBox():Exit\n");
   if (dbg) fflush(stdout);
}

bool xIrcChanNickBox::is(const char *pName)
{
   return(pNicks->is(pName));
}

void xIrcChanNickBox::setNicks(const char *pNames)
{
   xIrcNickList *pTmpList;
   xIrcNickListEntry *pEntry, *pEntry1;

   if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Enter\n");
   if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Creating new List\n");
   if (dbg) fflush(stdout);
   pTmpList = new xIrcNickList();
   pTmpList->merge(pNames);

   if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Testing for names to delete from old list\n");
   if (dbg) fflush(stdout);
   for (pEntry = pNicks->getNext(NULL); pEntry != NULL; pEntry = pEntry1)
   {
      pEntry1 = pNicks->getNext(pEntry);
      if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Testing |%s| from org list\n",
                              (const char *)pEntry->getNick());
      if (dbg) fflush(stdout);
      if (pTmpList->is((const char *)pEntry->getNick()))
      {
         if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():It's a keeper!!!!!\n");
         if (dbg) fflush(stdout);
         pTmpList->remove(pEntry->getNick(), TRUE);
      }
      else
      {
         if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():They're gone, chuck em!\n");
         if (dbg) fflush(stdout);
         pNicks->remove(pEntry->getNick(), TRUE);
      }
   }
   if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Adding new names to org list\n");
   if (dbg) fflush(stdout);
   for (pEntry = pTmpList->getNext(NULL); pEntry != NULL; pEntry = pEntry1)
   {
      pEntry1 = pTmpList->getNext(pEntry);
      if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Adding |%s|\n",
                              (const char *)pEntry->getNick());
      if (dbg) fflush(stdout);
      pNicks->merge(pEntry->getNick(TRUE));
      if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Removing |%s| from new list\n",
                              (const char *)pEntry->getNick());
      if (dbg) fflush(stdout);
      pTmpList->remove((const char *)pEntry->getNick(), FALSE);
      delete pEntry;
   }
   delete pTmpList;
// pNicks->purgeList();
   setHeight(pNicks->xIrcNickList::count());
   if (dbg) fprintf(stdout, "xIrcChanNickBox::setNicks():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcChanNickBox::merge(const char *pNames, char delim)
{
   pNicks->merge(pNames, delim);
}

void xIrcChanNickBox::remove(const char *pNames, char delim, int delflag)
{
   pNicks->remove(pNames, delim, delflag);
}

xIrcNickListEntry *xIrcChanNickBox::selected()
{
   return(pNicks->selected());
}

void xIrcChanNickBox::clear()
{
   pNicks->clearList();
}

const char *xIrcChanNickBox::getNames(char delim)
{
   return(pNicks->getNicks(delim));
}

const char *xIrcChanNickBox::getAddrs(char delim)
{
   return(pNicks->getAddrs(delim));
}

void xIrcChanNickBox::setHeight(int rows)
{
   char buf[50];
   
   sprintf(buf, "%s - %d Members", name(), rows);
   setCaption(buf);
/*
   QFontMetrics fm = pNicks->QWidget::fontMetrics();

   if (dbg) fprintf(stdout, "xIrcChanNickBox::setHeight():Enter\n");
   pNicks->resize(fm.maxWidth() * 20, (fm.lineSpacing() * ((rows < 15) ? rows : 15)) + 5);
   if (dbg) fprintf(stdout, "xIrcChanNickBox::setHeight():Fitting Frame\n");
   fitFrame();
   QSize s = mainFrame->size();
   resize(&s);
   if (dbg) fprintf(stdout, "xIrcChanNickBox::setHeight():Exit\n");
*/
}

void xIrcChanNickBox::gotSelected(const char *pName)
{
//   pName++;
   if (dbg) fprintf(stdout, "xIrcChanNickBox::gotSelected():Enter\n");
   if (dbg) fflush(stdout);
   emit nickSelected(pName);
//   emit buttonPressed(xCN_Query);
   if (dbg) fprintf(stdout, "xIrcChanNickBox::gotSelected():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcChanNickBox::buttonClicked(int iBtn)
{
   if (dbg) fprintf(stdout, "xIrcChanNickBox::buttonPressed():Enter\n");
   if (iBtn == xCN_Clear)
      clear();
   else
      emit buttonPressed(iBtn);
}

//#include "xIrcChanNickBox.moc"
