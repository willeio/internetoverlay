#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QDialog>

#include <IOClient/api.h>
#include "contact.h"


namespace Ui {
class Conversation;
}

class Conversation : public QDialog
{
  Q_OBJECT

public:
  explicit Conversation(QWidget *parent, const Contact &contact, IO* io);
  ~Conversation();

  void addMessage(const QString &msg);

private:
  Ui::Conversation *ui;
  Contact m_contact;
  IO* m_io;

private slots:
  void slotSend();
};

#endif // CONVERSATION_H
