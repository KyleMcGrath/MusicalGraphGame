#ifndef GAMELIST_H
#define GAMELIST_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QListWidgetItem>
#include <include/view/main/gamedetails.h>
#include <QFile>
#include <map>

//#define DEBUG

namespace Ui {
class GameList;
}

class GameList : public QDialog
{
    Q_OBJECT

public:
    explicit GameList(QWidget *parent = nullptr);
    ~GameList();

public slots:
    void repopulateList();

signals:
    void loadGame(QString);

public slots:
    void chooseGame(QListWidgetItem *);
    void chooseGame(QString);

private slots:
    void on_pushButton_released();

private:
    Ui::GameList *ui;
    std::map<int, QString> fileNameMap;
};

#endif // GAMELIST_H
