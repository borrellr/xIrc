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
     setCaption(tmpStr);
     peopleTable->horizontalHeader()->setLabel(3, name);
    
     initData();
     if (name == "Notify") {
        pTimer = new QTimer(this, NULL);
        connect(pTimer, SIGNAL(timeout()), this, SLOT(pollList()));
     }
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
        byAddr = chkbox->isChecked();
     if (chkbox1 != 0)
        byName = chkbox1->isChecked();
     
     flag = byAddr ? 1 : (byName ? -1 : 0);
     e.setFlag(flag);
     it = std::find(peopleList.begin(), peopleList.end(), e);
     editDlg.initEntry(&e, n);
     if (editDlg.exec() == QDialog::Accepted) {
         peopleTable->setText(row, 0, e.nick());
         peopleTable->setText(row, 1, e.mask());
         peopleTable->setText(row, 4, e.message());
         if (chkbox == 0)
             chkbox = new QCheckTableItem(peopleTable, "By Addr");
         if (chkbox1 == 0)
              chkbox1 = new QCheckTableItem(peopleTable, "By Name");
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
    QCheckTableItem *byAddr = (QCheckTableItem *)peopleTable->item(row, 2);
    QCheckTableItem *byName = (QCheckTableItem *)peopleTable->item(row, 3);
    flag = (!byAddr->isChecked()) ? 1 : (byName->isChecked()) ? -1 : 0;
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
               chkbox = new QCheckTableItem(peopleTable, tr("By Addr"));
           chkbox1 = (QCheckTableItem *) peopleTable->item(row, 3);
           if (chkbox1 == 0)
               chkbox1 = new QCheckTableItem(peopleTable, tr("By Name"));

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


bool peopleDialog::matches( xIrcMessage *msg )
{
     xIrcPeopleEntry e; 
     std::vector<xIrcPeopleEntry>::iterator it;
     
     for (it = peopleList.end(); it != peopleList.end(); ++it) {
          e = *it;
          if (e.is(msg, e.flag()))
              return true;
     }
     return false;
}


xIrcPeopleEntry * peopleDialog::entry( xIrcMessage *pMsg )
{
     xIrcPeopleEntry *e = new xIrcPeopleEntry; 
     std::vector<xIrcPeopleEntry>::iterator it;

     for (it = peopleList.begin(); it != peopleList.end(); ++it) {
          *e = *it;
          if (e->is(pMsg, e->flag()))
              return e;
     }
     delete e;
     return 0;    
}


void peopleDialog::add( xIrcPeopleEntry &e )
{
    peopleList.push_back(e);
    initTable();
}


void peopleDialog::shutDown()
{
   pTimer->stop();
}


void peopleDialog::startUp()
{
   pTimer->start(500, TRUE);
}


void peopleDialog::pollList()
{
    gotNotification(NULL);
}

bool peopleDialog::gotNotification( xIrcMessage *pMsg)
{
   bool rv = FALSE;
#if 0
   xIrcPeopleEntry *pEntry;
   xIrcMessage msg;
   const char *cp;

   if (pMsg != NULL)
   {
      rv = TRUE;
      QString str;

      cp = pMsg->dstStr.latin1();
      if (pMsg->rspCode == 352)
      {
         if (*cp != '#')
         {
            str = pMsg->dstStr;
            cp = pMsg->msgStr.latin1();
         }
         else
         {
            for (cp = pMsg->msgStr.latin1(); *cp != ' '; cp++)
               str += *cp;
            for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
         }
         str += "@";
         for (cp = pMsg->msgStr.latin1(); *cp != ' '; cp++)
            str += *cp;
         pEntry = pTable->entry(pMsg);
         if (pEntry != NULL)
         {
            QString realNick(getNick(pMsg));
            pEntry->setRealNick(realNick);
         }
         if (pEntry != NULL && pEntry->state() == 1)
         {
            char buf[256];
            sprintf(buf, "\x02%s (%s) Has Arrived!!\n",
               (const char *)pEntry->realNick().latin1(),
               (const char *)pEntry->mask().latin1());
            if (pEntry->message() != NULL && !pEntry->message().isEmpty())
            {
               msg.rspCode = ircResponses.code("PRIVMSG");
               msg.dstStr = getNick(pMsg);
               msg.msgStr = pEntry->message();
               pTWindow->sendMessage(&msg);
            }
            pEntry->setState(3);
            emit updateList(pTable->list());
         }
         else if (pEntry != NULL && pEntry->state() == 2)
         {
            pEntry->setState(3);
         }
      }
      else if (pMsg->rspCode == 315 || (pMsg->rspCode >= 400 && pMsg->rspCode < 600))
      {
         pEntry = pTable->entry(pMsg);
         if (pEntry != NULL && pEntry->state() == 3)
         {
            pEntry->setState(2);
         }
         else if (pEntry != NULL && (pEntry->state() == 1 || pEntry->state() == 2))
         {
            if (pEntry->state() == 2)
            {
               char buf[256];
               sprintf(buf, "\x02%s (%s) Has Left!!\n",
                       (const char *)pEntry->realNick().latin1(),
                       (const char *)pEntry->mask().latin1());
               pTWindow->putWindow(buf);
            }
            pEntry->setState(0);
            emit updateList(pTable->list());
         }
         pTimer->start(2500, TRUE);
         whoSent = FALSE;
         ++(*pPeople);
      }
   }
   else if (pPeople != NULL && pPeople->count() > 0)
   {
      QString strNick, strTmp;
      char ch;
      rv = TRUE;
      if (pPeople->current() == NULL)
      {
         delete pPeople;
         pPeople = NULL;
         if (pTable->list()->count() > 0)
         {
            pPeople = new xIrcPeopleListIterator(*pTable->list());
            pPeople->toFirst();
            pTimer->start(5000, TRUE);
         }
      }
      else
      {
         while (pPeople->current() != NULL && pPeople->current()->flag() == 0)
            ++(*pPeople);
         if (pPeople->current() != NULL)
         {
            if (pPeople->current()->state() == 0)
               pPeople->current()->setState(1);
            if (pPeople->current()->flag() < 0)
               strNick = pPeople->current()->nick();
            else if (pPeople->current()->flag() > 0)
            {
               strTmp = pPeople->current()->mask();
               for (ch = ' ', cp = strTmp.latin1(); *cp != '\0'; cp++)
               {
                  if (ch == '@')
                     strNick += *cp;
                  else if (*cp == '@')
                     ch = '@';
               }
            }
            msg.rspCode = ircResponses.code("WHO");
            msg.dstStr = strNick;
            msg.msgStr = "";
            Dispatcher.dispatchMsg(this, SLOT(messageIn(xIrcMessage*)), &msg);
            whoSent = TRUE;
         }
      }
   }
   else if (pPeople != NULL)
   {
      rv = TRUE;
      delete pPeople;
      pPeople = NULL;
      whoSent = FALSE;
      pTimer->start(2500, TRUE);
   }
   else
   {
      if (pTable->list()->count() > 0)
      {
         pPeople = new xIrcPeopleListIterator(*pTable->list());
         pPeople->toFirst();
      }
      pTimer->start(2500, TRUE);
   }
#endif
   return(rv);
}
