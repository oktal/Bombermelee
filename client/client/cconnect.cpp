#include "cconnect.h"
#include "cclient.h"

#include <QtGui>

/*
This file is part of Bombermelee.

    Bombermelee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bombermelee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bombermelee.  If not, see <http://www.gnu.org/licenses/>.
*/

CConnect::CConnect(QWidget *parent) : QDialog(parent)
{
    QLabel *lbl_ip = new QLabel("Enter remote IP Address");
    QLabel *lbl_nick = new QLabel("Enter your nickname");
    m_txt_address = new QLineEdit();
    m_txt_address->setInputMask("000.000.000.000; ");
    m_txt_nick = new QLineEdit();
    m_btn_connect = new QPushButton("Connect");
    m_btn_cancel = new QPushButton("Cancel");
    m_buttonBox = new QDialogButtonBox(Qt::Horizontal);
    m_buttonBox->addButton(m_btn_connect, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_btn_cancel, QDialogButtonBox::ActionRole);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(lbl_ip, 0, 0);
    mainLayout->addWidget(m_txt_address, 0, 1);
    mainLayout->addWidget(lbl_nick, 1, 0);
    mainLayout->addWidget(m_txt_nick, 1, 1);
    mainLayout->addWidget(m_buttonBox, 2, 1);
    setLayout(mainLayout);
    setWindowIcon(QIcon("../server.png"));

    QObject::connect(m_btn_connect, SIGNAL(clicked()), this, SLOT(connect()));
    QObject::connect(m_btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void CConnect::connect()
{
    CClient *c = new CClient(this, m_txt_address->text(), m_txt_nick->text());
    c->show();
    close();
}
