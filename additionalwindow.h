#ifndef ADDITIONALWINDOW_H
#define ADDITIONALWINDOW_H

#include <QDialog>
#include <QMap>
#include "mainwindow.h"

namespace Ui {
class AdditionalWindow;
}

static QString new_point_add;

class AdditionalWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdditionalWindow(QWidget *parent = nullptr);
    QMap<QString, QVector<QString>> map_of_points;
    QMap<QString, QVector<QString>>::iterator it;
    QVector<QString>::iterator jt;
//    static QString new_point_add;
    /*void setQString (QString x)
    {
        new_point_add = x;
    }*/
    ~AdditionalWindow();

signals:
    void send_text_back(QString str);

public slots:
    void recieve_text(QString str);

private slots:
    void on_supermarkets_clicked();

    void on_cafedelivery_clicked();

    void on_drugs_clicked();

    void on_transport_clicked();

    void on_communicationandinternet_clicked();

    void on_apartments_clicked();

    void on_investment_clicked();

    void on_otherspends_clicked();

    void on_salary_clicked();

    void on_otherincomes_clicked();

    void on_pushButton_ok_clicked();

    void on_pushButton_get_more_info_clicked();

    void on_pushButton_stop_pocess_clicked();

private:
    Ui::AdditionalWindow *ui;
};

#endif // ADDITIONALWINDOW_H
