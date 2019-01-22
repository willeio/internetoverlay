#include "contactsmodel.h"

#include <assert.h>


ContactsModel::ContactsModel(QObject *parent)
  : QAbstractTableModel(parent)
{

}


void ContactsModel::addContact(const Contact &contact)
{
  beginInsertRows(QModelIndex(), m_contactList.count(), m_contactList.count());
  m_contactList.append(contact);
  endInsertRows();
}


QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  const Contact &c = m_contactList.at(index.row());

  switch (index.column())
  {
    case Name:
      return c.name;

    case PublicKey:
      return c.publicKey;

    case PartnerId:
      return c.partnerid;

    default:
      return QVariant();
  }

  return QVariant(); // sec
}


const Contact &ContactsModel::getContact(int row) const
{
  //assert(row > 0 && row < rowCount());

  return m_contactList.at(row);
}


int ContactsModel::rowCount(const QModelIndex &parent) const
{
  (void)parent;
  return m_contactList.count();
}


int ContactsModel::columnCount(const QModelIndex &parent) const
{
  (void)parent;
  return Count;
}


void ContactsModel::setContactList(const QList<Contact> &contactList)
{
  m_contactList = contactList;
}
