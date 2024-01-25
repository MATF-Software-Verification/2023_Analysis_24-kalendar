#include "../headers/loginwindow.h"
#include "../headers/mainscreen.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LoginWindow) {
  ui->setupUi(this);
}

LoginWindow::~LoginWindow() { delete ui; }

void LoginWindow::on_pbLogin_clicked() {
  auto ms = qobject_cast<MainScreen *>(parentWidget());
  ms->calendarManager()->getAuth()->getGoogleCalendarAuthorization();
  ms->calendarManager()->grantAccess();
}

void LoginWindow::onAccessGranted() {

  auto ms = qobject_cast<MainScreen *>(parentWidget());
  if (!ms->calendarManager()->getAuth()->loggedIn("tokenCalendar.json")) {
    ms->calendarManager()->getCalendarIds();
    hide();
    ms->show();
  }
}
