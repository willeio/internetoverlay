#pragma once

#include <QObject>
#include <QList>


class IO;


class Worker : public QObject
{
  Q_OBJECT
public:
  explicit Worker(QObject *parent = 0);
  void work();
  void setIO(IO* io);

private:
  QList<int> alreadyRecvdMsgs;
  IO* m_io;

signals:
  void signalNewMessage(int partnerid, const QString &msg);
  void signalError();

public slots:
  void slotTwerk();

};
