//
// Panteleymonov Aleksandr Konstantinovich 03.08.2017
//

#ifndef NODESEDITOR_H
#define NODESEDITOR_H

#include <QDockWidget>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPathItem>

class NodesEditor : public QDockWidget
{
    Q_OBJECT

public:
    class Node {
    public:
        explicit Node();
        ~Node();

        struct NodeConnect {
            Node *node;
            int out;
            QGraphicsPathItem *connector;
        };

        Node * setPos(QPointF pos) { item->setPos(pos); return this; }// item->setPos(pos);
        virtual QString GetTitle()=0;
        virtual int InputsCount()=0;
        virtual int OutputsCount()=0;
        virtual char** Inputs()=0;
        virtual char** Outputs()=0;
        virtual void ItemsInit(QGraphicsScene * scene);
        virtual QPointF GetInputPoint(int id);
        virtual QPointF GetOutputPoint(int id);
        virtual int GetInputId(QPointF pos);
        virtual int GetOutputId(QPointF pos);

        QRectF windowRect;
        NodeConnect *inputs;
        QGraphicsItem *item;
    };

    explicit NodesEditor(const QString title, QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
    ~NodesEditor();

    Node* addNode(Node *node);

    template <typename T>
    T* addNode(QPoint pos = QPoint(0,0))
    {
        return (T*)addNode(new T())->setPos(pos);
    }

    void DeleteNode(Node* node);


    void DeleteConnections(Node* node);
    void DeleteConnection(QGraphicsPathItem* item);

    Node * GetNode(QPointF pos);
    Node * GetNode(QGraphicsItem *item);

    bool LoadFromFile(QString path);
    bool SaveToFile(QString path);

    static QMap<QString,void*> nodeCreator;

protected:
    //void resizeEvent(QResizeEvent *event);
    //void mouseMoveEvent(QMouseEvent * event);
    bool eventFilter(QObject *obj, QEvent *event);
private:
    QGraphicsView * nodesViewArea;
    QGraphicsScene * nodesScene;
    QList<Node *> nodes;
    Node *connectFrom;
    Node *connectTo;
    int idconnet;
    QGraphicsPathItem * connectitem;

    void InitNodeItems(Node * node);
    QGraphicsPathItem *updateConnection(QGraphicsPathItem*item,QPointF p1,QPointF p2,bool select =true);
    void updateConnectors();
    void clearConnection();
};

#endif // NODESEDITOR_H
