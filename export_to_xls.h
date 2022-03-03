#ifndef EXPORT_TO_XLS_H
#define EXPORT_TO_XLS_H

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QDate>
#include <mutex>
#include "mainwindow.h"

// функция export_xls является дружественной по отношению к классу MainWindow и объявлена там
void get_name_of_sheet_and_count_days(QString date, char name_of_current_sheet[], char name_of_prev_sheet[], int*);

#endif // EXPORT_TO_XLS_H
