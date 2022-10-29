/***************************************************************************
**    xIrcServerQuery.cpp  $Revision: 1.6 $ - $Name:  $ 
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
#include "xIrcServerEdit.h"
#include "xIrcServerQuery.h"

static bool dbg = false;
                    
static const char *pInitialResources[] =
{
   NULL
};

xServerQuery::xServerQuery(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("serverdialog"),
                                                   QString("ServerDialog")),
                       pParent, pName, TRUE)
{
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   initClass(pName);
}

void xServerQuery::initClass(const char *pName)
{
   const char *pFn, *pPath;
   QString serverFile;

   if (dbg) fprintf(stdout, "xinitClass::xinitClass():Enter\n\r");
   if (dbg) fflush(stdout); 

   xWidgetResInfo wdtTmp(wdtRes, QString("menu"), QString("Menu"));
   xWidgetResInfo wdtPopTmp(&wdtTmp, QString("popup"), QString("Popup"));

//   mainFrame->setAutoDelete(TRUE);
   if (pName)
      setCaption(pName);

   setFocusPolicy(StrongFocus);
   pLabel = new QLabel(this);
   pTable = new xIrcServerTable(wdtRes, this, "servertable", 10);

   pFileMenu = new QPopupMenu;
   pFileMenu->insertItem("New", this, SLOT(newList()));
   pFileMenu->insertItem("Load", this, SLOT(loadList()));
   pFileMenu->insertItem("Import", this, SLOT(importList()));
   pFileMenu->insertItem("Save As", this, SLOT(saveList()));
   pFileMenu->insertItem("Done", this, SLOT(reject()));
   setDefFont(pFileMenu, &wdtPopTmp);
   setDefPallet(pFileMenu, &wdtPopTmp);


   pServerMenu = new QPopupMenu();
   pServerMenu->insertItem("Connect", this, SLOT(connectServer()));
   pServerMenu->insertItem("Disconnect", this, SLOT(disconnectServer()));
   pServerMenu->insertItem("New", this, SLOT(newEntry()));
   pServerMenu->insertItem("Edit", this, SLOT(editEntry()));
   pServerMenu->insertItem("Delete", this, SLOT(deleteEntry()));
   setDefFont(pServerMenu, &wdtPopTmp);
   setDefPallet(pServerMenu, &wdtPopTmp);

   pMenu = new QMenuBar(this);
   pMenu->insertItem("&File", pFileMenu);
   pMenu->insertItem("&Server", pServerMenu);
   setDefFont(pMenu, &wdtTmp);
   setDefPallet(pMenu, &wdtTmp);

   pAccel = new QAccel(this, "");
   pAccel->connectItem(pAccel->insertItem(Key_Prior), pTable, SLOT(scrollPageBack()));
   pAccel->connectItem(pAccel->insertItem(Key_Next), pTable, SLOT(scrollPageFwd()));
   pAccel->connectItem(pAccel->insertItem(Key_Up), pTable, SLOT(highlightPrev()));
   pAccel->connectItem(pAccel->insertItem(Key_Down), pTable, SLOT(highlightNext()));
   pAccel->connectItem(pAccel->insertItem(Key_Home), this, SLOT(gotoFirst()));
   pAccel->connectItem(pAccel->insertItem(Key_End), this, SLOT(gotoLast()));
   
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
      pPath = "/usr/local/lib/xIrc";
   serverFile = pPath;
   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".servers";
   serverFile += '/';
   serverFile += pFn;
   printf ("The server file to load is |%s|\n", serverFile.latin1());
   pTable->readFile(serverFile.latin1());
   
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->addButton("Connect",     xServerQuery::Accepted);
   pButtons->addButton("Disconnect",  xServerQuery::Disconnect);
/*
   pButtons->addButton("Edit",  xServerQuery::Edit);
   pButtons->addButton("Delete",  xServerQuery::Delete);
   pButtons->addButton("Load",  xServerQuery::Load);
   pButtons->addButton("Save",  xServerQuery::Save);
   pButtons->addButton("Import",  xServerQuery::Import);
   pButtons->addButton("Clear",  xServerQuery::Clear);
*/
   pButtons->addButton("Cancel", xServerQuery::Rejected);

   addWidget(pLabel);
   addWidget(pTable);
   addWidget(pButtons);
   setResizeMode(xSPACE_Resize);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   
   connect(pTable, SIGNAL(rowDoubleClicked(int)),
           this, SLOT(serverSelected(int)));
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
   connect(pTable, SIGNAL(returnPressed()),
           this, SLOT(gotReturn()));

   pConnect = NULL;
   if (dbg) fprintf(stdout, "xinitClass::xinitClass():Exit\n\r");
   if (dbg) fflush(stdout); 
}

void xServerQuery::gotButton(int btn)
{
   QString tmpStr;

   if (dbg) fprintf(stdout, "xServerQuery::gotButton(%d):Enter\n\r", btn);
   if (dbg) fflush(stdout); 
   switch((QryResults)btn)
   {
      case Disconnect:
         if (dbg) fprintf(stdout, "xServerQuery::gotButton():Disconnecting??\n\r");
         if (dbg) fflush(stdout); 
         done(btn);
         break;

      case Accepted:                                                         
         if (dbg) fprintf(stdout, "xServerQuery::gotButton():Accepting??\n\r");
         if (dbg) fflush(stdout);
         if (pTable != NULL)
            serverSelected(pTable->selectedRow());
         else
            serverSelected(-1);
         break;                                                              

      case Rejected:                                                         
         if (dbg) fprintf(stdout, "xServerQuery::gotButton():Rejecting??\n\r");
         if (dbg) fflush(stdout); 
         reject();
         break;
   }
   if (dbg) fprintf(stdout, "xServerQuery::gotButton():Exit\n\r");
}

void xServerQuery::accept()
{
   QDialog::accept();
}

void xServerQuery::reject()
{
   QDialog::reject();
}

void xServerQuery::gotReturn()
{

   if (dbg) fprintf(stdout, "xServerQuery::gotReturn():Enter\n\r");
   serverSelected(pTable->selectedRow());
   if (dbg) fprintf(stdout, "xServerQuery::gotReturn():Exit\n\r");
}

void xServerQuery::newEntry()
{
   xIrcServerEntry e("", "", "", "", "", "");

   xIrcServerEdit editDlg(wdtRes, NULL, "Server Entry Edit", &e);
   editDlg.exec();
   pTable->add(e);
   pTable->showRows(pTable->currentRow());
}

void xServerQuery::serverSelected(int row)
{
   int x;

   if (dbg) fprintf(stdout, "xServerQuery::serverSelected():Enter\n");
   if (dbg) fflush(stdout);
   if (pTable != NULL && pTable->entry(row) != NULL)
      pConnect = new xIrcConnectDialog(wdtRes, NULL, "Server Connect", pTable->entry(row));
   else
      pConnect = new xIrcConnectDialog(wdtRes, NULL, "Server Connect", NULL);
   connect(pConnect, SIGNAL(completed(int)),
           this, SLOT(doConnect(int)));
   x = pConnect->exec();
   if (dbg) fprintf(stdout, "xServerQuery::serverSelected():Connect Dialog Complete\n");
   if (dbg) fflush(stdout);
   delete pConnect;
   pConnect = NULL;
   if (x == QDialog::Accepted)
      done(x);
   if (dbg) fprintf(stdout, "xServerQuery::serverSelected():Exit\n");
   if (dbg) fflush(stdout);
}

void xServerQuery::doConnect(int status)
{
   if (dbg) fprintf(stdout, "xServerQuery::doConnect(%d):Enter\n", status);
   if (dbg) fflush(stdout);
   if (status)
   {
      if (dbg) fprintf(stdout, "xServerQuery::doConnect():Have good status\n");
      if (dbg) fflush(stdout);
      Port = pConnect->port();
      if (dbg) fprintf(stdout, "xServerQuery::doConnect():Port = |%s|\n",
                       (const char *)Port.latin1());
      if (dbg) fflush(stdout);
   }
   if (dbg) fprintf(stdout, "xServerQuery::doConnect():Exit\n");
   if (dbg) fflush(stdout);
}

void xServerQuery::newList()
{
   if (dbg) fprintf(stdout, "xServerQuery::newList():Enter\n");
   if (dbg) fflush(stdout);
   pTable->clear();
   if (dbg) fprintf(stdout, "xServerQuery::newList():Calling shoRows()\n");
   if (dbg) fflush(stdout);
   pTable->showRows(0);
   if (dbg) fprintf(stdout, "xServerQuery::newList():Exit\n");
   if (dbg) fflush(stdout);
}

void xServerQuery::importList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "importfile", "ImportFile")) == NULL)
      pFn = "servers.ini";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
      pPath = "./";
   if ((pFilt = Resources->get(wdtRes, "importfilter", "ImportFilter")) == NULL)
      pFilt = "*.ini";

   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);
   if (!fileName.isEmpty())
      pTable->import(fileName.latin1());
}

void xServerQuery::saveList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "filename", "Filename")) == NULL)
      pFn = ".servers";
   if ((pPath = Resources->get(wdtRes, "path", "Path")) == NULL)
      pPath = "/usr/local/lib/xIrc";
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".*";

   fileName = QFileDialog::getSaveFileName(pPath + "/" + pFn, pFilt, this);
   if (!fileName.isEmpty())
      pTable->writeFile(fileName.latin1());
}

void xServerQuery::editEntry()
{
   xIrcServerEdit editDlg(wdtRes, NULL, "Server Entry Edit", 
                          pTable->entry(pTable->selectedRow()));
   editDlg.exec();
   pTable->showRows(pTable->currentRow());
}

void xServerQuery::deleteEntry()
{
   pTable->remove(pTable->selectedRow());
}

void xServerQuery::connectServer()
{
   serverSelected(pTable->selectedRow());
}

void xServerQuery::disconnectServer()
{
   done(Disconnect);
}

void xServerQuery::loadList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   if ((pFn = Resources->get(wdtRes, "file", "File")) == NULL)
      pFn = ".servers";
   if ((pPath = getenv("HOME")) == NULL)
      pPath = "./";
   if ((pFilt = Resources->get(wdtRes, "filter", "Filter")) == NULL)
      pFilt = ".*";
   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);
   pTable->readFile(fileName.latin1());
   pTable->showRows(pTable->currentRow());
}

void xServerQuery::gotoFirst()
{
//   pTable->scroll(0);
   pTable->highlightRow(0);
   pTable->highlightPrev();
}

void xServerQuery::gotoLast()
{
   pTable->highlightRow(pTable->rows() - 1);
//   pTable->scroll(pTable->rows() - 1);
}

xServerQuery::~xServerQuery()
{
   if (pAccel)
      delete pAccel;

   if (pTable)
   {
      if (dbg) fprintf(stdout, "xServerQuery::~xServerQuery():Deleting pTable\n");
      if (dbg) fflush(stdout);
      delete pTable;
   }
   if (pLabel)
   {
      if (dbg) fprintf(stdout, "xServerQuery::~xServerQuery():Deleting pLabel\n");
      if (dbg) fflush(stdout);
      delete pLabel;

   }
   if(pButtons)
   {
      if (dbg) fprintf(stdout, "xServerQuery::~xServerQuery():Deleting pButtons\n");
      if (dbg) fflush(stdout);
      delete pButtons;
   }
   if (dbg) fprintf(stdout, "xServerQuery::~xServerQuery():Done!\n");
   if (dbg) fflush(stdout);
}

//#include "xIrcServerQuery.moc"
