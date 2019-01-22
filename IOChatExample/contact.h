#pragma once

#include <QObject>

#include <stdint.h>


struct Contact
{
  QString name;
  QString publicKey;
  uint32_t partnerid;
};
