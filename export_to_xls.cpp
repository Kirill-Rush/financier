#include "export_to_xls.h"

std::mutex mutex_export;

QString MainWindow::export_xls(QMainWindow *ui, QString date, QString begin_date, QString begin_day_of_week, \
                   QString end_date, QString end_day_of_week, QMap<QString, double> sum_of_category)
{
    std::unique_lock<std::mutex> ulmutex_export; // блокирует повторное использование функции, пока выполняется в потоке
    try
    {
        // определяем название листа, которое нужно
        char name_of_current_sheet[20];
        char name_of_prev_sheet[20];
        int count_days;
        get_name_of_sheet_and_count_days(date, name_of_current_sheet, name_of_prev_sheet, &count_days);
        QString name_of_prev_sheet_in_QString(name_of_prev_sheet);
        emit(value_progressbar_changed(10));
        // работаем с Excel-файлом
        // указатель на Excel
        QAxObject *m_excel = new QAxObject("Excel.Application", ui);
        // указатель на Книгу
        QAxObject *workbooks = m_excel->querySubObject("Workbooks");
        // путь к нужному файлу xlsx
        QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", QDir::currentPath() + "\\reports\\finances.xlsm"); // C:\\Users\\Kirill\\Documents\\Qt\\financier\\build-financier-Desktop_Qt_6_2_0_MinGW_64_bit-Debug\\finances.xlsm !!!!!!!!!!
        // указатель на листы
        QAxObject *m_sheets = workbook->querySubObject("Sheets");

        emit(value_progressbar_changed(20));
        //указатель на нужный лист книги
        QAxObject *current_sheet = m_sheets->querySubObject("Item(const QVariant&)", QVariant(name_of_current_sheet));
        if(!current_sheet)
        {
            // копирование листа "Старый лист" в "Новый лист"
            // получение указателя
            QAxObject* sheetToCopy = workbook->querySubObject("Worksheets(const QVariant&)", "default");
            // создание копии листа в место перед копируемым (слева). имя по умолчанию - "Старый лист (2)"
            sheetToCopy->dynamicCall("Copy(const QVariant&)", sheetToCopy->asVariant());
            emit(value_progressbar_changed(30));
            // очистка памяти
            delete sheetToCopy;
            // получение ссылки на новый лист
            current_sheet = workbook->querySubObject("Worksheets(const QVariant&)", "default (2)");
            // переименование нового листа
            current_sheet->setProperty("Name", name_of_current_sheet);

            // установка формул в новом листе
            // получение указателя на предыдущий лист
            QAxObject* prev_sheet = 0;
            prev_sheet = workbook->querySubObject("Worksheets(const QVariant&)", name_of_prev_sheet_in_QString);
            if(prev_sheet)
            {
                // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
                QAxObject* cell = current_sheet->querySubObject("Cells(QVariant,QVariant)", 14, 2);
                // вставка формулы взятия оставшейся суммы на карте с прошлого месяца (любой тип, приводимый к QVariant) в полученную ячейку
                cell->setProperty("Value", QVariant("=" + name_of_prev_sheet_in_QString + "!H14"));

                // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
                cell = current_sheet->querySubObject("Cells(QVariant,QVariant)", 14, 13); // Формула суммы средств
                // вставка формулы сравнения суммы средств с предыдущим месяцем (любой тип, приводимый к QVariant) в полученную ячейку
                cell->setProperty("Value", QVariant("=L14/" + name_of_prev_sheet_in_QString + "!L14-1"));

                cell = current_sheet->querySubObject("Cells(QVariant,QVariant)", 18, 5); // Формула инвестиций с прошлого месяца
                // вставка формулы суммы инвестиций с прошлого месяца (любой тип, приводимый к QVariant) в полученную ячейку
                cell->setProperty("Value", QVariant("=" + name_of_prev_sheet_in_QString + "!F21"));

                cell = current_sheet->querySubObject("Cells(QVariant,QVariant)", 18, 3); // Формула средств с других карт и счетов с прошлого месяца
                // вставка формулы средств с других карт и счетов с прошлого месяца (любой тип, приводимый к QVariant) в полученную ячейку
                cell->setProperty("Value", QVariant("=(B18+B19)/" + name_of_prev_sheet_in_QString + "!B20-1"));
                // освобождение памяти
                delete cell;
            }
        }

        emit(value_progressbar_changed(35));
        // Проставление дат анализируемого отчета
        int j = 0;
        if((begin_date.left(2).toInt() == 1) && (end_day_of_week.left(3) == "Sun"))
            j = 3; // если первая неделя месяца, проставление рез-тов в 3 столбец (1 свободный для рез-тов)
        else if((begin_day_of_week.left(3) == "Mon") && (end_day_of_week.left(3) == "Sun"))
        {
            int k;
            int diff_with_full_week = begin_date.left(2).toUInt() - begin_date.left(2).toInt()/7*7;
            ((diff_with_full_week != 1) && (diff_with_full_week != 0)) ? (k = 1) : (k = 0);
            j = begin_date.left(2).toInt()/7 + 3 + k; // простановка столбца, в который будет запис-ся период
        }
        else if((begin_day_of_week.left(3) == "Mon") && (end_date.left(2).toInt() == count_days))
            j = 8;
        else
        {
             //ui->textEdit->append("НЕПРАВИЛЬНЫЕ ДАТЫ ОТЧЕТА");
            // освобождение памяти
            delete current_sheet;
            delete m_sheets;
            workbook->dynamicCall("Save()");
            delete workbook;
            //закрываем книги
            delete workbooks;
            //закрываем Excel
            m_excel->dynamicCall("Quit()");
            delete m_excel;
            return "Ошибка. Неправильные даты отчета.\
                \nДля записи используются недельные отчеты, а именно:\
                \n- отчеты полной недели (пн - вс);\n- с первого числа месяца до вс;\n- отчеты c понедельника до последнего числа месяца (неполная неделя).";
        }
        // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
        QAxObject* cell = current_sheet->querySubObject("Cells(QVariant,QVariant)", 1, j);
        // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
        cell->setProperty("Value", QVariant(begin_date.left(5) + " - " + end_date.left(5)));
        // освобождение памяти
        delete cell;

        emit(value_progressbar_changed(50));
        // запись доходов в Excel
        QVariant category_in_cell;
        int i = 2;
        while(category_in_cell != "ИТОГО ДОХОДЫ:")
        {
            // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
            QAxObject* cell_text = current_sheet->querySubObject("Cells(QVariant,QVariant)", i, 2);
            // получение содержимого
            category_in_cell = cell_text->property("Value");
            if(category_in_cell == "ИТОГО ДОХОДЫ:")
                break;
            // запись средств в ячейку листа
            QAxObject* cell_number = current_sheet->querySubObject("Cells(QVariant,QVariant)", i, j);
            cell_number->setProperty("Value", QVariant(sum_of_category[category_in_cell.toString()]));
            // освобождение памяти
            delete cell_text;
            delete cell_number;
            //ui->textEdit->append(category_in_cell.toString());
            i++;
        }

        emit(value_progressbar_changed(75));
        // запись расходов в Excel
        i = 5;
        while(category_in_cell != "ИТОГО РАСХОДЫ:")
        {
            // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
            QAxObject* cell_text = current_sheet->querySubObject("Cells(QVariant,QVariant)", i, 2);
            // получение содержимого
            category_in_cell = cell_text->property("Value");
            if(category_in_cell == "ИТОГО РАСХОДЫ:")
                break;
            // запись средств в ячейку листа
            QAxObject* cell_number = current_sheet->querySubObject("Cells(QVariant,QVariant)", i, j);
            cell_number->setProperty("Value", QVariant(sum_of_category[category_in_cell.toString()]));
            // освобождение памяти
            delete cell_text;
            delete cell_number;
            //ui->textEdit->append(category_in_cell.toString());
            i++;
        }
        emit(value_progressbar_changed(90));
        // освобождение памяти
        delete current_sheet;
        delete m_sheets;
        workbook->dynamicCall("Save()");
        delete workbook;
        //закрываем книги
        delete workbooks;
        //закрываем Excel
        m_excel->dynamicCall("Quit()");
        delete m_excel;
        emit(value_progressbar_changed(100));
        //throw current_sheet, m_sheets, workbook, m_excel;
    }
    catch(...)
    {
        std::cout << "Unknown Exception" << std::endl;
        //ui->progressBar->setVisible(false);
        return "Ошибка при работе с EXCEL. Попробуйте повторить попытку или обратиться к разработчику.";
    }
    //ui->progressBar->setVisible(false);
    return "OK";
}

void get_name_of_sheet_and_count_days(QString date, char name_of_current_sheet[], char name_of_prev_sheet[], int *count_days)
{
    char months[12][20] {"ЯНВАРЬ", "ФЕВРАЛЬ", "МАРТ", "АПРЕЛЬ", "МАЙ", "ИЮНЬ", "ИЮЛЬ",\
                            "АВГУСТ", "СЕНТЯБРЬ", "ОКТЯБРЬ", "НОЯБРЬ", "ДЕКАБРЬ"};
    strcpy(name_of_current_sheet, date.mid(3, 2).toStdString().c_str());
    int number_of_month = atoi(name_of_current_sheet);
    strcpy(name_of_current_sheet, months[number_of_month-1]);
    strcat(name_of_current_sheet, "_");
    strncat(name_of_current_sheet, date.right(4).toStdString().c_str(), 4);

    if(number_of_month > 1)
    {
        strcpy(name_of_prev_sheet, months[number_of_month-2]);
        strcat(name_of_prev_sheet, "_");
        strncat(name_of_prev_sheet, date.right(4).toStdString().c_str(), 4);
    }
    else
    {
        int prev_year = date.right(4).toInt() - 1;
        char prev_year_ch[5];
        itoa(prev_year, prev_year_ch, 4);
        strcpy(name_of_prev_sheet, months[11]);
        strcat(name_of_prev_sheet, "_");
        strncat(name_of_prev_sheet, prev_year_ch, 4);
    }


    int year = atoi(date.right(4).toStdString().c_str());
    switch(number_of_month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: *count_days = 31; break;
    case 4:
    case 6:
    case 9:
    case 11: *count_days = 30; break;
    case 2: (year%400 || ((year%4) && (year%100 == 0))) ? *count_days = 28 : *count_days = 29;
    }
    return;
}
