#include "../headers/eventwindow.h"
#include "../headers/activitiesList.h"
#include "../headers/errormsg.h"
#include "../headers/event.h"
#include "../headers/mainscreen.h"
#include "ui_eventwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QString>
#include <QTimer>

static bool flagAllDay = false;
static bool flagRepeat = false;
static bool noAlert = false;

EventWindow::EventWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::EventWindow) {
  ui->setupUi(this);

  ui->deStart->setDate(QDate::currentDate());
  ui->deEnd->setDate(QDate::currentDate());
  ui->teStart->setTime(QTime::currentTime());
  ui->teEnd->setTime(QTime::currentTime());
  ui->deStart->setLocale(QLocale(QLocale::English, QLocale::Serbia));
  ui->deEnd->setLocale(QLocale(QLocale::English, QLocale::Serbia));

  m_attachmentManager = new AttachmentManager();

  if (m_attachmentManager->auth()->loggedIn("tokenDrive.json")) {
    m_attachmentManager->auth()->refreshAccessTokenAtBeginning(
        "tokenDrive.json");
    showAttachmentWidgets();
  } else {
    ui->pbBrowseAttachment->hide();
    ui->pbUpload->hide();
    ui->lwAddedFiles->hide();
    ui->lwToAddFiles->hide();
    ui->pbRemoveAttachment->hide();
  }
  connect(m_attachmentManager, &AttachmentManager::fileUploaded, this,
          &EventWindow::onFileUploaded);
}

void EventWindow::setEvent(QString title, QDate startDate, QDate endDate,
                           QTime startTime, QTime endTime, QString description,
                           QString alertType, unsigned alertNum, QString repeat,
                           int duration, int interval,
                           QVector<AttachmentInfo *> attachments) {
  ui->deStart->setDate(startDate);
  ui->deEnd->setDate(endDate);
  ui->teStart->setTime(startTime);
  ui->teEnd->setTime(endTime);
  ui->leTitle->setText(title);
  ui->teDescription->setText(description);

  if (repeat == "DAILY")
    ui->cbRepeat->setCurrentText("days");
  else if (repeat == "WEEKLY")
    ui->cbRepeat->setCurrentText("weeks");
  else if (repeat == "MONTHLY")
    ui->cbRepeat->setCurrentText("months");
  else if (repeat == "YEARLY")
    ui->cbRepeat->setCurrentText("years");

  if (alertType == "popup")
    ui->cbAlertType->setCurrentText("Notification");
  else if (alertType == "email")
    ui->cbAlertType->setCurrentText("Email");
  ui->cbRepeat->setCurrentText("days");
  ui->cbAlertUnitOfTime->setCurrentText("minutes");
  ui->spbDuration->setValue(duration);
  ui->spbInterval->setValue(interval);
  ui->spbAlert->setValue(alertNum);
  if (!attachments.empty())
    ui->lwAddedFiles->addItem("Added files: ");
  for (auto att : attachments)
    ui->lwAddedFiles->addItem(att->name());
  m_attachmentManager->attachmnets = attachments;
}

EventWindow::~EventWindow() { delete ui; }

void EventWindow::setStartAndEndDate(QDate &date) {
  ui->deStart->setDate(date);
  ui->deEnd->setDate(date);
}

void EventWindow::on_pbCancel_clicked() {
  parentWidget()->show();
  hide();
  resetWindow();
}

void EventWindow::on_pbSave_clicked() {
  Event *event = new Event();

  event->lastsAllDay = ui->rbAllDay->isChecked();
  event->set_title(ui);
  event->set_description(ui);
  event->set_startDate(ui);
  event->set_endDate(ui);
  if (!event->lastsAllDay) {
    event->set_startTime(ui);
    event->set_endTime(ui);
  }
  event->set_repeat(ui);
  event->set_alert(ui);
  event->set_duration(ui);
  event->set_interval(ui);

  event->setAttachments(m_attachmentManager->attachmnets);

  if ((ui->deStart->date() > ui->deEnd->date()) ||
      (ui->deStart->date() == ui->deEnd->date() &&
       ui->teStart->time() > ui->teEnd->time())) {
    ErrorMsg msgE(this);
    msgE.writeMsg("Wrong date or time!");
    return;
  }

  if (!m_updateEvent)
    emit eventSaved(event);
  else {
    ActivitiesList &myActivitiesListManager = ActivitiesList::getInstance();
    auto events = myActivitiesListManager.getEventsList();
    for (auto e : *events)
      if (e->getId() == m_oldEventId)
        myActivitiesListManager.getEventsList()->removeOne(e);
    event->setId(m_oldEventId);
    emit eventUpdated(event);
  }

  auto ms = qobject_cast<MainScreen *>(parentWidget());
  auto calendarWindow = ms->calendarWindow();
  calendarWindow->show();
  hide();
  resetWindow();
}

void EventWindow::on_pbBrowseAttachment_clicked() {

  QFile *file = new QFile(QFileDialog::getOpenFileName(
      this, "Choose attachment",
      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)));

  QFileInfo fileInfo(*file);
  QString fileName = fileInfo.fileName();

  m_files.push_back(file);
  ui->lwToAddFiles->addItem(fileName);
}

void EventWindow::sendFileToGoogleDrive() {
  m_attachmentManager->uploadAttachment(m_files.back());
}

void EventWindow::onFileUploaded() {
  m_files.pop_back();

  if (m_files.empty()) {
    ui->lwToAddFiles->clear();
    ui->lwToAddFiles->addItem("Upload finished");
    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this,
            [=]() { ui->lwToAddFiles->clear(); });
    timer->start(3000);
    ui->lwAddedFiles->addItem("Added files: ");
    for (auto a : m_attachmentManager->attachmnets)
      ui->lwAddedFiles->addItem(a->name());

    ui->pbUpload->setEnabled(true);
    return;
  }
  sendFileToGoogleDrive();
}

void EventWindow::showAttachmentWidgets() {
  ui->pbBrowseAttachment->show();
  ui->pbUpload->show();
  ui->pbRemoveAttachment->show();
  ui->lwAddedFiles->show();
  ui->lwToAddFiles->show();
  ui->pbLogin->setText("You are logged in!");
  ui->pbLogin->setEnabled(false);
}

void EventWindow::on_pbUpload_clicked() {
  if (m_files.isEmpty()) {
    ErrorMsg errorM(this);
    errorM.writeMsg("No files added");
    return;
  }
  sendFileToGoogleDrive();
  ui->pbUpload->setEnabled(false);
  ui->lwToAddFiles->addItem("Uploading...");
}

void EventWindow::on_pbLogin_clicked() {
  m_attachmentManager->grantAccess();
  connect(m_attachmentManager, &AttachmentManager::accessGrantedSignal, this,
          &EventWindow::showAttachmentWidgets);
}

void EventWindow::on_pbRemoveAttachment_clicked() {
  if (!m_files.empty()) {
    auto currItem = ui->lwToAddFiles->currentItem()->text();
    int i = 0;
    for (auto file : m_files) {
      QFileInfo fileInfo(*file);
      QString fileName = fileInfo.fileName();
      if (fileName == currItem) {
        m_files.remove(i);
        ui->lwToAddFiles->currentItem()->setHidden(true);
        return;
      }
      i++;
    }
  }

  if (!m_attachmentManager->attachmnets.empty()) {
    auto currItem = ui->lwAddedFiles->currentItem()->text();
    int i = 0;
    for (auto att : m_attachmentManager->attachmnets) {
      if (att->name() == currItem) {
        m_attachmentManager->attachmnets.remove(i);
        ui->lwAddedFiles->currentItem()->setHidden(true);
      }
      i++;
    }
  }
}

void EventWindow::on_rBRepeat_clicked() {
  if (!flagRepeat) {
    ui->cbRepeat->hide();
    ui->lblDuration->hide();
    ui->spbInterval->hide();
    ui->spbDuration->hide();
    ui->lblRepeats->hide();
    flagRepeat = !flagRepeat;
  } else {
    ui->cbRepeat->show();
    ui->lblDuration->show();
    ui->spbInterval->show();
    ui->spbDuration->show();
    ui->lblRepeats->show();
    flagRepeat = !flagRepeat;
  }
}
void EventWindow::on_rbAllDay_clicked() {
  if (!flagAllDay) {
    ui->teStart->hide();
    ui->teEnd->hide();
    flagAllDay = !flagAllDay;
  } else {
    ui->teStart->show();
    ui->teEnd->show();
    flagAllDay = !flagAllDay;
  }
}
void EventWindow::on_rbNoAlert_clicked() {
  if (!noAlert) {
    ui->cbAlertType->hide();
    ui->spbAlert->hide();
    ui->cbAlertUnitOfTime->hide();
    ui->imgAlert->hide();
    noAlert = !noAlert;
  } else {
    ui->cbAlertType->show();
    ui->spbAlert->show();
    ui->cbAlertUnitOfTime->show();
    ui->imgAlert->show();
    noAlert = !noAlert;
  }
}

void EventWindow::setOldEventId(const QString &oldEventId) {
  m_oldEventId = oldEventId;
  m_updateEvent = true;
}
void EventWindow::resetWindow() {
  ui->leTitle->clear();
  ui->teDescription->clear();
  ui->deStart->setDate(QDate::currentDate());
  ui->deEnd->setDate(QDate::currentDate());
  ui->teStart->setTime(QTime::currentTime());
  ui->teEnd->setTime(QTime::currentTime());
  ui->cbAlertType->show();
  ui->cbAlertUnitOfTime->show();
  ui->spbAlert->show();
  ui->spbAlert->setValue(0);
  ui->imgAlert->show();
  ui->cbRepeat->show();
  ui->lblDuration->show();
  ui->spbInterval->show();
  ui->spbInterval->setValue(1);
  ui->rBRepeat->setChecked(false);
  ui->spbDuration->show();
  ui->spbDuration->setValue(1);
  ui->lblRepeats->show();
  ui->teStart->show();
  ui->teEnd->show();
  ui->rbAllDay->setChecked(false);
  ui->rbNoAlert->setChecked(false);
  ui->lwAddedFiles->clear();
  ui->lwToAddFiles->clear();
  flagAllDay = false;
  noAlert = false;
  flagRepeat = false;
  m_attachmentManager->attachmnets.clear();
  ui->cbRepeat->setCurrentText("days");
  ui->cbAlertType->setCurrentText("Notification");
  ui->cbAlertUnitOfTime->setCurrentText("minutes");
  m_updateEvent = false;
}
