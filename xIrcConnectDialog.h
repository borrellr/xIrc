/***************************************************************************
**    xIrcConnectDialog.h  $Revision: 1.5 $ - $Name:  $ 
**    Dialog Box to Complete Connection Parameter Entry
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
#ifndef _XIRCCONNECTDIALOG_H
#define _XIRCCONNECTDIALOG_H

#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <qgrid.h>
#include <xResources.h>
#include "xIrcServerEntry.h"

class xIrcConnectDialog : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcConnectDialog(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL, xIrcServerEntry *entry = NULL);
   ~xIrcConnectDialog();
   void initClass(const char *pName);

   QString port();

   enum QryResults { Rejected, Accepted };

signals:
   void completed(int status);
      
protected slots:
   void accept();
   void reject();
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   
private:
   QGrid          *pFrame;
   xFrame         *pPortFrame;
   QLabel         *pPortLabel;
   QLineEdit      *pServer;
   QLineEdit      *pCountry;
   QLineEdit      *pState;
   QLineEdit      *pCity;
   QLineEdit      *pGroup;
   xComboBox      *pPort;
   xPshBtnFrame   *pButtons;
   xWidgetResInfo *wdtRes;
};

#endif
