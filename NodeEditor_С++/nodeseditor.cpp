//
// Panteleymonov Aleksandr Konstantinovich 03.08.2017
//

#include "nodeseditor.h"
#include <QLayout>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMimeData>
#include <QDomDocument>

QMap<QString,void*> NodesEditor::nodeCreator;

NodesEditor::NodesEditor(const QString title, QWidget *parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
    nodesViewArea = new QGraphicsView(this);
    nodesScene = new QGraphicsScene(this);
    setWidget(nodesViewArea);

    nodesViewArea->setScene(nodesScene);
    nodesScene->installEventFilter(this);
    //nodesViewArea->installEventFilter(this);
    nodesViewArea->setAcceptDrops(true);
    nodesViewArea->setDragMode(QGraphicsView::RubberBandDrag);

    connectFrom=0;
    connectTo=0;
    connectitem = 0;
    //installEventFilter(nodesScene);
    //installEventFilter(nodesViewArea);
    //childEvent();
    //QGraphicsRectItem *rect = nodesScene->addRect(0,0,10,10);
    //rect->setFlag(QGraphicsItem::ItemIsMovable);
    //nodesScene->addText("123")->setParentItem(rect);
    //nodesScene->addWidget(new QWidget())->setFlag(QGraphicsItem::ItemIsMovable);
    //delete rect;
}

NodesEditor::~NodesEditor()
{

}

/*void NodesEditor::resizeEvent(QResizeEvent *event)
{
    //nodesViewArea->setSi
}*/

NodesEditor::Node* NodesEditor::addNode(Node* node)
{
    nodes.append(node);
    InitNodeItems(node);
    return node;
}

void NodesEditor::DeleteNode(Node* node)
{
    DeleteConnections(node);
    nodes.removeAll(node);
    nodesScene->removeItem(node->item);
    delete node->item;
    node->item=0;
    delete node;
}

void NodesEditor::DeleteConnections(Node* node)
{
    for (int i=0;i<nodes.length();i++) {
        Node::NodeConnect *inputs = nodes[i]->inputs;
        for (int j=0;j<nodes[i]->InputsCount();j++)
            if (inputs[j].node == node || nodes[i]==node)
            {
                if (inputs[j].connector!=0)
                    nodesScene->removeItem(inputs[j].connector);
                inputs[j].connector = 0;
                inputs[j].node = 0;
                inputs[j].out = 0;
            }
    }
}

void NodesEditor::DeleteConnection(QGraphicsPathItem* item)
{
    for (int i=0;i<nodes.length();i++) {
        Node::NodeConnect *inputs = nodes[i]->inputs;
        for (int j=0;j<nodes[i]->InputsCount();j++)
            if (inputs[j].connector == item)
            {
                nodesScene->removeItem(inputs[j].connector);
                inputs[j].connector=0;
                inputs[j].node = 0;
                inputs[j].out = 0;
            }
    }
}

NodesEditor::Node * NodesEditor::GetNode(QPointF pos)
{
    return GetNode(nodesScene->itemAt(pos,nodesViewArea->transform()));
}

NodesEditor::Node * NodesEditor::GetNode(QGraphicsItem *item)
{
    Node *node=0;
    if (item) {
        if (item->parentItem()) item = item->parentItem();
        for (int i=0;i<nodes.count();i++)
            if (nodes[i]->item == item)
                node = nodes[i];
    }
    return node;
}

bool NodesEditor::LoadFromFile(QString path)
{
    /*QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if (document.setContent(&file)) {
        }
    }*/

    return true;
}

bool NodesEditor::SaveToFile(QString path)
{
    QFile f(path);
    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&f);
        stream.setCodec("UTF-8");
        QDomDocument doc;
        QDomNode xmlInstruct = doc.createProcessingInstruction (tr("xml"), "version=\"1\" encoding=\"UTF-8\"");
        doc.appendChild(xmlInstruct);
        QDomElement mainEl = doc.createElement("Nodes");
        doc.appendChild(mainEl);

        for(int i = 0; i < nodes.count(); i++)
        {
            QDomElement objectEl = doc.createElement("Node");
            mainEl.appendChild(objectEl);

            QDomElement idoEl = doc.createElement("Id");
            objectEl.appendChild(idoEl);
            QDomText idoEltext = doc.createTextNode(QString::number(i));
            idoEl.appendChild(idoEltext);

            QDomElement typeEl = doc.createElement("Type");
            objectEl.appendChild(typeEl);
            QDomText nameEltext = doc.createTextNode(nodes[i]->GetTitle());
            typeEl.appendChild(nameEltext);

            Node::NodeConnect * links=nodes[i]->inputs;
            for (int j=0;j<nodes[i]->InputsCount();j++)
                if (links[j].node)
            {
                QDomElement linkEl = doc.createElement("Link");
                objectEl.appendChild(linkEl);

                QDomElement idEl = doc.createElement("Id");
                linkEl.appendChild(idEl);
                QDomText idEltext = doc.createTextNode(QString::number(j));
                idEl.appendChild(idEltext);

                QDomElement nodeEl = doc.createElement("Node");
                linkEl.appendChild(nodeEl);
                QDomText idnEltext = doc.createTextNode(QString::number(nodes.indexOf(links[j].node)));
                nodeEl.appendChild(idnEltext);

                QDomElement outEl = doc.createElement("Out");
                linkEl.appendChild(outEl);
                QDomText outEltext = doc.createTextNode(QString::number(links[j].out));
                outEl.appendChild(outEltext);
            }
        }
        doc.save(stream, 4);//Второй параметр - это размер отсупа у строчек
        f.close();
        return true;
    }
    return false;
}

typedef NodesEditor::Node*  ( *nodeCreatorType)();

bool NodesEditor::eventFilter(QObject *obj, QEvent *event)
{
    //qDebug() << event->type();
    if (event->type() == QEvent::KeyRelease && ((QKeyEvent*)event)->key()==Qt::Key_Delete)
    {
        QList<QGraphicsItem*> list=nodesScene->selectedItems();
        for (int i=0;i<list.count();i++) {
            if (list[i]->type()==QGraphicsPathItem::Type)
            {
                DeleteConnection((QGraphicsPathItem*)list[i]);
            }
            if (list[i]->type()==QGraphicsRectItem::Type)
            {
                DeleteNode(GetNode(list[i]));
            }
        }
        clearConnection();
        updateConnectors();
    }

    if (event->type() == QEvent::GraphicsSceneMousePress && (connectTo || connectFrom))
    {
        QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);
        if (me->buttons() & Qt::MouseButton::LeftButton) {
            QPointF p = me->scenePos();
            Node *node = GetNode(p);
            if (node) {
                if (connectTo) {
                    int ido=node->GetOutputId(p);
                    if (ido>=0) {
                        connectTo->inputs[idconnet].node=node;
                        connectTo->inputs[idconnet].out=ido;
                    }
                }
                if (connectFrom) {
                    int idi=node->GetInputId(p);
                    if (idi>=0) {
                        node->inputs[idi].node=connectFrom;
                        node->inputs[idi].out=idconnet;
                    }
                }
            }
            clearConnection();
            updateConnectors();
        }
    }

    if (event->type() == QEvent::GraphicsSceneContextMenu)
    {
        QGraphicsSceneContextMenuEvent * cme = static_cast<QGraphicsSceneContextMenuEvent  *>(event);
        QPointF p = cme->scenePos();
        Node *node = GetNode(p);
        clearConnection();
        if (node) {
            int idi=node->GetInputId(p);
            int ido=node->GetOutputId(p);
            if (idi>=0) {
                connectTo = node;
                idconnet = idi;
            } else if (ido>=0) {
                connectFrom = node;
                idconnet = ido;
            }

            if (idi>=0 || ido>=0)
                return true;
        }
    }
    if (event->type() == QEvent::GraphicsSceneDragEnter || event->type() == QEvent::GraphicsSceneDragMove || event->type() == QEvent::GraphicsSceneDrop) {
        QGraphicsSceneDragDropEvent* dad = static_cast<QGraphicsSceneDragDropEvent *>(event);
        dad->acceptProposedAction();
        if (event->type() == QEvent::GraphicsSceneDrop) {
            QByteArray encoded = dad->mimeData()->data(dad->mimeData()->formats()[0]);
            QDataStream stream(&encoded, QIODevice::ReadOnly);

            int r,c;
            QMap<int, QVariant> roleDataMap;
            stream >> r >> c >> roleDataMap;
            while (!stream.atEnd()) {
                stream >> roleDataMap;
            }
            QString type=roleDataMap[0].toString();

            if (nodeCreator.contains(type)) {
                nodeCreatorType creator = (nodeCreatorType)nodeCreator[type];
                addNode(creator())->setPos(dad->scenePos());
            }
        }
        return true;
    }

    if (event->type() == QEvent::GraphicsSceneMouseMove) {
        QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);
        //nodesViewArea->setDragMode((me->buttons() & Qt::MouseButton::MidButton) ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
        if ((me->buttons() & Qt::MouseButton::LeftButton)) {
            updateConnectors();
        }

        if ((connectFrom || connectTo))
        {
            QPointF p1 = QPointF(0,0);
            QPointF p2 = QPointF(0,0);
            if (connectFrom) {
                p1 = me->scenePos();
                p2 = connectFrom->GetOutputPoint(idconnet);
            }
            if (connectTo) {
                p1 = connectTo->GetInputPoint(idconnet);
                p2 = me->scenePos();
            }
            connectitem = updateConnection(connectitem,p1,p2,false);
        }
        //return true;
    }

    return QWidget::eventFilter(obj, event);
}

void NodesEditor::InitNodeItems(Node * node)
{
    QRectF wr = node->windowRect;
    QGraphicsRectItem *winRect = nodesScene->addRect(wr,QPen(),QBrush(QColor(0xFFFFFFFF)));
    winRect->setFlag(QGraphicsItem::ItemIsMovable);
    winRect->setFlag(QGraphicsItem::ItemIsSelectable);
    //winRect->setFlag(QGraphicsItem::ItemStacksBehindParent);
    QGraphicsRectItem *titleRect = nodesScene->addRect(wr.x(),wr.y(),wr.width(),20);
    titleRect->setParentItem(winRect);
    QGraphicsTextItem *title = nodesScene->addText(node->GetTitle());
    title->setParentItem(titleRect);
    title->setPos(wr.topLeft());
    
    //nodesScene->addPath()

    //nodesScene->addPixmap(node)->setParentItem(winRect);

    node->item = winRect;
    node->ItemsInit(nodesScene);
    winRect->setRect(node->windowRect);
}

void NodesEditor::updateConnectors()
{
    for (int i=0;i<nodes.count();i++)
    {
        Node::NodeConnect * c=nodes[i]->inputs;
        if (!c)
            return;
        for (int j=0;j<nodes[i]->InputsCount();j++)
        if (c[j].node) {
            QPointF p1 = nodes[i]->GetInputPoint(j);
            QPointF p2 = c[j].node->GetOutputPoint(c[j].out);
            c[j].connector = updateConnection(c[j].connector,p1,p2);
        } else if (c[j].connector) {
            delete c[j].connector;
            c[j].connector = 0;
        }
    }
}

QGraphicsPathItem *NodesEditor::updateConnection(QGraphicsPathItem*item,QPointF p1,QPointF p2,bool select)
{
    QPainterPath path = QPainterPath(p1);
    path.quadTo(p1+QPointF(-15,0),QPointF((p1+p2))*0.5);
    path.quadTo(QPointF(p2+QPoint(15,0)),QPointF(p2));
    QGraphicsPathItem *connector=item;
    if (!connector) connector = nodesScene->addPath(path);
    else
        connector->setPath(path);
    connector->setZValue(-1);
    if (select) connector->setFlag(QGraphicsItem::ItemIsSelectable);
    item = connector;
    return item;
}

void NodesEditor::clearConnection()
{
    connectFrom = 0;
    connectTo = 0;
    if (connectitem) {
        delete connectitem;
        connectitem=0;
    }
}

NodesEditor::Node::Node()
{
    windowRect = QRect(0,0,100,20);
    item = NULL;
}

NodesEditor::Node::~Node()
{
    //if (item!=NULL)
        //delete item;
}

void NodesEditor::Node::ItemsInit(QGraphicsScene *scene)
{
    int ic=InputsCount();
    int oc=OutputsCount();
    int m=(ic>oc?ic:oc)*10;
    windowRect.setHeight(m*2+20);
    char **is=Inputs();
    int io=20+m-ic*10;
    int oo=20+m-oc*10;
    for (int i=0;i<ic;i++) {
        QGraphicsTextItem *ti=scene->addText(is[i]);
        ti->setPos(0+windowRect.left(),i*20+io+windowRect.top());
        ti->setParentItem(item);
        QGraphicsEllipseItem * ie=scene->addEllipse(QRectF(QPointF(-5,i*20+5+io) +windowRect.topLeft(),QSizeF(10,10)));
        ie->setParentItem(item);
        ie->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent);
        ie->setZValue(-1);
    }
    is=Outputs();
    for (int i=0;i<oc;i++) {
        QGraphicsTextItem *ti=scene->addText(is[i]);
        ti->setPos(QPointF(windowRect.width()-ti->boundingRect().width() +windowRect.left(),i*20+oo +windowRect.top()));
        ti->setParentItem(item);
        QGraphicsEllipseItem * ie=scene->addEllipse(QRectF(QPointF(windowRect.width()-5,i*20+5+oo) +windowRect.topLeft(),QSizeF(10,10)));
        ie->setParentItem(item);
        ie->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent);
        ie->setZValue(-1);

    }
    inputs = 0;
    if (ic>0) {
        inputs = new NodeConnect[ic];
        for (int i=0;i<ic;i++) {
            inputs[i].connector=0;
            inputs[i].node=0;
        }
    }
}

QPointF NodesEditor::Node::GetInputPoint(int id)
{
    int ic=InputsCount();
    int oc=OutputsCount();
    int m=(ic>oc?ic:oc)*10;
    int io=25+m-ic*10+id*20;
    return item->pos()+QPoint(-5,io+5);
}

QPointF NodesEditor::Node::GetOutputPoint(int id)
{
    int ic=InputsCount();
    int oc=OutputsCount();
    int m=(ic>oc?ic:oc)*10;
    int io=25+m-oc*10+id*20;
    return item->pos()+QPoint(windowRect.width()+5,io+5);
}

int NodesEditor::Node::GetInputId(QPointF pos)
{
    pos -= item->pos();
    int ic=InputsCount();
    int oc=OutputsCount();
    int m=(ic>oc?ic:oc)*10;
    int io=20+m-ic*10;
    int id=int((pos.y()-io)/20.0f);
    if (id<0 || id>=ic || pos.x()>windowRect.width()/2)
        return -1;
    return id;
}

int NodesEditor::Node::GetOutputId(QPointF pos)
{
    pos -= item->pos();
    int ic=InputsCount();
    int oc=OutputsCount();
    int m=(ic>oc?ic:oc)*10;
    int io=20+m-oc*10;
    int id=int((pos.y()-io)/20.0f);
    if (id<0 || id>=oc || pos.x()< windowRect.width()/2)
        return -1;
    return id;
}
