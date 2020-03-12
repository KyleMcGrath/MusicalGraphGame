#include "include/view/main/gamelist.h"
#include "ui_gamelist.h"

#ifdef DEBUG
#include <iostream>
#endif

GameList::GameList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameList)
{
#ifdef DEBUG
    std::cerr << "In GameList::repopulateList(). Fail to open gameList." << std::endl;
#endif
    ui->setupUi(this);
    repopulateList();
}

GameList::~GameList()
{
    delete ui;
}

void GameList::repopulateList()
{
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(chooseGame(QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(repopulateList()));

    QFile gameList(tr("gameList"), parent());
    if(!gameList.open(QIODevice::ReadOnly)) {
#ifdef DEBUG
    std::cerr << "In GameList::repopulateList(). Fail to open gameList." << std::endl;
#endif
        return;
    }
    ui->listWidget->clear();

    QJsonDocument gameListDocument;
    QJsonParseError *error = nullptr;
    gameListDocument = QJsonDocument::fromJson(gameList.readAll(), error);
    gameList.close();
    if(error != nullptr) {
#ifdef DEBUG
    std::cerr << "In GameList::repopulateList(). Fail to convert gameList into JSON from file." << std::endl;
#endif
        delete error;
        return;
    }

    QJsonObject gameListObject = gameListDocument.object();
    if(!gameListObject.contains(tr("games"))) return;
    QJsonArray games = gameListObject.find("games").value().toArray();

    int count = 0;

    for(auto it = games.constBegin(); it != games.constEnd(); it++) {

        if(!it->isString()) continue;
        QString name = it->toString();

        QFile gameFile(name);
        if(!gameFile.open(QFile::ReadOnly)) continue;

        QJsonParseError *e = nullptr;
        QJsonDocument gameJsonDocument = QJsonDocument::fromJson(gameFile.readAll(), e);
        if(e != nullptr) {
#ifdef DEBUG
    std::cerr << "In GameList::repopulateList(). Fail to parse game into JSON from file." << std::endl;
#endif
            delete e;
            gameFile.close();
            continue;
        }

        QJsonObject gameJsonObject = gameJsonDocument.object();

        QJsonObject metadata = gameJsonObject.find(tr("metadata")).value().toObject();

        QString rowText = QString::fromStdString(std::to_string(count));

        if(metadata.contains(tr("name")))
            rowText += " " + metadata.find(tr("name")).value().toString();
        if(metadata.contains(tr("theme")))
            rowText += " " + metadata.find(tr("theme")).value().toString();
        if(metadata.contains(tr("modulus")))
            rowText += " m" + QString::fromStdString(std::to_string(metadata.find(tr("modulus")).value().toInt()));
        if(metadata.contains(tr("order")))
            rowText += " o" + QString::fromStdString(std::to_string(metadata.find(tr("order")).value().toInt()));
        if(metadata.contains(tr("edgeSetCardinality")))
            rowText += " e" + QString::fromStdString(std::to_string(metadata.find(tr("edgeSetCardinality")).value().toInt()));

        QListWidgetItem *item = new QListWidgetItem(rowText, ui->listWidget);

        QFont font;
        font.setBold(true);
        font.setPointSize(16);
        font.setFamily(tr("Comic Sans Serif"));
        item->setFont(font);

        item->setTextColor(QColor::fromRgb(0, 0, 5));

        item->setBackgroundColor(QColor::fromRgb(35, 85, 141, 52));

        ui->listWidget->addItem(item);

        fileNameMap.insert(std::make_pair(count, name));
        ++count;

        gameFile.close();

    }


}

void GameList::chooseGame(QListWidgetItem *item)
{
    disconnect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(repopulateList()));
    disconnect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(chooseGame(QListWidgetItem*)));
    QString name = fileNameMap.at(std::stoi(item->text().split(" ")[0].toStdString()));
    GameDetails *gameDetails = new GameDetails(this, name);
    gameDetails->show();
    connect(gameDetails, SIGNAL(accepted()), this, SLOT(repopulateList()));
    connect(gameDetails, SIGNAL(rejected()), this, SLOT(repopulateList()));
    connect(gameDetails, SIGNAL(load(QString)), this, SLOT(chooseGame(QString)));
    connect(gameDetails, SIGNAL(load(QString)), gameDetails, SLOT(close()));
    connect(gameDetails, SIGNAL(load(QString)), this, SLOT(close()));
}

void GameList::chooseGame(QString name)
{
    emit loadGame(name);
    close();
}

void GameList::on_pushButton_released()
{
    close();
}
