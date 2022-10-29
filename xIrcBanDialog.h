/***************************************************************************
**    xIrcBanDialog.h  $Revision: 1.3 $ - $Name:  $ 
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
#ifndef _xIRCBANDIALOG_H
#define _xIRCBANDIALOG_H

#include <qcheckbox.h>
#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include <xComboBox.h>
#include <xDialog.h>

class xIrcBanDialog : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcBanDialog(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL);
   ~xIrcBanDialog();
   
   const char *nick() { return(pNick->text().latin1()); };
   const char *chan() { return(pChan->text().latin1()); };
   const char *mask() { return(pMask->text().latin1()); };
   const char *text() { return(pText->text().latin1()); };
   void setNick(const char *pStr) { pNick->setText(pStr); };
   void setChan(const char *pStr) { pChan->setText(pStr); };
   void setMask(const char *pStr) { pMask->setText(pStr); };
   void setText(const char *pStr) { pText->setText(pStr); };

   enum QryResults { Rejected, Accepted, Kick, UnBan, Default, Clear };

protected slots:
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   void reject();
   void accept();
   
private slots:
   void gotReturn();

private:   
   QLineEdit      *pChan;
   QLineEdit      *pNick;
   QLineEdit      *pMask;
   QLineEdit      *pText;
   xFrame         *pEditFrame;
   xPshBtnFrame   *pButtons;
   xWidgetResInfo *wdtRes;
};

#endif
