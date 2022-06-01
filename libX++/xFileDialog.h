/***************************************************************************
**    xFileDialog.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog box to display a File Dialog
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1997  Joseph Croft <jcroft@unicomp.net>  
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
#ifndef _XFILEDIALOG_H
#define _XFILEDIALOG_H

#include <qdir.h>
#include "xDialog.h"
#include "xLabel.h"
#include "xLineEdit.h"
#include "xListBox.h"
#include "xPshBtnFrame.h"
#include "xCheckBox.h"
#include "xResources.h"

class xFileDialog : public xDialog
{
   Q_OBJECT
   
public:
   enum { Cancel, Filter, Select };

   xFileDialog(xWidgetResInfo *pPRes,
               const char *dirName,
               const char *file = NULL,
               const char * filter = NULL,
               QWidget *pParent = NULL, const char *pName = NULL,
               bool exist = FALSE, bool modal=FALSE);

   xFileDialog(xWidgetResInfo *pPRes,
               QWidget *pParent = NULL, const char *pName = NULL,
               bool exist = FALSE, bool modal=FALSE);
   ~xFileDialog()
   {
      delete pDirs;
      delete pFiles;
      delete wdtRes;
   };

   static QString getOpenFileName(xWidgetResInfo *pPRes,
                                  const char *dir = NULL,
                                  const char *file = NULL,
                                  const char *filter = NULL,
                                  QWidget *parent = NULL,
                                  const char *name = NULL);

   static QString getSaveFileName(xWidgetResInfo *pPRes,
                                  const char *dir = NULL,
                                  const char *file = NULL,
                                  const char *filter = NULL,
                                  QWidget *parent = NULL,
                                  const char *name = NULL);

   int exec(const char *pFile = NULL);
   QString selectedFile();
   void setHidden(bool f);
   void setExists(bool f);

signals:
   void fileSelected(const char *pFn);

protected slots:
   void moveToDirectory(const char *cp);
   void moveToDirectory(int index);
   void haveSelection();
   void haveSelection(int index);
   void haveHighlight(int index);
   void haveButton(int btn);
   void setFilter();
   void newPath();
   void toggleHidden();

private:
   void           init(const char *dir = NULL,
                       const char *file = NULL,
                       const char *filter = NULL,
                       bool exist = FALSE);

   xFrame         *pFrame;
   xFrame         *pDirFrame;
   xFrame         *pPathFrame;
   xFrame         *pPathNameFrame;
   xFrame         *pChkBoxFrame;
   xLineEdit      *pFileName;
   xLineEdit      *pFilter;
   xListBox       *pDirList;
   xListBox       *pFileList;
   xLineEdit      *pPath;
   xPshBtnFrame   *pButtons;
   xCheckBox      *pExistChkBox;
   xCheckBox      *pHiddenChkBox;
   QDir           *pDirs;
   QDir           *pFiles;
   xWidgetResInfo *wdtRes;
};

#endif
