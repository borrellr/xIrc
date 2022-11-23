#include <stdlib.h>
#include <qfiledialog.h>
#include "xIrcServerQuery.h"
#include "servereditdialog.h"
#include "serverconnectdialog.h"

xServerQuery::xServerQuery(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
   initClass();
   initActions();
   initMenus();
   initLoadData();
   if (name)
      setCaption(name);
}

xServerQuery::~xServerQuery()
{
}

void xServerQuery::initClass()
{
   serverList = new xIrcServerList();
   pTable = new xIrcServerTable(this);
   pMenuBar = new QMenuBar(this);

   QRect r(0,0,400,400);
   pLayout = new QBoxLayout(this, QBoxLayout::Down);
   pLayout->setGeometry(r);
   pLayout->setMenuBar(pMenuBar);
   pLayout->add(pTable);
   pServerEntry = NULL;
}

void xServerQuery::initActions()
{
   newListAct = new QAction(tr("New"), 0, this);
   connect(newListAct, SIGNAL(activated()), this, SLOT(newList()));

   loadListAct = new QAction(tr("Load"), 0, this);
   connect(loadListAct, SIGNAL(activated()), this, SLOT(loadList()));

   importListAct = new QAction(tr("Import"), 0, this);
   connect(importListAct, SIGNAL(activated()), this, SLOT(importList()));

   saveListAct = new QAction(tr("Save As"), 0, this);
   connect(saveListAct, SIGNAL(activated()), this, SLOT(saveList()));

   doneAct = new QAction(tr("Done"), 0, this);
   connect(doneAct, SIGNAL(activated()), this, SLOT(reject()));

   connectServerAct = new QAction(tr("Connect"), 0, this);
   connect(connectServerAct, SIGNAL(activated()), this, SLOT(connectServer()));

   disconnectServerAct = new QAction(tr("Disconnect"), 0, this);
   connect(disconnectServerAct, SIGNAL(activated()), this, SLOT(disconnectServer()));

   newEntryAct = new QAction(tr("New"), 0, this);
   connect(newEntryAct, SIGNAL(activated()), this, SLOT(newEntry()));

   editEntryAct = new QAction(tr("Edit"), 0, this);
   connect(editEntryAct, SIGNAL(activated()), this, SLOT(editEntry()));

   deleteEntryAct = new QAction(tr("Delete"), 0, this);
   connect(deleteEntryAct, SIGNAL(activated()), this, SLOT(deleteEntry()));
}

void xServerQuery::initMenus()
{
   pFileMenu = new QPopupMenu();
   newListAct->addTo(pFileMenu);
   loadListAct->addTo(pFileMenu);
   importListAct->addTo(pFileMenu);
   saveListAct->addTo(pFileMenu);
   pFileMenu->insertSeparator();
   doneAct->addTo(pFileMenu);

   pServerMenu = new QPopupMenu();
   connectServerAct->addTo(pServerMenu);
   disconnectServerAct->addTo(pServerMenu);
   pServerMenu->insertSeparator();
   newEntryAct->addTo(pServerMenu);
   editEntryAct->addTo(pServerMenu);
   deleteEntryAct->addTo(pServerMenu);

   pMenuBar->insertItem("&File", pFileMenu);
   pMenuBar->insertItem("&Server", pServerMenu);
}

void xServerQuery::initLoadData()
{
   QString fileName("/usr/local/lib/xIrc/servers.dat");
   if (!serverList->readFile(fileName)) {
      qWarning("The file servers.dat is not present");
      return;
   }
//   serverList->showEntries();
   pTable->loadTable(serverList);
}

void xServerQuery::newList()
{
   if (serverList)
      delete serverList;
   serverList = new xIrcServerList();

   pTable->clearTable();
}

void xServerQuery::importList()
{
}

void xServerQuery::saveList()
{
   QString pFn(".servers"), pPath(getenv("HOME")), pFilt(".*");
   QString fileName;

   fileName = QFileDialog::getSaveFileName(pPath + "/" + pFn, pFilt, this);
   if (!fileName.isEmpty())
      serverList->writeFile(fileName);
}

void xServerQuery::newEntry()
{
   xIrcServerEdit *newEdit = new xIrcServerEdit(this);

   newEdit->show();
   if (newEdit->exec()) {
      xIrcServerEntry *result = newEdit->getEntry();
      serverList->add(*result);
      pTable->addEntry(*result);
   }
}

void xServerQuery::editEntry()
{
   xIrcServerEdit *modEdit = new xIrcServerEdit(this);

   int row = pTable->currentRow();
   pTable->getRowData(row);
   xIrcServerEntry *currEntry = pTable->getCurrentEntry();
//   currEntry->showEntries();

   modEdit->initEntry(serverList->findEntry(currEntry));
   modEdit->show();
   if (modEdit->exec()) {
      xIrcServerEntry *result = modEdit->getEntry(); 
      serverList->replaceEntry(currEntry, result);
//      serverList->showEntries();
      pTable->replaceEntry(row, result);
   }
}

void xServerQuery::deleteEntry()
{
   int row = pTable->currentRow();
   pTable->getRowData(row);
   xIrcServerEntry *currEntry = pTable->getCurrentEntry();
//   currEntry->showEntries();
   pTable->removeRow(row);
   pTable->decCurNumRows();
   serverList->removeEntry(currEntry);
}

void xServerQuery::loadList()
{
   QString pFn(".servers"), pPath(getenv("HOME")), pFilt(".*");
   QString fileName;

   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);

   if (!fileName.isEmpty())
   {
      if (!serverList->readFile(fileName))
      {
         qWarning("The file %s is not present", fileName.latin1());
         return;
      }
      //serverList->showEntries();
      //pTable->loadTable(serverList);
      pTable->readFile(fileName);
   }
}

void xServerQuery::connectServer()
{
   xIrcConnectDialog *conServer = new xIrcConnectDialog(this);
   int row = pTable->currentRow();
   pTable->getRowData(row);
   xIrcServerEntry *currEntry = pTable->getCurrentEntry();
   conServer->initEntry(serverList->findEntry(currEntry));
   conServer->show();
   conServer->raise();
   if (int x = conServer->exec()) {
      pServerEntry = conServer->getEntry();
      connServer = pServerEntry->server();
      connPort = pServerEntry->ports();
      done(x);
   }
}

void xServerQuery::disconnectServer()
{
   done(Disconnect);
}

void xServerQuery::reject()
{
   QDialog::reject();
}
