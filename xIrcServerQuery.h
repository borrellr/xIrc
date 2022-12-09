#ifndef XIRCSERVERQUERY_H
#define XIRCSERVERQUERY_H

#include <qdialog.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qaction.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtable.h>
#include "xIrcServerEntry.h"
#include "xIrcServerList.h"

class xServerQuery : public QDialog
{
   Q_OBJECT
public:
   xServerQuery(QWidget *parent = 0, const char *name = 0);
   ~xServerQuery();
   const char *server() { return connServer.latin1(); }
   const char *port() { return connPort.latin1(); }
   enum QryResults { Rejected, Accepted, Disconnect };

signals:
   void msgToSocket(const char *pBuf);

private slots:
   void newList();
   void importList();
   void saveList();
   void newEntry();
   void editEntry();
   void deleteEntry();
   void loadList();
   void connectServer();
   void disconnectServer();
   void getRowData(int rows);

private:
   void initClass();
   void initActions();
   void initMenus();
   void initLoadData();
   void loadTable(xIrcServerList *srvList);
   void clearTable();
   void addEntry(xIrcServerEntry &e);
   int curNumRows;
   void setCurNumRows(int rows) { curNumRows = rows; };
    int getCurNumRows() { return curNumRows; };
   void incCurNumRows() { ++curNumRows; };
   void decCurNumRows() { --curNumRows; };
   xIrcServerEntry findEntry;
   xIrcServerEntry *getCurrentEntry() { return &findEntry; };
   void replaceEntry(int r, xIrcServerEntry *e);
   void initTable();
   bool readTableFile(const QString &fn);

   QString connServer;
   QString connPort;
   QLayout           *pLayout;
   QMenuBar          *pMenuBar;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pServerMenu;
   xIrcServerEntry   *pServerEntry;
   xIrcServerList    *serverList;
   QTable            *pTable;
   enum { NumRows = 20, NumCols = 5 };
   enum { Group, Country, State, City, Server, Ports, Max };

   QAccel  *pAccel;
   QAction *newListAct;
   QAction *loadListAct;
   QAction *importListAct;
   QAction *saveListAct;
   QAction *doneAct;
   QAction *connectServerAct;
   QAction *disconnectServerAct;
   QAction *newEntryAct;
   QAction *editEntryAct;
   QAction *deleteEntryAct;
};

#endif
