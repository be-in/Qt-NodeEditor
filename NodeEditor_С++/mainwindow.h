//
// Panteleymonov Aleksandr Konstantinovich 03.08.2017
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "nodeseditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool LoadNodes(QString path);

private slots:
    void open();
    void save();
    void saveAs();
    void quit();

private:
    NodesEditor *ne;

    QString filename;
};

#endif // MAINWINDOW_H
