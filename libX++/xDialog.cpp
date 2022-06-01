/***************************************************************************
**    xDialog.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Self fitting Dialog and Frame classes
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
#include <qsize.h>
#include <qcursor.h>
#include <qtimer.h>
#include "xApp.h"
#include "xDialog.h"

static int dbg = 0;

extern xApplication *pApp;

#ifdef QT2
xDialog::xDialog(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
                 bool modal, WFlags flags) :
         QDialog(pParent, pName, modal, flags)
#else

xDialog::xDialog(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
                 bool modal) :
         QDialog(pParent, pName, modal)
#endif
{
   const char *ccp;

   if (dbg) fprintf(stdout, "xDialog::xDialog():Enter\n");
   if (dbg) fflush(stdout);
   mainFrame = NULL;
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   if (dbg) fprintf(stdout, "xDialog::xDialog():Setting Palette\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   if (dbg) fprintf(stdout, "xDialog::xDialog():Setting Font\n");
   if (dbg) fflush(stdout);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xDialog::xDialog():Creating Frame\n");
   if (dbg) fflush(stdout);
   needsDeleting = TRUE;
   if ((ccp = Resources->get(wdtRes, "autoFocus", "AutoFocus")) == NULL)
   {
      if (dbg) fprintf(stdout, "xDialog::xDialog():Auto Focus Accepted by Default\n");
      if (dbg) fflush(stdout);
      af = TRUE;
   }
   else
   {
      QString focusStr = ccp;
      focusStr = focusStr.upper();
      if (strcmp(focusStr, "TRUE") == 0 || strcmp(focusStr, "T") == 0)
      {
         if (dbg) fprintf(stdout, "xDialog::xDialog():Auto Focus Accepted\n");
         if (dbg) fflush(stdout);
         af = TRUE;
      }
      else
         af = FALSE;
   }
   mainFrame = new xFrame(wdtRes, this);
   if (mainFrame)
      QDialog::resize(mainFrame->size());
   if (dbg) fprintf(stdout, "xDialog::xDialog():Exit\n");
   if (dbg) fflush(stdout);
}

xDialog::xDialog(xWidgetResInfo *pPRes, xFrame *pFrame, QWidget *pParent,
                   const char *pName) :
         QDialog(pParent, pName, FALSE)
{
   // Appease the Compiler Warnings
   pParent = pParent;
   pName = pName;

   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   if (pFrame)
   {
      needsDeleting = FALSE;
      mainFrame = pFrame;
   }
   else
   {
      needsDeleting = TRUE;
      mainFrame = new xFrame(wdtRes, this);
   }
   if (mainFrame)
      QDialog::resize(mainFrame->size());
}

xDialog::~xDialog()
{
   if (needsDeleting && mainFrame)
      delete mainFrame;
}

int xDialog::addWidget(QWidget *pWidget, QWidget *pNeighbor, int relativePos)
{
   return(mainFrame->addWidget(pWidget, pNeighbor, relativePos));
}
   
void xDialog::removeWidget(QWidget *pWidget)
{
   mainFrame->removeWidget(pWidget);
}

void xDialog::setAlignment(int alignment)
{
   mainFrame->setAlignment(alignment);
}

void xDialog::setWidgetSpacing(int space)
{
   mainFrame->setWidgetSpacing(space);
}

void xDialog::setMargins(int vertMargin, int horzMargin)
{
   mainFrame->setMargins(vertMargin, horzMargin);
}
   
void xDialog::margins(int& vertMargin, int& horzMargin)
{
   int vm, hm;
   mainFrame->margins(vm, hm);
   vertMargin = vm; horzMargin = hm;
}

void xDialog::setResizeMode(int reszMode)
{
   mainFrame->setResizeMode(reszMode);
}

void xDialog::fitFrame(const QSize *frameSize, bool force)
{
   if (mainFrame)
   {
      mainFrame->fitFrame(frameSize, force);
//      QDialog::resize(mainFrame->size());
   } 
}

void xDialog::initFrame(const QSize *frameSize)
{
   if (dbg) fprintf(stdout, "xDialog::initFrame():Enter\n");
   if (dbg) fflush(stdout);
   // hide();
   if (mainFrame)
   {
      if (dbg) fprintf(stdout, "xDialog::initFrame():Fitting mainFrame\n");
      if (dbg) fflush(stdout);
      mainFrame->fitFrame(frameSize, TRUE);
      if (dbg) fprintf(stdout, "xDialog::initFrame():Resizing mainFrame\n");
      if (dbg) fflush(stdout);
      QWidget::resize(mainFrame->size());
      if (dbg) fprintf(stdout, "xDialog::initFrame():Done w/ mainFrame\n");
      if (dbg) fflush(stdout);
   } 
   if (dbg) fprintf(stdout, "xDialog::initFrame():Exit\n");
   if (dbg) fflush(stdout);
}
   
void xDialog::resize(QSize *frameSize)
{
     mainFrame->QWidget::resize(frameSize->width(), frameSize->height());
}

void xDialog::resizeEvent(QResizeEvent *pEvt)
{
   if (resizeRecurse < 1)
   {
      resizeRecurse++;
      fitFrame(&(pEvt->size()));
      resizeRecurse--;
   }
   QDialog::resizeEvent(pEvt);
}

void xDialog::doubleClickEvent(QMouseEvent *pEvt)
{
   if (mainFrame)
      mainFrame->doubleClickEvent(pEvt);
}

void xDialog::setPalette(const QPalette &p)
{
   QDialog::setPalette(p);
   if (mainFrame != NULL)
      mainFrame->setPalette(p);
}

void xDialog::grabFocus()
{
   if (dbg) fprintf(stdout, "xDialog::grabFocus():Enter\n");
   if (dbg) fflush(stdout);
   if (isVisible() && mainFrame->isVisible())
   {
      if (dbg) fprintf(stdout, "xDialog::grabFocus():Setting self active\n");
      if (dbg) fflush(stdout);
      if (dbg) fprintf(stdout, "xDialog::grabFocus():Processing events\n");
      if (dbg) fflush(stdout);
      pApp->processEvents();
      pApp->flushX();
      pApp->setActiveWindow(this);
   }
   else
   {
      if (dbg) fprintf(stdout, "xDialog::grabFocus():Not visible! Setting up to try again!\n");
      if (dbg) fflush(stdout);
      QTimer::singleShot(50, this, SLOT(grabFocus()));
   }
   if (dbg) fprintf(stdout, "xDialog::grabFocus():Exit\n");
   if (dbg) fflush(stdout);
}

void xDialog::show()
{
   if (dbg) fprintf(stdout, "xDialog::show():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xDialog::show():Setting Timer\n");
   if (dbg) fflush(stdout);
   if (af)
      QTimer::singleShot(100, this, SLOT(grabFocus()));
   if (dbg) fprintf(stdout, "xDialog::show():Calling QDialog::show()\n");
   if (dbg) fflush(stdout);
   QDialog::show();
   if (dbg) fprintf(stdout, "xDialog::show():Exit\n");
   if (dbg) fflush(stdout);
}

void xDialog::hide()
{
   if (dbg) fprintf(stdout, "xDialog::show():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xDialog::show():Calling QDialog::show()\n");
   if (dbg) fflush(stdout);
   QDialog::hide();
   if (af)
   {
      pApp->processEvents();
      pApp->flushX();
      pApp->restoreActiveWindow();
   }
   if (dbg) fprintf(stdout, "xDialog::show():Exit\n");
   if (dbg) fflush(stdout);
}

//#include "xDialog.moc"
