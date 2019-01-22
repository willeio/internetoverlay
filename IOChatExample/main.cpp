#include "overview.h"

#include <IOClient/api.h>
#include <IOLib/io.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> // srand
#include <QApplication>


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  Overview w;

  w.show();

  return app.exec();
}
