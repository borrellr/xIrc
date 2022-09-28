/***************************************************************************
**    xIrcServerEdit.h  $Revision: 1.4 $ - $Name:  $ 
**    Server List Entry Edit Dialog
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
#ifndef _XIRCSERVEREDIT_H
#define _XIRCSERVEREDIT_H

#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <qgrid.h>
#include "xIrcServerEntry.h"

class xIrcServerEdit : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcServerEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL, xIrcServerEntry *entry = NULL);
   ~xIrcServerEdit();
   void initClass(const char *pName);

   enum QryResults { Rejected, Accepted };

protected slots:
   void gotReturn();
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   
private:
   xIrcServerEntry   *pEntry;
//   xFrame            *pFrame;
   QGrid             *pFrame;
   QLineEdit         *pServer;
   QLineEdit         *pCountry;
   QLineEdit         *pState;
   QLineEdit         *pCity;
   QLineEdit         *pGroup;
   QLineEdit         *pPort;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
};

#endif
