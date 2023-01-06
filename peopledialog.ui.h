/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void peopleDialog::init()
{
     initActions();
     initMenus();
}


void peopleDialog::initCols()
{
    //  Initiliaze column three with checkboxes
    for (int row = 0; row < peopleTable->numRows(); row++) {
          QCheckTableItem *chkBox = new QCheckTableItem(peopleTable, tr("By Addr"));
          peopleTable->setItem(row, 3, chkBox);
    }
    
    //  Initiliaze column two with checkboxes
    for (int row =  0; row < peopleTable->numRows(); row++) {
          QCheckTableItem *chkBox = new QCheckTableItem(peopleTable, tr("By Name"));
          peopleTable->setItem(row, 2, chkBox);
    }
}

void peopleDialog::initClass( const QString &name )
{
     QString tmpStr(name);
     dlgName = name;
     tmpStr.append(" List");
     setCaption(tr(tmpStr));
     peopleTable->horizontalHeader()->setLabel(2, tr(name));
    
     initData();
}


void peopleDialog::initMenus()
{
   pFileMenu = new QPopupMenu;
   newListAct->addTo(pFileMenu);
   loadListAct->addTo(pFileMenu);
   saveListAct->addTo(pFileMenu);
   pFileMenu->insertSeparator();
   doneAct->addTo(pFileMenu);

   pEntryMenu = new QPopupMenu();
   newEntryAct->addTo(pEntryMenu);
   editEntryAct->addTo(pEntryMenu);
   delEntryAct->addTo(pEntryMenu);

   pMenu = new QMenuBar(this);
   pMenu->insertItem(tr("&File"), pFileMenu);
   pMenu->insertItem(tr("&Entry"), pEntryMenu);
}

void peopleDialog::initActions()
{
   newListAct = new QAction(tr("New"), 0, this);
   newListAct->setStatusTip(tr("Create a new list"));
   connect(newListAct, SIGNAL(activated()), this, SLOT(newList()));

   loadListAct = new QAction(tr("Load"), 0, this);
   loadListAct->setStatusTip(tr("Load list from file"));
   connect(loadListAct, SIGNAL(activated()), this, SLOT(loadList()));

   saveListAct = new QAction(tr("Save As"), 0, this);
   saveListAct->setStatusTip(tr("Save list to a file"));
   connect(saveListAct, SIGNAL(activated()), this, SLOT(saveList()));

   doneAct = new QAction(tr("Done"), 0, this);
   doneAct->setStatusTip(tr("Exit"));
   connect(doneAct, SIGNAL(activated()), this, SLOT(reject()));

   newEntryAct = new QAction(tr("New"), 0, this);
   newEntryAct->setStatusTip(tr("Create new entry"));
   connect(newEntryAct, SIGNAL(activated()), this, SLOT(newEntry()));

   editEntryAct = new QAction(tr("Edit"), 0, this);
   editEntryAct->setStatusTip(tr("Edit a entry"));
   connect(editEntryAct, SIGNAL(activated()), this, SLOT(editEntry()));

   delEntryAct = new QAction(tr("Delete"), 0, this);
   delEntryAct->setStatusTip(tr("Delete a entry"));
   connect(delEntryAct, SIGNAL(activated()), this, SLOT(delEntry()));
}


void peopleDialog::newList()
{
   if (!peopleList.empty()) peopleList.clear();
   clearTable();
}


void peopleDialog::loadList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   pFn = ".xIrc." + dlgName;
   
   if ((pPath = getenv("HOME")) == NULL)
       pPath = "./";

   pFilt = ".xIrc*";

   fileName = QFileDialog::getOpenFileName(pPath + "/" + pFn, pFilt, this);
   
   if (!fileName.isEmpty()) {
       readPeopleData(fileName);
       initTable();
   }
}

void peopleDialog::saveList()
{
   QString pFn, pPath, pFilt;
   QString fileName;

   pFn = ".xIrc." + dlgName;
   
   if ((pPath = getenv("HOME")) == NULL)
       pPath = "./";

   pFilt = ".xIrc*";

   fileName = QFileDialog::getSaveFileName(pPath + "/" + pFn, pFilt, this);
   
   if (!fileName.isEmpty())
       savePeopleData(fileName);
}


void peopleDialog::newEntry()
{
   xIrcPeopleEntry e;
   QString n("Ignore Entry Edit");

   xIrcPeopleEdit editDlg;
   editDlg.initEntry(&e, n);
   if (editDlg.exec() == QDialog::Accepted) {
       peopleList.push_back(e);
       addEntryToTable(e);
   }
}


void peopleDialog::editEntry()
{
     int row, flag;
     bool byName = false, byAddr = false;
     QCheckTableItem *chkbox, *chkbox1;
     QString n("Ignore Entry Edit");
     xIrcPeopleEntry e;
     xIrcPeopleEdit editDlg;
     std::vector<xIrcPeopleEntry>::iterator it;
     
     row = peopleTable->currentRow();
     e.setNick(peopleTable->text(row, 0));
     e.setMask(peopleTable->text(row, 1));
     e.setMessage(peopleTable->text(row, 4));
     chkbox = (QCheckTableItem *) peopleTable->item(row, 2);
     chkbox1 = (QCheckTableItem *) peopleTable->item(row, 3);
     
     if (chkbox != 0) 
        byName = chkbox->isChecked();
     if (chkbox1 != 0)
        byAddr = chkbox1->isChecked();
     
     flag = byName ? 1 : (byAddr ? -1 : 0);
     e.setFlag(flag);
     it = std::find(peopleList.begin(), peopleList.end(), e);
     editDlg.initEntry(&e, n);
     if (editDlg.exec() == QDialog::Accepted) {
         peopleTable->setText(row, 0, e.nick());
         peopleTable->setText(row, 1, e.mask());
         peopleTable->setText(row, 4, e.message());
         if (chkbox == 0)
             chkbox = new QCheckTableItem(peopleTable, "By Name");
         if (chkbox1 == 0)
              chkbox1 = new QCheckTableItem(peopleTable, "By Addr");
         flag = e.flag();
         if (flag > 0) {
             chkbox->setChecked(true);
             chkbox1->setChecked(false);
         } else if (flag < 0) {
             chkbox->setChecked(false);
             chkbox1->setChecked(true);
         }
         if (it == peopleList.end())
             peopleList.push_back(e);
         else
             *it = e;
     }
}


void peopleDialog::delEntry()
{
    std::vector<xIrcPeopleEntry>::iterator it;
    xIrcPeopleEntry e;
    int row = peopleTable->currentRow();
    int flag;

    e.setNick(peopleTable->text(row, 0));
    e.setMask(peopleTable->text(row, 1));
    e.setMessage(peopleTable->text(row, 4));
    QCheckTableItem *byName = (QCheckTableItem *)peopleTable->item(row, 2);
    QCheckTableItem *byAddr = (QCheckTableItem *)peopleTable->item(row, 3);
    flag = (!byName->isChecked()) ? 0 : (byAddr->isChecked()) ? 1 : -1;
    e.setFlag(flag);
    e.setState(0);
    
    peopleTable->removeRow(row);
    it = std::find(peopleList.begin(), peopleList.end(), e);
    if ( it != peopleList.end())
        peopleList.erase(it);
}



void peopleDialog::clearTable()
{
   int max = peopleTable->numRows();
   QMemArray<int> f(max);

   for (int i = 0; i < max; i++)
       f[i] = i;

   peopleTable->removeRows(f);
   peopleTable->setNumRows(20);
}


void peopleDialog::addEntryToTable( xIrcPeopleEntry &entry )
{
     int rows = peopleList.size();
     int row = rows - 1 ;
     QCheckTableItem *chkbox;
     QCheckTableItem *chkbox1;
     QString nick(entry.nick()), mask(entry.mask()), message(entry.message());

     if (rows > peopleTable->numRows())
        peopleTable->setNumRows(rows);

     chkbox = (QCheckTableItem *)peopleTable->item(row, 2);
     if (chkbox == 0)
         chkbox = new QCheckTableItem(peopleTable, tr("By Name"));
     chkbox1 = (QCheckTableItem *)peopleTable->item(row, 3);
     if (chkbox1 == 0)
         chkbox1 = new QCheckTableItem(peopleTable, tr("By Addr"));
     Q_CHECK_PTR(chkbox);
     Q_CHECK_PTR(chkbox1);
     if (entry.flag() > 0) {
         chkbox->setChecked(true);
         chkbox1->setChecked(false);
     }
     if (entry.flag() < 0) {
         chkbox->setChecked(false);
         chkbox1->setChecked(true);
     }

     peopleTable->setText(row, 0, nick);
     peopleTable->setText(row, 1, mask);
     peopleTable->setItem(row, 2, chkbox);
     peopleTable->setItem(row, 3, chkbox1);
     peopleTable->setText(row, 4, message);
}


void peopleDialog::savePeopleData( const QString &fileName )
{
    QString line, tmpString;
    QStringList tmpList;
    int flag;
     
    QFile f(fileName);
    if (!f.open(IO_WriteOnly)) {
         qWarning("File %s is not writeable\n", fileName.latin1());
         return;
    }
     
    QTextStream stream(&f);
    std::vector<xIrcPeopleEntry>::iterator it = peopleList.begin();
    
    for (; it != peopleList.end(); ++it) {
          xIrcPeopleEntry e = *it;
           tmpList.clear();
           tmpList.append(e.nick());
           tmpList.append(e.mask());
           flag = e.flag();
           tmpString = (flag > 0) ? "+" : ((flag < 0) ? "-" : ".");
           tmpList.append(tmpString);
           tmpList.append(e.message());
           line = tmpList.join(":");
           stream << line.latin1() << endl;
   }
   f.close(); 
}


void peopleDialog::readPeopleData( const QString &fileName )
{
    QString line, tmpString;
    QStringList tmpList;
    int flag;
     
    QFile f(fileName);
    if (!f.exists()) {
        qWarning("File %s does not exists\n", fileName.latin1());
        return;
    }

    if (!f.open(IO_ReadOnly)) {
         qWarning("File %s is not readable\n", fileName.latin1());
         return;
    }
     
    QTextStream stream(&f);

    while ( !stream.atEnd()) {
        line = stream.readLine();
        if (!line.isEmpty()) {
            xIrcPeopleEntry e;
            tmpList.clear();
            tmpList = QStringList::split(":", line);
            e.setNick(tmpList[0]);
            e.setMask(tmpList[1]);
            tmpString = (tmpList[2]);
            flag = (tmpString == "+" ) ? 1 : ((tmpString == "-") ? -1 : 0);
            e.setFlag(flag);
            e.setMessage(tmpList[3]);
            peopleList.push_back(e);
            }
    }
    f.close();
}


void peopleDialog::initData()
{    
   QString pFn, pPath;
   QString fileName;

   pFn = ".xIrc." + dlgName;
   
   if ((pPath = getenv("HOME")) == NULL)
       pPath = ".";

   fileName = pPath + "/" + pFn ;
   
   if (!fileName.isEmpty()) {
       readPeopleData(fileName);
       if (!peopleList.empty())
          initTable();
   }
}


void peopleDialog::initTable()
{
     int count, rows, row, flag;
     std::vector<xIrcPeopleEntry>::iterator it;
     xIrcPeopleEntry e;
     QCheckTableItem *chkbox, *chkbox1;
     
     count = peopleList.size();
     
     if (count == 0)
         return;

     rows = peopleTable->numRows();
     
     if (count > rows)
         peopleTable->setNumRows(count);

     clearTable();
     for (it = peopleList.begin(), row = 0; it != peopleList.end(); ++it, row++) {
           e = *it;
           flag = e.flag();
           chkbox = (QCheckTableItem *) peopleTable->item(row, 2);
           if (chkbox == 0)
               chkbox = new QCheckTableItem(peopleTable, tr("By Name"));
           chkbox1 = (QCheckTableItem *) peopleTable->item(row, 3);
           if (chkbox1 == 0)
               chkbox1 = new QCheckTableItem(peopleTable, tr("By Addr"));

           Q_CHECK_PTR(chkbox);
           Q_CHECK_PTR(chkbox1);

           if (flag > 0) {
               chkbox->setChecked(true);
               chkbox1->setChecked(false);
           } else if (flag < 0) {
               chkbox->setChecked(false);
               chkbox1->setChecked(true);
           }
           peopleTable->setText(row, 0, e.nick());
           peopleTable->setText(row, 1, e.mask());
           peopleTable->setItem(row, 2, chkbox);
           peopleTable->setItem(row, 3, chkbox1);
           peopleTable->setText(row, 4, e.message());
     }
     peopleTable->adjustColumn(0);
     peopleTable->adjustColumn(1);
     peopleTable->adjustColumn(4);
}
