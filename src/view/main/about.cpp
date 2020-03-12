#include "include/view/main/about.h"
#include "ui_about.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
#ifdef DEBUG
    cerr << "Constructing About" << endl;
#endif
    ui->setupUi(this);
    GraphicsScene *scene = new GraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    scene->addText(tr("Check back for a tutorial in future releases."));

    connect(scene, SIGNAL(click(QPointF)), this, SLOT(close()));

#ifdef DEBUG
    cerr << "Finish constructing About" << endl;
#endif
}

About::~About()
{
#ifdef DEBUG
    cerr << "Destroying About" << endl;
#endif
    delete ui;
}
