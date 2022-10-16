/***************************************************************************
**    xPshBtn.cpp  $Revision: 1.3 $ - $Name:  $ 
**    QPushButton to be used by xPshBtnFrames
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1996 Joseph Croft <jcroft@unicomp.net>
**
**    This library is free software; you can redistribute it and/or
**    modify it under the terms of the GNU Library General Public
**    License as published by the Free Software Foundation; either
**    version 2 of the License, or (at your option) any later version.
**
**    This library is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**    Library General Public License for more details.
**
**    You should have received a copy of the GNU Library General Public
**    License along with this library; if not, write to the Free
**    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <stdio.h>
#include "xResources.h"
#include "xPshBtn.h"

static int dbg = 0;

static const char *pInitialResources[] =
{
   "*PushBtn.Font.Weight: bold",
   NULL
};

xPshBtn::xPshBtn(xWidgetResInfo *pPRes, QWidget *pParent,
        const char *pName, const char *pText, int id) :
   QPushButton(pText, pParent, pName)
{
   if (dbg) fprintf(stdout, "xPshBtn::xPshBtn():New Button = %s:%s:%d\n",
                            pName, pText, id);
   wdtRes = new xWidgetResInfo(pPRes, QString("pushbtn"), QString("PushBtn"));
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   ident = id;
   if (pText)
      setText(pText);
   adjustSize();
   connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void xPshBtn::buttonClicked()
{
   if (dbg) fprintf(stdout, "xPshBtn::buttonClicked():CLICKED! = %s:%d\n",
                            (const char *)text(), ident);
   emit clicked(ident);
}

//#include "xPshBtn.moc"

