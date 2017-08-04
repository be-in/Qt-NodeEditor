#include "nodesmanager.h"
#include <QHeaderView>

NodesManager::NodesManager(const QString title, QWidget *parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    nodesTree = new QTreeWidget(this);
    //nodesTree->setHeaderLabels(QStringList());
    nodesTree->header()->hide();
    setWidget(nodesTree);
    nodesTree->setDragDropMode(QAbstractItemView::DragOnly);

    QTreeWidgetItem *sub1 = new QTreeWidgetItem(nodesTree);
    sub1->setText(0,"Some1");
    QTreeWidgetItem *tMul=new QTreeWidgetItem(sub1);
    tMul->setText(0,"Mul");
    QTreeWidgetItem *sub2 = new QTreeWidgetItem(nodesTree);
    sub2->setText(0,"Some2");
    QTreeWidgetItem *tGenerate=new QTreeWidgetItem(sub2);
    tGenerate->setText(0,"Generate");
    QTreeWidgetItem *tMany=new QTreeWidgetItem(sub2);
    tMany->setText(0,"Many");
}

NodesManager::~NodesManager()
{

}
