#include "worker.h"
#include "message.h"

#include <IOClient/api.h>

#include <iostream>


Worker::Worker(QObject *parent)
  : QObject(parent)
{

}


void Worker::setIO(IO* io)
{
  m_io = io;
}


void Worker::work()
{
  while (1)
  {
    struct client_blob cb;

    if (io_get(m_io, &cb) < 0)
      continue;

    union chat_msg cm;
    memcpy(cm.buffer, cb.blob, sizeof(cm.buffer) /* cm is smaller - only copy that */);

    if (cm.data.magic[0] != 'C' ||
        cm.data.magic[1] != 'M')
    {
      std::cout << "worker: invalid magic (" << cm.data.magic[0] << cm.data.magic[1] << ")" << std::endl;
      continue;
    }

    if (alreadyRecvdMsgs.contains(cm.data.msg_id)) // already received, dont show again
      continue;

    alreadyRecvdMsgs.append(cm.data.msg_id);

    QString msg = QString::fromLatin1((const char*)cm.data.msg, 128); // TODO: non-const => const ?!

    emit signalNewMessage(cm.data.partner_id, msg);
  }
}


void Worker::slotTwerk()
{
  work();
  emit signalError();
}










