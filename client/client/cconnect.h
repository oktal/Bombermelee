#ifndef CCONNECT_H
#define CCONNECT_H

#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>

class CConnect : public QDialog
{
    Q_OBJECT
public:
    CConnect(QWidget *parent = 0);
private:
    QPushButton *m_btn_connect;
    QPushButton *m_btn_cancel;
    QDialogButtonBox *m_buttonBox;
    QLineEdit *m_txt_address;
    QLineEdit *m_txt_nick;

private slots:
    void connect();

};

#endif // CCONNECT_H
