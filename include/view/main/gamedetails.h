#ifndef GAMEDETAILS_H
#define GAMEDETAILS_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

//#define DEBUG

namespace Ui {
class GameDetails;
}

class GameDetails : public QDialog
{
    Q_OBJECT

public:
    explicit GameDetails(QWidget *parent = nullptr, QString filename = QString());
    ~GameDetails();

signals:
    void load(QString);

private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

    void on_pushButton_3_released();

private:
    Ui::GameDetails *ui;
    QJsonObject object;
    QJsonObject metadata;
    QFile file;

    void save();
};

#endif // GAMEDETAILS_H
