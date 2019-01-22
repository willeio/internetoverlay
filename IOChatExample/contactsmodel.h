#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QObject>
#include <QAbstractTableModel>

#include "contact.h"


class ContactsModel : public QAbstractTableModel
{
public:
  enum Columns
  {
    Name = 0,
    PublicKey,
    PartnerId,
    Count,
  };

  ContactsModel(QObject* parent);
  void setContactList(const QList<Contact> &contactList);
  const QList<Contact> getContacts() const { return m_contactList; }
  const Contact &getContact(int row) const;

  QVariant data(const QModelIndex &index, int role) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  void addContact(const Contact &contact);

private:
  QList<Contact> m_contactList;
};

#endif // CONTACTSMODEL_H
