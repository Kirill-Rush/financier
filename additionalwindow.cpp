#include "additionalwindow.h"
#include "ui_additionalwindow.h"

QString name_of_selected_category;

AdditionalWindow::AdditionalWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalWindow)
{
    ui->setupUi(this);
    setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint );
    QFont font = ui->label_6->font();
    font.setPixelSize(18);
    ui->label_6->setFont(font);
    ui->textEdit->setVisible(false); // скрывает текстовое поле (нужно для отладки программы)
}

AdditionalWindow::~AdditionalWindow()
{
    delete ui;
}

void AdditionalWindow::recieve_text(QString str)
{
    //ui->textEdit->append(str);
    ui->label_6->setStyleSheet("color: red");
    ui->label_6->setText(str);
    return;
}

void AdditionalWindow::on_supermarkets_clicked()
{
    name_of_selected_category = "Супермаркеты";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}

void AdditionalWindow::on_cafedelivery_clicked()
{
    name_of_selected_category = "Кафе/рестораны";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_drugs_clicked()
{
    name_of_selected_category = "Здоровье";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_transport_clicked()
{
    name_of_selected_category = "Транспорт";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_communicationandinternet_clicked()
{
    name_of_selected_category = "Связь и интернет";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_apartments_clicked()
{
    name_of_selected_category = "Жилье";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_investment_clicked()
{
    name_of_selected_category = "Инвестирование";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_otherspends_clicked()
{
    name_of_selected_category = "Прочие расходы";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_salary_clicked()
{
    name_of_selected_category = "Зар. плата";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_otherincomes_clicked()
{
    name_of_selected_category = "Прочие доходы";
    //ui->textEdit->append(name_of_selected_category);
    ui->pushButton_ok->setEnabled(true);
    return;
}


void AdditionalWindow::on_pushButton_ok_clicked()
{
    emit(send_text_back(name_of_selected_category));
    this->close();
}


void AdditionalWindow::on_pushButton_get_more_info_clicked()
{
    name_of_selected_category.clear();
    emit(send_text_back(name_of_selected_category));
    this->close();
}


void AdditionalWindow::on_pushButton_stop_pocess_clicked()
{
    /* QMessageBox::StandardButton stop_process = QMessageBox::question(this, "Подтверждение действия",\
                                                                     "Вы точно хотите прервать процесс? Расчетные данные будут потеряны.",\
                                                                     QMessageBox::Yes | QMessageBox::No);
    if(stop_process == QMessageBox::Yes)
    {
        name_of_selected_category = "stop_process";
        emit(send_text_back(name_of_selected_category));
        this->close();
    } */

    QMessageBox exit_window;
    exit_window.setWindowTitle("Подтверждение действия");
    exit_window.setText("Вы точно хотите прервать процесс? Расчетные данные будут потеряны.");
    QAbstractButton *button_yes = new QPushButton("Да");
    QAbstractButton *button_no = new QPushButton("Нет");
    exit_window.addButton(button_yes, QMessageBox::NoRole);
    exit_window.addButton(button_no, QMessageBox::NoRole);
    //exit_window.setFixedSize();
    exit_window.exec();
    if(exit_window.clickedButton() == button_yes)
    {
        name_of_selected_category = "stop_process";
        emit(send_text_back(name_of_selected_category));
        this->close();
    }
}

