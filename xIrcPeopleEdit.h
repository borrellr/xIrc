/***************************************************************************
**    xIrcPeopleEdit.h  $Revision: 1.3 $ - $Name:  $ 
**    People List Entry Edit Dialog
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
#ifndef _XIRCPEOPLEEDIT_H
#define _XIRCPEOPLEEDIT_H

#include <qaccel.h>
#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include <xCheckBox.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <xResources.h>
#include "xIrcPeopleEntry.h"

class xIrcPeopleEdit : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcPeopleEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL, xIrcPeopleEntry *entry = NULL);
   ~xIrcPeopleEdit();
   void initClass(const char *pName);

   enum QryResults { Rejected, Accepted };

protected slots:
   void gotTab();
   void gotReturn();
   void gotButton(int btn);
   virtual void done(int results) { QDialog::done(results); };
   
private:
   xIrcPeopleEntry   *pEntry;
   xFrame            *pFrame;
   QLineEdit         *pNick;
   QLineEdit         *pMask;
   QLineEdit         *pMessage;
   xCheckBox         *pFlag;
   xCheckBox         *pByAddr;
   xPshBtnFrame      *pButtons;
   QAccel            *pAccel;
   xWidgetResInfo    *wdtRes;
};

#endif
