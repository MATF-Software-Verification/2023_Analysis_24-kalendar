#include <QApplication>
#include "../headers/mainscreen.h"
#include "../headers/loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainScreen ms;
    LoginWindow lw(&ms);
    ms.setLoginWindow(&lw);

    if (!ms.calendarManager()->getAuth()->loggedIn("tokenCalendar.json")) {
        lw.show();
    }
    else {
        ms.show();
        ms.calendarManager()->getAuth()->refreshAccessTokenAtBeginning("tokenCalendar.json");
    }


    return a.exec();
}
