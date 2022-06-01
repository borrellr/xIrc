/***************************************************************************
**    xMouseEvent.h  $Revision: 1.2 $ - $Name:  $ 
**    Mouse Event Class
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
#ifndef _XMOUSEEVENT_H
#define _XMOUSEEVENT_H

#include <qlineedit.h>

class xMouseEvent
{
public:
   xMouseEvent(QWidget *pWdt, QMouseEvent *pEvt)
   {
      pWidget = pWdt;
      pEvent = pEvt;
   };

   QWidget *widget()
   {
      return(pWidget);
   };

   QMouseEvent *event()
   {
      return(pEvent);
   };

private:
   QWidget     *pWidget;
   QMouseEvent *pEvent;
};

#endif

