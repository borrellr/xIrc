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
   pTable = new QTable(this);
   pMenuBar = new QMenuBar(this);

   serverList.clear();
   QRect r(0,0,400,400);
   pLayout = new QBoxLayout(this, QBoxLayout::Down);
   pLayout->setGeometry(r);
   pLayout->setMenuBar(pMenuBar);
   pLayout->add(pTable);
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
   if (!readTableFile(fileName)) {
      qWarning("The file servers.dat is not present");
   }
}

void xServerQuery::newList()
{
   if (!serverList.empty())
      serverList.clear();

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
      writeFile(fileName);
}

void xServerQuery::newEntry()
{
   xIrcServerEdit *newEdit = new xIrcServerEdit(this);

   newEdit->show();
   if (newEdit->exec()) {
      QString result = newEdit->getEntry();
      QStringList tmpList = serverList.grep(result);
      if (tmpList.empty()) {
         serverList.append(result);
         addEntry(result);
      }
   }
}

void xServerQuery::editEntry()
{
   xIrcServerEdit *modEdit = new xIrcServerEdit(this);

   int row = pTable->currentRow();
   const QString currEntry = getRowData(row);
   QStringList tmpList = serverList.grep(currEntry);
   if (!tmpList.empty()) {
      QStringList::Iterator it = serverList.find(tmpList[0]);

      if (it != serverList.end()) {
         printf("Iterator |%s| String |%s|\n", (*it).latin1(), tmpList[0].latin1());
         modEdit->initEntry(*it);
         modEdit->show();
         if (modEdit->exec()) {
            QString result = modEdit->getEntry(); 
            serverList.remove(it);
            serverList.append(result);
            replaceEntry(row, result);
         }
      }
   }
}

void xServerQuery::deleteEntry()
{
   int row = pTable->currentRow();
   const QString currEntry = getRowData(row);
   QStringList tmpList = serverList.grep(currEntry);
   if (!tmpList.empty()) {
      QStringList::Iterator it = serverList.find(tmpList[0]);
      pTable->removeRow(row);
      decCurNumRows();
      serverList.remove(it);
   }
}

void xServerQuery::loadList()
{
   QString pFn(".servers"), pPath(getenv("HOME")), pFilt(".*");
   QString fileName;

   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);

   if (!fileName.isEmpty()) {
      if (!readTableFile(fileName)) {
         qWarning("The file %s is not present", fileName.latin1());
         return;
      }
   }
}

void xServerQuery::connectServer()
{
   xIrcConnectDialog *conServer = new xIrcConnectDialog(this);
   int row = pTable->currentRow();
   const QString currEntry = getRowData(row);
   QStringList tmpList = serverList.grep(currEntry);

   if (!tmpList.empty()) {
      conServer->initEntry(tmpList[0]);
      conServer->show();
      conServer->raise();
      if (int x = conServer->exec()) {
         QString result = conServer->getEntry();
         QStringList tmpList1 = QStringList::split(":", result);
         connServer = tmpList1[Server];
         connPort = tmpList1[Ports];
         done(x);
      }
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
   pTable->horizontalHeader()->setLabel(0, "Group");
   pTable->horizontalHeader()->setLabel(1, "Country");
   pTable->horizontalHeader()->setLabel(2, "State");
   pTable->horizontalHeader()->setLabel(3, "City");
   pTable->horizontalHeader()->setLabel(4, "Server");
   pTable->selectRow(0);
   setCurNumRows(0);
}

void xServerQuery::loadTable()
{
    for (QStringList::iterator it = serverList.begin();
         it != serverList.end(); ++it) {
        addEntry(*it);
    }
    pTable->adjustColumn(4);
}

const QString &xServerQuery::getRowData(int row)
{
   QStringList tmpList;
   QString *tmpStr = new QString();

   tmpList.append(pTable->text(row, 0));
   tmpList.append(pTable->text(row, 1));
   tmpList.append(pTable->text(row, 2));
   tmpList.append(pTable->text(row, 3));
   tmpList.append(pTable->text(row, 4));

   *tmpStr = tmpList.join(":");
   return *tmpStr;
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

void xServerQuery::addEntry(const QString &entry)
{
   QStringList tmpList = QStringList::split(":", entry);
   int row = getCurNumRows();

   if (row == pTable->numRows())
      pTable->setNumRows(row + 1);

   for (int col = 0; col < Ports; col++) {
      pTable->setText(row, col, tmpList[col]);
   }

   row++;
   setCurNumRows(row);
}

void xServerQuery::replaceEntry(int row, const QString &modEntry)
{
   QStringList tmpList = QStringList::split(":", modEntry);

   for (int col = 0; col < Ports; col++) {
      pTable->setText(row, col, tmpList[col]);
   }
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

   while ( !stream.atEnd()) {
      line = stream.readLine();
      if (!line.isEmpty()) {
            serverList.append(line);
            addEntry(line);
      }
   }
   f.close();
   return true;
}

bool xServerQuery::writeFile(const QString &file)
{
   QFile f(file);

   if (!f.open(IO_WriteOnly)) {
      qWarning("File %s is not writeable\n", file.latin1());
      return false;
   }

   QTextStream stream(&f);
   QString line;

   for (QStringList::Iterator it = serverList.begin();
        it != serverList.end(); ++it) {
      line = *it;
      stream << line.latin1() << endl;
   } 
   f.close();
   return true;
}
