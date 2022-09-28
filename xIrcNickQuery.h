/***************************************************************************
**    xIrcNickQuery.h  $Revision: 1.3 $ - $Name:  $ 
**    Dialog box to get nick name
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
#ifndef _xIRCNICKQUERY_H
#define _xIRCNICKQUERY_H

#include <qcheckbox.h>
#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include <xEditList.h>
#include <xDialog.h>
#include "xIrcSocket.h"

class xIrcNickQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcNickQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL);
   ~xIrcNickQuery();
   
   const char *text() { return(pNick->text()); };
   void setText(const char *str) { pNick->setText(str); };
   const char *nextNick();
   bool autoSelectNick() { return(pAutoNickBtn->isChecked()); };

   enum QryResults { Rejected, Accepted, Disconnect, Clear };

protected slots:
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   void reject();
   void accept();
   
private slots:
   void gotReturn(const char *);

private:   
   xEditList      *pNick;
   QCheckBox      *pAutoNickBtn;
   xFrame         *pNickBtnFrame;
   xPshBtnFrame   *pButtons;
   xWidgetResInfo    *wdtRes;
};

#endif
