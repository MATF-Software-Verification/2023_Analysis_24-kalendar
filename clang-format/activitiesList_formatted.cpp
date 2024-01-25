#include "../headers/activitiesList.h"
#include <QDebug>

ActivitiesList::ActivitiesList() {}

void ActivitiesList::new_event(Event *e) { eventsList.append(e); }

void ActivitiesList::repeatEvent(Event *event) {
  if (event->get_repeat() != Event::Dont_repeat) {
    int duration = event->getDuration() - 1;
    int interval = event->getInterval();
    for (int i = 0; i < duration; i++) {
      if (event->get_repeat() == Event::Weeks) {
        Event *eventNew = new Event(
            event, event->get_startDate().addDays(7 * (i + 1) * interval),
            event->get_endDate().addDays(7 * (i + 1) * interval));
        eventNew->setId(QString::number(gen_random()));
        new_event(eventNew);
      } else if (event->get_repeat() == Event::Days) {
        Event *eventNew =
            new Event(event, event->get_startDate().addDays((i + 1) * interval),
                      event->get_endDate().addDays((i + 1) * interval));
        eventNew->setId(QString::number(gen_random()));
        new_event(eventNew);
      } else if (event->get_repeat() == Event::Months) {
        Event *eventNew = new Event(
            event, event->get_startDate().addMonths((i + 1) * interval),
            event->get_endDate().addMonths((i + 1) * interval));
        eventNew->setId(QString::number(gen_random()));
        new_event(eventNew);
      } else if (event->get_repeat() == Event::Years) {
        Event *eventNew = new Event(
            event, event->get_startDate().addYears((i + 1) * interval),
            event->get_endDate().addYears((i + 1) * interval));
        eventNew->setId(QString::number(gen_random()));
        new_event(eventNew);
      }
    }
  }
}

quint64 ActivitiesList::gen_random() {
  QRandomGenerator gen = QRandomGenerator::securelySeeded();
  quint64 randomNum = gen.generate64();
  return randomNum;
}

QList<Event *> *ActivitiesList::getWeekEvents(const QDate &date) {
  QList<Event *> *weekEvents = new QList<Event *>;
  foreach (Event *e, *this->getEventsList()) {
    if (e->get_startDate().weekNumber() == date.weekNumber() &&
        e->get_startDate().year() == date.year()) {
      weekEvents->append(e);
    }
  }
  return weekEvents;
}
