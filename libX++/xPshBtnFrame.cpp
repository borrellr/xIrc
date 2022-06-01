/***************************************************************************
**    xPshBtnFrame.cpp  $Revision: 1.2 $ - $Name:  $ 
**    xFrame for handling multiple buttons
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
#include <qfont.h>
#include "xPshBtnFrame.h"

static int dbg = 0;

xPshBtnFrame::xPshBtnFrame(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName,
                           bool autoDelete) :
              xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                                  QString(""))),
                     pParent, pName)
{
   if (dbg) fprintf(stdout, "xPshBtnFrame::xPshBtnFrame():Enter- autodelete = %d\n",
                            autoDelete);
   if (dbg) fprintf(stdout, "xPshBtnFrame::xPshBtnFrame():Name = %s\n", pName);
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   btnList.setAutoDelete(autoDelete);
//   setFont(QFont("helvetica", 12, QFont::Bold));
   if (dbg) fprintf(stdout, "xPshBtnFrame::xPshBtnFrame():Exit\n");
};

xPshBtnFrame::~xPshBtnFrame()
{
   if (dbg) fprintf(stdout, "xPshBtnFrame::~xPshBtnFrame():Enter- autoDelete = %d\n",
                           btnList.autoDelete());
   if (dbg) fflush(stdout);
   for (xPshBtn *pBtn = btnList.first(); pBtn; pBtn = btnList.first())
   {
      if (dbg) fprintf(stdout, "xPshBtnFrame::~xPshBtnFrame():Removing Button\n");
      if (dbg) fflush(stdout);
      btnList.remove(pBtn);
   }
   if (dbg) fprintf(stdout, "xPshBtnFrame::~xPshBtnFrame():Exit\n");
   if (dbg) fflush(stdout);
}

void xPshBtnFrame::addButton(xPshBtn *pButton, int id)
{
   addButton(pButton);
   pButton->setId(id);
}

void xPshBtnFrame::addButton(xPshBtn *pButton)
{
   btnList.append(pButton);
//   pButton->setFont(font());
   addWidget(pButton);
   fitFrame();
   connect(pButton, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
}

xPshBtn *xPshBtnFrame::addButton(const char *pBtnName, int id)
{
   xPshBtn *pBtn;
   
   if ((pBtn = new xPshBtn(wdtRes, this, pBtnName, pBtnName, id)) != NULL)
   {
      if (dbg) fprintf(stdout, "xPshBtnFrame::addButton():Adding Button %s:%d\n",
                                pBtnName, id);
      addButton(pBtn);
   }
   return(pBtn);
}

void xPshBtnFrame::rmvButton(int id)
{
   xPshBtn *pBtn;

   for (pBtn = btnList.first(); pBtn; pBtn = btnList.next())
   {
      if (id == pBtn->id())
      {
         disconnect(pBtn, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
         btnList.removeRef(pBtn);
      }
   }
}

void xPshBtnFrame::buttonClicked(int id)
{
   emit clicked(id);
}

//#include "xPshBtnFrame.moc"


