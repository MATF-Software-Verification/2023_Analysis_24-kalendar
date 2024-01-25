#include "../headers/calendarwindow.h"
#include "ui_calendarwindow.h"
#include "../headers/mainscreen.h"
#include "../headers/eventwindow.h"
#include "ui_eventwindow.h"
#include "../headers/event.h"
#include "../headers/eventinfo.h"
#include "../headers/activitiesList.h"

#include <QDebug>

QDate globalDate;


CalendarWindow::CalendarWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCalendarWidgetProperties();
    setAttribute(Qt::WA_StyledBackground, true);

}

CalendarWindow::~CalendarWindow()
{
    delete ui;
}

void CalendarWindow::on_pbBack_clicked()
{
    parentWidget()->show();
    hide();
    resetWindow();
}

void CalendarWindow::resetWindow(){
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    ui->tableWidget->clearContents();
    ui->tableWidget->clearSpans();
}

void CalendarWindow::on_calendarWidget_activated(const QDate &date)
{
    auto ms = qobject_cast<MainScreen*> (parentWidget());
    globalDate = date;
    auto ew = ms->eventWindow();
    ew->setStartAndEndDate(globalDate);
    ew->show();
    hide();

}



void CalendarWindow::on_calendarWidget_clicked(const QDate &date)
{
    Event event;


    ui->tableWidget->clearContents();
    ui->tableWidget->clearSpans();
    ActivitiesList& myActivitiesList = ActivitiesList::getInstance();

    QFont policeTitre;
    policeTitre.setPointSize(12);

    foreach(class Event* e, *myActivitiesList.getWeekEvents(date)){

        int dayOfTheWeekS = e->get_startDate().dayOfWeek()-1;
        int dayOfTheWeekE = e->get_endDate().dayOfWeek()-1;
        int startTime = e->get_startTime().hour() + 1;
        int endTime = e->get_endTime().hour() + 1;
        if(startTime == -1 && endTime == -1) {
            startTime = 1;
            endTime = 1;
        }  

        QTableWidgetItem * case_event = new QTableWidgetItem();
        case_event->setFont(policeTitre);
        case_event->setTextAlignment(Qt::AlignCenter);
        case_event->setBackground(QColor("#ddafaf"));
        case_event->setToolTip(e->get_title());
        case_event->setText(e->get_title());
        case_event->setWhatsThis((e->getId()));

        int numHour = endTime - startTime;
        int rowSpan = {numHour <= 0 ? 1 : numHour+1};


        ui->tableWidget->removeCellWidget(startTime,dayOfTheWeekS);
        ui->tableWidget->setItem(startTime,dayOfTheWeekS,case_event);
        ui->tableWidget->setSpan(startTime, dayOfTheWeekS, rowSpan, dayOfTheWeekE-dayOfTheWeekS+1);
    }

}


void CalendarWindow::setCalendarWidgetProperties()
{
    ui->calendarWidget->setLocale(QLocale(QLocale::English,QLocale::Serbia));
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    ui->calendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    QTextCharFormat formatSat = ui->calendarWidget->weekdayTextFormat(Qt::Saturday);
    formatSat.setForeground(QBrush(QColor("#6c5555"), Qt::SolidPattern));
    QTextCharFormat formatSun = ui->calendarWidget->weekdayTextFormat(Qt::Sunday);
    formatSun.setForeground(QBrush(QColor("#6c5555"), Qt::SolidPattern));
    ui->calendarWidget->setWeekdayTextFormat(Qt::Saturday, formatSat);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Sunday, formatSun);
}

void CalendarWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    auto ms = qobject_cast<MainScreen*> (parentWidget());
    ms->eventInfo()->setEventData(item->whatsThis());
    ms->eventInfo()->show();
    hide();
}
