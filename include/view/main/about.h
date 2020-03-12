#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <include/view/main/graphicsscene.h>

//#define DEBUG

namespace Ui {
class About;
}

class About : public QWidget
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
