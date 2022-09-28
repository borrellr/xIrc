/***************************************************************************
**    xIrcChanNickBox.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog Box for Disp./Manip. channel members
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net> 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#ifndef _xIRCCHANNICKBOX
#define _xIRCCHANNICKBOX

#include <qfont.h>
#include <xDialog.h>
#include <xPshBtnFrame.h>
#include "xIrcNickCombo.h"

#define xCN_Whois   1
#define xCN_Query   2
#define xCN_Invite  3
#define xCN_Kick    4
#define xCN_Clear   5
#define xCN_Ping    6
#define xCN_Close   7

class xIrcChanNickBox : public xDialog
{
   Q_OBJECT

public:   
   xIrcChanNickBox(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                   const char *pName = NULL);
   ~xIrcChanNickBox();
   bool is(const char *pName);
   void setNicks(const char *pNames);
   void rmvNicks(const char *pNames, char delim = ' ');
   void merge(const char *pNames, char delim = ' ');
   void remove(const char *pNames, char delim = ' ', int delflag = TRUE);
   xIrcNickListEntry *selected();
   const char *text() { return(pNicks->text()); };
   void clear();
   const char *getNames(char delim = ' ');
   const char *getAddrs(char delim = ' ');
   void setHeight(int rows);
   
signals:
   void buttonPressed(int action);
   void nickSelected(const char *pNick);

protected slots:
   void gotSelected(const char *pName);
   void buttonClicked(int iBtn);
   
private:
   xIrcNickCombo *pNicks;
   xPshBtnFrame  *pBtns;
   xFrame        *pFrame;
   xWidgetResInfo *wdtRes;
};

#endif
