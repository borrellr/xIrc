#ifndef XIRCSERVERQUERY_H
#define XIRCSERVERQUERY_H

#include <qdialog.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qaction.h>
#include <qstring.h>
#include "xIrcServerEntry.h"
#include "xIrcServerList.h"
#include "xIrcServerTable.h"

class xServerQuery : public QDialog
{
   Q_OBJECT
public:
   xServerQuery(QWidget *parent = 0, const char *name = 0);
   ~xServerQuery();
   const char *server() { return connServer.latin1(); }
   const char *port() { return connPort.latin1(); }
   enum QryResults { Rejected, Accepted, Disconnect };

public slots:
   void newList();
   void importList();
   void saveList();
   void newEntry();
   void editEntry();
   void deleteEntry();
   void loadList();
   void connectServer();
   void disconnectServer();
   void reject();

protected slots:
   virtual void done(int results) { QDialog::done(results); };

private:
   void initClass();
   void initActions();
   void initMenus();
   void initLoadData();

   QString connServer;
   QString connPort;
   QLayout           *pLayout;
   QMenuBar          *pMenuBar;
   QPopupMenu        *pFileMenu;
   QPopupMenu        *pServerMenu;
   xIrcServerEntry   *pServerEntry;
   xIrcServerList    *serverList;
   xIrcServerTable   *pTable;

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
