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

void xIrcInviteBox::acceptSlot()
{
    emit acknowledged(strChannel);
}

void xIrcInviteBox::chatSlot()
{
    emit acknowledged(strNick);
}

void xIrcInviteBox::initInviteBox( QObject *pReceiver, const QString &newSlot, xIrcMessage *pMsg )
{
    QStringList strList;
    strChannel = pMsg->msgStr.stripWhiteSpace();
    strNick = pMsg->srcNick;
    
    strList.append(strChannel);
    strList.append("cordially invites you to channel");
    strList.append(strNick);

    inviteEdit->setText(strList.join(" "));
    connect(this, SIGNAL(acknowledged(QString)), pReceiver, newSlot.latin1());
}
