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
     if (entry->flag() > 0)
        addrCheckBox->setChecked(TRUE);
}
