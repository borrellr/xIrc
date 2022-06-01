/***************************************************************************
**    xIrcNotifyTable.h  $Revision: 1.4 $ - $Name:  $ 
**    Frame showing Who's on, who's not
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
#ifndef _xIRCNOTIFYTABLE_H
#define _xIRCNOTIFYTABLE_H

#include <qmenubar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <xComboBox.h>
#include <xDialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <xPshBtnFrame.h>
#include <xResources.h>
#include "xIrcPeopleTable.h"

class xIrcNotifyTable : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcNotifyTable(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xIrcNotifyTable() {};

signals:
   void selected(const char *);

public slots:
   void update(xIrcPeopleList *pList);

protected slots:
   void haveSelection(int x);

private:
   virtual void resizeEvent(QResizeEvent *pEvt);
   xListBox          *pListBox;
   xWidgetResInfo    *wdtRes;
};

#endif
