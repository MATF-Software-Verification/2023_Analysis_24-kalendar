#include "headers/eventinfo.h"
#include "ui_eventinfo.h"
#include "../headers/activitiesList.h"
#include "../headers/mainscreen.h"
#include <QDesktopServices>


Event eventGlobal;
EventInfo::EventInfo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EventInfo)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

}

EventInfo::~EventInfo()
{
    delete ui;
}

void EventInfo::setEventData(QString id)
{
    auto events = ActivitiesList::getInstance().getEventsList();
    Event event;

    for (auto e : *events)
        if(e->getId() == id)
            event = *e;

    eventGlobal = event;

    QString dateNtime;
    if(event.get_startDate() == event.get_endDate()){
        dateNtime = QLocale(QLocale::English,QLocale::Serbia).toString(event.get_startDate());
        dateNtime = dateNtime + " : " + event.get_startTime().toString() + " - " + event.get_endTime().toString();
    }
    else{
        dateNtime = QLocale(QLocale::English,QLocale::Serbia).toString(event.get_startDate());
        dateNtime = dateNtime + ", " + event.get_startTime().toString() + " -\n";
        dateNtime = dateNtime + QLocale(QLocale::English,QLocale::Serbia).toString(event.get_endDate()) + ", " + event.get_endTime().toString();

    }
    ui->lblTiltle->setText(event.get_title());
    ui->lblTiltle->setStyleSheet("color: white;"
                                 "font-size: 40pt;");
    ui->lblDateTime->setText(dateNtime);

    QString repeatString;
    if(event.m_repeat == Event::Dont_repeat)
        ui->lblRepeat->setText("Event doesn't repeat");

    else{
        if(event.m_repeat == Event::Days)
            repeatString = "days";
        else if(event.m_repeat == Event::Weeks)
            repeatString = "weeks";
        else if(event.m_repeat == Event::Months)
            repeatString = "months";
        else if(event.m_repeat == Event::Years)
            repeatString = "years";
        QString str = QString("Repeats every %1 %2, %3 times").arg(QString::number(event.getInterval())).arg(repeatString).arg(QString::number(event.getDuration()));

        ui->lblRepeat->setText(str);
    }

    ui->lblDescription->setText(event.get_description());

    auto attachments = event.getAttachments();
    if (attachments.size() > 0) {
        ui->listWidget->clear();
        for (auto att : attachments)
           ui->listWidget->addItem(att->name());
    }
    else{
        ui->listWidget->clear();
        ui->listWidget->addItem("No attachments");
    }


    QString alertString;
    if(event.getNoAlert()){
        alertString = "Alert isn't set";
    }
    else {
        if (event.getAlertNum() == 0)
            alertString = "Alert is set at time of event";
        else if(event.getAlertNum() % 10080 == 0){
            alertString = "Alert is set " + QString::number(event.getAlertNum() / 10080) + " weeks before event";
        }
        else if(event.getAlertNum() % 1440 == 0){
            alertString = "Alert is set " + QString::number(event.getAlertNum() / 1440) + " days before event";
        }
        else if(event.getAlertNum() % 60 == 0){
            alertString = "Alert is set " + QString::number(event.getAlertNum() / 60) + " hours before event";
        }
        else if(event.getAlertNum() < 60){
            alertString = "Alert is set " + QString::number(event.getAlertNum()) + " minutes before event";
        }
        if (event.getAlertType() == "popup")
            alertString += ", alert type: notification";
        else
            alertString += ", alert type: email";
    }

    ui->lblAlert->setText(alertString);
}

void EventInfo::on_listWidget_itemClicked(QListWidgetItem *item)
{
    auto attachments = eventGlobal.getAttachments();
    for (auto att : attachments)
        if (att->name() == item->text())
            QDesktopServices::openUrl(QUrl(att->fileUrl()));    
}

void EventInfo::on_pbBack_clicked()
{
    auto ms = qobject_cast<MainScreen*> (parentWidget());
    ms->calendarWindow()->show();
    hide();
}

void EventInfo::on_pbUpdate_clicked()
{

    Event event = eventGlobal;

    auto ms = qobject_cast<MainScreen*> (parentWidget());
    auto *ew = ms->eventWindow();

    ew->setEvent(event.get_title(),event.get_startDate(), event.get_endDate(),
                 event.get_startTime(), event.get_endTime(), event.get_description(),
                 event.getAlertType(),event.getAlertNum(),
                 event.recurrenceToString(), event.getDuration(),event.getInterval(), event.getAttachments());

    ew->setOldEventId(event.getId());
    ew->show();
    hide();
}

void EventInfo::on_pbDelete_clicked()
{
    Event event = eventGlobal;
    ActivitiesList& myActivitiesList = ActivitiesList::getInstance();

    foreach(Event* e, *myActivitiesList.getEventsList()){
        if(e->getId() == event.getId()){
            myActivitiesList.getEventsList()->removeOne(e);
         }
    }

    emit eventDeleted(event.getId());

    auto ms = qobject_cast<MainScreen*> (parentWidget());
    ms->calendarWindow()->show();
    hide();
}
