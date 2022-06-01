/***************************************************************************
**    xProgressBar.h  $Revision: 1.2 $ - $Name:  $
**    QProgressBar
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
#ifndef _XPROGRESSBAR_H
#define _XPROGRESSBAR_H

#include <qprogressbar.h>
#include "xResources.h"

class xProgressBar : public QProgressBar
{
   Q_OBJECT

public:
   xProgressBar(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   xProgressBar(xWidgetResInfo *pPRes, int totalSteps, QWidget *pParent = NULL,
          const char *pName = NULL);

private:
   xWidgetResInfo *wdtRes;
};

#endif
