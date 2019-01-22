#include "overview.h"
#include "ui_overview.h"

#include "conversation.h"
#include "contactsmodel.h"
#include "addcontact.h"
#include <QTimer>

#include "message.h"

#include "worker.h"
#include <stdlib.h>
#include <stdio.h> // fopen

#include <IOClient/api.h>

#include <iostream>


#include <QThread>
#include <QMessageBox>




Overview::Overview(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Overview)
  , m_model(new ContactsModel(this))
{
  ui->setupUi(this);

  ui->contactsView->setModel(m_model);
  ui->contactsView->setColumnHidden(ContactsModel::PublicKey, true);
  ui->contactsView->setColumnHidden(ContactsModel::PartnerId, true);

  connect(ui->pbAdd, &QPushButton::clicked,
          this, &Overview::addContact);

  connect(ui->start, &QPushButton::clicked,
          this, &Overview::start);











  w = new Worker();
  QThread* t = new QThread();

  w->moveToThread(t);


  connect(w, &Worker::signalError,
          this, [this]() { QMessageBox::critical(this, "", "Error - abort!"); exit(-1); },
  Qt::QueuedConnection);




  connect(w, &Worker::signalNewMessage,
          this, &Overview::slotNewMessage);








  m_io = new_IO();
  io_init();

  w->setIO(m_io);




  char prv_key[] =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIIEowIBAAKCAQEAzEWkZs7v2vg5lk7BnbMl7/Y5zh1N4lsfdoCfs3CXzI578mzv\n"
      "WCM6Wwi8Gm5zgTmff2W4cxsZg5a03GztcjJWe6U5INegUl8N0Q/ck+ulQIFqi47Q\n"
      "/gOPvI38qF/evPHBhSIEoVSYv9V3hsZT6p5KPGBkhaWNzYLrXgSS5AhTLDTMk7w7\n"
      "SPSOLNgcsUR3K3wd2DUMjvC+olCz/IMMCuTSXnc7RsR2efQ0p3XDVior4saMjw9n\n"
      "qL8LPqT6IcHYmZCNhdWWz0YS+IMPIzbJApVmg/kPsiaa0ys508gNHLLGuiCKIOat\n"
      "AzXdm+ZI+FIgUrO7cSKKNyT6zSi1CufGTz4fKQIDAQABAoIBAFMbFkq5cD4O9Sbg\n"
      "vcbSBuI8s8HWjERGPGOy6sZPTYLsiSLVDP83vdshj53VR+rQLcLf+I3j/ZpaqxRu\n"
      "wJ/QOP0u+DITZ+d7QpzGrkAEBARdYqKKpR5jBox4280IzA7k8I2gjG+ygj6xs2JC\n"
      "uAthAOTVdrAk7a8pz2Y9yCE7/dKiom3XtDiX39fp4GxlGQQmhQMGIpCt6TSSjyn9\n"
      "uzv0N2iqIP8Cb8nkL8yzcE/MgDGAABQE8dESjwp15lbv/6ItZ6PVLIX+OW0IGroV\n"
      "NcPEasc11Ex1+vkvA0fTDwRTOIKo7WC89N4irL2uMwH4Nz7usxvtvLBcnkDHZvXD\n"
      "i+B5voECgYEA56k2/t3nKINv1ACbfyQhdU3nlqFFA/sDelo2ensqqgfx8ENWwu0d\n"
      "NoM8K1PtHMSh4R8ceUSHlDR8Ah9sz7iI8GrAw4QsmfZsCLVeQdNoGCB4BmZBqPiW\n"
      "H4cy9MxLrM/KQVcC5HXj/gbAITNseztR2WHPfrzYU/Ng7/vmRDsEaJECgYEA4bvF\n"
      "NYCbO7S9Vrv0rlEngrSw5VgKDbgxUQ5QoLcgbWbCzWXiypmK574hwOrtpYgyk5F3\n"
      "iWA1lQFWRQ059oxky9e9TlA/jCkMCK8AQAxe1YzQDN/987cE7zg2+YnEskyQXzh0\n"
      "kzm00O/rpdyU1BkVEnQ+zxWgatkmZK3RA+rd2RkCgYBJFCZXLCU5sIzccfyblQhA\n"
      "+vxtlh4xbUPhmwfIh4Yj8TGvO27pOueWRLv8ZxNLmfP/ElNnJeknJY9hFAy9UldQ\n"
      "kDyS8OUpBIV8EcrUNR6HTebPDf70kMa612mJs9DDiSEij5JIA7CSeAqv7orwLPxa\n"
      "+PH95xDPGt2flvVC3OM9IQKBgDWS3PEfvrfi43TsUtv0Qs5QmBM2W9x5tKqqHRSn\n"
      "obiVRPJ+ld7DpiXuCBDDMPeyZV6Ejbw66zyb/p+Qzgrnsxx17WXiwdJAA60j64lH\n"
      "5lIEg3QqzRjEfRVRYCrFObufVDxlJ7WxYmqcJm9M9L2J3mhs0mNPnKz/MI+AeTTW\n"
      "zyQxAoGBAL6XbdZVeqAoye/y9U8OUQMTZiXQ1ZFGVl1OI6Ja0FLyand4dV4HAdpI\n"
      "DpPHOKaRhJL3kjCxsObjongp1OZQN6UohDyvoPTfBEmirgzN76E1quy6LpqFwdR+\n"
      "5y7wmLNIIBLYYoIjxjCT374zKUvxTV5UWzwlINASfXPWH957MLWZ\n"
      "-----END RSA PRIVATE KEY-----";




  if (io_set_prv_key(m_io, prv_key) < 0)
  {
    QMessageBox::critical(this, "", "io_set_prv_key failed!");
    exit(-1);
  }


  if (io_connect(m_io) < 1) // no nodes? no connection.
  {
    QMessageBox::critical(this, "", "io_connect failed!");
    exit(-2);
  }


  t->start();
  QTimer::singleShot(0, w, &Worker::slotTwerk);
}


Overview::~Overview()
{
  delete ui;
}


void Overview::addContact()
{
  AddContact* add = new AddContact(this);

  Contact contact;
  if (!add->getNewContact(contact))
    return;

  m_model->addContact(contact);
}


void Overview::start()
{
  QModelIndexList l = ui->contactsView->selectionModel()->selectedRows();

  if (l.count() != 1)
    return;

  const Contact &c = m_model->getContact(l.at(0).row());

  openCon(c);
}


void Overview::delCon()
{
  QObject* del = QObject::sender();

  for (auto i = m_conList.begin(); i != m_conList.end(); ++i)
  {
    if ((*i).second == del)
    {
      m_conList.erase(i);
      return;
    }
  }
}


void Overview::slotNewMessage(int id, const QString msg)
{
  for (auto i = m_conList.begin(); i != m_conList.end(); ++i)
  {
    if ((*i).first.partnerid == (uint32_t)id)
    {
      (*i).second->addMessage(msg);
      return;
    }
  }

  // if it comes here, conversation wnd not found!
  // look if contact exists

  const QList<Contact> &cl = m_model->getContacts();
  for (auto i = cl.constBegin(); i != cl.constEnd(); ++i)
  {
    if ((*i).partnerid == (uint32_t)id)
    {
      openCon((*i));
      return;
    }
  }


  // still not returned? then this is a new contact - say we have a new contact

  // TODO: !!!!

  QMessageBox::information(this, "", "New contact request!");


}


void Overview::openCon(const Contact &c)
{
  Conversation* x = new Conversation(this, c, m_io);
  x->setAttribute(Qt::WA_DeleteOnClose);

  connect(x, &Conversation::destroyed, this, &Overview::delCon);

  m_conList.append(QPair<Contact, Conversation*> (c, x));

  x->show();
}







