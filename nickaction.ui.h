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


const char * xIrcNickActionQuery::nick()
{
   return(nickEdit->text().latin1());
}


const char * xIrcNickActionQuery::chan()
{
   return(channelEdit->text().latin1());
}


void xIrcNickActionQuery::setNick( const char * pStr )
{
   QString str(pStr);
   nickEdit->setText(str);
}


void xIrcNickActionQuery::setChan( const char *pStr )
{
   QString str(pStr);
   channelEdit->setText(str);
}


void xIrcNickActionQuery::wakeUp( xMultiLineTextSelection text )
{
   txtSel = text;
   if (txtSel.text != NULL && !txtSel.text.isEmpty())
   {
      nickEdit->setText(text.text);
   }
   if (txtSel.winName != NULL)  // && strlen(txtSel.winName) > 0)
   {
      channelEdit->setText(text.winName);
   }
   raise();
   show();
}


void xIrcNickActionQuery::wakeUp( const char *pNick )
{
   txtSel.text = "";
   txtSel.winName = "";
   for (; *pNick != '\0' && *pNick != ' '; pNick++)
      txtSel.text += *pNick;
   txtSel.winName = "";
   if (!txtSel.text.isEmpty())
      nickEdit->setText(txtSel.text);
   if (!txtSel.winName.isEmpty())
      channelEdit->setText(txtSel.winName);
   raise();
   show();
}


void xIrcNickActionQuery::buttonPressed( int id )
{
   txtSel.text = nickEdit->text();
   txtSel.winName = channelEdit->text();
   txtSel.iData = id;
   emit done(txtSel);
}


void xIrcNickActionQuery::banAct()
{
   buttonPressed(Ban);
}


void xIrcNickActionQuery::dccChatAct()
{
   buttonPressed(DccChat);
}


void xIrcNickActionQuery::fileSendAct()
{
   buttonPressed(FileSend);
}


void xIrcNickActionQuery::fingerAct()
{
   buttonPressed(Finger);
}


void xIrcNickActionQuery::giveopsAct()
{
   buttonPressed(GiveOps);
}


void xIrcNickActionQuery::ignoreAct()
{
   buttonPressed(Ignore);
}


void xIrcNickActionQuery::kickAct()
{
   buttonPressed(Kick);
}


void xIrcNickActionQuery::notfyAct()
{
   buttonPressed(Notify);
}


void xIrcNickActionQuery::pingAct()
{
   buttonPressed(Ping);
}


void xIrcNickActionQuery::privchatAct()
{
   buttonPressed(PrivChat);
}


void xIrcNickActionQuery::takeopsAct()
{
   buttonPressed(TakeOps);
}


void xIrcNickActionQuery::timeAct()
{
   buttonPressed(Time);
}


void xIrcNickActionQuery::userinfoAct()
{
   buttonPressed(UserInfo);
}


void xIrcNickActionQuery::versionAct()
{
   buttonPressed(Version);
}


void xIrcNickActionQuery::whoAct()
{
   buttonPressed(Who);
}


void xIrcNickActionQuery::whoisAct()
{
   buttonPressed(WhoIs);
}


void xIrcNickActionQuery::whowasAct()
{
   buttonPressed(WhoWas);
}
