/***************************************************************************
**    xLabeledList.cpp  $Revision: 1.1 $ - $Name:  $
**    xFrame w/ xEdit and QLabel widgets
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
#include <ctype.h>
#include "xLabeledList.h"

static int dbg = 1;

static const char *pInitialResources[] =
{
   "*Label.Font.Weight: Bold",
   NULL
};

xLabeledList::xLabeledList(xWidgetResInfo *pPRes, QWidget *pParent,
                     const char *pName, int alignLabel) :
           xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                               QString(""))),
                  pParent, pName)
{
   if (dbg) fprintf(stdout, "xLabeledList::xLabeledList():Enter\n\r");
   wdtResPrv = new xWidgetResInfo(pPRes);
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   eWidth = 9;
   eHeight = 5;

   setResizeMode(xSPACE_Resize);
   setAlignment(alignLabel);
   setFocusPolicy(QWidget::StrongFocus);
//   setFont(QFont("helvetica", 10, QFont::Bold));

   pList = new xListBox(wdtResPrv, this, pName);
   setSize(eWidth, eHeight);

   if (dbg) fprintf(stdout, "xLabeledList::xLabeledList():List Width = %d, height = %d\n",
                            pList->width(), pList->height());
   pLabel = new xLabel(wdtRes, pName, this);
   setDefFont(pLabel, wdtRes);
//   pLabel->setFont(font());
   addWidget(pLabel);
   addWidget(pList);
   setWidgetSpacing((alignLabel == xALIGN_Vert) ? 0 : 3);
   setMargins(5, 5);
   setFocusProxy(pList);
   xFrame::fitFrame();
   if (dbg) fprintf(stdout, "xLabeledList::xLabeledList():After fitting- List Width = %d, height = %d\n",
                            pList->width(), pList->height());
   if (dbg) fprintf(stdout, "xLabeledList::xLabeledList():Exit\n\r");
}

xLabeledList::~xLabeledList()
{
   if (pList)
      delete pList;
   if (pLabel)
      delete pLabel;
}

void xLabeledList::setLabel(const char *pText)
{
   if (dbg) fprintf(stdout, "xLabeledList::setLabel():Enter\n\r");
   pLabel->setText(pText);
   pLabel->adjustSize();
   if (dbg) fprintf(stdout, "xLabeledList::setLabel():size w=%d X h=%d\n\r",
                            pLabel->size().width(), pLabel->size().height());
   if (dbg) fprintf(stdout, "xLabeledList::setLabel():Exit\n\r");
}

void xLabeledList::fitFrame(QSize *frameSize)
{
   xFrame::fitFrame(frameSize);
}

void xLabeledList::setSize(int width, int height)
{
   eWidth = width;
   eHeight = height;

   QFontMetrics fm = pList->fontMetrics();
   if (dbg) fprintf(stdout, "xLabeledList::setWidth(%d):Enter\n", width);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xLabeledList::setWidth():maxWidth = %d, width = %d\n",
                            fm.maxWidth(), fm.maxWidth() * width);
   if (dbg) fflush(stdout);
   pList->resize(fm.maxWidth() * width + 8,  fm.lineSpacing()  * height + 8);
   xFrame::fitFrame();
   if (dbg) fprintf(stdout, "xLabeledList::setWidth():Exit\n");
   if (dbg) fflush(stdout);
}

int xLabeledList::width()
{
   return(eWidth);
}

/*
void xLabeledList::keyPressEvent(QKeyEvent *pEvt)
{
   if (pEvt->ascii() == '\t')
      emit tabPressed();
   else
      xList::keyPressEvent(pEvt);
}
*/

//#include "xLabeledList.moc"
