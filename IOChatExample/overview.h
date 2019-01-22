#ifndef OVERVIEW_H
#define OVERVIEW_H

#include <QDialog>
#include <QPair>
#include <QList>

#include <IOClient/api.h>

#include "contact.h"
#include "conversation.h"


class ContactsModel;
class Worker;


namespace Ui {
class Overview;
}

class Overview : public QDialog
{
  Q_OBJECT

public:
  explicit Overview(QWidget *parent = 0);
  ~Overview();

private:
  Ui::Overview *ui;
  ContactsModel *m_model;
  IO* m_io;
  Worker* w;
  QList<QPair<Contact, Conversation*> > m_conList;

  void openCon(const Contact &c);

private slots:
  void addContact();
  void start();
  void delCon();

public slots:
  void slotNewMessage(int id, const QString msg);

};

#endif // OVERVIEW_H
