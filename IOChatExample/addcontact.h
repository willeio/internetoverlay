#ifndef ADDCONTACT_H
#define ADDCONTACT_H

#include <QDialog>

#include "contact.h"


namespace Ui {
class AddContact;
}

class AddContact : public QDialog
{
  Q_OBJECT

public:
  explicit AddContact(QWidget *parent = 0);
  ~AddContact();

  bool getNewContact(Contact &contact);

private:
  Ui::AddContact *ui;
  Contact m_contact;

private slots:
  void slotAdd();
};

#endif // ADDCONTACT_H
