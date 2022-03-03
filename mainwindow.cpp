#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setFixedSize(950, 710);
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->label_res_of_export->setVisible(false);
    ui->pushButton_error_info->setVisible(false);
    ui->textEdit->setReadOnly(true);
    QObject::connect(this, SIGNAL(value_progressbar_changed(int)), ui->progressBar, SLOT(setValue(int)));
    QTableWidgetItem *item0 = new QTableWidgetItem("Дата операции");
    QTableWidgetItem *item1 = new QTableWidgetItem("Категория");
    QTableWidgetItem *item2 = new QTableWidgetItem("Описание операции");
    QTableWidgetItem *item3 = new QTableWidgetItem("Сумма списания");
    QTableWidgetItem *item4 = new QTableWidgetItem("Баланс после\nсовершения операции");
    ui->tableWidget->insertColumn(0);
    ui->tableWidget->insertColumn(1);
    ui->tableWidget->insertColumn(2);
    ui->tableWidget->insertColumn(3);
    ui->tableWidget->insertColumn(4);
    ui->tableWidget->setHorizontalHeaderItem(0, item0);
    ui->tableWidget->setHorizontalHeaderItem(1, item1);
    ui->tableWidget->setHorizontalHeaderItem(2, item2);
    ui->tableWidget->setHorizontalHeaderItem(3, item3);
    ui->tableWidget->setHorizontalHeaderItem(4, item4);
    ui->tableWidget->horizontalHeader()->resizeSection(0, 100);
    ui->tableWidget->horizontalHeader()->resizeSection(1, 170);
    ui->tableWidget->horizontalHeader()->resizeSection(2, 230);
    ui->tableWidget->horizontalHeader()->resizeSection(3, 170);
    ui->tableWidget->horizontalHeader()->resizeSection(4, 170);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // запрет на редактирование ячеек tableWidget
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// считывание всего текста с PDF страницы
void MainWindow::DumpAllText(ElementReader& reader, int *number_of_string)
{
    Element element;
    bool begin_of_copy = false; // начало копирования информации в строку
    int number_of_column = 0;
    int number_of_text = 0;
    while ((element = reader.Next()) != 0)
    {
        switch (element.GetType())
        {
        case Element::e_text_begin:
            //ui->textEdit->append("\nText Block Begin\n");
            break;
        case Element::e_text_end:
            //ui->textEdit->append("\n--> Text Block End\n");
            break;
        case Element::e_text:
            {
                Rect bbox;
                element.GetBBox(bbox);
                UString arr = element.GetTextString();
                QString stroka = QString::fromUtf8(arr.ConvertToUtf8().c_str());
                //ui->textEdit->append(stroka);

                // блок нахождения периода отчета (дата "с" "по")
                if(stroka.contains("Итого по операциям с "))
                {
                    date.clear();
                    int size_of_stroka = stroka.count();
                    for(int i = 0; i < size_of_stroka; i++)
                        if(stroka.at(i).isDigit() || (stroka.at(i) == '.'))
                                date.append(stroka.at(i));
                    begin_date = date.left(10);
                    end_date = date.right(10);
                    ui->textEdit->append("Даты отчета:\n" + begin_date + " - " + end_date);
                    //ui->textEdit->append(begin_date);
                    //ui->textEdit->append(end_date);

                    QDate begin_date_with_letters(begin_date.right(4).toInt(), begin_date.mid(3, 2).toInt(), begin_date.left(2).toInt());
                    QDate end_date_with_letters(end_date.right(4).toInt(), end_date.mid(3, 2).toInt(), end_date.left(2).toInt());
                    //ui->textEdit->append(begin_date_with_letters.toString());
                    //ui->textEdit->append(end_date_with_letters.toString());

                    begin_day_of_week = begin_date_with_letters.toString().left(3);
                    end_day_of_week = end_date_with_letters.toString().left(3);
                    //ui->textEdit->append(begin_day_of_week);
                    //ui->textEdit->append(end_day_of_week);
                }

                // начало считывания данных для отображения в QTableWidget
                if(stroka == "В валюте счёта")
                {
                    begin_of_copy = true;
                    //ui->textEdit->append("ПОПАЛСЯ, ГОЛУБЧИК");
                    break;
                }

                // прекращение считывания данных при обнаружении конца страницы
                if(stroka.contains("Продолжение на следующей странице") || stroka.contains("Реквизиты для перевода"))
                {
                    begin_of_copy = false;
                    number_of_column = 0;
                    //ui->textEdit->append("ПОПАЛСЯ ГОЛУБЧИК");
                    break;
                }

                if(begin_of_copy == true)
                {
                    //static const regex pattern("[^,]+");
                    static const regex pattern("[\(]?[A-Za-zА-Яа-я ]+[)]?");
                    QTableWidgetItem *item = new QTableWidgetItem;
                    if(number_of_text == 1 || number_of_text == 2 || number_of_text == 3 || stroka.isEmpty() || stroka.contains(" KZT") || stroka.contains(" $"))
                    {
                        number_of_text++;
                        break;
                    }
                    if((number_of_column == 3) && stroka.contains("+"))
                    {
                        QTableWidgetItem *item1 = new QTableWidgetItem;
                        QTableWidgetItem *item2 = new QTableWidgetItem;
                        QString str1 = ui->tableWidget->item(*number_of_string, 2)->text() + " (доход)";
                        QString str2 = ui->tableWidget->item(*number_of_string, 1)->text() + " (доход)";
                        item1->setText(str1);
                        item2->setText(str2);
                        ui->tableWidget->setItem(*number_of_string, number_of_column - 1, item1);
                        ui->tableWidget->setItem(*number_of_string, number_of_column - 2, item2);
                    }
                    //ui->textEdit->append(stroka);
                    item->setText(stroka);

                    // проверка строки на удовлетворение условиям regex (если в строке есть буквы, то строка относится к предыдущему столбцу)
                    if ((number_of_column == 3) && (regex_match(stroka.toStdString(), pattern))) //(!stroka.toStdString().find_first_not_of("-+0123456789,/. "))) (НО НЕ ВО ВСЕХ СЛУЧАЯХ ПРАВИЛЬНЫЙ)
                    {
                        //ui->textEdit->append("ПРОШЕЛ ПРОВЕРКУ");
                        stroka = ui->tableWidget->item(*number_of_string, 2)->text() + " " + stroka;
                        item->setText(stroka);
                        ui->tableWidget->setItem(*number_of_string, 2, item);
                    }
                    else
                    {
                        ui->tableWidget->setItem(*number_of_string, number_of_column, item);
                        number_of_column++;
                        number_of_text++;
                    }

                    if(number_of_column == 5)
                    {
                        *number_of_string = *number_of_string + 1;
                        number_of_column = 0;
                        number_of_text = 0;
                        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                    }
                }
            }
            break;
        default: break;
        }
    }
    return;
}

void MainWindow::on_pushButton_exit_clicked()
{
    /*QMessageBox::StandardButton exit = QMessageBox::question(this, "Подтверждение действия",
                                                             "Выйти из программы?",
                                                             QMessageBox::Yes | QMessageBox::No);
    if (exit == QMessageBox::Yes)
        QApplication::quit(); */

    QMessageBox exit_window;
    exit_window.setWindowTitle("Подтверждение действия");
    exit_window.setText("Выйти из программы?");
    QAbstractButton *button_yes = new QPushButton("Да");
    QAbstractButton *button_no = new QPushButton("Нет");
    exit_window.addButton(button_yes, QMessageBox::NoRole);
    exit_window.addButton(button_no, QMessageBox::NoRole);
    //exit_window.setFixedSize();
    exit_window.exec();
    if(exit_window.clickedButton() == button_yes)
        QApplication::quit();
}


void MainWindow::on_pushButton_calculate_clicked()
{
    QVector<QString> vector_of_unknown_operations {"Прочие операции", "Прочие расходы",\
                                                 "Неизвестная категория(+)", "Неизвестная категория(-)"};
    QJsonParseError parseerror;
    QJsonArray subobj; // Элемент массива конкретного ключа в QJson
    QFile file("JSON\\categories.json");

    // Открываю файл JSON (с категориями) на чтение
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!file.isOpen())
    {
        QMessageBox::critical(this, "Ошибка!", "Файл с категориями не открыт!");
        return;
    }
    QByteArray value = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(value, &parseerror);
    QJsonObject obj = doc.object();
    QVector<QString> vector_of_categories = obj.keys().toVector();
    int count_of_categories = vector_of_categories.size();
    sum_of_category.clear();
//    for(QString object_qjson = obj.begin().key(); !obj.keys().empty(); obj)
//        objects_of_qjson.push_back(object_qjson);
    for(int i = 0; ui->tableWidget->item(i, 3); i++)
    {
        int flag = 0; // переход на следующую строку в случае подбора категории, вызов additionalwindow в противном случае
        int column_with_category_to_compare;
        (vector_of_unknown_operations.contains(ui->tableWidget->item(i, 1)->text()))\
                ? column_with_category_to_compare = 2 : column_with_category_to_compare = 1;
        QJsonValue current_category_in_table = ui->tableWidget->item(i, column_with_category_to_compare)->text();
        QJsonValue current_category_in_table_addition = ui->tableWidget->item(i, 2)->text(); // вызывается пользователем при необходимости получить больше инфы (в additionalwindow)

        // поиск категорий в JSON'е
        for(int j = 0; j < count_of_categories; j++)
        {
            subobj = obj[vector_of_categories[j]].toArray();
            if(subobj.contains(current_category_in_table) || subobj.contains(current_category_in_table_addition))
            {
                QTableWidgetItem *item;
                item = ui->tableWidget->item(i, 3);
                QString sum_of_operation_QStr = item->text().toUtf8();
                sum_of_operation_QStr = sum_of_operation_QStr.simplified(); // код пробела меняется со 160 на 32
                sum_of_operation_QStr.replace( " ", "" );
                string sum_of_operation = sum_of_operation_QStr.toStdString();
                sum_of_operation.replace(sum_of_operation.find(","), 1, ".");
                sum_of_category[vector_of_categories[j]] += stod(sum_of_operation);
                flag = 1;
            }
            if(flag)
                break;
        }
        if (!flag)
        {
            bool found;
            AdditionalWindow addwindow;
            QObject::connect(this, SIGNAL(send_text(QString)), &addwindow, SLOT(recieve_text(QString)));
            QObject::connect(&addwindow, SIGNAL(send_text_back(QString)), this, SLOT(recieve_text(QString)));
            emit send_text(ui->tableWidget->item(i, column_with_category_to_compare)->text());
            addwindow.setModal("true");
            addwindow.exec(); // запуск окна для добавления наименования в категорию

            if(selected_category == "stop_process")
            {
                selected_category.clear();
                return;
            }

            // В случае необходимости получить бОльшую информацию об операции, используется блок кода ниже
            selected_category.isEmpty() ? (found = false) : (found = true);
            while(!found)
            {
                emit send_text(ui->tableWidget->item(i, 2)->text()); // отправка current_category_in_table_addition в текстовом формате
                addwindow.setModal("true");
                addwindow.exec(); // запуск окна для добавления наименования в категорию
                if(!selected_category.isEmpty())
                {
                    found = true;
                    current_category_in_table = current_category_in_table_addition;
                }
            }

            subobj = obj[selected_category].toArray();
            subobj.append(current_category_in_table);
            //obj[selected_category].toArray().append(current_category_in_table);
            QJsonObject::iterator iter_obj;
            obj.remove(selected_category);
            obj.insert(selected_category, subobj);
            QTableWidgetItem *item;
            item = ui->tableWidget->item(i, 3);
            QString sum_of_operation_QStr = item->text().toUtf8();
            sum_of_operation_QStr = sum_of_operation_QStr.simplified(); // код пробела меняется со 160 на 32
            sum_of_operation_QStr.replace( " ", "" );
            string sum_of_operation = sum_of_operation_QStr.toStdString();
            sum_of_operation.replace(sum_of_operation.find(","), 1, ".");
            sum_of_category[selected_category] += stod(sum_of_operation);
            //obj.erase(obj.find(selected_category));
            //ui->textEdit->append(obj[selected_category].toArray().last().toString());
        }
    }
    ui->textEdit->clear();

    QString other_expenses;
    QString other_incomes;
    // выписка категорий с тратами в textEdit после подсчета
    for(int i = 0; i < count_of_categories; i++)
    {
        if(vector_of_categories[i].contains("Прочие расходы"))
        {
            other_expenses = vector_of_categories[i] + ": " + \
                    QString::number(sum_of_category[vector_of_categories[i]]) + " руб.";
            continue;
        }
        if(vector_of_categories[i].contains("Прочие доходы"))
        {
            other_incomes = vector_of_categories[i] + ": " + \
                    QString::number(sum_of_category[vector_of_categories[i]]) + " руб.";
            continue;
        }
        QString finish_count_to_textedit = vector_of_categories[i] + ": " + \
                QString::number(sum_of_category[vector_of_categories[i]]) + " руб.";
        ui->textEdit->append(finish_count_to_textedit);
    }
    ui->textEdit->append(other_expenses); // добавление строки с прочими расходами
    ui->textEdit->append(other_incomes); // добавление строки с прочими доходами

    // Открываю файл JSON (с категориями) на запись измененного текста
    QJsonDocument doc_to_write(obj);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc_to_write.toJson());
    file.close();
    if(!second_thread_is_on)
        ui->pushButton_export->setEnabled(true);
    return;
}

// считывание файла PDF
void MainWindow::on_pushButton_open_read_clicked()
{
    qDebug() << "Должно работать";
    PDFNet::Initialize();
    QString input_path = QFileDialog::getOpenFileName(0, "Open Dialog", "\\Qt", "*.pdf"); // C:\\Users\\Kirill\\Downloads !!!!!!!!!!
/*    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка!", "Файл не открыт!");
        this->close();
    } */
    qDebug() << "Должно работать";
    string input_path_in_string = input_path.toStdString();
    const char* filein = input_path_in_string.c_str();
    try
    {
        PDFDoc doc(filein);
        doc.InitSecurityHandler();
        // Extract all text content from the document (from all pages)
        ElementReader reader;
        int number_of_string = 0;
        // Read every page
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->textEdit->clear();
        for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next())
        {
            reader.Begin(itr.Current());
            DumpAllText(reader, &number_of_string);
            reader.End();
        }
    }
    catch(...)
    {
        cout << "Unknown Exception" << endl;
    }
    ui->pushButton_calculate->setEnabled(true);
    return;
}

void MainWindow::on_pushButton_export_clicked()
{
    ui->label_res_of_export->setVisible(false);
    ui->pushButton_error_info->setVisible(false);
    ui->progressBar->setValue(0);
    ui->progressBar->isEnabled();
    ui->progressBar->setVisible(true);
//    QString result;
//    QMessageBox::StandardButton result_button;
    std::thread EXPORT([this](){
        QString res_of_export;
        second_thread_is_on = true;
        ui->pushButton_export->setEnabled(false);
        res_of_export = export_xls(this, date, begin_date, begin_day_of_week, end_date, end_day_of_week, sum_of_category);
        if(res_of_export.contains("Ошибка"))
        {
            error_info = res_of_export;
            emit(value_progressbar_changed(0));
            ui->pushButton_error_info->setVisible(true);
            ui->label_res_of_export->setStyleSheet("color: red");
            ui->label_res_of_export->setText("Ошибка!");
            ui->label_res_of_export->setVisible(true);
        }
        else
        {
            ui->label_res_of_export->setStyleSheet("color: green");
            ui->label_res_of_export->setText("Загружено!");
            ui->label_res_of_export->setVisible(true);
        }
        ui->pushButton_export->setEnabled(true);
        second_thread_is_on = false;
    });
    EXPORT.detach();
    return;
}

void MainWindow::recieve_text(QString str)
{
    selected_category = str;
    //ui->textEdit->append(selected_category);
    return;
}

void MainWindow::on_pushButton_error_info_clicked()
{
    QMessageBox::critical(this, "Результат", error_info, QMessageBox::Ok);
    return;
}

