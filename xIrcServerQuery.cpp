#include <stdlib.h>
#include <qfile.h>
#include <qmemarray.h>
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
   pTable = new QTable(this);
   pMenuBar = new QMenuBar(this);

   QRect r(0,0,400,400);
   pLayout = new QBoxLayout(this, QBoxLayout::Down);
   pLayout->setGeometry(r);
   pLayout->setMenuBar(pMenuBar);
   pLayout->add(pTable);
   pServerEntry = NULL;
   initTable();
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
   loadTable(serverList);
}

void xServerQuery::newList()
{
   if (serverList)
      delete serverList;
   serverList = new xIrcServerList();

   clearTable();
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
      addEntry(*result);
   }
}

void xServerQuery::editEntry()
{
   xIrcServerEdit *modEdit = new xIrcServerEdit(this);

   int row = pTable->currentRow();
   getRowData(row);
   xIrcServerEntry *currEntry = getCurrentEntry();
//   currEntry->showEntries();

   modEdit->initEntry(serverList->findEntry(currEntry));
   modEdit->show();
   if (modEdit->exec()) {
      xIrcServerEntry *result = modEdit->getEntry(); 
      serverList->replaceEntry(currEntry, result);
//      serverList->showEntries();
      replaceEntry(row, result);
   }
}

void xServerQuery::deleteEntry()
{
   int row = pTable->currentRow();
   getRowData(row);
   xIrcServerEntry *currEntry = getCurrentEntry();
//   currEntry->showEntries();
   pTable->removeRow(row);
   decCurNumRows();
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
      //loadTable(serverList);
      readTableFile(fileName);
   }
}

void xServerQuery::connectServer()
{
   xIrcConnectDialog *conServer = new xIrcConnectDialog(this);
   int row = pTable->currentRow();
   getRowData(row);
   xIrcServerEntry *currEntry = getCurrentEntry();
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

void xServerQuery::initTable()
{
   pTable->setNumRows(0);
   pTable->setNumCols(0);
   pTable->setNumRows(NumRows);
   pTable->setNumCols(NumCols);
//   void addEntry(xIrcServerEntry &e);
   pTable->horizontalHeader()->setLabel(0, "Group");
   pTable->horizontalHeader()->setLabel(1, "Country");
   pTable->horizontalHeader()->setLabel(2, "State");
   pTable->horizontalHeader()->setLabel(3, "City");
   pTable->horizontalHeader()->setLabel(4, "Server");
   pTable->selectRow(0);
   setCurNumRows(0);
}

void xServerQuery::loadTable(xIrcServerList *srvList)
{
    xIrcServerListIterator si(*srvList);

    while (si.current() != NULL) {
       addEntry(*si.current());
       ++si;
    }
    pTable->adjustColumn(4);
}

void xServerQuery::getRowData(int row)
{
   findEntry.setGroup(pTable->text(row, 0).latin1());
   findEntry.setCountry(pTable->text(row, 1).latin1());
   findEntry.setState(pTable->text(row, 2).latin1());
   findEntry.setCity(pTable->text(row, 3).latin1());
   findEntry.setServer(pTable->text(row, 4).latin1());
//   findEntry.showEntries();
}

void xServerQuery::clearTable()
{
   int max = pTable->numRows();
   QMemArray<int> f(max);

   for (int i = 0; i < max; i++)
       f[i] = i;

   pTable->removeRows(f);
   pTable->setNumRows(20);
   setCurNumRows(0);
}

void xServerQuery::addEntry(xIrcServerEntry &entry)
{
   QString tmpStr;
   int row = getCurNumRows();
   int col = 0;

   if (row == pTable->numRows())
      pTable->setNumRows(row + 1);

   tmpStr = entry.group();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.country();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.state();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.city();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.server();
   pTable->setText(row, col, tmpStr);

   row++;
   setCurNumRows(row);
}

void xServerQuery::replaceEntry(int row, xIrcServerEntry *modEntry)
{
   QString tmpStr;
   int col = 0;

   tmpStr = modEntry->group();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->country();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->state();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->city();
   pTable->setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->server();
   pTable->setText(row, col, tmpStr);
}

bool xServerQuery::readTableFile(const QString &fn)
{
   QFile f(fn);

   if (!f.exists()) {
       qWarning("File %s does not exists\n", fn.latin1());
       return false;
   }

   if (!f.open(IO_ReadOnly)) {
      qWarning("File %s is not readonly\n", fn.latin1());
      return false;
   }

   QTextStream stream(&f);
   QString line;
   QString groupStr, countryStr, stateStr, cityStr, serverStr, portsStr;
   QStringList dataLines;

   while ( !stream.atEnd()) {
      line = stream.readLine();
      if (!line.isEmpty()) {
         dataLines = QStringList::split(":", line);
         if (dataLines.count() == 6) {
            groupStr = dataLines[0];
            countryStr = dataLines[1];
            stateStr = dataLines[2];
            cityStr = dataLines[3];
            serverStr = dataLines[4];
            portsStr = dataLines[5];
            xIrcServerEntry e(groupStr, countryStr, stateStr, cityStr, serverStr, portsStr);
            addEntry(e);
         }
         dataLines.clear();
      }
   }
   f.close();
   return true;
}
