/***************************************************************************
**    xIrcSocket.h  $Revision: 1.2 $ - $Name:  $ 
**    xSocket Class to parse IRC Messages
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
#ifndef _xIRCSOCKET_H
#define _xIRCSOCKET_H

#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <xMultiLineWin.h>
#include <xResources.h>
#include <xSocket.h>

typedef enum
{
   ipmUnknown,
   ipmMessage,
   ipmAction,
   ipmPing,
   ipmDCC,
   ipmDCCSend,
   ipmDCCChat,
   ipmVersion,
   ipmUserInfo,
   ipmClientInfo,
   ipmSource,
   ipmFinger,
   ipmErrMsg,
   ipmTime
} xIrcPrvMsgType;

typedef struct
{
   int               fd;
   xIrcPrvMsgType    pmsgTyp;
   int               rspCode;
   QString           rspStr;
   QString           srcNick;
   QString           srcAddr;
   QString           dstStr;
   QString           msgStr;
   QString           rawMsg;
} xIrcMessage;

typedef QList<xIrcMessage>           xIrcMessageListBase;
typedef QListIterator<xIrcMessage>   xIrcMessageListIterator;

class xIrcMessageList : public xIrcMessageListBase
{
public:
   xIrcMessageList() { setAutoDelete(TRUE); };
   bool is(xIrcMessage *pMsg);
   void append(xIrcMessage *pMsg)
      {
         xIrcMessage *pMsg1 = new xIrcMessage;
         pMsg1->fd = pMsg->fd;
         pMsg1->pmsgTyp = pMsg->pmsgTyp;
         pMsg1->rspCode = pMsg->rspCode;
         pMsg1->rspStr = pMsg->rspStr;
         pMsg1->srcNick = pMsg->srcNick;
         pMsg1->srcAddr = pMsg->srcAddr;
         pMsg1->dstStr = pMsg->dstStr;
         pMsg1->msgStr = pMsg->msgStr;
         pMsg1->rawMsg = pMsg->rawMsg;
         xIrcMessageListBase::append(pMsg1);
      };
};

class xIrcSocket : public xSocketTCP
{
   Q_OBJECT
   
public:
   xIrcSocket(xWidgetResInfo *pPRes, QObject *pParent, const char *pHost,
              const char *pService, QObject *pSocketDialog, 
              const char *pSlotStatus, const char *pSlotProgress, 
              int &err);
   ~xIrcSocket();
   void parseMsgSpecial(xIrcMessage *pMsg, const char *pStr,
                        const char **pRtn);

signals:
   void ircServerMessage(xIrcMessage *pMsg);
   void sendToServer(const char *pText);
   
   // Used internaly, best not mess with
   void socketDataOut(QString msgStr);

public slots:
   void sendIrcServerMessage(xIrcMessage *pMsg);
      
protected slots:
   virtual void socketError(int err);

private slots:
   void rawSocketDataIn(char *pText);
   void socketDataIn(QString msgStr);
   
private:
//   QString translateMessage(const char *cp, xIrcMessage *pMsg);

   int      serr;
   QString  socketBuffer;
};

#define IMT_MaxLength         512

QString translateMessage(const char *cp, xIrcSocket *pSocket = NULL, xIrcMessage *pMsg = NULL);
QString parseLine(const char *cp, bool mircColors, bool ctcp2);
QString parseAttr(const char **p, bool mircColors, bool ctcp2);
void setMircColors(xMultiLineWin *p, bool f);

#endif
