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
   const QString &getRowData(int rows);

private:
   void initClass();
   void initActions();
   void initMenus();
   void initLoadData();
   void loadTable();
   void clearTable();
   void addEntry(const QString &e);
   int curNumRows;
   void setCurNumRows(int rows) { curNumRows = rows; };
    int getCurNumRows() { return curNumRows; };
   void incCurNumRows() { ++curNumRows; };
   void decCurNumRows() { --curNumRows; };
   void replaceEntry(int r, const QString &e);
   void initTable();
   bool readTableFile(const QString &fn);
   bool writeFile(const QString &fn);

   QStringList serverList;
   QString connServer;
   QString connPort;
   QLayout           *pLayout;
   QMenuBar          *pMenuBar;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pServerMenu;
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
