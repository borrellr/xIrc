/****************************************************************************
** Form interface generated from reading ui file 'servereditdialog.ui'
**
** Created: Thu Oct 20 20:51:41 2022
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SERVEREDITDIALOG_H
#define SERVEREDITDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLineEdit;
class QLabel;

class serverEditDialog : public QDialog
{
    Q_OBJECT

public:
    serverEditDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~serverEditDialog();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* serverEdit;
    QLabel* stateLabel;
    QLabel* cityLabel;
    QLabel* groupLabel;
    QLineEdit* countryEdit;
    QLineEdit* portEdit;
    QLabel* serverLabel;
    QLineEdit* groupEdit;
    QLabel* countryLabel;
    QLineEdit* cityEdit;
    QLabel* portLabel;
    QLineEdit* stateEdit;

    virtual bool isEnable();

public slots:
    virtual void enableOkButton();

protected:
    QHBoxLayout* layout3;
    QGridLayout* layout2;

protected slots:
    virtual void languageChange();

};

#endif // SERVEREDITDIALOG_H
