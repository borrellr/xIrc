/***************************************************************************
**    xLabel.cpp  $Revision: 1.3 $ - $Name:  $ 
**    QLabel
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
#include <ctype.h>
#include <stdio.h>
#include "xDefaults.h"
#include "xLabel.h"
#include <qlineedit.h>

extern xDefaults Defaults;

static int dbg = 0;

xLabel::xLabel(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName) :
        QLabel(pParent, pName)
{
   if (dbg) fprintf(stdout, "xLabel::xLabel():Enter\n");
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xLabel::xLabel():Exit\n");
}

xLabel::xLabel(xWidgetResInfo *pPRes, const char *pText, QWidget *pParent,
               const char *pName) :
        QLabel(pText, pParent, pName)
{
   if (dbg) fprintf(stdout, "xLabel::xLabel():Enter\n");
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xLabel::xLabel():Exit\n");
}

void xLabel::setWidth(int width)
{
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xEdit::setWidth(%d):Enter\n", width);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xEdit::setWidth():maxWidth = %d, width = %d\n",
                            fm.maxWidth(), fm.maxWidth() * width);
   if (dbg) fflush(stdout);
   resize(fm.maxWidth() * width + 8,  fm.lineSpacing() + 8);
   if (dbg) fprintf(stdout, "xEdit::setWidth():Exit\n");
   if (dbg) fflush(stdout);
}

void xLabel::mousePressEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   emit mousePressed(&evt);
}

void xLabel::mouseReleaseEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   emit mouseReleased(&evt);
}

void xLabel::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   emit mouseDoubleClicked(&evt);
}

void xLabel::mouseMoveEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   emit mouseMoved(&evt);
}

//#include "xLabel.moc"
