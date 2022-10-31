/***************************************************************************
**    xIrcIgnoreQuery.cpp  $Revision: 1.6 $ - $Name:  $ 
**    Dialog box to get server name
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
#include <stdlib.h>
#include <qt.h>
#include <qfiledialog.h>
#include "xIrcPeopleEdit.h"
#include "xIrcIgnoreQuery.h"
#include "xDefaults.h"

static bool  dbg = false;
                    
extern xDefaults Defaults;

static const char *pInitialResources[] =
{
   NULL
};

xIrcIgnoreQuery::xIrcIgnoreQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("serverdialog"),
                                                   QString("IgnoreDialog")),
                       pParent, pName, FALSE)
{
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   initializeActions();
   initClass(pName);
}

void xIrcIgnoreQuery::initializeActions()
{
   newListAct = new QAction(tr("New"), 0, this);
   newListAct->setStatusTip(tr("Create a new list"));
   connect(newListAct, SIGNAL(activated()), this, SLOT(newList()));

   loadListAct = new QAction(tr("Load"), 0, this);
   loadListAct->setStatusTip(tr("Load a list of servers"));
   connect(loadListAct, SIGNAL(activated()), this, SLOT(loadList()));

   saveListAct = new QAction(tr("Save As"), 0, this);
   saveListAct->setStatusTip(tr("Save the server list"));
   connect(saveListAct, SIGNAL(activated()), this, SLOT(saveList()));

   doneAct = new QAction(tr("Done"), 0, this);
   doneAct->setStatusTip(tr("Exit the dialog"));
   connect(doneAct, SIGNAL(activated()), this, SLOT(reject()));

   newEntryAct = new QAction(tr("New"), 0, this);
   newEntryAct->setStatusTip(tr("Create new server entry"));
   connect(newEntryAct, SIGNAL(activated()), this, SLOT(newEntry()));

   editEntryAct = new QAction(tr("Edit"), 0, this);
   editEntryAct->setStatusTip(tr("Edit a server entry"));
   connect(editEntryAct, SIGNAL(activated()), this, SLOT(editEntry()));

   deleteEntryAct = new QAction(tr("Delete"), 0, this);
   deleteEntryAct->setStatusTip(tr("Delete a server entry"));
   connect(deleteEntryAct, SIGNAL(activated()), this, SLOT(deleteEntry()));
}

void xIrcIgnoreQuery::initClass(const char *pName)
{
   const char *pFn, *pPath;
   QString serverFile;

   if (dbg) fprintf(stdout, "xinitClass::xinitClass():Enter\n\r");
   if (dbg) fflush(stdout); 

   xWidgetResInfo wdtTmp(wdtRes, QString("menu"), QString("Menu"));
   xWidgetResInfo wdtPopTmp(&wdtTmp, QString("popup"), QString("Popup"));

   setCaption("Ignore List");
   mainFrame->setAutoDelete(TRUE);
   if (pName)
      setCaption(pName);

   setFocusPolicy(StrongFocus);
   pLabel = new QLabel(this);
   pTable = new xIrcPeopleTable(wdtRes, this, "servertable", "Ignore", 10);

   pFileMenu = new QPopupMenu;
   newListAct->addTo(pFileMenu);
   loadListAct->addTo(pFileMenu);
   saveListAct->addTo(pFileMenu);
   doneAct->addTo(pFileMenu);
   setDefFont(pFileMenu, &wdtPopTmp);
   setDefPallet(pFileMenu, &wdtPopTmp);


   pIgnoreMenu = new QPopupMenu();
   newEntryAct->addTo(pIgnoreMenu);
   editEntryAct->addTo(pIgnoreMenu);
   deleteEntryAct->addTo(pIgnoreMenu);
   setDefFont(pIgnoreMenu, &wdtPopTmp);
   setDefPallet(pIgnoreMenu, &wdtPopTmp);

   pMenu = new QMenuBar(this);
   pMenu->insertItem("&File", pFileMenu);
   pMenu->insertItem("&Nick", pIgnoreMenu);
   setDefFont(pMenu, &wdtTmp);
   setDefPallet(pMenu, &wdtTmp);

   pAccel = new QAccel(this, "");
   pAccel->connectItem(pAccel->insertItem(Key_Prior), pTable, SLOT(scrollPageBack()));
   pAccel->connectItem(pAccel->insertItem(Key_Next), pTable, SLOT(scrollPageFwd()));
   pAccel->connectItem(pAccel->insertItem(Key_Up), pTable, SLOT(highlightPrev()));
   pAccel->connectItem(pAccel->insertItem(Key_Down), pTable, SLOT(highlightNext()));
   pAccel->connectItem(pAccel->insertItem(Key_Escape), this, SLOT(reject()));
   pAccel->connectItem(pAccel->insertItem(Key_Home), this, SLOT(gotoFirst()));
   pAccel->connectItem(pAccel->insertItem(Key_End), this, SLOT(gotoLast()));
   
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   serverFile = pPath;
   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".xIrc.ignore";
   serverFile += '/';
   serverFile += pFn;
   pTable->readFile(serverFile.latin1());
   
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Done",     xIrcIgnoreQuery::Accepted);
/*
   pButtons->addButton("Edit",  xIrcIgnoreQuery::Edit);
   pButtons->addButton("Delete",  xIrcIgnoreQuery::Delete);
   pButtons->addButton("Load",  xIrcIgnoreQuery::Load);
   pButtons->addButton("Save",  xIrcIgnoreQuery::Save);
   pButtons->addButton("Import",  xIrcIgnoreQuery::Import);
   pButtons->addButton("Clear",  xIrcIgnoreQuery::Clear);
*/

   addWidget(pLabel);
   addWidget(pTable);
   addWidget(pButtons);
   setResizeMode(xSPACE_Resize);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pTable, SIGNAL(rowDoubleClicked(int)),
           this, SLOT(editEntry(int)));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
   connect(pTable, SIGNAL(returnPressed()),
           this, SLOT(editEntry()));
   if (dbg) fprintf(stdout, "xinitClass::xinitClass():Exit\n\r");
   if (dbg) fflush(stdout); 
}

void xIrcIgnoreQuery::gotButton(int btn)
{
   QString tmpStr;

   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::gotButton(%d):Enter\n\r", btn);
   if (dbg) fflush(stdout); 
   done(btn);
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::gotButton():Exit\n\r");
}

void xIrcIgnoreQuery::accept()
{
   QDialog::accept();
}

void xIrcIgnoreQuery::reject()
{
   QDialog::reject();
}

void xIrcIgnoreQuery::gotReturn()
{

   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::gotReturn():Enter\n\r");
   editEntry();
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::gotReturn():Exit\n\r");
}

void xIrcIgnoreQuery::newList()
{
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::newList():Enter\n");
   if (dbg) fflush(stdout);
   pTable->clear();
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::newList():Calling shoRows()\n");
   if (dbg) fflush(stdout);
   pTable->showRows(0);
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::newList():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcIgnoreQuery::saveList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".xIrc.ignore";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".xIrc*";

   fileName = QFileDialog::getSaveFileName(pPath + "/" + pFn, pFilt, this);
   if (!fileName.isNull())
      pTable->writeFile(fileName.latin1());
}

void xIrcIgnoreQuery::newEntry()
{
   xIrcPeopleEntry e("", "", "", 1);

   xIrcPeopleEdit editDlg(wdtRes, NULL, "Ignore Entry Edit", &e);
   if (editDlg.exec() == QDialog::Accepted)
      pTable->add(e);
   pTable->showRows(pTable->currentRow());
}

void xIrcIgnoreQuery::editEntry(int entry)
{
   if (pTable->entry(entry) != NULL)
   {
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Ignore Entry Edit", pTable->entry(entry));
      editDlg.exec();
      pTable->showRows(pTable->currentRow());
   }
   else
      newEntry();
}

void xIrcIgnoreQuery::editEntry()
{
   if (pTable->entry(pTable->selectedRow()) != NULL)
   {
      xIrcPeopleEdit editDlg(wdtRes, NULL, "Ignore", 
                             pTable->entry(pTable->selectedRow()));
      editDlg.exec();
      pTable->showRows(pTable->currentRow());
   }
}

void xIrcIgnoreQuery::add(xIrcPeopleEntry &e)
{
   pTable->add(e);
   pTable->showRows(pTable->currentRow());
}

void xIrcIgnoreQuery::deleteEntry()
{
   if (pTable->selectedRow() >= 0)
      pTable->remove(pTable->selectedRow());
}

void xIrcIgnoreQuery::loadList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "file", "File")) == NULL)
      pFn = ".xIrc.ignore";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
   {
      if ((pPath = getenv("HOME")) == NULL)
         pPath = "./";
   }
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".xIrc*";
   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);
   pTable->readFile(fileName.latin1());
   pTable->showRows(pTable->currentRow());
}

void xIrcIgnoreQuery::gotoFirst()
{
//   pTable->scroll(0);
   pTable->highlightRow(0);
   pTable->highlightPrev();
}

void xIrcIgnoreQuery::gotoLast()
{
   pTable->highlightRow(pTable->rows() - 1);
//   pTable->scroll(pTable->rows() - 1);
}

xIrcIgnoreQuery::~xIrcIgnoreQuery()
{
   if (pAccel)
      delete pAccel;

/*
   if (pLabel)
      delete pLabel;
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xIrcIgnoreQuery::~xIrcIgnoreQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
*/
   if (dbg) fprintf(stdout, "xIrcIgnoreQuery::~xIrcIgnoreQuery():Done!\n");
   if (dbg) fflush(stdout);
}

//#include "xIrcIgnoreQuery.moc"
