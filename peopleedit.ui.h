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
#include <qstring.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>

void xIrcPeopleEdit::updateEntry()
{
     int f;

     pEntry->setNick(nickEdit->text().latin1());
     pEntry->setMask(addressEdit->text().latin1());
     pEntry->setMessage(messageEdit->text().latin1());
     f = (nameCheckBox->isChecked() == FALSE) ? 0 : (addrCheckBox->isChecked() == TRUE) ? 1 : -1;
     pEntry->setFlag(f);
     done(Accepted);
}


void xIrcPeopleEdit::initEntry( xIrcPeopleEntry *entry, const QString &name )
{
     setCaption(name);
     nameCheckBox->setText(name);
     pEntry = entry;
     nickEdit->setText(entry->nick());
     addressEdit->setText(entry->mask());
     messageEdit->setText(entry->message());
     if (entry->flag())
        nameCheckBox->setChecked(TRUE);
     if (entry->state() > 0)
        addrCheckBox->setChecked(TRUE);
}

void xIrcPeopleEdit::initEntry( const QString &entry, const QString & name )
{
     enum {Nick, RealNick, Mask, Message, Flag, State};

     QStringList entryList = QStringList::split(":", entry);
     setCaption(name);
     nickEdit->setText(entryList[Nick]);
     realNick = entryList[RealNick];
     addressEdit->setText(entryList[Mask]);
     messageEdit->setText(entryList[Message]);
     if (entryList[Flag] == "true")
        nameCheckBox->setChecked(TRUE);
     if (entryList[State].toInt() > 0)
        addrCheckBox->setChecked(TRUE);
}


void xIrcPeopleEdit::newDataEntry()
{
     QStringList strList; 
     QString state;

     strList.append(nickEdit->text());
     strList.append(realNick);
     strList.append(addressEdit->text());
     strList.append(messageEdit->text());
     if (nameCheckBox->isChecked())
        strList.append("true");
     else
        strList.append("false");
     state = (nameCheckBox->isChecked() == FALSE) ? "0" : (addrCheckBox->isChecked() == TRUE) ? "1" : "-1";
     strList.append(state);
     retEntry = strList.join(":");
}


const QString & xIrcPeopleEdit::getEntry()
{
     return retEntry;
}
