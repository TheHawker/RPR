#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  //  QLocale Rus(QLocale::Russia, QLocale::Russian);

    ui->setupUi(this);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

 //   QTextCodec::setCodecForLocale;
    date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString str = date.toString();

    ID=0;
    QString nullName = "Выбрать Фамилию";


    db = QSqlDatabase::addDatabase("QMYSQL", "my_sql_db");
    db.setHostName ("192.168.12.224");
    db.setDatabaseName ("sakila");
    db.setUserName ("root");
    db.setPassword ("134679");
    db.setPort(3306);


    if(db.open())
       {
   //        qDebug()<<db.lastError().text;
         ui->label_6->setText("success!");

       }
    else
    {
         ui->label->setText("breaking bad news");
    }

//обзываем базу для работы с ней
    quaery = QSqlQuery(db);

//подготовка таблиц
    quaery.prepare( "CREATE TABLE IF NOT EXISTS RPR (id INTEGER UNIQUE PRIMARY KEY, firstname VARCHAR(30), lastname VARCHAR(30), incdate DATE, inctime TIME, outtime TIME)" );

//Проверка подготовки таблиц
    if(quaery.exec())
        {
            ui->textEdit->append("Table created");
        }
        else
        {
            ui->textEdit->append("Error");
            ui->textEdit->append("MySQL error:" + quaery.lastError().text());
            ui->textEdit->append("MySQL error code:"+ QString::number(quaery.lastError().number()));
        }
//заполнение стоковых полей
       ui->label->setText("Сегодняшняя дата:");
       ui->label_2->setText(str);
       ui->timeinc->setTime(time);
       ui->timeout->setTime(time);
       ui->table->setColumnCount(5);
       ui->table->setRowCount(quaery.size());
       ui->comboName->addItem(nullName);
       inctime=ui->timeinc->time();
       tableUpdate();
/*Вывод всей таблицы
       quaery.exec("SELECT * FROM rpr");
       ui->table->setColumnCount(5);
       ui->table->setRowCount(quaery.size());
       int index=0;
       while (quaery.next())
       {
       ui->table->setItem(index,0,new QTableWidgetItem(quaery.value(0).toString()));
       ui->table->setItem(index,1,new QTableWidgetItem(quaery.value(1).toString()));
       ui->table->setItem(index,2,new QTableWidgetItem(quaery.value(2).toString()));
       ui->table->setItem(index,3,new QTableWidgetItem(quaery.value(3).toString()));
       ui->table->setItem(index,4,new QTableWidgetItem(quaery.value(4).toString()));
       index++;

       bool exist=false;
       //Поиск и добавление из таблицы имен/новых имен
       for (int i=1; i<(sizeof(firstnames)/sizeof(firstnames[0])); i++)
       {
            //маркер повторяющихся имен
           if (firstnames[i]==quaery.value(1).toString())
           {
               exist=true;
           }
           //найдено новое имя + добавить в список и матрицу
           if (firstnames[i]=="" && !exist)
           {
               firstnames[i]=quaery.value(1).toString();
               ui->comboName->addItem(firstnames[i]);
               exist=true;

           }
           //Принудительный выход из цикла
           if (firstnames[i+1]=="" && exist)
           {
              i=(sizeof(firstnames)/sizeof(firstnames[0]));
           }
        //   qDebug()<<firstnames[i-1];
       }
       ID=quaery.value(0).toInt();
       }
       ui->table->show();
*/
 //  ui->inctime->setTime(Qt::LocalTime); //текущее время для времени ухода

      // ----------События---------------------------
      connect(ui->pushButton, SIGNAL (clicked()),this,SLOT(incdateclick()));
      connect(ui->pushButton_2, SIGNAL (clicked()),this,SLOT(outdateclick()));
      connect(ui->pushButton_3, SIGNAL (clicked()),this,SLOT(showtables()));
      connect(ui->pushButton_4, SIGNAL (clicked()),this,SLOT(showname()));
      connect(ui->debugButton, SIGNAL (clicked()),this,SLOT(debugfunc()));
      connect(ui->comboName, SIGNAL (currentIndexChanged(int)), this, SLOT (nameChanged()));
      //--------------------------------------------------------

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::incdateclick()
{
//обновляем данные о времени
    inctime=ui->timeinc->time();
 //просто чтоб знать что нажато
 ui->textEdit->setText("clicked-1");
 if (ui->comboName->currentIndex()==0)
 {
     textName=ui->Name->toPlainText();
 }
// Проверка заполнения фамилии
 if (textName!="") //textName not null
 {
     quaery.exec("SELECT * FROM RPR WHERE incdate='"+date.toString("yyyy-MM-dd")+"' AND firstname='"+textName+"'");
    // if(quaery.value(0).toInt())
     if (quaery.next())
         {
         QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "Предупреждение", "Данные о приходе сотрудника на текущую дату содержатся в таблице. Изменить?",
           QMessageBox::Yes|QMessageBox::No);
           if (reply==QMessageBox::Yes)
                {
               qDebug()<<inctime.toString();
               quaery.exec("UPDATE RPR SET inctime ='"+inctime.toString()+"' WHERE incdate='"+date.toString("yyyy-MM-dd")+"' AND firstname='"+textName+"'");
                }
            }
     else
     {
         quaery.prepare("INSERT INTO rpr (id, firstname, lastname, incdate, inctime) "
         "VALUES (:id, :firstname, :lastname, :incdate, :inctime)");
         quaery.bindValue(":id", ++ID);
         quaery.bindValue(":firstname", textName);
         quaery.bindValue(":lastname", "");
         quaery.bindValue(":incdate", QDate::currentDate());
         quaery.bindValue(":inctime", inctime);
         quaery.exec();
         tableUpdate();
     }
 }
 else ui->textEdit->setText("Введите имя");
/*
   */
}

//Событие записи времени ухода
void MainWindow::outdateclick()
{
ui->textEdit->setText("clicked-2");
outtime=ui->timeout->time();
//просто чтоб знать что нажато
ui->textEdit->setText("clicked-1");
if (ui->comboName->currentIndex()==0)
{
 textName=ui->Name->toPlainText();
}
// Проверка заполнения фамилии
if (textName!="") //textName not null
{
 quaery.exec("SELECT * FROM RPR WHERE incdate='"+date.toString("yyyy-MM-dd")+"' AND firstname='"+textName+"'");
// if(quaery.value(0).toInt())
 if (quaery.next())
     {
     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Предупреждение", "Данные о приходе сотрудника на текущую дату содержатся в таблице. Изменить?",
       QMessageBox::Yes|QMessageBox::No);
       if (reply==QMessageBox::Yes)
            {
           qDebug()<<inctime.toString();
           quaery.exec("UPDATE RPR SET outtime ='"+outtime.toString()+"' WHERE incdate='"+date.toString("yyyy-MM-dd")+"' AND firstname='"+textName+"'");
            }
        }

}
tableUpdate();
}

//Отображение даных за сегодняшний день
void MainWindow::showtables()
{

    quaery.exec("SELECT * FROM RPR WHERE incdate='"+date.toString("yyyy-MM-dd")+"'");

//()<<date.toString("yyyy-MM-dd");
//qDebug()<<date;
     ui->table->setColumnCount(5);
     ui->table->setRowCount(quaery.size());


     int index=0;
     while (quaery.next())
     {
     ui->table->setItem(index,0,new QTableWidgetItem(quaery.value(1).toString()));
     ui->table->setItem(index,1,new QTableWidgetItem(quaery.value(2).toString()));
     ui->table->setItem(index,2,new QTableWidgetItem(quaery.value(3).toString()));
     ui->table->setItem(index,3,new QTableWidgetItem(quaery.value(4).toString()));
     ui->table->setItem(index,4,new QTableWidgetItem(quaery.value(5).toString()));
     index++;
     }

     ui->table->show();
}

//данные указанного сотрудника
void MainWindow::showname()
{

  //  textName = ui->Name->toPlainText();
    quaery.exec("SELECT * FROM RPR WHERE firstname='"+textName+"'");
    ui->table->setColumnCount(5);
    ui->table->setRowCount(quaery.size());
    int index = 0;
     while (quaery.next())
     {
         ui->table->setItem(index,0,new QTableWidgetItem(quaery.value(1).toString()));
         ui->table->setItem(index,1,new QTableWidgetItem(quaery.value(2).toString()));
         ui->table->setItem(index,2,new QTableWidgetItem(quaery.value(3).toString()));
         ui->table->setItem(index,3,new QTableWidgetItem(quaery.value(4).toString()));
         ui->table->setItem(index,4,new QTableWidgetItem(quaery.value(5).toString()));
         index++;
     }
}

//отладочные функции
void MainWindow::debugfunc()
{
    quaery.exec("SELECT * FROM RPR WHERE incdate='"+date.toString("yyyy-MM-dd")+"' AND firstname='"+textName+"'");
   // if(quaery.value(0).toInt())
    if (quaery.next())
    {
    qDebug()<<"False"<<quaery.value(1).toString();
 //   qDebug()<<ui->comboName->currentText();
       }
              else
              qDebug()<<"true"<<quaery.value(1).toString();

}

//Событие туллбокса
void MainWindow::nameChanged()
{
    if (ui->comboName->currentIndex()!=0){
    textName = ui->comboName->currentText();
    }
    else
    {
        textName = ui->Name->toPlainText();
    }
    qDebug()<<textName;
}


void MainWindow::tableUpdate()
{
//Вывод всей таблицы
    quaery.exec("SELECT * FROM rpr");
    ui->table->setColumnCount(5);
    ui->table->setRowCount(quaery.size());
    int index=0;
    while (quaery.next())
    {
    ui->table->setItem(index,0,new QTableWidgetItem(quaery.value(0).toString()));
    ui->table->setItem(index,1,new QTableWidgetItem(quaery.value(1).toString()));
    ui->table->setItem(index,2,new QTableWidgetItem(quaery.value(3).toString()));
    ui->table->setItem(index,3,new QTableWidgetItem(quaery.value(4).toString()));
    ui->table->setItem(index,4,new QTableWidgetItem(quaery.value(5).toString()));
    index++;

    bool exist=false;
    //Поиск и добавление из таблицы имен/новых имен
    for (int i=1; i<(sizeof(firstnames)/sizeof(firstnames[0])); i++)
    {
         //маркер повторяющихся имен
        if (firstnames[i]==quaery.value(1).toString())
        {
            exist=true;
        }
        //найдено новое имя + добавить в список и матрицу
        if (firstnames[i]=="" && !exist)
        {
            firstnames[i]=quaery.value(1).toString();
            ui->comboName->addItem(firstnames[i]);
            exist=true;

        }
        //Принудительный выход из цикла
        if (firstnames[i+1]=="" && exist)
        {
           i=(sizeof(firstnames)/sizeof(firstnames[0]));
        }
     //   qDebug()<<firstnames[i-1];
    }
    ID=quaery.value(0).toInt();
    }
    ui->table->show();
}
