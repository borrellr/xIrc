/***************************************************************************
**    xMultiLineFrame.cpp  $Revision: 1.2 $ - $Name:  $ 
**    QFrame for manipulating Multiple lines of text in a window
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
#include "xMultiLineFrame.h"

static int dbg = 0;

xMultiLineFrame::xMultiLineFrame(xWidgetResInfo *pPRes, QWidget *pParent,
                        const char *pName,
                        int width, int height,
                        int maxLines) :
             QFrame(pParent, pName)
{
   QRect rectWin;

   wdtRes = new xWidgetResInfo(pPRes, QString("multiline.frame"), QString("MultiLine.Frame"));
   wdtPrv = new xWidgetResInfo(pPRes);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setFocusPolicy(QWidget::NoFocus);

   setLineWidth(3);
   setFrameStyle(QFrame::Panel | QFrame::Sunken);
   pScrollBar = new xVertScrollBar(wdtRes, this);
   pScrollBar->setTracking(TRUE);

   pWin = new xMultiLineWin(wdtPrv, this, NULL, width, height, maxLines);
   connect(pWin, SIGNAL(textSelected(QString)),
           this, SLOT(haveTextSelection(QString)));

   QFontMetrics fmWin = pWin->fontMetrics();
   widthSB = (int)((float)fmWin.maxWidth() * 2.0);
   rectWin = pWin->geometry();
   pWin->move(widthSB, 0);
   pScrollBar->setGeometry(0, 0, widthSB, (pWin->geometry()).height());
   
   setSizeIncrement(fmWin.maxWidth(), fmWin.lineSpacing());
   setMinimumSize(fmWin.maxWidth() * 10, fmWin.lineSpacing() * 3);
   rectWin.setRight(rectWin.right() + widthSB);
   setGeometry(rectWin);
   
   connect(pWin, SIGNAL(changePageSize(int)), this, SLOT(changePageStep(int)));
   connect(pWin, SIGNAL(linesUsedChanged(int)), this, SLOT(changeSBRange(int)));
   connect(pScrollBar, SIGNAL(prevPage()), this, SLOT(gotoPrevPage()));
   connect(pScrollBar, SIGNAL(nextPage()), this, SLOT(gotoNextPage()));
   connect(pScrollBar, SIGNAL(prevLine()), this, SLOT(gotoPrevLine()));
   connect(pScrollBar, SIGNAL(nextLine()), this, SLOT(gotoNextLine()));
   connect(pScrollBar, SIGNAL(sliderMoved(int)), this, SLOT(gotoOffset(int)));
   changePageStep(height);
}


xMultiLineFrame::~xMultiLineFrame()
{
   if (pWin)
      delete pWin;
   if (pScrollBar)
      delete pScrollBar;
}

/*
void  xMultiLineFrame::reset()
{
   pWin->reset();
}
*/

void xMultiLineFrame::resizeEvent(QResizeEvent *pEvt)
{
   QSize s, sizeTW, sizeNewTW, sizeSB;
   
   sizeSB = sizeTW = pEvt->size();
   sizeTW.setWidth(sizeTW.width() - widthSB);
   pWin->resize(sizeTW);
   sizeNewTW = pWin->size();
   if (sizeTW != sizeNewTW)
   {
      s = sizeNewTW;
      s.setWidth(sizeNewTW.width() + widthSB);
      resize(s);
   }
   sizeSB.setWidth(widthSB);
   sizeSB.setHeight(pWin->size().height());
   pScrollBar->resize(sizeSB);
}

void xMultiLineFrame::changeSBRange(int linesUsed)
{
   if (dbg) fprintf(stdout, "xMultiLineFrame::changeLinesUsed(%d)\n", linesUsed);
   if (dbg) fprintf(stdout, "xMultiLineFrame::changeLinesUsed():linesUsed = %d, pageStep = %d\n", linesUsed, (int)pScrollBar->pageStep());
   if (linesUsed > pScrollBar->pageStep())
   {
      if (dbg) fprintf(stdout, "xMultiLineFrame::changeLinesUsed():Showing Scroll Bar:linesUsed = %d, pageStep = %d\n", linesUsed, (int)pScrollBar->pageStep());
      pScrollBar->show();
      pScrollBar->setRange(0, linesUsed - pScrollBar->pageStep());
      pScrollBar->setValue(pWin->getPosition() - pScrollBar->pageStep() + 1);
   }
   else
   {
      if (dbg) fprintf(stdout, "xMultiLineFrame::changeLinesUsed):Hiding Scroll Bar:linesUsed = %d, pageStep = %d\n", linesUsed, (int)pScrollBar->pageStep());
      pScrollBar->setRange(0, 0);
      pScrollBar->show();
   }
}

void xMultiLineFrame::changePageStep(int pageLength)
{
   if (dbg) fprintf(stdout, "xMultiLineFrame::changePageStep(%d):Enter\n", pageLength);
   if (dbg) fprintf(stdout, "xMultiLineFrame::changePageStep():linesUsed = %d\n", pWin->getLinesUsed());
   pScrollBar->setSteps(1, pageLength);
   if (pScrollBar->pageStep() > pWin->getLinesUsed())
   {
      pScrollBar->setRange(0, 0);
      pScrollBar->show();
   }
   else
   {
      pScrollBar->setRange(0, pWin->getLinesUsed() - pScrollBar->pageStep());
      pScrollBar->setValue(pWin->getPosition() - pScrollBar->pageStep() + 1);
      pScrollBar->show();
   }
}

void xMultiLineFrame::gotoOffset(int offset)
{
   if (dbg) fprintf(stdout, "xMultiLineFrame::gotoOffset():offset = %d\n", offset);
   if (dbg) fflush(stdout);
   pWin->scrollToLine(offset);
   if (dbg) fprintf(stdout, "xMultiLineFrame::gotoOffset():Exit\n");
   if (dbg) fflush(stdout);
}

void xMultiLineFrame::gotoPrevPage()
{
   pWin->scrollWindow(-(pScrollBar->pageStep()));
}

void xMultiLineFrame::gotoNextPage()
{
   pWin->scrollWindow(pScrollBar->pageStep());
}

void xMultiLineFrame::gotoPrevLine()
{
   pWin->scrollWindow(-1);
}

void xMultiLineFrame::gotoNextLine()
{
   pWin->scrollWindow(1);
}

void xMultiLineFrame::haveTextSelection(QString str)
{
   xMultiLineTextSelection msg;
   const char *cp;

   if (dbg) fprintf(stdout, "MultiLineFrame::haveTextSelection():Enter\n");
   if (dbg) fflush(stdout);
   if (name() != NULL && strlen(name()) > 0)
      msg.winName = name();
   else if (parent()->name() != NULL && strlen(parent()->name()) > 0)
      msg.winName = parent()->name();
   else
      msg.winName = "";
   if (dbg) fprintf(stdout, "MultiLineFrame::haveTextSelection():winName = |%s|\n",
                             (const char*)msg.winName);
   if (dbg) fflush(stdout);
   
   cp = str;
   if (*cp == '#')
   {
      msg.winName = str;
      msg.text = "";
   }
   else
      msg.text = str;
   if (dbg) fprintf(stdout, "MultiLineFrame::haveTextSelection():text = |%s|\n",
                             (const char*)msg.text);
   if (dbg) fflush(stdout);
   emit textSelected(msg);
   if (dbg) fprintf(stdout, "MultiLineFrame::haveTextSelection():Enter\n");
   if (dbg) fflush(stdout);
}

xVertScrollBar::xVertScrollBar(xWidgetResInfo *pPRes, QWidget *pParent,
                               const char *pName) :
                QScrollBar(QScrollBar::Vertical, pParent, pName)
{
   wdtRes = new xWidgetResInfo(pPRes, QString("scrollbar"), QString("ScrollBar"));
   setRange(0, 0);
   setSteps(1, 1);
}

void   xMultiLineFrame::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "MultiLineFrame::doubleClickEvent():Enter\n");
   if (dbg) fflush(stdout);
   pWin->mouseDoubleClickEvent(pEvt);
   if (dbg) fprintf(stdout, "MultiLineFrame::doubleClickEvent():Exit\n");
   if (dbg) fflush(stdout);
}

//#include "xMultiLineFrame.moc"


