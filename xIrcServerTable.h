#ifndef XIRCSERVERTABLE_H
#define XIRCSERVERTABLE_H

#include <qtable.h>
#include "xIrcServerList.h"
#include "xIrcServerEntry.h"

class xIrcServerTable: public QTable
{
   Q_OBJECT

public:
   xIrcServerTable(QWidget *parent = 0, const char *name = 0);
   ~xIrcServerTable();

   void loadTable(xIrcServerList *srvList);
   void clearTable();
   void addEntry(xIrcServerEntry &e);
   void setCurNumRows(int rows) { curNumRows = rows; }
    int getCurNumRows() { return curNumRows; }
   void incCurNumRows() { ++curNumRows; }
   void decCurNumRows() { --curNumRows; }
   xIrcServerEntry *getCurrentEntry() { return &findEntry; }
   void replaceEntry(int r, xIrcServerEntry *e);
   bool readFile(QString &e);

public slots:
   void getRowData(int rows);

private:
   enum { NumRows = 12, NumCols = 5 };
   int curNumRows;

   void clear();

   xIrcServerEntry findEntry;
};

#endif
