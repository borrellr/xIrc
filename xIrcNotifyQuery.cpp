/***************************************************************************
**    xIrcNotifyQuery.cpp  $Revision: 1.5 $ - $Name:  $ 
**    Dialog box to manage the Notification list
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
#include <qt.h>
#include <qfiledialog.h>
#include "xIrcCommands.h"
#include "xIrcConnect.h"
#include "xIrcMsgDispatch.h"
#include "xIrcNotifyQuery.h"
#include "xIrcPeopleEdit.h"

static bool dbg = false;
                    
extern xIrcConnect *pTWindow;
extern xIrcCommands ircResponses;
extern xIrcMsgDispatch Dispatcher;

static const char *pInitialResources[] =
{
   NULL
};

xIrcNotifyQuery::xIrcNotifyQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("notifydialog"),
                                                   QString("NotifyDialog")),
                       pParent, pName, FALSE)
{
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   initClass(pName);
}

void xIrcNotifyQuery::initClass(const char *pName)
{
   const char *pFn, *pPath;
   QString serverFile;

   if (dbg) fprintf(stdout, "xIrcNotifyQuery::xinitClass():Enter\n");
   if (dbg) fflush(stdout); 

   xWidgetResInfo wdtTmp(wdtRes, QString("menu"), QString("Menu"));
   xWidgetResInfo wdtPopTmp(&wdtTmp, QString("popup"), QString("Popup"));

   setCaption("Notify List");
   mainFrame->setAutoDelete(TRUE);
   if (pName)
      setCaption(pName);

   setFocusPolicy(StrongFocus);

   whoSent = FALSE;

   pTimer = new QTimer(this, NULL);
   connect(pTimer, SIGNAL(timeout()), this, SLOT(pollList()));

   pLabel = new QLabel(this);
   pTable = new xIrcPeopleTable(wdtRes, this, "servertable", "Notify", 10);

   pFileMenu = new QPopupMenu;
   pFileMenu->insertItem("New", this, SLOT(newList()));
   pFileMenu->insertItem("Load", this, SLOT(loadList()));
   pFileMenu->insertItem("Save As", this, SLOT(saveList()));
   pFileMenu->insertItem("Done", this, SLOT(reject()));
   setDefFont(pFileMenu, &wdtPopTmp);
   setDefPallet(pFileMenu, &wdtPopTmp);


   pNotifyMenu = new QPopupMenu();
   pNotifyMenu->insertItem("Edit", this, SLOT(editEntry()));
   pNotifyMenu->insertItem("New", this, SLOT(newEntry()));
   pNotifyMenu->insertItem("Delete", this, SLOT(deleteEntry()));
   setDefFont(pNotifyMenu, &wdtPopTmp);
   setDefPallet(pNotifyMenu, &wdtPopTmp);

   pMenu = new QMenuBar(this);
   pMenu->insertItem("&File", pFileMenu);
   pMenu->insertItem("&Nick", pNotifyMenu);
   setDefFont(pMenu, &wdtTmp);
   setDefPallet(pMenu, &wdtTmp);

   pAccel = new QAccel(this, "");
   pAccel->connectItem(pAccel->insertItem(Key_Prior), pTable, SLOT(scrollPageBack()));
   pAccel->connectItem(pAccel->insertItem(Key_Next), pTable, SLOT(scrollPageFwd()));
   pAccel->connectItem(pAccel->insertItem(Key_Up), pTable, SLOT(highlightPrev()));
   pAccel->connectItem(pAccel->insertItem(Key_Down), pTable, SLOT(highlightNext()));
   pAccel->connectItem(pAccel->insertItem(Key_Escape), this, SLOT(reject()));
   pAccel->connectItem(pAccel->insertItem(Key_Home), this, SLOT(gotoFirst()));
   pAccel->connectItem(pAccel->insertItem(Key_End), this, SLOT(gotoLast()));
   
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   serverFile = pPath;
   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".xIrc.notify";
   serverFile += '/';
   serverFile += pFn;
   pTable->readFile(serverFile.latin1());

   pPeople = NULL;
   if (pTable->list()->count() > 0)
   {
      pPeople = new xIrcPeopleListIterator(*pTable->list());
      pPeople->toFirst();
   }
   
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Done",     xIrcNotifyQuery::Accepted);
/*
   pButtons->addButton("Edit",  xIrcNotifyQuery::Edit);
   pButtons->addButton("Delete",  xIrcNotifyQuery::Delete);
   pButtons->addButton("Load",  xIrcNotifyQuery::Load);
   pButtons->addButton("Save",  xIrcNotifyQuery::Save);
   pButtons->addButton("Import",  xIrcNotifyQuery::Import);
   pButtons->addButton("Clear",  xIrcNotifyQuery::Clear);
*/

   addWidget(pLabel);
   addWidget(pTable);
   addWidget(pButtons);
   setResizeMode(xSPACE_Resize);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pTable, SIGNAL(rowDoubleClicked(int)),
           this, SLOT(editEntry(int)));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
   connect(pTable, SIGNAL(returnPressed()),
           this, SLOT(editEntry()));
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::xinitClass():Exit\n");
   if (dbg) fflush(stdout); 
}

void xIrcNotifyQuery::gotButton(int btn)
{
   QString tmpStr;

   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotButton(%d):Enter\n\r", btn);
   if (dbg) fflush(stdout); 
   done(btn);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotButton():Exit\n\r");
}

void xIrcNotifyQuery::accept()
{
   QDialog::accept();
}

void xIrcNotifyQuery::reject()
{
   QDialog::reject();
}

void xIrcNotifyQuery::gotReturn()
{

   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotReturn():Enter\n\r");
   editEntry();
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotReturn():Exit\n\r");
}

void xIrcNotifyQuery::newList()
{
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::newList():Enter\n");
   if (dbg) fflush(stdout);
   pTable->clear();
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::newList():Calling shoRows()\n");
   if (dbg) fflush(stdout);
   pTable->showRows(0);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::newList():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNotifyQuery::saveList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".xIrc.notify";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".xIrc*";

   fileName = QFileDialog::getSaveFileName(pPath + "/" + pFn, pFilt, this);
   if (!fileName.isNull())
      pTable->writeFile(fileName.latin1());
}

void xIrcNotifyQuery::newEntry()
{
   xIrcPeopleEntry e("", "", "", 1);

   xIrcPeopleEdit editDlg(wdtRes, NULL, "Notify Entry Edit", &e);
   if (editDlg.exec() == QDialog::Accepted)
      pTable->add(e);
   pTable->showRows(pTable->currentRow());
}

void xIrcNotifyQuery::editEntry(int entry)
{
   if (pTable->entry(entry) != NULL)
   {
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Notify Entry Edit", pTable->entry(entry));
      editDlg.exec();
      pTable->showRows(pTable->currentRow());
   }
   else
      newEntry();
}

void xIrcNotifyQuery::editEntry()
{
   if (pTable->entry(pTable->selectedRow()) != NULL)
   {
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Notify", 
                             pTable->entry(pTable->selectedRow()));
      editDlg.exec();
      pTable->showRows(pTable->currentRow());
   }
}

void xIrcNotifyQuery::add(xIrcPeopleEntry &e)
{
   pTable->add(e);
   pTable->showRows(pTable->currentRow());
}

void xIrcNotifyQuery::deleteEntry()
{
   if (pTable->selectedRow() >= 0)
      pTable->remove(pTable->selectedRow());
}

void xIrcNotifyQuery::loadList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "file", "File")) == NULL)
      pFn = ".xIrc.notify";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".xIrc*";
   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);
   pTable->readFile(fileName.latin1());
   pTable->showRows(pTable->currentRow());
}

QString xIrcNotifyQuery::getNick(xIrcMessage *pMsg)
{
   QString nickStr;
   const char *cp, *cp1;

   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Gleening Nick from: |%s|\n",
                             (const char *)pMsg->msgStr.latin1());
   if (dbg) fflush(stdout);
   cp = pMsg->msgStr.latin1();
   cp1 = pMsg->dstStr.latin1();
   if (*cp1 == '#')
   {
      if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping Username: |%s|\n",
                               (const char *)cp);
      for (; *cp != ' '; cp++);
   }
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping WS: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp == ' '; cp++);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping Hostname: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp != ' '; cp++);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping WS: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp == ' '; cp++);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping Server name: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp != ' '; cp++);
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Skipping WS: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp == ' '; cp++);
   /*
   ** Assume that the 4th field is the nick
   */
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::getNick():Nick: |%s|\n",
      (const char *)cp);
   if (dbg) fflush(stdout);
   for (; *cp != ' '; cp++)
      nickStr += *cp;
   return(nickStr);
}

void xIrcNotifyQuery::gotoFirst()
{
//   pTable->scroll(0);
   pTable->highlightRow(0);
   pTable->highlightPrev();
}

void xIrcNotifyQuery::gotoLast()
{
   pTable->highlightRow(pTable->rows() - 1);
//   pTable->scroll(pTable->rows() - 1);
}

xIrcNotifyQuery::~xIrcNotifyQuery()
{
   if (pAccel)
      delete pAccel;

/*
   if (pLabel)
      delete pLabel;
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xIrcNotifyQuery::~xIrcNotifyQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
*/
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::~xIrcNotifyQuery():Done!\n");
   if (dbg) fflush(stdout);
}

void xIrcNotifyQuery::pollList()
{
   gotNotification(NULL);
}

bool xIrcNotifyQuery::gotNotification(xIrcMessage *pMsg)
{
   xIrcPeopleEntry *pEntry;
   xIrcMessage msg;
   const char *cp;
   bool rv = FALSE;

   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Enter\n");
   if (dbg) fflush(stdout);
   if (pMsg != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Have Response\n");
      if (dbg) fflush(stdout);
      rv = TRUE;

      QString str;
      cp = pMsg->dstStr.latin1();
      if (pMsg->rspCode == 352)
      {
         if (*cp != '#')
         {
            str = pMsg->dstStr;
            cp = pMsg->msgStr.latin1();
         }
         else
         {
            for (cp = pMsg->msgStr.latin1(); *cp != ' '; cp++)
               str += *cp;
            for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
         }
         str += "@";
         for (cp = pMsg->msgStr.latin1(); *cp != ' '; cp++)
            str += *cp;
         pEntry = pTable->entry(pMsg);
         if (pEntry != NULL)
         {
            QString realNick(getNick(pMsg));
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Current State = %d\n",
                                    pEntry->state());
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Setting realNick to: |%s|\n",
                                    (const char *)realNick.latin1());
            if (dbg) fflush(stdout);
            pEntry->setRealNick(realNick.latin1());
         }
         if (pEntry != NULL && pEntry->state() == 1)
         {
            char buf[256];
            
            sprintf(buf, "\x02%s (%s) Has Arrived!!\n",
               (const char *)pEntry->realNick().latin1(),
               (const char *)pEntry->mask().latin1());
            if (pEntry->message() != NULL && !pEntry->message().isEmpty())
            {
               if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Sending Message %d:|%s|\n",
                                    (int)((const char *)pEntry->message().latin1()),
                                    (const char *)pEntry->message().latin1());
               if (dbg) fflush(stdout);
               msg.rspCode = ircResponses.code("PRIVMSG");
               msg.dstStr = getNick(pMsg);
               msg.msgStr = pEntry->message();
               pTWindow->sendMessage(&msg);
            }
            pEntry->setState(3);
            emit updateList(pTable->list());
         }
         else if (pEntry != NULL && pEntry->state() == 2)
         {
            pEntry->setState(3);
         }
         if (pEntry != NULL)
         {
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():New State = %d\n",
                                    pEntry->state());
         }
      }
      else if (pMsg->rspCode == 315 || (pMsg->rspCode >= 400 && pMsg->rspCode < 600))
      {
         if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Have end of who message (315)\n");
         if (dbg) fflush(stdout);
         pEntry = pTable->entry(pMsg);
         if (pEntry != NULL)
         {
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Current State = %d\n",
                                    pEntry->state());
         }
         if (pEntry != NULL && pEntry->state() == 3)
         {
            pEntry->setState(2);
         }
         else if (pEntry != NULL && (pEntry->state() == 1 || pEntry->state() == 2))
         {
            if (pEntry->state() == 2)
            {
               char buf[256];
               sprintf(buf, "\x02%s (%s) Has Left!!\n",
                       (const char *)pEntry->realNick().latin1(),
                       (const char *)pEntry->mask().latin1());
               pTWindow->putWindow(buf);
            }
            pEntry->setState(0);
            emit updateList(pTable->list());
         }
         if (pEntry != NULL)
         {
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():New State = %d\n",
                                    pEntry->state());
         }
         if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Setting timer for next who\n");
         if (dbg) fflush(stdout);
         pTimer->start(2500, TRUE);
         whoSent = FALSE;
         ++(*pPeople);
      }
   }
   else if (pPeople != NULL && pPeople->count() > 0)
   {
      QString strNick, strTmp;
      char ch;

      if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():No response, Count > 0\n");
      if (dbg) fflush(stdout);
      rv = TRUE;
      if (pPeople->current() == NULL)
      {
         if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Reseting list\n");
         if (dbg) fflush(stdout);
         delete pPeople;
         pPeople = NULL;
         if (pTable->list()->count() > 0)
         {
            pPeople = new xIrcPeopleListIterator(*pTable->list());
            pPeople->toFirst();
            pTimer->start(5000, TRUE);
         }
      }
      else
      {   
         if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Finding Next Person in the list\n");
         if (dbg) fflush(stdout);
         while (pPeople->current() != NULL && pPeople->current()->flag() == 0)
            ++(*pPeople);
         if (pPeople->current() != NULL)
         {
            if (pPeople->current()->state() == 0)
               pPeople->current()->setState(1);

            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Nick = |%s|\n",
                                     (const char *)pPeople->current()->nick().latin1());
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Mask = |%s|\n",
                                     (const char *)pPeople->current()->mask().latin1());
            if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Flag = %d\n",
                                     pPeople->current()->flag());

            if (pPeople->current()->flag() < 0)
               strNick = pPeople->current()->nick();

            else if (pPeople->current()->flag() > 0)
            {
               strTmp = pPeople->current()->mask();
               if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():Sending 'WHO %s'\n",
                                       (const char *)strTmp.latin1());
               if (dbg) fflush(stdout);
               for (ch = ' ', cp = strTmp.latin1(); *cp != '\0'; cp++)
               {
                  if (ch == '@')
                     strNick += *cp;
                  else if (*cp == '@')
                     ch = '@';
               }
            }
            msg.rspCode = ircResponses.code("WHO");
            msg.dstStr = strNick;
            msg.msgStr = "";
            Dispatcher.dispatchMsg(this, SLOT(messageIn(xIrcMessage*)), &msg);
//            pTWindow->sendMessage(&msg);
            whoSent = TRUE;
         }
      }
   }
   else if (pPeople != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():No response, Count == 0\n");
      if (dbg) fflush(stdout);
      rv = TRUE;
      delete pPeople;
      pPeople = NULL;
      whoSent = FALSE;
      pTimer->start(2500, TRUE);
   }
   else
   {
      if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification():No response, No Table\n");
      if (dbg) fflush(stdout);
      if (pTable->list()->count() > 0)
      {
         pPeople = new xIrcPeopleListIterator(*pTable->list());
         pPeople->toFirst();
      }
      pTimer->start(2500, TRUE);
   }
   if (dbg) fprintf(stdout, "xIrcNotifyQuery::gotNotification(%d):Exit\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}
            
void xIrcNotifyQuery::startUp()
{
   pTimer->start(500, TRUE);
}

void xIrcNotifyQuery::shutDown()
{
   pTimer->stop();
}

//#include "xIrcNotifyQuery.moc"
