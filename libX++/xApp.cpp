/***************************************************************************
**    xApp.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#include <stdio.h>
#include "xApp.h"

static int dbg = 0;

void xApplication::setActiveWindow(QWidget *pWdt)
{
   if (dbg) fprintf(stdout, "xApplication::setActiveWindow():Setting widget %d (%s) active\n",
                    (int)pWdt, pWdt->name());
   if (dbg) fflush(stdout);
   activeStack.push(pWdt);
   pWdt->setActiveWindow();
   if (dbg) fprintf(stdout, "xApplication::setActiveWindow():Exit\n");
   if (dbg) fflush(stdout);
}

void xApplication::restoreActiveWindow()
{
   activeStack.pop();
   QWidget *pWdt = activeStack.top();
   if (pWdt != NULL)
   if (dbg) fprintf(stdout, "xApplication::restoreActiveWindow():Restoring widget %d (%s) active\n",
                    (int)pWdt, pWdt->name());
   if (dbg) fflush(stdout);
   processEvents();
   flushX();
   pWdt->setActiveWindow();
   if (dbg) fprintf(stdout, "xApplication::restoreActiveWindow():Exit\n");
   if (dbg) fflush(stdout);
}
