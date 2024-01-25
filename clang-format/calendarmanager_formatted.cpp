#include "../headers/calendarmanager.h"
#include "../headers/activitiesList.h"
#include "../headers/event.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QVector>

CalendarManager::CalendarManager(QObject *parent, OAuth2 *oauth2)
    : QObject(parent), m_auth(oauth2) {
  auto replyHandler = new QOAuthHttpServerReplyHandler(m_auth);
  m_auth->getGoogle()->setReplyHandler(replyHandler);
  connect(this, &CalendarManager::savedCalendarIdsToJsonObject,
          &CalendarManager::onSavedCalendarIdsToJsonObject);
  connect(this, &CalendarManager::savedCalendarEventsToJsonObject,
          &CalendarManager::onSavedCalendarEventsToJsonObject);
  connect(m_auth->getGoogle(), &QOAuth2AuthorizationCodeFlow::tokenChanged,
          this, &CalendarManager::onTokenChanged);
  m_auth->refreshAccessToken();
}

void CalendarManager::onSavedCalendarIdsToJsonObject() {

  auto items = m_jsonObject->find("items")->toArray();

  for (auto item : items)
    m_ids.push_back(item.toObject()["id"].toString());

  emit savedCalendarIds();
}

void CalendarManager::onSavedCalendarEventsToJsonObject() {

  auto items = m_jsonObject->find("items")->toArray();

  for (auto item : items) {

    QString id = item.toObject()["id"].toString();
    QString title = item.toObject()["summary"].toString();
    QString description = item.toObject()["description"].toString();
    QString start = item.toObject()["start"].toObject()["dateTime"].toString();
    QString end = item.toObject()["end"].toObject()["dateTime"].toString();

    auto recurrence = item.toObject()["recurrence"].toArray();
    auto reminders = item.toObject()["reminders"].toObject();
    auto attachments = item.toObject()["attachments"].toArray();

    auto def = reminders["useDefault"].toBool();
    bool noAlert = false;
    QString alertType;
    unsigned alertNum;

    // po default-u je 30 minuta pre pocetka dogadjaja
    if (def) {
      alertType = "popup";
      alertNum = 30;
    } else {
      if (reminders.contains("overrides")) {
        auto overrides = reminders["overrides"].toArray();
        alertNum = overrides[0].toObject()["minutes"].toInt();
        alertType = overrides[0].toObject()["method"].toString();
      }
      // ukoliko ne sadrzi overrides i def nije true znaci da nema alert-a
      else
        noAlert = true;
    }

    QDate startDate;
    QDate endDate;
    if (start == "") {
      QString s = item.toObject()["start"].toObject()["date"].toString();
      QString e = item.toObject()["end"].toObject()["date"].toString();
      startDate = QDate::fromString(s, Qt::ISODate);
      endDate = QDate::fromString(e, Qt::ISODate);
      ;
    }

    else {
      startDate = QDate::fromString(start.left(10), Qt::ISODate);
      endDate = QDate::fromString(end.left(10), Qt::ISODate);
    }
    QTime startTime = QTime::fromString(start.mid(11, 8), "hh:mm:ss");
    QTime endTime = QTime::fromString(end.mid(11, 8), "hh:mm:ss");

    Event *event = new Event(id, title, description, startDate, endDate,
                             startTime, endTime);

    event->setAlertNum(alertNum);
    event->setAlertType(alertType);
    event->setNoAlert(noAlert);

    if (recurrence.empty())
      event->m_repeat = Event::Repeat::Dont_repeat;
    else {
      QString recStr = recurrence[0].toString();
      if (recStr.contains("DAILY"))
        event->m_repeat = Event::Repeat::Days;
      else if (recStr.contains("WEEKLY"))
        event->m_repeat = Event::Repeat::Weeks;
      else if (recStr.contains("MONTHLY"))
        event->m_repeat = Event::Repeat::Months;
      else if (recStr.contains("YEARLY"))
        event->m_repeat = Event::Repeat::Years;
      if (recStr.contains("COUNT")) {
        int pos = recStr.indexOf("COUNT");
        QString s = recStr.mid(pos + 6, recStr.size());
        QString duration = s.left(s.indexOf(";"));
        event->setDuration(duration.toInt());
      } else
        event->setDuration(10);
      if (recStr.contains("INTERVAL")) {
        int pos = recStr.indexOf("INTERVAL");
        QString s = recStr.mid(pos + 9, recStr.size());
        if (s.contains(";")) {
          QString interval = s.left(s.indexOf(";"));
          event->setInterval(interval.toInt());
        } else {
          event->setInterval(s.toInt());
        }
      } else
        event->setInterval(1);
    }

    for (auto attachment : attachments) {
      AttachmentInfo *at =
          new AttachmentInfo(attachment.toObject()["fileId"].toString(),
                             attachment.toObject()["fileUrl"].toString(),
                             attachment.toObject()["mimeType"].toString(),
                             attachment.toObject()["title"].toString(),
                             attachment.toObject()["iconLink"].toString());
      event->addAttachment(at);
    }

    m_eventsFromCalendar.push_back(event);
  }

  emit savedEventsFromCalendar();
}

OAuth2 *CalendarManager::getAuth() const { return m_auth; }

const QString CalendarManager::fromEventToString(Event *event) {
  QString str = QString("{\n");

  if (event->recurrenceToString() != "NO_REPEAT") {
    str +=
        QString(
            "\"recurrence\": [\n \"RRULE:FREQ=%1;COUNT=%2;INTERVAL=%3\"\n],\n")
            .arg(event->recurrenceToString())
            .arg(event->getDuration())
            .arg(event->getInterval());
  }
  if (event->lastsAllDay) {
    str += QString("\"start\": { \"date\": \"%1\" },\n")
               .arg(event->get_startDate().toString(Qt::ISODate)) +
           QString("\"end\":   { \"date\": \"%1\" },\n")
               .arg(event->get_endDate().toString(Qt::ISODate));
  } else {

    QDateTime startDT(event->get_startDate(), event->get_startTime()),
        endDT(event->get_endDate(), event->get_endTime());
    int offsetStart = startDT.offsetFromUtc();
    startDT.setOffsetFromUtc(offsetStart);
    int offsetEnd = endDT.offsetFromUtc();
    endDT.setOffsetFromUtc(offsetEnd);
    QString strStart = startDT.toString(Qt::ISODate);
    QString strEnd = endDT.toString(Qt::ISODate);

    // QTimeZone tz = QTimeZone::systemTimeZone(); QTimeZone ne moze u QString
    // :(
    str += QString("\"start\": { \"dateTime\": \"%1\", \"timeZone\":\"%2\"},\n")
               .arg(strStart)
               .arg("Europe/Belgrade") +
           QString("\"end\": { \"dateTime\": \"%1\" , \"timeZone\":\"%2\"},\n")
               .arg(strEnd)
               .arg("Europe/Belgrade");
  }

  if (event->getUseDefaultAlert())
    str += QString("\"reminders\": {\n\"useDefault\": \"%1\"},\n").arg("true");
  else {
    str += QString("\"reminders\": {\n\"useDefault\": \"%1\"").arg("false");
    if (!event->getNoAlert())
      str += QString(",\n \"overrides\": [\n{\n \"method\": "
                     "\"%1\",\n\"minutes\": \"%2\" \n}\n]")
                 .arg(event->getAlertType())
                 .arg(event->getAlertNum());

    str += QString("\n},");
  }

  auto attachments = event->getAttachments();
  int n = attachments.size();
  int i = 0;
  // pocetak attachments
  str += QString("\"attachments\": [\n");
  for (auto att : attachments) {
    i++;
    str += QString("{\n") +
           QString("\"fileUrl\": \"%1\",\n").arg(att->fileUrl()) +
           QString("\"title\": \"%1\"\n,").arg(att->name()) +
           QString("\"mimeType\": \"%1\"\n,").arg(att->mimeType()) +
           QString("\"iconLink\": \"%1\"\n,").arg(att->iconLink()) +
           QString("\"fileId\": \"%1\"\n").arg(att->fileId());
    if (i == n)
      str += QString("}");
    else
      str += QString("},");
  }
  // kraj attachments
  str += QString("\n],\n");
  str += QString("\"description\": \"%1\"\n,").arg(event->get_description()) +
         QString("\"summary\": \"%1\"\n").arg(event->get_title());

  str += QString("}");

  return str;
}

void CalendarManager::createEvent(Event *event) {

  QByteArray params = fromEventToString(event).toUtf8();
  QString at;
  if (event->getAttachments().isEmpty())
    at = "";
  else
    at = "?supportsAttachments=true";

  auto reply = m_auth->getGoogle()->post(
      QUrl(QString(
               "https://www.googleapis.com/calendar/v3/calendars/%1/events%2")
               .arg(m_calendarId)
               .arg(at)),
      params);
  connect(reply, &QNetworkReply::finished, [=]() {
    qDebug() << "POST REQUEST FINISHED. Error? "
             << (reply->error() != QNetworkReply::NoError);
    QString strReply = (QString)reply->readAll();
    qDebug() << strReply;

    QJsonObject jsonObject =
        QJsonObject(QJsonDocument::fromJson(strReply.toUtf8()).object());

    ActivitiesList &myActivitiesListManager = ActivitiesList::getInstance();
    event->setId(jsonObject["id"].toString());
    myActivitiesListManager.new_event(event);
    myActivitiesListManager.repeatEvent(event);
  });
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void CalendarManager::updateEvent(Event *event) {

  // QByteArray params = fromEventToString(event).toUtf8();
  /*qDebug() << params;
  auto reply = m_auth->getGoogle()->put(getUrlEvent(m_calendarId,
  event->getId()), params); connect(reply, &QNetworkReply::finished, [=](){
      qDebug() << "PUT REQUEST FINISHED. Error? " << (reply->error() !=
  QNetworkReply::NoError); QString strReply = (QString) reply->readAll();
      qDebug() << strReply;
  });
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
*/

  deleteEvent(event->getId());
  createEvent(event);
}

void CalendarManager::deleteEvent(QString eventId) {
  auto reply = m_auth->getGoogle()->deleteResource(
      this->getUrlEvent(m_calendarId, eventId));
  // ukoliko je uspesan delete vraca se prazan objekat
  connect(reply, &QNetworkReply::finished, [=]() {
    qDebug() << "DELETE REQUEST FINISHED. Error? "
             << (reply->error() != QNetworkReply::NoError);
    QString strReply = (QString)reply->readAll();
    qDebug() << strReply;
  });
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void CalendarManager::getCalendarIds() {

  auto reply = m_auth->getGoogle()->get(
      QUrl("https://www.googleapis.com/calendar/v3/users/me/calendarList/"));
  connect(reply, &QNetworkReply::finished, [=]() {
    qDebug() << "GET CALENDAR IDS REQUEST FINISHED. Error? "
             << (reply->error() != QNetworkReply::NoError);
    QString strReply = (QString)reply->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(strReply.toUtf8());
    m_jsonObject = new QJsonObject(jsonDocument.object());

    emit savedCalendarIdsToJsonObject();
  });
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void CalendarManager::getEventsFromCalendar(QString calendarId) {

  auto reply = m_auth->getGoogle()->get(this->getUrlEvents(calendarId));
  connect(reply, &QNetworkReply::finished, [=]() {
    qDebug() << "GET EVENTS REQUEST FINISHED. Error? "
             << (reply->error() != QNetworkReply::NoError);
    QString strReply = (QString)reply->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(strReply.toUtf8());
    m_jsonObject = new QJsonObject(jsonDocument.object());

    emit savedCalendarEventsToJsonObject();
  });
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void CalendarManager::grantAccess() {
  connect(m_auth->getGoogle(), &QOAuth2AuthorizationCodeFlow::granted, [=]() {
    qDebug() << "Access Granted!";

    QString fileName =
        QDir::currentPath() + "/../calendar/resources/tokenCalendar.json";
    QFile file(fileName);
    QString string = QString("{\"refreshToken\" : \"%2\" }")
                         .arg(m_auth->getGoogle()->refreshToken());
    if (file.open(QIODevice::ReadWrite)) {
      file.write(string.toUtf8());
    }
  });

  m_auth->getGoogle()->grant();
}

void CalendarManager::onTokenChanged() {
  qDebug() << "Calendar access token changed";
  qDebug() << m_auth->getGoogle()->token();
  if (m_auth->loggedIn("tokenCalendar.json")) {
    getCalendarIds();
  }
}

QVector<QString> CalendarManager::getIds() const { return m_ids; }

QVector<Event *> CalendarManager::getEventsFromCalendar() const {
  return m_eventsFromCalendar;
}

const QUrl CalendarManager::getUrlEvents(QString calendarId) {
  return QUrl(
      QString("https://www.googleapis.com/calendar/v3/calendars/%1/events")
          .arg(calendarId));
}

const QUrl CalendarManager::getUrlEvent(QString calendarId, QString eventId) {
  return QUrl(
      QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2")
          .arg(calendarId)
          .arg(eventId));
}

QString CalendarManager::getCalendarId() const { return m_calendarId; }

void CalendarManager::setCalendarId(const QString &calendarId) {
  m_calendarId = calendarId;
}
