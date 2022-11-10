#include <qmemarray.h>
#include "xIrcServerTable.h"

xIrcServerTable::xIrcServerTable(QWidget *parent, const char *name)
    : QTable(parent, name)
{
   clear();
}

xIrcServerTable::~xIrcServerTable()
{
}

void xIrcServerTable::clear()
{
   setNumRows(0);
   setNumCols(0);
   setNumRows(NumRows);
   setNumCols(NumCols);
   horizontalHeader()->setLabel(0, "Group");
   horizontalHeader()->setLabel(1, "Country");
   horizontalHeader()->setLabel(2, "State");
   horizontalHeader()->setLabel(3, "City");
   horizontalHeader()->setLabel(4, "Server");
   selectRow(0);
   setCurNumRows(0);
}

void xIrcServerTable::loadTable(xIrcServerList *srvList)
{
    xIrcServerListIterator si(*srvList);

    while (si.current() != NULL) {
       addEntry(*si.current());
       ++si;
    }
    adjustColumn(4);
}

void xIrcServerTable::getRowData(int row)
{
   findEntry.setGroup(text(row, 0).latin1());
   findEntry.setCountry(text(row, 1).latin1());
   findEntry.setState(text(row, 2).latin1());
   findEntry.setCity(text(row, 3).latin1());
   findEntry.setServer(text(row, 4).latin1());
//   findEntry.showEntries();
}

void xIrcServerTable::clearTable()
{
   int max = numRows();
   QMemArray<int> f(max);

   for (int i = 0; i < max; i++)
       f[i] = i;

   removeRows(f);
   setNumRows(12);
   setCurNumRows(0);
}

void xIrcServerTable::addEntry(xIrcServerEntry &entry)
{
   QString tmpStr;
   int row = getCurNumRows();
   int col = 0;

   if (row == numRows())
      setNumRows(row + 1);

   tmpStr = entry.group();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.country();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.state();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.city();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = entry.server();
   setText(row, col, tmpStr);

   row++;
   setCurNumRows(row);
}

void xIrcServerTable::replaceEntry(int row, xIrcServerEntry *modEntry)
{
   QString tmpStr;
   int col = 0;

   tmpStr = modEntry->group();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->country();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->state();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->city();
   setText(row, col, tmpStr);
   ++col;

   tmpStr = modEntry->server();
   setText(row, col, tmpStr);
}
