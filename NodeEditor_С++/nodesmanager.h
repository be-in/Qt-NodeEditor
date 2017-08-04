//
// Panteleymonov Aleksandr Konstantinovich 03.08.2017
//

#ifndef NODESMANAGER_H
#define NODESMANAGER_H

#include <QDockWidget>
#include <QTreeWidget>

class NodesManager : public QDockWidget
{
    Q_OBJECT

public:
    explicit NodesManager(const QString title, QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
    ~NodesManager();

private:
    QTreeWidget *nodesTree;
};

#endif // NODESMANAGER_H
