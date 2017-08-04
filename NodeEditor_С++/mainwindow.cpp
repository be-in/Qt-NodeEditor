#include "mainwindow.h"
#include "nodeseditor.h"
#include "nodesmanager.h"
#include "nodesexample.h"
#include <QDockWidget>
//#include "ui_mainwindow.h"

NodesEditor::Node *createMull() { return new NodesType::Mul(); }
NodesEditor::Node *createGenerate() { return new NodesType::Generate(); }
NodesEditor::Node *createInt() { return new NodesType::Int(); }
NodesEditor::Node *createMany() { return new NodesType::Many(); }

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  //,ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    NodesEditor *ne = new NodesEditor("NodesEditor",this);
    NodesManager *nm = new NodesManager("NodesManager",this);
    addDockWidget(Qt::TopDockWidgetArea, nm);
    addDockWidget(Qt::TopDockWidgetArea, ne);

    /*NodesEditor::Node *n1= ne->addNode<NodesType::Mul>(QPoint(0,0));
    NodesEditor::Node *n2= ne->addNode<NodesType::Generate>(QPoint(0,0));
    n1->inputs[0].node=n2;
    n1->inputs[0].out=0;
    n1->inputs[1].node=n2;
    n1->inputs[1].out=1;*/

    NodesEditor::nodeCreator["Mul"]=&createMull;
    NodesEditor::nodeCreator["Generate"]=&createGenerate;
    NodesEditor::nodeCreator["Int"]=&createInt;
    NodesEditor::nodeCreator["Many"]=&createMany;
}

MainWindow::~MainWindow()
{
    //delete ui;
}
