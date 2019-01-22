#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class IO;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;


signals:
  void go();

private slots:
  void slotSend();
};

#endif // MAINWINDOW_H
