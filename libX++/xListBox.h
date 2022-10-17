/***************************************************************************
**    xListBox.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    QListBox replacement, just because
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
#ifndef _xLISTBOX_H
#define _xLISTBOX_H

#include <qglobal.h>
#include <qkeycode.h>
#include <qlistbox.h>
#include <stdio.h>
#include "xResources.h"

class xComboBox;

class xListBox : public QListBox
{
public:
   xListBox(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
            const char *pName = NULL) :
      QListBox(pParent, pName) 
   {
      wdtRes = new xWidgetResInfo(pPRes, QString("listbox"), QString("ListBox"));
      setDefPallet(this, wdtRes);
      setDefFont(this, wdtRes);
   };
   friend class xComboBox;
   
protected:
   virtual void keyPressEvent(QKeyEvent *pEvt)
   {
/*
      int x;
      
      printf("xListBox::keyPressEvent():Got key 0x%x\n", pEvt->key());
      if (pEvt->key() == Key_Up)
      {
         printf("xListBox::keyPressEvent():Key_Up!!\n");
         if ((x = currentItem()) > 0)
            setCurrentItem(--x);
      }
      else if (pEvt->key() == Key_Down)
      {
      printf("xListBox::keyPressEvent():Key_Down!!\n");
         if ((x = currentItem()) < count())
            setCurrentItem(++x);
      }
*/
      QListBox::keyPressEvent(pEvt);
   };
private:
   xWidgetResInfo *wdtRes;

};

#endif
