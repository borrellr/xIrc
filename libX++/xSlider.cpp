/***************************************************************************
**    xSlider.h  $Revision: 1.2 $ - $Name:  $ 
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
#include "xMisc.h"
#include "xSlider.h"

xSlider::xSlider(xWidgetResInfo *pPRes, QWidget *pParent,
                 const char *pName) :
         QSlider(pParent, pName)
{
   initSlider(pPRes);
}

xSlider::xSlider(xWidgetResInfo *pPRes, QSlider::Orientation orientation,
                 QWidget *pParent,
                 const char *pName) :
         QSlider(orientation, pParent, pName)
{
   initSlider(pPRes);
}

void xSlider::initSlider(xWidgetResInfo *pPRes)
{
   wdtRes = new xWidgetResInfo(pPRes, QString("slider"),
                               QString("Slider"));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
}

//#include "xSlider.moc"
