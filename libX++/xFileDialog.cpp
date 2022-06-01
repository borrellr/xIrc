/***************************************************************************
**    xFileDialog.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Dialog box to display File Dialog
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
#include <stdio.h>
#include "xApp.h"
#include "xFileDialog.h"
#include "xMessageBox.h"

static int dbg = 0;

xFileDialog::xFileDialog(xWidgetResInfo *pPRes,
                         const char *dirName, 
                         const char *file,
                         const char *filter,
                         QWidget *pParent, const char *pName,
                         bool exist, bool modal) :
             xDialog((wdtRes = new xWidgetResInfo(pPRes,
                                                  QString("filedialog"),
                                                  QString("FileDialog"))),
                     pParent, pName, modal)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   init(dirName, file, filter, exist);
}

xFileDialog::xFileDialog(xWidgetResInfo *pPRes,
                         QWidget *pParent, const char *pName,
                         bool exist, bool modal) :
             xDialog((wdtRes = new xWidgetResInfo(pPRes,
                                                  QString("filedialog"),
                                                  QString("FileDialog"))),
                     pParent, pName, modal)
{
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   init(NULL, NULL, NULL, exist);
}

void xFileDialog::init(const char *dir, 
                       const char *file,
                       const char *filter,
                       bool exist)
{
   if (dbg) fprintf(stdout, "xFileDialog::init():Enter\n");
   if (dbg) fprintf(stdout, "xFileDialog::init():dir = |%s|\n", dir);
   if (dbg) fprintf(stdout, "xFileDialog::init():file = |%s|\n", file);
   if (dbg) fprintf(stdout, "xFileDialog::init():filter = |%s|\n", filter);
   if (dbg) fflush(stdout);
   pFrame = new xFrame(wdtRes, xALIGN_Vert, xSPACE_ToFit, 0, 
                       this, NULL);
   pFileName = new xLineEdit(wdtRes, pFrame, NULL);
   pFileName->setMargins(3, 0);
   pFileName->setLabel("File:");
   if (file != NULL)
      pFileName->setText(file);
   pDirFrame = new xFrame(wdtRes, xALIGN_Horz, xSPACE_ToFit, 0, 
                          pFrame, NULL);
   pDirFrame->setMargins(0, 0);
   {
      pDirList = new xListBox(wdtRes, pDirFrame, NULL);
      QFontMetrics fm = pDirList->QWidget::fontMetrics();
      pDirList->resize(fm.maxWidth() * 15,
                                (fm.lineSpacing() * 10) - 2);
   }
   {
      pFileList = new xListBox(wdtRes, pDirFrame, NULL);
      QFontMetrics fm = pFileList->QWidget::fontMetrics();
      pFileList->resize(fm.maxWidth() * 15,
                                 (fm.lineSpacing() * 10) - 2);
   }
   pDirFrame->addWidget(pDirList);
   pDirFrame->addWidget(pFileList);

   pDirs = new QDir(dir, "*", QDir::Name, QDir::Dirs);
   pFiles = new QDir(dir, filter, QDir::Name, QDir::Files);
#ifdef QT2
   pDirList->insertStringList(pDirs->entryList());
   pFileList->insertStringList(pFiles->entryList());
#else
   pDirList->insertStrList(pDirs->entryList());
   pFileList->insertStrList(pFiles->entryList());
#endif

   pDirFrame->fitFrame();

   pPath = new xLineEdit(wdtRes, pFrame);
   pPath->setMargins(3, 0);
   pPath->setLabel("Path:");
   pPath->setText(pDirs->absPath());

   pFilter = new xLineEdit(wdtRes, pFrame, NULL);
   pFilter->setMargins(3, 0);
   pFilter->setLabel("Filter:");
   if (filter == NULL || strlen(filter) == 0)
      pFilter->setText("*");
   else
      pFilter->setText(filter);

   pChkBoxFrame = new xFrame(wdtRes, xALIGN_Vert, xSPACE_ToFit, 0, 
                       this, NULL);
   pChkBoxFrame->setMargins(5, 10);
   pChkBoxFrame->setWidgetSpacing(5);
   pChkBoxFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);

   pHiddenChkBox = new xCheckBox(wdtRes, pChkBoxFrame, NULL, "Show Hidden Files");
   pExistChkBox = new xCheckBox(wdtRes, pChkBoxFrame, NULL, "File Must Exist");
   pExistChkBox->setChecked(exist);


   pChkBoxFrame->addWidget(pHiddenChkBox);
   pChkBoxFrame->addWidget(pExistChkBox);
   pChkBoxFrame->fitFrame();

   pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame->setMargins(0, 10);
   pFrame->addWidget(pFileName);
   pFrame->addWidget(pDirFrame);
   pFrame->addWidget(pPath);
   pFrame->addWidget(pFilter);
   pFrame->fitFrame();

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setMargins(10, 10);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Select", Select);
   pButtons->addButton("Filter", Filter);
   pButtons->addButton("Cancel", Cancel);

   setWidgetSpacing(0);
   setMargins(0, 0);
   addWidget(pFrame);
   addWidget(pChkBoxFrame);
   addWidget(pButtons);

   connect(pButtons, SIGNAL(clicked(int)),
           this, SLOT(haveButton(int)));
   connect(pHiddenChkBox, SIGNAL(clicked()),
           this, SLOT(toggleHidden()));
   connect(pFileName, SIGNAL(returnPressed()),
           this, SLOT(haveSelection()));
   connect(pFilter, SIGNAL(returnPressed()),
           this, SLOT(setFilter()));
   connect(pPath, SIGNAL(returnPressed()),
           this, SLOT(newPath()));
   connect(pDirList, SIGNAL(selected(int)),
           this, SLOT(moveToDirectory(int)));
   connect(pFileList, SIGNAL(selected(int)),
           this, SLOT(haveSelection(int)));
   connect(pFileList, SIGNAL(highlighted(int)),
           this, SLOT(haveHighlight(int)));

   initFrame();
   pFileList->setCurrentItem(0);
   pFileList->setFocus();
   if (dbg) fprintf(stdout, "xFileDialog::init():Exit\n");
   if (dbg) fflush(stdout);
}

int xFileDialog::exec(const char *pFile)
{
   if (pFile != NULL)
      pFileName->setText(pFile);
   return(xDialog::exec());
}

void xFileDialog::moveToDirectory(const char *cp)
{
   if (pDirs->exists(cp))
   {
      if (pDirs->cd(cp))
      {
         pFiles->cd(cp);
         pDirList->clear();
#ifdef QT2
         pDirList->insertStringList(pDirs->entryList());
#else
         pDirList->insertStrList(pDirs->entryList());
#endif
         pFileList->clear();
#ifdef QT2
         pFileList->insertStringList(pFiles->entryList());
#else
         pFileList->insertStrList(pFiles->entryList());
#endif
         pPath->setText(pDirs->absPath());
         pFileList->setCurrentItem(0);
         pFileList->setFocus();
      }
      else
      {
         QString err;

         err = "Error: ";
         err += cp;
         err += ": Directory is not Readable!";
         xMessageBox::message("Error", err);
      }
   }
   else
   {
      QString err;

      err = "Error: ";
      err += cp;
      err += ": Directory does not exist!";
      xMessageBox::message("Error", err);
   }
}

void xFileDialog::moveToDirectory(int index)
{
   const char *cp;

   cp = pDirList->text(index);
   moveToDirectory(cp);
}

void xFileDialog::haveSelection()
{
   const char *cp;
   QString err, f;
   QDir d(pDirs->absPath());

   if (dbg) fprintf(stdout, "xFileDialog::haveSelection():Enter\n");
   if (dbg) fflush(stdout);
   cp = pFileName->text();
   if (dbg) fprintf(stdout, "xFileDialog::haveSelection():pFilename->text() = |%s|\n", cp);
   if (dbg) fflush(stdout);
   if (strlen(cp) == 0)
   {
      int x;
      if ((x = pFileList->currentItem()) < 0)
      {
         xMessageBox::message("Error", "No File Selected");
         return;
      }
      else
         cp = pFileList->text(x);
   }
   if (dbg) fprintf(stdout, "xFileDialog::haveSelection():pFilename->text() = |%s|\n", cp);
   if (dbg) fflush(stdout);
         
   QFileInfo di(cp);
   if (!di.isDir())
   {
      if (*cp != '/')
      {
         f = pDirs->absPath();
         f += "/";
      }
      f += cp;
   }

   if (dbg) fprintf(stdout, "xFileDialog::haveSelection():f = |%s|\n", 
                           (const char *)f);
   if (dbg) fflush(stdout);
         
   QFileInfo fi(f);
   if (fi.isDir())
   {
      if (d.exists() && d.isReadable())
      {
         moveToDirectory(cp);
         pFileName->setText("");
      }
   }
   else
   {
      if (pExistChkBox->isChecked() && !fi.exists())
      {
         QString err("Error: ");
         err += cp;
         err += ": File Does Not exists";
         xMessageBox::message("Error", err);
         return;
      }
//      pFileName->setText(f);
      emit fileSelected(pFileName->text());
      accept();
   }
   if (dbg) fprintf(stdout, "xFileDialog::haveSelection():Exit\n");
   if (dbg) fflush(stdout);
}

void xFileDialog::haveSelection(int index)
{
   const char *cp;
   QString s;

   if (dbg) fprintf(stdout, "xFileDialog::haveSelection(%d):Enter\n", index);
   if (dbg) fflush(stdout);
   cp = pFileList->text(index);
   QFileInfo di(cp);
   if (!di.isDir())
   {
      s += cp;
      pFileName->setText(s);
      haveSelection();
   }
   else
      moveToDirectory(cp);
   if (dbg) fprintf(stdout, "xFileDialog::haveSelection(%d):Exit\n", index);
   if (dbg) fflush(stdout);
}

void xFileDialog::haveHighlight(int index)
{
    const char *cp = pFileList->text(index);
    pFileName->setText(cp);
}

void xFileDialog::haveButton(int btn)
{
   switch (btn)
   {
      case Filter:
         setFilter();
         break;

      case Select:
         {
            int x = pFileList->currentItem();
            if (strlen(pFileName->text()) == 0 && x >= 0)
               haveSelection(x);
            else
               haveSelection();
         }
         break;

      case Cancel:
         reject();
         break;
   }
}
         
void xFileDialog::newPath()
{
   const char *cp;
   QString s;

   cp = pPath->text();
   QFileInfo di(cp);
   if (!di.isDir())
   {
      QString err("Error: No Such Directory: ");
      err += cp;
      xMessageBox::message("Error", err);
   }
   else
      moveToDirectory(cp);
}

void xFileDialog::setFilter()
{
   const char *cp = pFilter->text();

   if (cp == NULL || strlen(cp) == 0)
   {
      cp = "*";
      pFilter->setText("*");
   }
   pFiles->setNameFilter(cp);
   pFileList->clear();
#ifdef QT2
   pFileList->insertStringList(pFiles->entryList());
#else
   pFileList->insertStrList(pFiles->entryList());
#endif
}

QString xFileDialog::getOpenFileName(xWidgetResInfo *pPRes,
                                     const char *dir, 
                                     const char *file,
                                     const char *filter,
                                     QWidget *parent,
                                     const char *name)
{
   QString rv("");

   xFileDialog *p = new xFileDialog(pPRes, dir, file, filter, parent, name, 
                                    TRUE, TRUE);
   p->setCaption("Open");
   if (file != NULL && *file == '.')
      p->setHidden(TRUE);
   if (p->exec() == QDialog::Accepted)
      rv = p->selectedFile();
   delete p;
   return(rv);
}

QString xFileDialog::getSaveFileName(xWidgetResInfo *pPRes,
                                     const char *dir,
                                     const char *file,
                                     const char *filter,
                                     QWidget *parent,
                                     const char *name)
{
   QString rv("");

   xFileDialog *p = new xFileDialog(pPRes, dir, file, filter, parent, name,
                                    FALSE, TRUE);
   p->setCaption("Save As");
   if (*file == '.')
      p->setHidden(TRUE);
//   p->setExists(FALSE);
   if (p->exec() == QDialog::Accepted)
      rv = p->selectedFile();
   delete p;
   return(rv);
}

/*
int xFileDialog::exec()
{
   return(pFileDlg->exec());
}
*/

void xFileDialog::toggleHidden()
{
   if (pHiddenChkBox->isChecked())
   {
      pFiles->setFilter(QDir::Files | QDir::Hidden);
      pDirs->setFilter(QDir::Dirs | QDir::Hidden);
   }
   else
   {
      pFiles->setFilter(QDir::Files);
      pDirs->setFilter(QDir::Dirs);
   }
   pFileList->clear();
#ifdef QT2
   pFileList->insertStringList(pFiles->entryList());
#else
   pFileList->insertStrList(pFiles->entryList());
#endif
   pDirList->clear();
#ifdef QT2
   pDirList->insertStringList(pDirs->entryList());
#else
   pDirList->insertStrList(pDirs->entryList());
#endif
}

QString xFileDialog::selectedFile()
{
   const char *cp = pFileName->text();
   QString rv;
   if (*cp != '/')
   {
      rv  += pDirs->absPath();
      rv += '/';
   }
   rv += pFileName->text();
   return(rv);
}

void xFileDialog::setHidden(bool f)
{
   if (pHiddenChkBox)
   {
      pHiddenChkBox->setChecked(f);
      toggleHidden();
   }
}

void xFileDialog::setExists(bool f)
{
   if (pExistChkBox)
      pExistChkBox->setChecked(f);
}

//#include "xFileDialog.moc"
