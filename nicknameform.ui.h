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
#include <qstringlist.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

void nickNameDialog::insertStringList( QStringList & strList )
{
   nickNameComboBox->insertStringList(strList, 1);

   if (nickNameCheckBox->isChecked())
      nickNameComboBox->setCurrentItem(1);
}


QString nickNameDialog::text() const
{
   int currItem = 0;
   QLineEdit *lineEdit;
   QString str;

   if (nickNameComboBox->editable()) {
      lineEdit = nickNameComboBox->lineEdit();
      if (lineEdit != 0) {
         str = lineEdit->text();
      }
   }

   if (nickNameCheckBox->isChecked() && str.isEmpty()) {
      currItem = nickNameComboBox->currentItem();
      str = nickNameComboBox->text(currItem);
   }
   return str;
}


void nickNameDialog::enableOkButton(const QString & str )
{
   if (!nickNameCheckBox->isChecked()) {
      if (!str.isEmpty())
         okPushButton->setEnabled(true);
   }
}


void nickNameDialog::toggled( bool flag )
{
  QString str(nickNameComboBox->text(0));

  if (!flag && str.isEmpty()) {
     okPushButton->setEnabled(false);    
  } else
     okPushButton->setEnabled(true);
}


void nickNameDialog::setCurrentItem( int ci )
{
  if (nickNameCheckBox->isChecked()) {
     nickNameComboBox->setCurrentItem(ci);
  } else {
     QString str(nickNameComboBox->text(ci));
     nickNameComboBox->setCurrentItem(0);
     nickNameComboBox->changeItem(str, 0);
  }
}



QString nickNameDialog::nextNick() const
{
    QString nextNickStr;
    int ci = nickNameComboBox->currentItem();
    ci++;
    if (ci < nickNameComboBox->sizeLimit()) {
        nickNameComboBox->text(ci);
    }
    return nextNickStr;
}




bool nickNameDialog::autoSelectNick()
{
     return nickNameCheckBox->isChecked();
}


void nickNameDialog::setText( const QString &str )
{
   QLineEdit *lineEdit;
   
   if (nickNameComboBox->editable()) {
      lineEdit = nickNameComboBox->lineEdit();
      if (lineEdit != 0) {
          lineEdit->setText(str);
      }
  }
}
