/***************************************************************************
**    xSlider.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    QSlider w/ X-Resources
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1997 Joseph Croft <jcroft@unicomp.net>
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
#ifndef _XSLIDER_H
#define _XSLIDER_H

#include <qslider.h>
#include "xResources.h"

class xSlider : public QSlider
{
   Q_OBJECT

public:
   xSlider(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   xSlider(xWidgetResInfo *pPRes, QSlider::Orientation orientation,
          QWidget *pParent = NULL,
          const char *pName = NULL);

private:
   void initSlider(xWidgetResInfo *pPRes);
   xWidgetResInfo *wdtRes;
};

#endif

