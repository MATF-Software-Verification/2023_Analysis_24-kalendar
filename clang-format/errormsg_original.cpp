#include "../headers/errormsg.h"


ErrorMsg::ErrorMsg(QWidget* parent)
{
    msgBox.setParent(parent);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#ddafaf"));
    msgBox.setPalette(palette);
    msgBox.setWindowModality(Qt::WindowModal);

}

void ErrorMsg::writeMsg(QString msg) {

    msgBox.setText(msg);
    msgBox.setWindowTitle("Error");
    msgBox.exec();
}

void ErrorMsg::moveBox(){

    msgBox.hide();
}
