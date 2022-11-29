/***************************************************************************
**    xIrcNickActionQuery.h  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box to ask about Private messages
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
#ifndef _xIRCNICKACTIONQUERY_H
#define _xIRCNICKACTIONQUERY_H

#include <xMultiLineFrame.h>
#include <xPshBtnTable.h>
#include <xDialog.h>
#include <xResources.h>
#include <qlineedit.h>

class xIrcNickActionQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcNickActionQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                       const char *pName = NULL);
   ~xIrcNickActionQuery();
   const char *nick() { return(pNickEdit->text()); };
   const char *chan() { return(pChanEdit->text()); };
   void setNick(const char *pStr) { pNickEdit->setText(pStr); };
   void setChan(const char *pStr) { pChanEdit->setText(pStr); };

   enum { DccChat, WhoIs, Ping, GiveOps, TakeOps, PrivChat, WhoWas,
          Kick, UserInfo, Time, Finger, Version, Who, Ban, Close,
          Ignore, Notify, FileSend };

signals:   
   void done(xMultiLineTextSelection);
   
protected slots:
   virtual void buttonPressed(int results);
   virtual void wakeUp(xMultiLineTextSelection);
   virtual void wakeUp(const char *pNick);
   
private slots:

private:
   QLineEdit      *pNickEdit;
   QLineEdit      *pChanEdit;
   xPshBtnTable   *pButtons;
   xWidgetResInfo *wdtRes;
   xMultiLineTextSelection txtSel;
};

#endif

