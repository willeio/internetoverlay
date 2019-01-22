#include "conversation.h"
#include "ui_conversation.h"

#include "message.h"
#include <iostream>
#include <IOLib/io.h>


Conversation::Conversation(QWidget *parent, const Contact &contact, IO *io) :
  QDialog(parent),
  ui(new Ui::Conversation)
, m_contact(contact)
, m_io(io)
{
  ui->setupUi(this);

  connect(ui->send, &QLineEdit::returnPressed,
          this, &Conversation::slotSend);

  ui->lbName->setText(contact.name);
}

Conversation::~Conversation()
{
  delete ui;
}



void Conversation::slotSend()
{
  union chat_msg cm;

  cm.data.magic[0] = 'C';
  cm.data.magic[1] = 'M';

  int r = rand();

  std::cout << "msg id: " << r << std::endl;

  cm.data.msg_id = r;
  cm.data.partner_id = m_contact.partnerid;

  memset(cm.data.msg, 0, 128);
  strcpy((char*)cm.data.msg, ui->send->text().toStdString().c_str()); // TODO: more sec

  std::cout << "sending.." << std::endl;

  if (io_out(m_io, (char*)m_contact.publicKey.toStdString().c_str(),
             cm.buffer, sizeof(union chat_msg)) < 0)
  {
    std::cout << "failed :(" << std::endl;
    return;
  }

  std::cout << "sent!" << std::endl;

  ui->send->clear();
}



void Conversation::addMessage(const QString &msg)
{
  QTableWidgetItem *newItem = new QTableWidgetItem(msg);
  int row = ui->messages->rowCount();
  ui->messages->insertRow(row);
  ui->messages->setItem(row, 0, newItem);
}
