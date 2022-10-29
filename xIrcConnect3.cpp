/***************************************************************************
**    xIrcConnect3.cpp  $Revision: 1.5 $ - $Name:  $ 
**    Class for making the server connections Part 3
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
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <qt.h>
#include <qkeycode.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "xIrcCommands.h"
#include "xIrcLineEditQuery.h"
#include "xIrcMsgDispatch.h"
#include "xIrcPeopleEdit.h"
#include "xIrcPeopleEntry.h"
#include "xDefaults.h"
#include "xIrcConnect.h"

static bool  dbg = false;

extern xIrcLineEditQuery *KickQuery;
extern xIrcCommands ircResponses;
extern xIrcMsgDispatch Dispatcher;

void xIrcConnect::nickActionHandler(xMultiLineTextSelection txtSel)
{
   int err = 0;
   char buf[256];
   const char *cp;
   xIrcMessage msg;
   xIrcMessageFrame *pMsg;
   xIrcDccChatFrame *pDccChatFrame;
   struct sockaddr_in addr = pSocket->socketName();

   switch (txtSel.iData)
   {
      case xIrcNickActionQuery::DccChat:
         sprintf(buf, "%d", addr.sin_addr.s_addr);
         if ((pDccChatFrame = new xIrcDccChatFrame(wdtPrv, NULL, txtSel.text.latin1())) != NULL)
         {
            connect(pDccChatFrame, SIGNAL(initiateDCCChat(xIrcDccChatFrame*)),
                    this, SLOT(initiateDCCChat(xIrcDccChatFrame*)));
            if ((err = pDccChatFrame->acceptdcc()) == 0)
            {
               connect(pDccChatFrame, SIGNAL(textSelected(xMultiLineTextSelection)),
                       pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
               pDccChatFrame->show();
            }
            else
            {
               sprintf(buf, "Error Accepting Connection from %s:%s",
                            (const char *)txtSel.text.latin1(), strerror(err));
               QMessageBox::warning(this, "Error", buf);
               disconnect(pDccChatFrame, SIGNAL(initiateDCCChat(xIrcDccChatFrame*)),
                          this, SLOT(initiateDCCChat(xIrcDccChatFrame*)));
               delete pDccChatFrame;
            }
         }
         return;
                  
      case xIrcNickActionQuery::PrivChat:
         if (findMsgFrame(txtSel.text.latin1()) == NULL)
         {
            if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Creating Message Frame\n");
            if (dbg) fflush(stdout);
            if ((pMsg = makeNewMsgFrame(NULL, txtSel.text.latin1())) != NULL)
            {
               if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Showing Message Frame\n");
               if (dbg) fflush(stdout);
               pMsg->show();
               if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Getting channel name\n");
               if (dbg) fflush(stdout);
               cp = txtSel.text.latin1();
               if (*cp == '#')
               {
                  if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Joining channel\n");
                  if (dbg) fflush(stdout);
                  sprintf(buf, "JOIN %s\n", (const char *)txtSel.text.latin1());
                  sendMsgToSocket(buf);
               }
            }     
         }
         else
            QMessageBox::warning(this, "Error", "Channel already opened");
         pNickAction->hide();
         return;

      case xIrcNickActionQuery::Kick:
         cp = txtSel.winName.latin1();
         if (*cp != '#')
         {
            QMessageBox::warning(this, "Error", "Channel not specified!!");
            return;
         }
         else if (KickQuery->exec() != QDialog::Rejected)
         {
            msg.rspCode = ircResponses.code("KICK");
            msg.dstStr = txtSel.winName;
            msg.dstStr += ' ';
            msg.dstStr += txtSel.text;
            msg.msgStr = KickQuery->text();
         }
         else
            return;
         break;

      case xIrcNickActionQuery::WhoIs:
         msg.rspCode = ircResponses.code("WHOIS");
         msg.dstStr = txtSel.text;
         msg.msgStr = "";
         break;

      case xIrcNickActionQuery::Who:
         msg.rspCode = ircResponses.code("WHO");
         msg.dstStr = txtSel.text;
         msg.msgStr = "";
         break;

      case xIrcNickActionQuery::Ping:
         sprintf(buf, "%ld", time(NULL));
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = txtSel.text;
         msg.msgStr = "\x01";
         msg.msgStr += "PING ";
         msg.msgStr += buf;
         msg.msgStr += "\x01";
         break;

      case xIrcNickActionQuery::Ban:
         cp = txtSel.winName.latin1();
         if (*cp != '#')
         {
            QMessageBox::warning(this, "Error", "Channel not specified!!");
            return;
         }
         else
         {
            banFlag = TRUE;
            banChan = txtSel.winName;
            banNick = txtSel.text;
            msg.rspCode = ircResponses.code("WHO");
            msg.dstStr = txtSel.text;
            msg.msgStr = "";
            Dispatcher.dispatchMsg(this, SLOT(doBanResp(xIrcMessage*)), &msg);
            return;
         }
         break;

      case xIrcNickActionQuery::GiveOps:
         cp = txtSel.winName.latin1();
         if (*cp != '#')
         {
            QMessageBox::warning(this, "Error", "Channel not specified!!");
            return;
         }
         else
         {
            msg.rspCode = ircResponses.code("MODE");
            msg.dstStr = txtSel.winName;
            msg.dstStr += " +o ";
            msg.dstStr += txtSel.text;
            msg.msgStr = "";
         }
         break;

      case xIrcNickActionQuery::TakeOps:
         cp = txtSel.winName.latin1();
         if (*cp != '#')
         {
            QMessageBox::warning(this, "Error", "Channel not specified!!");
            return;
         }
         else
         {
            msg.rspCode = ircResponses.code("MODE");
            msg.dstStr = txtSel.winName;
            msg.dstStr += " -o ";
            msg.dstStr += txtSel.text;
            msg.msgStr = "";
         }
         break;

      case xIrcNickActionQuery::WhoWas:
         msg.rspCode = ircResponses.code("WHOWAS");
         msg.dstStr = txtSel.text;
         msg.msgStr = "";
         break;

      case xIrcNickActionQuery::UserInfo:
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = txtSel.text;
         msg.msgStr = "\x01";
         msg.msgStr += "USERINFO";
         msg.msgStr += "\x01";
         break;

      case xIrcNickActionQuery::Time:
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = txtSel.text;
         msg.msgStr = "\x01";
         msg.msgStr += "TIME";
         msg.msgStr += "\x01";
         break;

      case xIrcNickActionQuery::Finger:
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = txtSel.text;
         msg.msgStr = "\x01";
         msg.msgStr += "FINGER";
         msg.msgStr += "\x01";
         break;

      case xIrcNickActionQuery::Version:
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = txtSel.text;
         msg.msgStr = "\x01";
         msg.msgStr += "VERSION";
         msg.msgStr += "\x01";
         break;

      case xIrcNickActionQuery::Close:
         pNickAction->hide();
         return;
         break;

      case xIrcNickActionQuery::Ignore:
         ignoreFlag = TRUE;
         ignoreNick = txtSel.text;
         msg.rspCode = ircResponses.code("WHO");
         msg.dstStr = txtSel.text;
         msg.msgStr = "";
         Dispatcher.dispatchMsg(this, SLOT(doIgnoreResp(xIrcMessage*)), &msg);
         return;
         break;

      case xIrcNickActionQuery::Notify:
         notifyQueryFlag = TRUE;
         notifyNick = txtSel.text;
         msg.rspCode = ircResponses.code("WHO");
         msg.dstStr = txtSel.text;
         msg.msgStr = "";
         Dispatcher.dispatchMsg(this, SLOT(doNotifyResp(xIrcMessage*)), &msg);
         return;
         break;

      case xIrcNickActionQuery::FileSend:

         if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler(():Have File send request\n");
         if (dbg) fflush(stdout);
         if (pOpenDialog->exec() == QDialog::Accepted)
         {
            QString file = pOpenDialog->selectedFile();
            int fd;

            if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler(():Opening file |%s|\n",
                                      (const char *)file.latin1());
            if (dbg) fflush(stdout);
            if ((fd = open((const char*)file.latin1(), O_RDONLY)) >= 0)
            {
               struct stat fs;
               unsigned long fSize;

               if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler(():File fd = %d\n", fd);
               if (dbg) fflush(stdout);
               fstat(fd, &fs);
               fSize = fs.st_size;
               if (fSize == 0)
               {
                  if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler(():File is 0 Length\n");
                  if (dbg) fflush(stdout);
                  QMessageBox::warning(this, "Error", "File is 0 length");
                  if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler():Closing file %d...\n", fd);
                  if (dbg) fflush(stdout);
//                  close(fd);
               }
               else
               {
                  xIrcDccFile *pDccFileFrame;
                  if ((pDccFileFrame = new xIrcDccFile(wdtPrv, file.latin1(), NULL, txtSel.text.latin1())) != NULL)
                  {
                     connect(pDccFileFrame, SIGNAL(initiateDCCFile(xIrcDccFile*)),
                             this, SLOT(initiateDCCFile(xIrcDccFile*)));
                     if ((err = pDccFileFrame->accept(fd)) == 0)
                        pDccFileFrame->show();
                     else
                     {
                        char buf[256];
                        sprintf("Error making connection:%s", strerror(err));
                        QMessageBox::warning(this, "Error", buf);
                        delete pDccFileFrame;
                     }
                  }
               }
            }
            else
            {
               char buf[256];
               sprintf(buf, "Error opening file %s:%s", (const char*)file.latin1(), strerror(errno));
               QMessageBox::warning(this, "Error", buf);
            }
         }
         if (dbg) fprintf(stdout, "xIrcConnect::nickActionHandler():Returning unfulfilled\n");
         if (dbg) fflush(stdout);
         return;
         break;
   }
   Dispatcher.dispatchMsg(NULL, NULL, &msg);
}

QString xIrcConnect::buildBanMask(xIrcMessage *pMsg)
{
   QString rv("*!");
   const char *cp;

   if (pMsg->rspCode == 352)
   {
      cp = pMsg->dstStr.latin1();
      if (*cp != '#')
      {
         rv += pMsg->dstStr;
         for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
      }
      else
      {
         for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
         for (; *cp != ' '; cp++)
            rv += *cp;
      }
      rv += "@";
      for (; *cp == ' '; cp++);
      if (isdigit(*cp))
      {
         for (; *cp != ' '; cp++)
            rv += *cp;
      }
      else
      {
         rv += "*";
         for (; *cp != '.'; cp++);
         for (; *cp != ' '; cp++)
            rv += *cp;
      }
   }
   else
      rv = "";
   return(rv);
}

void xIrcConnect::doIgnoreResp(xIrcMessage *pMsg)
{
   if (dbg) fprintf(stdout, "xIrcConnect::doIgnoreResp():Have Ignore Who Response!\n");
   if (dbg) fflush(stdout);
   if (pMsg->rspCode == 352)
   {
      ignoreFlag = FALSE;
      xIrcPeopleEntry e(pMsg);
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Ignore Entry Edit", &e);
      if (editDlg.exec() == Accepted)
         pIgnore->add(e);
   }
}         

void xIrcConnect::doNotifyResp(xIrcMessage *pMsg)
{
   if (dbg) fprintf(stdout, "xIrcConnect::doNotifyResp():Have Notify Who Response!\n");
   if (dbg) fflush(stdout);
   if (pMsg->rspCode == 352)
   {
      notifyQueryFlag = FALSE;
      xIrcPeopleEntry e(pMsg);
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Notify Entry Edit", &e);
      if (editDlg.exec() == Accepted)
         pNotify->add(e);
   }
}

void xIrcConnect::doBanResp(xIrcMessage *pMsg)
{
   xIrcMessage msg;
   char str[120];
   int x;

   if (pMsg->rspCode == 352)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::doBanResp():Have Ban Who Response!\n");
      if (dbg) fflush(stdout);
      banFlag = FALSE;
      pBanBox->setNick(banNick.latin1());
      pBanBox->setChan(banChan.latin1());
      pBanBox->setMask(buildBanMask(pMsg).latin1());
      if ((x = pBanBox->exec()) != xIrcBanDialog::Rejected)
      {
         msg.rspCode = ircResponses.code("MODE");
         msg.dstStr = pBanBox->chan();
         msg.dstStr += (x == xIrcBanDialog::UnBan) ? " -b " : " +b ";
         msg.dstStr += pBanBox->mask();
         msg.msgStr = "";
         sprintf(str, "[B]*** %scanning from %s: %s\n",
                      (x == xIrcBanDialog::UnBan) ? "Unb" : "B",
                      (const char *)pBanBox->chan(),
                      (const char *)pBanBox->nick());
         pMainWin->pWin->putString(str);
         emit msgOut(&msg);
         banFlag = FALSE;
         if (x == xIrcBanDialog::Kick)
         {
            msg.rspCode = ircResponses.code("KICK");
            msg.dstStr = pBanBox->chan();
            msg.dstStr += " ";
            msg.dstStr += pBanBox->nick();
            msg.msgStr = pBanBox->text();
            emit msgOut(&msg);
         }
      }
   }   
}
