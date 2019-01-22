#include "addcontact.h"
#include "ui_addcontact.h"

#include <QMessageBox>


AddContact::AddContact(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddContact)
{
  ui->setupUi(this);

  connect(ui->pbAdd, &QPushButton::clicked,
          this, &AddContact::slotAdd);

  connect(ui->pushButton_2, &QPushButton::clicked, this, &AddContact::close);
}


AddContact::~AddContact()
{
  delete ui;
}


bool AddContact::getNewContact(Contact &contact)
{
  exec(); // ????????

  if (m_contact.name.isEmpty() ||
      m_contact.publicKey.isEmpty())
    return false;

  contact = m_contact;
  return true;
}


void AddContact::slotAdd()
{
  m_contact.name = ui->lineEdit->text();
  m_contact.publicKey = ui->recvr->document()->toPlainText();
  m_contact.partnerid = ui->spinBox->value();

  if (m_contact.name.isEmpty() ||
      m_contact.publicKey.isEmpty())
  {
    QMessageBox::critical(this, "", "Missing infos.");
    return;
  }

  close();
}
