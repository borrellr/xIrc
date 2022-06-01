/***************************************************************************
**    xInputFrame.cpp  $Revision: 1.2 $ - $Name:  $ 
**    QFrame w/ xMultiLineFrame & xLEdit widgets
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
#include "xInputFrame.h"

// static dbg = 1;

xInputFrame::xInputFrame(xWidgetResInfo *pPRes, QWidget *pParent,
                        const char *pName, WFlags iFlags,
                        int width, int height,
                        int maxLines) :
             QFrame(pParent, pName, iFlags)
{
   QSize sizeTerm, sizeFrame;

   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setFocusPolicy(QWidget::StrongFocus);
   pTitle = NULL;
   pMenuBar = NULL;
   pTerm = new xMultiLineFrame(wdtRes, this, NULL, width, height, maxLines);
   sizeFrame = pTerm->size();
   pEdit = new xLEdit(wdtRes, this);
   
   sizeTerm = pTerm->size();
   pTerm->move(0, 0);
   pEdit->setGeometry(0, sizeTerm.height() + 3, 
                      sizeTerm.width(), (pEdit->size()).height());
   
   sizeFrame.setHeight(sizeFrame.height() + pEdit->size().height() + 3);
   resize(sizeFrame);
}

xInputFrame::~xInputFrame()
{
   if (pTerm)
      delete pTerm;
   if (pEdit)
      delete pEdit;
   if (pTitle)
      delete pTitle;
}

void xInputFrame::resizeEvent(QResizeEvent *pEvt)
{
   QSize sizeFrame, sizeTitle, sizeTerm, sizeNewTerm, sizeMenu;
   int titleHeight = 0;
      
   sizeFrame = pEvt->size();
   
   if (pTitle)
   {
      pTitle->resize(sizeFrame.width(), 30);
      sizeTitle = pTitle->size();
      titleHeight = sizeTitle.height();
      pTitle->setAlignment(AlignCenter);
      pTitle->move(0, 0);
   }
   
   if (pMenuBar)
   {
      pMenuBar->resize(sizeFrame.width(), pMenuBar->size().height());
      pMenuBar->move(0, titleHeight);
      sizeMenu = pMenuBar->size();
      titleHeight += sizeMenu.height();
   }
   sizeTerm = sizeFrame;
   sizeTerm.setHeight(sizeFrame.height() - (titleHeight + pEdit->size().height()));
   pTerm->resize(sizeTerm);
   sizeNewTerm = pTerm->size();

   if (sizeNewTerm != sizeTerm)
   {
      sizeFrame = sizeNewTerm;
      sizeFrame.setHeight(sizeFrame.height() + titleHeight + pEdit->size().height());
      resize(sizeFrame);
   }

   pTerm->move(0, titleHeight);
   pEdit->setGeometry(0, titleHeight + sizeTerm.height(), 
                      sizeTerm.width(), (pEdit->size()).height());

}

void xInputFrame::attachMenu(QMenuBar *pMenu)
{
   pMenuBar = pMenu;
   resize(size().width(), size().height() + pMenu->size().height());
}

void xInputFrame::setTitle(char *pText)
{
   QSize sizeFrame;
   
   if (pText)
   {
      sizeFrame = pTerm->size();
      if (pTitle != NULL)
         delete pTitle;
      pTitle = new QLabel(pText, this);
      pTitle->setFont(font());
      pTitle->setFrameStyle(QFrame::Panel | QFrame::Raised);
      pTitle->resize(sizeFrame.width(), pTitle->size().height());
      pTitle->setAlignment(AlignCenter);
      pTitle->move(0, 0);
      resize(size().width(), size().height() + pTitle->size().height());
   }
   else
      pTitle = NULL;
}

void xLEdit::keyPressEvent(QKeyEvent *pEvt)
{
   char buffer[400];
   
   if (pEvt->ascii() == '\r')
   {
      strcpy(buffer, text());
      emit gotInput(buffer);
      setText("");
   }
   else
      QLineEdit::keyPressEvent(pEvt);
}

//#include "xInputFrame.moc"
