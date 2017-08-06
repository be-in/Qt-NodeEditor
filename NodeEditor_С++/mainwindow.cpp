#include "mainwindow.h"
#include "nodesmanager.h"
#include "nodesexample.h"
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QFileDialog>

NodesEditor::Node *createMull() { return new NodesType::Mul(); }
NodesEditor::Node *createGenerate() { return new NodesType::Generate(); }
NodesEditor::Node *createInt() { return new NodesType::Int(); }
NodesEditor::Node *createMany() { return new NodesType::Many(); }

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ne = new NodesEditor("NodesEditor",this);
    NodesManager *nm = new NodesManager("NodesManager",this);
    addDockWidget(Qt::TopDockWidgetArea, nm);
    addDockWidget(Qt::TopDockWidgetArea, ne);

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    connect(fileMenu->addAction(tr("Open")), SIGNAL(triggered()), this, SLOT(open()));
    connect(fileMenu->addAction(tr("Save")), SIGNAL(triggered()), this, SLOT(save()));
    connect(fileMenu->addAction(tr("Save as..")), SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(fileMenu->addAction(tr("Exit")), SIGNAL(triggered()), this, SLOT(quit()));

    NodesEditor::nodeCreator["Mul"]=&createMull;
    NodesEditor::nodeCreator["Generate"]=&createGenerate;
    NodesEditor::nodeCreator["Int"]=&createInt;
    NodesEditor::nodeCreator["Many"]=&createMany;

    filename = "";
}

MainWindow::~MainWindow()
{
}

bool MainWindow::LoadNodes(QString path)
{
    filename = path;
    return true;
}

void MainWindow::save()
{
    if (filename.isEmpty())
        saveAs();
    else
        ne->SaveToFile(filename);
}

void MainWindow::saveAs()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setNameFilter("*.nod");
    fileDialog.setWindowTitle(tr("Save Nodes File"));
    /*if (m_currentPath.isEmpty())
        fileDialog.setDirectory(picturesLocation());*/

    while (fileDialog.exec() == QDialog::Accepted && !ne->SaveToFile(fileDialog.selectedFiles().constFirst()));
}

void MainWindow::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setNameFilter("*.nod");
    fileDialog.setWindowTitle(tr("Open Nodes File"));
    /*if (m_currentPath.isEmpty())
        fileDialog.setDirectory(picturesLocation());*/

    while (fileDialog.exec() == QDialog::Accepted && !LoadNodes(fileDialog.selectedFiles().constFirst()));
}

void MainWindow::quit()
{
    QApplication::quit();
}
