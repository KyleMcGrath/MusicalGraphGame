#include "include/view/main/dialog.h"
#include "ui_dialog.h"

#include <iostream>
using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    QString gameVersion;
    if(settings.contains(tr("gameVersion")))
        gameVersion = settings.value(tr("gameVersion")).toString();
    else {
        settings.setValue(tr("gameVersion"), tr("easy"));
        gameVersion = tr("easy");
    }

    if(gameVersion == tr("hard"))
        ui->radioButton->setChecked(true);
    else if(gameVersion == tr("medium"))
        ui->radioButton_2->setChecked(true);
    else
        ui->radioButton_3->setChecked(true);
    if(ui->radioButton->isChecked())
        settings.setValue(tr("gameVersion"),"easy");
    else if(ui->radioButton_2->isChecked())
        settings.setValue(tr("gameVersion"),"medium");
    else if(ui->radioButton_3->isChecked()) {
        settings.setValue(tr("gameVersion"),"hard");}
    hide();
}
