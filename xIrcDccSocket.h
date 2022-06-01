/***************************************************************************
**    xIrcDccSocket.h  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#ifndef _XIRCDCCSOCKET_H
#define _XIRCDCCSOCKET_H

#include <qobject.h>
#include <qstring.h>
#include <xResources.h>
#include <xSocket.h>

class xIrcDccSocket : public xSocketTCP
{
   Q_OBJECT

public:
   xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, const char *pHost,
              const char *pService, QObject *pSocketDialog, 
              const char *pSlotStatus, const char *pSlotProgress, 
              int &err);
   xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, const char *pNetAddr,
              QObject *pSocketDialog, const char *pSlotStatus, 
              const char *pSlotProgress, int &err);
   xIrcDccSocket(xWidgetResInfo *pPRes, QObject *pParent, const char *pService,
              const char *pSlotStatus, int &err);
   ~xIrcDccSocket();

private:
   const char *na2Host(const char *pNetAddr);
   const char *na2Service(const char *pNetAddr);
   int         serr;
};

#endif
