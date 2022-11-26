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

#define xCN_Whois   1
#define xCN_Query   2
#define xCN_Invite  3
#define xCN_Kick    4
#define xCN_Clear   5
#define xCN_Ping    6
#define xCN_Close   7

void xIrcChanNickBox::whoisSlot()
{
   emit buttonPressed(xCN_Whois);
}


void xIrcChanNickBox::chatSlot()
{
   emit buttonPressed(xCN_Query);
}


void xIrcChanNickBox::pingSlot()
{
   emit buttonPressed(xCN_Ping);
}


void xIrcChanNickBox::inviteSlot()
{
   emit buttonPressed(xCN_Invite);
}


void xIrcChanNickBox::kickSlot()
{
   emit buttonPressed(xCN_Kick);
}


bool xIrcChanNickBox::is( const char *pName )
{
   QString findStr(pName);
   bool retVal = false;
   
   if (!nickStrList.isEmpty())
       retVal = (nickStrList.find(findStr) != nickStrList.end());

   return retVal;
}


void xIrcChanNickBox::setNicks( const char *pNames )
{
   QString nicksStr(pNames);
   if (!nickStrList.isEmpty()) nickStrList.clear();
   nickStrList = QStringList::split(" ", nicksStr);
   nickListBox->insertStringList(nickStrList);
}


void xIrcChanNickBox::gotSelected( QListBoxItem *item )
{
   QString str(item->text());
   if (!str.isEmpty())
      emit nickSelected(str.latin1());
}


void xIrcChanNickBox::gotReturned()
{
   QString str(nickEdit->text());
   if (!str.isEmpty())
      emit nickSelected(str.latin1());
}


void xIrcChanNickBox::clear()
{
   nickListBox->clear();
}


const char * xIrcChanNickBox::text()
{
   const char *retStr = NULL;
   QString str(nickEdit->text());

   if (!str.isEmpty()) {
      retStr = str.latin1();
   } else {
      str = nickListBox->currentText();
      if (!str.isEmpty())
         retStr = str.latin1();
   }
   return retStr;
}
