/***************************************************************************
**    xResources.h  $Revision: 1.2 $ - $Name:  $ 
**    X Resource handling class
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
#ifndef _XRESOURCES_H
#define _XRESOURCES_H

#include <qobject.h>
#include <qstring.h>
#include "xMisc.h"
#include <X11/Xlib.h>
#include <X11/Xresource.h>

class xResources
{
public:
   xResources(const char *pDefaults[], XrmOptionDescList *pOptDescList, int tblCount,
             int *argc, char **argv, const char *optFile = NULL);
   ~xResources() {};

   const char *get(xWidgetResInfo *pRInfo, const char *pResName,
                      const char *pClassName);
   void putByName(xWidgetResInfo *pRInfo, const char *pResName,
                  const char *value);
   void putByClass(xWidgetResInfo *pRInfo, const char *pClassName,
                   const char *value);
   void setWidgetInit(const char *pDefaults[]);
private:
   XrmDatabase rDB;
};

extern xResources *Resources;

#endif
