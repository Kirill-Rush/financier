#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "additionalwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QProcess>
#include <QFile>
#include <iostream>
#include <regex>
#include <PDF/PDFNet.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QRegularExpression>
#include <QMainWindow>
#include <PDF/ElementReader.h>
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QDate>
#include "export_to_xls.h"
#include <thread>

using namespace pdftron;
using namespace PDF;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QString error_info = "";
    QString selected_category;
    QMap<QString, double> sum_of_category;
    QString begin_date;
    QString end_date;
    QString begin_day_of_week;
    QString end_day_of_week;
    QString date;
    bool second_thread_is_on = false;
    friend void value_progressbar_changed(int);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void send_text(QString str);
    void value_progressbar_changed(int);

public slots:
    void recieve_text(QString str);

private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_calculate_clicked();

    void on_pushButton_open_read_clicked();

    void DumpAllText(ElementReader& reader, int *number_of_string);

    void on_pushButton_export_clicked();

    QString export_xls(QMainWindow *ui, QString date, QString begin_date, QString begin_day_of_week, \
                    QString end_date, QString end_day_of_week, QMap<QString, double> sum_of_category);

    void on_pushButton_error_info_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
