/***************************************************************************
**    xIrcConnect2.cpp  $Revision: 1.5 $ - $Name:  $
**    Class for making the server connections Part 2
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
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <qt.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qmenubar.h>
#include <qsize.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <xApp.h>
#include "xIrcLineEditQuery.h"
#include "xIrcChannelQuery.h"
#include "xIrcMsgDispatch.h"
#include "xIrcDccQuery.h"
#include "xIrcServerQuery.h"
#include "xIrcInviteBox.h"
#include "xDefaults.h"
#include "xIrcConnect.h"

static int dbg = 0;

extern xApplication *pApp;
extern xDefaults Defaults;
extern xChannelQuery *ChanQuery;
extern xServerQuery *ServQuery;
extern xIrcLineEditQuery *QuitQuery;
extern xIrcCommands ircResponses;
extern xIrcMsgDispatch Dispatcher;

void xIrcConnect::initiateDCCFile(xIrcDccFile *fFrame)
{
   struct sockaddr_in addr1, addr2;
   xIrcMessage msg;
   char buf[512], fName[512];
   QString tmpStr, tmpStr1, tmpStr2;
   const char *cp;
   unsigned int fSize;
   struct stat fs;

   sleep(1);
   if (pSocket == NULL)
   {
      QMessageBox::warning(this, "Error", "Not Connected to a Server");
      return;
   }

   //
   // Point to the port number we're accepting connections from
   //
   addr1 = fFrame->socketName();
   //
   // Then the IP address we're talking to the network on and
   // add the port we're accepting connections from
   //
   addr2 = pSocket->socketName();
   addr2.sin_family = AF_INET;
   addr2.sin_port = addr1.sin_port;

   if (dbg) fprintf(stdout, "xIrcConnect::initiateDCCFile():Enter\n");
   if (dbg) fflush(stdout);
   fstat(fFrame->file_d(), &fs);
   fSize = fs.st_size;
   strcpy(fName, fFrame->filename());
   sprintf(buf, "Initiating DCC File Send (%s:%d)", inet_ntoa(addr2.sin_addr), addr1.sin_port);
   pMainWin->pWin->putString(buf);
   pMainWin->pWin->putString("\n");
   msg.rspCode = ircResponses.code("NOTICE");
   msg.dstStr = fFrame->name();
   msg.msgStr = buf;
   emit msgOut(&msg);

   if (dbg) fprintf(stdout, "xIrcConnect::initiateDCCFile():File name: |%s|, %d\n",
                           (const char*)fName, strlen(fName));
   if (dbg) fflush(stdout);
   for (cp = fName + strlen(fName) - 1; cp != (const char*)fName && *cp != '/'; cp--);
   {
      if (dbg) fprintf(stdout, "xIrcConnect::initiateDCCFile():Backing over: |%c|\n", *cp);
      if (dbg) fflush(stdout);
   }
   if (*cp == '/')
      cp++;
   if (dbg) fprintf(stdout, "xIrcConnect::initiateDCCFile():File name: |%s|\n",
                           (const char*)cp);
   if (dbg) fflush(stdout);
   sprintf(buf, "%cDCC SEND %s %u %u %u%c", '\x01',
                cp,                
                ntohl(addr2.sin_addr.s_addr), ntohs(addr1.sin_port), 
                fSize, '\x01');
   pMainWin->pWin->putString(buf);
   pMainWin->pWin->putString("\n");
   if (dbg) fprintf(stdout, "xIrcConnect::initiateDCCFile():Sending message |%s|!\n", buf);
   msg.rspCode = ircResponses.code("PRIVMSG");
   msg.dstStr = fFrame->name();
   msg.msgStr = buf;
   emit msgOut(&msg);
}

void xIrcConnect::initiateDCCChat(xIrcDccChatFrame *chatFrame)
{
   struct sockaddr_in addr1, addr2;
   xIrcMessage msg;
   char buf[512];
   QString tmpStr, tmpStr1, tmpStr2;

   if (pSocket == NULL)
   {
      QMessageBox::warning(this, "Error", "Not Connected to a Server");
      return;
   }

   //
   // Point to the port number we're accepting connections from
   //
   addr1 = chatFrame->socketName();
   //
   // Then the IP address we're talking to the network on and
   // add the port we're accepting connections from
   //
   addr2 = pSocket->socketName();
   addr2.sin_family = AF_INET;
   addr2.sin_port = addr1.sin_port;

   sprintf(buf, "Initiating DCC Chat (%s)", inet_ntoa(addr2.sin_addr));
   pMainWin->pWin->putString(buf);
   pMainWin->pWin->putString("\n");
   msg.rspCode = ircResponses.code("NOTICE");
   msg.dstStr = chatFrame->name();
   msg.msgStr = buf;
   emit msgOut(&msg);
   sprintf(buf, "%cDCC CHAT chat %u %u%c", '\x01',
                ntohl(addr2.sin_addr.s_addr), ntohs(addr2.sin_port), '\x01');
   pMainWin->pWin->putString(buf);
   pMainWin->pWin->putString("\n");
   msg.rspCode = ircResponses.code("PRIVMSG");
   msg.dstStr = chatFrame->name();
   msg.msgStr = buf;
   emit msgOut(&msg);
}

void xIrcConnect::dccChatHandler(xIrcMessage *pMsg)
{
   xIrcMessageFrame *pMsgFrame;
   xIrcDccChatFrame *pDccChatFrame;
   xIrcDccFile *pDccFileFrame = NULL;
   char buf[256];
   int err;
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():got response |%s| from query\n",
                            (const char *)pMsg->srcNick);
   if (dbg) fflush(stdout);
   if (pMsg->rspCode == xIrcDccQuery::Message)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Setting up normal chat window!\n");
      if (dbg) fflush(stdout);
      if ((pMsgFrame = makeNewMsgFrame(NULL, pMsg->srcNick)) != NULL)
         pMsgFrame->show();
   }
   else if (pMsg->rspCode == xIrcDccQuery::Accepted)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Got Accept from Query, isolating IP from |%s|\n",
                               (const char *)pMsg->msgStr);
      for (cp = pMsg->msgStr; isspace(*cp); cp++);
      if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Spaces Skipped, remaining: |%s|\n", cp);
      for (; !isspace(*cp); cp++);
      if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Setting up DCC Chat to %s @ |%s|!\n",
                               (const char *)pMsg->srcNick, cp);
      if (dbg) fflush(stdout);
      if (pMsg->pmsgTyp == ipmDCCChat)
      {
         pDccChatFrame = new xIrcDccChatFrame(wdtPrv, NULL, pMsg->srcNick);
         if (pDccChatFrame != NULL)
         {
            if ((err = pDccChatFrame->connectTo(cp)) == 0)
            {
               connect(pDccChatFrame,
                       SIGNAL(textSelected(xMultiLineTextSelection)),
                       pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
               pDccChatFrame->show();
            }
            else
            {
               sprintf(buf, "Error Making DCC Connection to %s:%s",
                            (const char *)pMsg->srcNick, strerror(err));
               QMessageBox::warning(this, "Error", buf);
               delete pDccChatFrame;
            }
         }
      }
      else if (pMsg->pmsgTyp == ipmDCCSend)
      {
         int fd;
         const char *cp;
         QString file;
         const char *pDir;

         if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Have file send request\n");
         if (dbg) fflush(stdout);
         pDir = Resources->get(wdtRes, "dcc.dir", "DCC.Dir");
         for (cp = pMsg->msgStr; *(++cp) != ' ';)
            file += *cp;
         if (pSaveDialog->exec() == QDialog::Accepted)
         {
            if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():Accepted, getting filename\n");
            if (dbg) fflush(stdout);
            QString s(pSaveDialog->selectedFile());
            if (dbg) fprintf(stdout, "xIrcConnect::dccChatHandler():File name = |%s|\n",
                                     (const char *)s);
            if (dbg) fflush(stdout);
            if ((fd = open(s, O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH)) >= 0)
            {
               pDccFileFrame = new xIrcDccFile(wdtPrv, file, NULL, pMsg->srcNick);
               if (pDccFileFrame != NULL)
               {
                  if ((err = pDccFileFrame->connectTo(cp, fd)) == 0)
                     pDccFileFrame->show();
                  else
                  {
                     sprintf(buf, "Error Making DCC Connection to %s:%s",
                                  (const char *)pMsg->srcNick, strerror(err));
                     QMessageBox::warning(this, "Error", buf);
                     delete pDccFileFrame;
                  }
               }
            }
            else
            {
               sprintf(buf, "Error Opening file %s:%s",
                            (const char *)s, strerror(errno));
               QMessageBox::warning(this, "Error", buf);
               delete pDccFileFrame;
            }
         }
      }
   }
}

void xIrcConnect::msgQryHandler(xIrcMsgQuery *pMsgQuery)
{
   xIrcMessageFrame *pMsgFrame;
   xIrcMessageList *pMsgList;
   xIrcMessage *pMsg, *pMsg1;
   
   if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():got response from query\n");
   if (dbg) fflush(stdout);
//   if ((msgList = pMsgQuery->getList()) != NULL)
   pMsgList = pMsgQuery->getList();
   disconnect(pMsgQuery, SIGNAL(done(xIrcMsgQuery *)), 
           this, SLOT(msgQryHandler(xIrcMsgQuery *)));
   disconnect(pMsgQuery, SIGNAL(textSelected(xMultiLineTextSelection)),
           pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
   if (pMsgQuery->results() == QDialog::Accepted)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():Creating new message frame\n");
      if (dbg) fflush(stdout);
      pMsg = pMsgList->first();
      pMsg1 = pMsg;
      if ((pMsgFrame = makeNewMsgFrame(NULL, pMsg->srcNick)) != NULL)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():Showing message frame\n");
         if (dbg) fflush(stdout);
         pMsgFrame->show();
         for (pMsg = pMsgList->first(); pMsg != NULL; pMsg = pMsgList->next())
         {
            if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():Sending message to frame\n");
            if (dbg) fflush(stdout);
            pMsgFrame->procServerMsg(pMsg);
         }
      }
   }
   if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():Removing Query\n");
   if (dbg) fflush(stdout);
   msgQueryList.remove(pMsgQuery);
   if (dbg) fprintf(stdout, "xIrcConnect::msgQryHandler():Exit()\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::acceptInvite(QString strChannel)
{
   ChanQuery->setText((const char *)strChannel);
   newChannel();
}

void xIrcConnect::gotResponse(xIrcMessage *pMsg)
{
   xIrcMessageFrame *pMsgFrame;
   xIrcMsgQuery *pMsgQry;
   xIrcMessage msg;
   char str[120];
   int x;
   
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():msgTyp = %d\n", (int)pMsg->pmsgTyp);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():rspCode = %d\n", (int)pMsg->rspCode);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():rspStr = %s\n", (const char *)pMsg->rspStr);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():srcNick = %s\n", (const char *)pMsg->srcNick);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():srcAddr = %s\n", (const char *)pMsg->srcAddr);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():dstStr = %s\n", (const char *)pMsg->dstStr);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():msgStr = %s\n", (const char *)pMsg->msgStr);
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():rawMsg = %s\n", (const char *)pMsg->rawMsg);
   if (dbg) fflush(stdout);

   pSocketBox->hide();
   if (Dispatcher.proccessResponse(pMsg) == TRUE)
      return;
   if (isMsg(pMsg->rspCode, "NICK"))
   {
      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():A Nick is Changing!\n");
      if (dbg) fflush(stdout);
      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Check for any windows w/ the old nick!\n");
      if (dbg) fflush(stdout);
      for (x = 0, pMsgFrame = pIrcMsgFrames; pMsgFrame; pMsgFrame = pMsgFrame->next())
      {
         if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Testing window |%s|\n", (char *)pMsgFrame->name());
         if (dbg) fflush(stdout);
         if (strlen(pMsgFrame->name()) > 0 &&
             pMsgFrame->is((const char *)pMsg->srcNick))
         {
            if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Found a Match!!!\n");
            if (dbg) fflush(stdout);
            pMsgFrame->newName((const char *)pMsg->dstStr);
            sprintf(str, "[B]*** %s is now known as %s\n",
                         (const char *)pMsg->srcNick,
                         (const char *)pMsg->dstStr);
            pMsgFrame->putString(str);
         }
      }
   }

   /*
   ** Startup the notification machine on End of MOTD message
   */
   if (pMsg->rspCode == 376)
      pNotify->startUp();

/*
   if ((pMsg->rspCode == 352 || pMsg->rspCode == 315) &&
       pNotify->gotNotification(pMsg) == TRUE)
   {
      return;
   }
*/
   if (isMsg(pMsg->rspCode, "INVITE"))
   {
      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Got invitation!\n");
      if (dbg) fflush(stdout);
      xIrcInviteBox *pIB = new xIrcInviteBox(wdtPrv, NULL, "Invite", this, 
                                             SLOT(acceptInvite(QString)),
                                             pMsg);
      pIB->show();
      return;
   }

   if (pIgnore->list()->matches(pMsg))
   {
      xIrcPeopleEntry *p;
      if ((p = pIgnore->list()->entry(pMsg)) != NULL)
      {
         if (p->flag() != 0)
         {
            showResponse(pMsg);
            return;
         }
      }
   }

   for (x = 0, pMsgFrame = pIrcMsgFrames; pMsgFrame; pMsgFrame = pMsgFrame->next())
   {
      QRegExp regexp( " .*" );
      QString tempName((char*)pMsgFrame->name());
      tempName.replace( regexp, "");
      pMsgFrame->setName((const char *)tempName);

      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():testing window |%s|\n", (char *)pMsgFrame->name());
      if (dbg) fflush(stdout);
      if (strlen(pMsgFrame->name()) > 0 && pMsgFrame->procServerMsg(pMsg))
      {
         if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Matched!!!\n");
         if (dbg) fflush(stdout);
         x++;
         break;
      }
      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Done\n");
      if (dbg) fflush(stdout);
   }
   
   if (x == 0 & dbg) fprintf(stdout, "xIrcConnect::gotResponse():Testing if new PRIVMSG\n");
   if (dbg) fflush(stdout);
   if (x == 0 && ((!isMsg(pMsg->rspCode, "PRIVMSG") && 
                   !isMsg(pMsg->rspCode, "NOTICE")) ||
                  strlen(pMsg->srcNick) == 0))
   {
      if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Not new PRIVMSG\n");
      if (dbg) fflush(stdout);
      if ((pMsg->rspCode >= IRCRESP_First && pMsg->rspCode <= IRCRESP_Last) ||
          (pMsg->rspCode < 400))
         showResponse(pMsg);
      else
         showError(pMsg);
   }
   else if (x == 0)
   {
      if (!isMsg(pMsg->rspCode, "NOTICE") || 
          strstr((const char *)pMsg->msgStr, "DCC Chat") == NULL)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():is new PRIVMSG\n");
         if (dbg) fflush(stdout);
         if (!doSpecialMessage(pMsg))
         {
            if ((pMsgQry = msgQueryList.find(pMsg)) != NULL)
               pMsgQry->add(pMsg);
            else
            {
               if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Starting New message queryQuery\n");
               if (dbg) fflush(stdout);
               if ((pMsgQry = new xIrcMsgQuery(wdtPrv)) != NULL)
               {
                  msgQueryList.append(pMsgQry);
                  connect(pMsgQry, SIGNAL(done(xIrcMsgQuery *)), 
                          this, SLOT(msgQryHandler(xIrcMsgQuery *)));
                  connect(pMsgQry, SIGNAL(textSelected(xMultiLineTextSelection)),
                          pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
                  pMsgQry->add(pMsg);
                  pMsgQry->show();
               }
            }
         }
      }
      else if (isMsg(pMsg->rspCode, "NOTICE"))
         doSpecialMessage(pMsg);
   }
   
   if (dbg) fprintf(stdout, "xIrcConnect::gotResponse():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcMessageFrame *xIrcConnect::findMsgFrame(const char *pName)
{
   int x;
   xIrcMessageFrame *rv;
   QString nameTmp1, nameTmp2;
   char name[20];
   
   nameTmp1 = pName;
   nameTmp1.upper();
   if (dbg) fprintf(stdout, "xIrcConnect::findMsgFrame():Looking for %s\n", (const char *)nameTmp1);
   if (dbg) fflush(stdout);
   for (x = 0, rv = pIrcMsgFrames; rv != NULL; rv = rv->next())
   {
      nameTmp2 = rv->name();
      nameTmp2.upper();
      strcpy(name, (const char*)nameTmp2);
      if (dbg) fprintf(stdout, "xIrcConnect::findMsgFrame():Testing %s\n", name);
      if (dbg) fflush(stdout);
      if (strcmp((const char *)nameTmp1, name) == 0)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::findMsgFrame():Have match!!\n");
         if (dbg) fflush(stdout);
         break;
      }
   }
   if (dbg) fprintf(stdout, "xIrcConnect::findMsgFrame():Exit(%d)\n", (int)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

bool xIrcConnect::doSpecialMessage(xIrcMessage *pMsg)
{
   xIrcDccQuery *pMsgQry;
   xIrcMessage msg;
   bool rv = FALSE;
   const char *cp;
   char str[1024], *cp1;

   if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Enter\n");
   if (dbg) fflush(stdout);
   switch (pMsg->pmsgTyp)
   {
      case ipmPing:
         if (isMsg(pMsg->rspCode, "NOTICE"))
         {
            long pingTime = time(NULL) - atol(pMsg->msgStr);
            long pingMins = pingTime / 60;
            long pingSecs = pingTime % 60;
            if (pingMins > 0)
               sprintf(str, "*** Ping from %s: %ld:%02ld minutes\n", 
                            (const char *)pMsg->srcNick,
                            pingMins, pingSecs);
            else
               sprintf(str, "*** Ping from %s: %ld seconds\n", 
                             (const char *)pMsg->srcNick, pingSecs);
            pMainWin->pWin->putString(str);
/*
            sprintf(str, "*** Ping from %s: %s\n", (const char *)pMsg->srcNick,
                                                   (const char *)pMsg->msgStr);
*/
         }
         else if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            sprintf(str, "*** Pinged by %s\n", (const char *)pMsg->srcNick);
            pMainWin->pWin->putString(str);
            for (cp = (const char *)pMsg->msgStr, cp1 = str; *cp; cp++)
               if (*cp >= ' ')
               {
                  *(cp1++) = *cp;
                  *cp1 = '\0';
               }
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "PING ";
            msg.msgStr += str;
            msg.msgStr += "\x01";
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Sending |%s| to Socket\n",
                                     (const char *)msg.msgStr);
            if (dbg) fflush(stdout);
            emit msgOut(&msg);
         }
         rv = TRUE;
         break;

      case ipmDCCChat:
         if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Got DCC Chat Request!\n");
         if (dbg) fflush(stdout);
         if ((pMsgQry = new xIrcDccQuery(wdtPrv)) != NULL)
         {
            connect(pMsgQry, SIGNAL(done(xIrcMessage *)), this, SLOT(dccChatHandler(xIrcMessage *)));
            pMsgQry->setupQuery("Chat", pMsg);
         }
         if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Waiting Query Response!\n");
         if (dbg) fflush(stdout);
         rv = TRUE;
         break;

      case ipmDCCSend:
         if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Got DCC Send Request!\n");
         if (dbg) fflush(stdout);
         if ((pMsgQry = new xIrcDccQuery(wdtPrv)) != NULL)
         {
            QString file("Send: ");
            const char *cp;

            for (cp = pMsg->msgStr; *(++cp) != ' ';)
               file += *cp;
            file += " Size: ";
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Skipping IP |%s|\n", cp);
            if (dbg) fflush(stdout);
            for (; *(cp++) == ' ';);
            for (; *(cp++) != ' ';);

            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Skipping Port |%s|\n", cp);
            if (dbg) fflush(stdout);
            for (; *(cp++) == ' ';);
            for (; *(cp++) != ' ';);

            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Retrieving size |%s|\n", cp);
            if (dbg) fflush(stdout);
            for (; isdigit(*cp); cp++)
               file += *cp;
            file += " Bytes\n";
            connect(pMsgQry, SIGNAL(done(xIrcMessage *)), this, SLOT(dccChatHandler(xIrcMessage *)));
            pMsgQry->setupQuery(file, pMsg);
         }
         if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Waiting Query Response!\n");
         if (dbg) fflush(stdout);
         rv = TRUE;
         break;

      case ipmDCC:
         if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Got DCC Request!\n");
         if (dbg) fflush(stdout);
         sprintf(str, "* %s %s", (const char *)pMsg->srcNick, (const char *)pMsg->msgStr);
         pMainWin->pWin->putString(str);
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():It's a PRIVMSG DCC message\n");
            if (dbg) fflush(stdout);
            if ((cp = Defaults.get("DCC_RESPONSE")) != NULL)
            {
               if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Found DCC_RESPONSE Default\n");
               if (dbg) fflush(stdout);
               msg.rspCode = ircResponses.code("PRIVMSG");
               msg.dstStr = pMsg->srcNick;
               msg.msgStr = cp;
               if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Sending |%s| to Socket\n",
                                        (const char *)msg.msgStr);
               if (dbg) fflush(stdout);
               emit msgOut(&msg);
               if (pMsg->pmsgTyp == ipmDCCSend)
               {
                  if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Was a DCC Send request!\n");
                  if (dbg) fflush(stdout);
                  if ((cp = Defaults.get("DCC_SEND_RESPONSE")) != NULL)
                  {
                     msg.rspCode = ircResponses.code("PRIVMSG");
                     msg.dstStr = pMsg->srcNick;
                     msg.msgStr = cp;
                     if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Sending |%s| to Socket\n",
                                              (const char *)msg.msgStr);
                     if (dbg) fflush(stdout);
                     emit msgOut(&msg);
                  }
               }
            }
         }
         rv = TRUE;
         break;

      case ipmVersion:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            char vbuf[512], vstr[512], vstr1[512], *cp, *cp1;
            struct utsname sname;

            uname(&sname);

            strcpy(vstr, "$Name:  $");
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr);
            if (dbg) fflush(stdout);
            for (cp = vstr; *cp && *cp != ':'; cp++);
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr);
            if (dbg) fflush(stdout);
            for (++cp; *cp && isspace(*cp); cp++);
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr);
            if (dbg) fflush(stdout);
            for (cp1 = vstr1; *cp && *cp != '$' && !isspace(*cp); cp++)
               *(cp1++) = *cp;
            *cp = 0;
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr1);
            if (dbg) fflush(stdout);
            for (cp = vstr1; *cp; cp++)
            {
               if (*cp == '-')
               {
                  for (++cp; *cp; cp++)
                  {
                     if (*cp == '-')
                     {
                        *cp = '.';
                        break;
                     }
                  }
                  break;
               }
            }
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr1);
            if (dbg) fflush(stdout);
            if (strlen(vstr1) == 0)
               strcpy(vstr1, "xIrc-2.x");
            if (dbg) fprintf(stdout, "xIrcConnect::doSpecial():Version: have |%s|\n", vstr1);
            if (dbg) fflush(stdout);

            sprintf(vbuf, "VERSION %s %s-%s joe@croftj.net,http://www.croftj.net/~xirc", 
                    vstr1, sname.sysname, sname.release);

            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += vbuf;
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmUserInfo:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "USERINFO ";
            if ((cp = Defaults.get("USERINFO")) != NULL)
               msg.msgStr += cp;
            else
               msg.msgStr += "None Available";
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmSource:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "SOURCE ";
            msg.msgStr += "http://www.croftj.net/~xirc";
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmFinger:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "FINGER ";
            sprintf(str, "%s", (const char *)realName);
            msg.msgStr += str;
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmClientInfo:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "CLIENTINFO ";
            msg.msgStr += "CLIENTINFO PING SOURCE FINGER USERINFO VERSION ";
            msg.msgStr += "TIME DCC-CHAT DCC-SEND";
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmTime:
         if (isMsg(pMsg->rspCode, "PRIVMSG"))
         {
            char *cp, tbuf[512];
            time_t t;

            t = time(NULL);
            cp = ctime(&t);
            for (cp1 = tbuf; *cp; cp++)
            {
               if (*cp != '\n' && *cp != '\r')
                  *(cp1++) = *cp;
                  *cp1 = '\0';
            }
            
            msg.rspCode = ircResponses.code("NOTICE");
            msg.dstStr = pMsg->srcNick;
            msg.msgStr = "\x01";
            msg.msgStr += "TIME ";
            msg.msgStr += tbuf;
            msg.msgStr += "\x01";
            emit msgOut(&msg);
         }
         else
            showResponse(pMsg);
         rv = TRUE;
         break;

      case ipmErrMsg:
         showResponse(pMsg);
         rv = TRUE;
         break;

      default:
         rv = FALSE;
         break;
   }
   return(rv);
}

xIrcMessageFrame *xIrcConnect::makeNewMsgFrame(QWidget *pParent, const char *pName)
{
   xIrcMessageFrame *rv, *pMsg;

   rv = NULL;
/*
   if (pIrcMsgFrames != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():Testing for unused Irc Frame\n");
      if (dbg) fflush(stdout);
      for (rv = pIrcMsgFrames; rv != NULL; rv = rv->next())
      {
         if (strlen(rv->name()) == 0)
         {
            if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():Found one!!!!\n");
            if (dbg) fflush(stdout);
            rv->newName((char *)pName);
//            rv->reset();
            break;
         }
      }
   }
*/   
   if (rv == NULL && (rv = new xIrcMessageFrame(wdtPrv, pParent, pName)) != NULL)
   {
      if (pIrcMsgFrames != NULL)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():New Message Frame at %d\n", (int)rv);
         if (dbg) fflush(stdout);
         for (pMsg = pIrcMsgFrames; pMsg->next() != NULL; pMsg = pMsg->next());
         pMsg->link(rv);
      }
      else
         pIrcMsgFrames = rv;
      if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():Connecting xNickActionQuery Signals/slots\n");
      if (dbg) fflush(stdout);
      connect(rv, SIGNAL(textSelected(xMultiLineTextSelection)),
              pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
      if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():Connecting keybord to Socket!\n");
      if (dbg) fflush(stdout);
      connect(rv, SIGNAL(ircMsgFrameClosing(xIrcMessageFrame*)),
         this, SLOT(ircMsgFrameClosing(xIrcMessageFrame*)));
      connect(rv, SIGNAL(ircMessageOut(xIrcMessage*)),
         this, SLOT(sendMessage(xIrcMessage*)));
      connect(rv, SIGNAL(chatRequest()),
         this, SLOT(newChannel()));
   }
   else if (rv == NULL)
      fprintf(stderr, "Cannot allocate IRC Message Frame!\n");      
      
   if (dbg) fprintf(stdout, "xIrcConnect::makeNewMsgFrame():Exit(%d)\n", (int)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

void xIrcConnect::ircMsgFrameClosing(xIrcMessageFrame *pMF)
{
   if (pIrcMsgFrames == pMF)
      pIrcMsgFrames = pMF->next();
   disconnect(this, NULL, pMF, NULL);
//   disconnect((QObject*)this, NULL, (QObject*)pMF, (char *)NULL);
}

