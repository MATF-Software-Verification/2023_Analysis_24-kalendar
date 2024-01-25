#include "../headers/mainscreen.h"
#include "../headers/activitiesList.h"
#include "../headers/calendarmanager.h"
#include "../headers/oauth2.h"
#include "ui_mainscreen.h"
#include <QJsonObject>

MainScreen::MainScreen(QWidget *parent, CalendarManager *calendarManager)
    : QWidget(parent), ui(new Ui::MainScreen),
      m_calendarManager(calendarManager)

{
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground, true);

  OAuth2 *auth = new OAuth2();
  auth->setInformation();

  m_calendarManager = new CalendarManager(nullptr, auth);
  m_calendarWindow = new CalendarWindow(this);
  m_eventWindow = new EventWindow(this);
  m_eventInfo = new EventInfo(this);

  connect(m_calendarManager, &CalendarManager::savedCalendarIds, this,
          &MainScreen::onSavedCalendarIds);
  connect(m_calendarManager, &CalendarManager::savedEventsFromCalendar, this,
          &MainScreen::onSavedEventsFromCalendar);
  connect(m_eventWindow, &EventWindow::eventSaved, this,
          &MainScreen::onEventSaved);
  connect(m_eventInfo, &EventInfo::eventDeleted, this,
          &MainScreen::onEventDeleted);
  connect(m_eventWindow, &EventWindow::eventUpdated, this,
          &MainScreen::onEventUpdated);
}

MainScreen::~MainScreen() { delete ui; }

void MainScreen::on_pbCalendar_clicked() {
  m_calendarWindow->show();
  hide();
}

void MainScreen::on_pbEvent_clicked() {
  m_eventWindow->show();
  hide();
}

void MainScreen::onEventSaved(Event *event) {
  m_calendarManager->createEvent(event);
}

void MainScreen::onEventUpdated(Event *event) {
  m_calendarManager->updateEvent(event);
}

void MainScreen::onEventDeleted(QString id) {
  m_calendarManager->deleteEvent(id);
}

EventInfo *MainScreen::eventInfo() const { return m_eventInfo; }

void MainScreen::onSavedCalendarIds() {
  auto ids = m_calendarManager->getIds();
  for (auto id : ids)
    if (id.contains("@gmail"))
      m_calendarManager->setCalendarId(id);

  m_calendarManager->getEventsFromCalendar(m_calendarManager->getCalendarId());
}

void MainScreen::onSavedEventsFromCalendar() {
  ActivitiesList &activitiesList = ActivitiesList::getInstance();
  auto events = m_calendarManager->getEventsFromCalendar();
  for (auto event : events) {
    activitiesList.new_event(event);
    activitiesList.repeatEvent(event);
  }
}

void MainScreen::setLoginWindow(LoginWindow *loginWindow) {
  m_loginWindow = loginWindow;
  connect(m_calendarManager->getAuth()->getGoogle(),
          &QOAuth2AuthorizationCodeFlow::granted, m_loginWindow,
          &LoginWindow::onAccessGranted);
}

CalendarManager *MainScreen::calendarManager() const {
  return m_calendarManager;
}

CalendarWindow *MainScreen::calendarWindow() const { return m_calendarWindow; }

EventWindow *MainScreen::eventWindow() const { return m_eventWindow; }

void MainScreen::on_pbQuit_clicked() { QApplication::quit(); }
