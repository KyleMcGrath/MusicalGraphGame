#include "include/view/main/gamedetails.h"
#include "ui_gamedetails.h"

#ifdef DEBUG
#include <iostream>
#endif

GameDetails::GameDetails(QWidget *parent, QString filename) :
    QDialog(parent),
    ui(new Ui::GameDetails)
{
    ui->setupUi(this);

    file.setFileName(filename);
    if(!file.open(QFile::ReadWrite)) {
#ifdef DEBUG
        std::cerr << "Failure to open file " << file.fileName().toStdString() << "." << std::endl;
#endif
        return;
    }

    QJsonParseError *err = nullptr;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll(), err);
    if(err != nullptr) {
        file.close();
#ifdef DEBUG
        std::cerr << "Failure to parse JSON " << err->errorString().toStdString() << "." << std::endl;
#endif
        delete err;
        return;
    }

    object = jsonDocument.object();

    metadata = object.find(tr("metadata")).value().toObject();
    if(metadata.contains(tr("name"))) {
        QJsonValue v = metadata.find(tr("name")).value();
        if(v.isString())
            ui->lineEdit->setText(v.toString());
    }
    if(metadata.contains(tr("notes"))) {
        QJsonValue v = metadata.find(tr("notes")).value();
        if(v.isString())
            ui->textEdit->setText(v.toString());
    }
}

GameDetails::~GameDetails()
{
    if(file.isOpen())
        file.close();
    delete ui;
}

void GameDetails::save()
{
    metadata.insert(tr("name"), ui->lineEdit->text());
    metadata.insert(tr("notes"), ui->textEdit->toPlainText());
    object.insert(tr("metadata"), metadata);

    if(file.remove())
        if(!file.open(QFile::ReadWrite)) {
    #ifdef DEBUG
            std::cerr << "Failure to open file " << file.fileName().toStdString() << "." << std::endl;
    #endif
            return;
        }

    QJsonDocument jsonDocument(object);
    if(file.write(jsonDocument.toJson())<0) {
#ifdef DEBUG
        std::cerr << "Failure to write to file " << file.fileName().toStdString() << "." << std::endl;
#endif
        return;
    }

}

void GameDetails::on_pushButton_released()
{
    file.flush();
    save();
    file.flush();
    file.close();
    emit accepted();
    close();
}

void GameDetails::on_pushButton_2_released()
{
    emit accepted();
    file.flush();
    file.close();
    close();
}

void GameDetails::on_pushButton_3_released()
{
    file.flush();
    save();
    file.flush();
    file.close();
    emit load(file.fileName());
}
