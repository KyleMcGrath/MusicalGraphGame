#include "include/view/main/dialog.h"
#include "ui_dialog.h"

#include <iostream>
using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    QString gameVersion;
    if(settings.contains(tr("gameVersion")))
        gameVersion = settings.value(tr("gameVersion")).toString();
    else {
        settings.setValue(tr("gameVersion"), tr("easy"));
        gameVersion = tr("easy");
    }

    if(gameVersion == tr("hard"))
        ui->radioButton_3->setChecked(true);
    else if(gameVersion == tr("medium"))
        ui->radioButton_2->setChecked(true);
    else
        ui->radioButton->setChecked(true);

    ui->spinBox->setValue(settings.value(gameVersion+tr("problemIndex")).toInt());
    ui->horizontalSlider->setValue(settings.value(gameVersion+tr("problemIndex")).toInt());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    QString gameVersion;
    if(ui->radioButton->isChecked()) {
        gameVersion = "easy";
        settings.setValue(tr("gameVersion"),"easy");
    } else if(ui->radioButton_2->isChecked()) {
            gameVersion = "medium";
        settings.setValue(tr("gameVersion"),"medium");
    } else if(ui->radioButton_3->isChecked()) {
            gameVersion = "hard";
        settings.setValue(tr("gameVersion"),"hard");
    }

    settings.setValue(gameVersion+tr("problemIndex"), ui->spinBox->value());
    hide();
}

void Dialog::on_buttonBox_rejected()
{
    hide();
}

void Dialog::on_horizontalSlider_sliderMoved(int position)
{
    ui->spinBox->setValue(position);
}

void Dialog::on_spinBox_valueChanged(int arg1)
{
    ui->horizontalSlider->setValue(arg1);
}
