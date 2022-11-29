/***************************************************************************
**    xIrcDccSocket.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Class for Irc DCC Sockets
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
#include <ctype.h>
#include <stdlib.h>
#include <qmessagebox.h>
#include "xIrcDccSocket.h"

static int dbg = 0;
static QString strHost;
static QString strService;

xIrcDccSocket::xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, 
                       const char *pNetAddr, QObject *pSocketDialog, 
                       const char *pSlotStatus, const char *pSlotProgress, 
                       int &err) :
            xSocketTCP(pPRes, pParent, na2Host(pNetAddr), na2Service(pNetAddr),
                       pSocketDialog, pSlotStatus, pSlotProgress, serr)
{
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket1():Enter\n");
   if (dbg) fflush(stdout);
   err = serr;
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcDccSocket::xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, 
                       const char *pHost, const char *pService, 
                       QObject *pSocketDialog, const char *pSlotStatus, 
                       const char *pSlotProgress, int &err) :
            xSocketTCP(pPRes, pParent, pHost, pService, pSocketDialog,
                       pSlotStatus, pSlotProgress, serr)
{
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket2():Enter\n");
   if (dbg) fflush(stdout);
   err = serr;
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcDccSocket::xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, 
                       const char *pService, const char *pSlotStatus, int &err) :
            xSocketTCP(pPRes, pParent, pService, pSlotStatus, serr)
{
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket3():Enter\n");
   if (dbg) fflush(stdout);
   err = serr;
   if (dbg) fprintf(stdout, "xIrcDccSocket::xIrcDccSocket():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcDccSocket::~xIrcDccSocket()
{
//   disconnect(this);
}

const char *xIrcDccSocket::na2Host(const char *pNetAddr)
{
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Host():Enter\n");
   if (dbg) fflush(stdout);
   QString strService1 = "";
   strHost = "";
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Host():pNetAddr = |%s|\n", pNetAddr);
   if (dbg) fflush(stdout);
   for (cp = (const char *)pNetAddr; isspace(*cp); cp++);
   for (; *cp && !isspace(*cp); cp++)
      strHost += *cp;
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Host():strHost = |%s|\n", 
                             (const char *)strHost);
   if (dbg) fflush(stdout);
   while (isspace(*cp)) cp++;
   for (; *cp && !isspace(*cp); cp++)
      strService1 += *cp;
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Host():strServ = |%s|\n", 
                             (const char *)strService);
   cp = strHost;
   return(cp);
}

const char *xIrcDccSocket::na2Service(const char *pNetAddr)
{
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Service():Enter\n");
   if (dbg) fflush(stdout);
   strService = "";
   QString strHost1 = "";
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Service():pNetAddr = |%s|\n", pNetAddr);
   if (dbg) fflush(stdout);
   for (cp = (const char *)pNetAddr; isspace(*cp); cp++);
   for (; *cp && !isspace(*cp); cp++)
      strHost1 += *cp;
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Service():strHost = |%s|\n", 
                             (const char *)strHost);
   if (dbg) fflush(stdout);
   while (isspace(*cp)) cp++;
   for (; *cp && !isspace(*cp); cp++)
      strService += *cp;
   if (dbg) fprintf(stdout, "xIrcDccSocket::na2Service():strServ = |%s|\n", 
                             cp);
   cp = strService;
   return(cp);
}

//#include "xIrcDccSocket.moc"
