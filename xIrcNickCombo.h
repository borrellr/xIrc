/***************************************************************************
**    xIrcNickBox.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    xEditList for Handling Nicks and addresses
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
#ifndef _xIRCCHANNICKS_H
#define _xIRCCHANNICKS_H

#include <ctype.h>
#include <qstring.h>
#include <xEditList.h>
#include "xIrcNickList.h"

#define NCT_NAME     01
#define NCT_ADDR     02
#define NCT_FULL     03

class xIrcNick;
class xIrcChanNicks;

#define MAX_NICKLEN  30

class xIrcNickCombo : public xEditList, public xIrcNickList
{
   Q_OBJECT
   
public:
   xIrcNickCombo(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL, int type = NCT_NAME);
   ~xIrcNickCombo();

   virtual void merge(const char *pNicks, char delim = ' ');
   virtual void merge(xIrcNickListEntry *pList);
   virtual void remove(const char *pNicks, bool delflag = TRUE, char delim = ' ');
   virtual void remove(xIrcNickListEntry *pEntry, bool delflag = TRUE);
   virtual xIrcNickListEntry *getNext(xIrcNickListEntry *pEntry)
      { 
          return(xIrcNickList::getNext(pEntry)); 
      };
   virtual const char *getNicks(char delim = ' ', bool fullName = FALSE)
      {
         return(xIrcNickList::getNicks(delim, fullName));
      };
   virtual const char *getAddrs(char delim = ' ')
      { 
         return(xIrcNickList::getAddrs(delim));
      };
   void clearList();
   void purgeList();
   const char *names(char delim = ' ', bool fullName = FALSE);
   xIrcNickListEntry *selected();
   void resize(int width, int height);

signals:
   void nickSelected(const char *pName);

protected slots:
   void gotSelected(const char *pName);
   
private:
   QString        nameStr;
   int            nameType;
   xIrcNickList  *pTmpList;
   xWidgetResInfo *wdtRes;
};

#endif
