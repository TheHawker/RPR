#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QtWidgets/QMainWindow>
#endif
#include <QtSql/qsql.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/QSqlQuery>
#include <QDate>
#include <QtSql/QSqlError>
#include <QString>
#include <QMessageBox>
#include <QtDebug>
#include <QCoreApplication>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase db;
    QSqlQuery quaery;
    QString textName;
    QDate date;
    QTime inctime;
    QTime outtime;
    QString firstnames[100];
    int ID;
    explicit MainWindow(QWidget *parent = 0);
    void tableUpdate();
    ~MainWindow();
public slots:
    void incdateclick();
    void outdateclick();
    void showtables();
    void showname();
    void debugfunc();
    void nameChanged();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
