/***************************************************************************
**    xApp.h  $Revision: 1.2 $ - $Name:  $ 
**    Application Class
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
#ifndef _XAPP_H
#define _XAPP_H

#include <qapplication.h>
#include <qstack.h>

class xApplication : public QApplication
{
public:
   xApplication(int &argc, char **argv) : 
      QApplication(argc, argv) {};
   ~xApplication() {};

   void setActiveWindow(QWidget *pWdt);
   void restoreActiveWindow();

private:
   QStack<QWidget>   activeStack;
};
   
#endif
