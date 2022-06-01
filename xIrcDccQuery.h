/***************************************************************************
**    xIrcDccQuery.h  $Revision: 1.2 $ - $Name:  $ 
**    Dialog box to ask about Dcc Requests
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
#ifndef _xIRCDCCQUERY_H
#define _xIRCDCCQUERY_H

#include <qlabel.h>
#include <xPshBtnFrame.h>
#include <xDialog.h>
#include <xResources.h>
#include "xIrcSocket.h"

class xIrcDccQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcDccQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL, bool autoDelete = TRUE);
   ~xIrcDccQuery();
   
   enum QryResults { Rejected, Accepted, Message };

   void setupQuery(const char *pType, xIrcMessage *pMsg);

signals:   
   void done(xIrcMessage *pMsg);
   
protected slots:
   virtual void buttonPressed(int results);
   
private:
   bool              deleteFlag;
   xFrame            *pFrame;
   QLabel            *pHeader;
   xIrcMessage       msg;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo    *wdtRes;
   void initClass(QWidget *pParent, const char *pName, bool autoDelete);
};

#endif
