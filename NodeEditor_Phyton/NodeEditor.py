#!/usr/bin/env python

from PyQt5.QtCore import (pyqtProperty, Qt, QRect, QRectF, QPointF, QSizeF, QEvent, QDataStream, QVariant,
                          QFile, QIODevice, QTextStream, QDir)
from PyQt5.QtGui import (QColor, QPen, QBrush, QPainter, QPainterPath, QPixmap)
from PyQt5.QtWidgets import (QApplication, QDockWidget, QMainWindow, QWidget, QFileDialog, QDialog,
                             QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPathItem,
                             QSizePolicy, QTreeWidget, QAbstractItemView, QTreeWidgetItem)
from PyQt5.QtXml import (QDomDocument, QDomElement, QDomText)

class NodeConnector:
    def __init__(self):
        super(NodeConnector, self)
        self.connector = None
        self.node = None
        self.out = 0
        
class Node:
    width = 120
    
    def __init__(self, icon:str):
        super(Node, self)
        self.windowRect = QRectF(0,0,Node.width,20)
        self.item = None
        self.inputs = [];
        self.icon = QPixmap(icon);

    def setPos(self, pos:QPointF):
        self.item.setPos(pos)
        
    def GetTitle(self):
        return ""

    def Inputs(self):
        return []

    def Outputs(self):
        return []

    def GetInputPoint(self,ids:int):
        ic=len(self.Inputs())
        oc=len(self.Outputs())
        m=max(ic,oc)*10
        io=25+m-ic*10+ids*20
        return self.item.pos()+QPointF(-5,io+5);

    def GetOutputPoint(self,ids:int):
        ic=len(self.Inputs())
        oc=len(self.Outputs())
        m=max(ic,oc)*10
        io=25+m-oc*10+ids*20
        return self.item.pos()+QPointF(self.windowRect.width()+5,io+5);

    def GetInputId(self, pos:QPointF):
        pos = pos - self.item.pos()
        ic=len(self.Inputs())
        oc=len(self.Outputs())
        m=max(ic,oc)*10
        io=20+m-ic*10
        ids=int((pos.y()-io)/20.0)
        if (ids<0 or ids>=ic or pos.x()>self.windowRect.width()/2):
            return -1
        return ids

    def GetOutputId(self, pos:QPointF):
        pos = pos - self.item.pos()
        ic=len(self.Inputs())
        oc=len(self.Outputs())
        m=max(ic,oc)*10
        io=20+m-oc*10
        ids=int((pos.y()-io)/20.0)
        if (ids<0 or ids>=oc or pos.x()<self.windowRect.width()/2):
            return -1
        return ids

    def ItemsInit(self, scene: QGraphicsScene ):
        ic=len(self.Inputs());
        oc=len(self.Outputs());
        m = max(ic,oc)*10
        self.windowRect.setHeight(m*2+20)
        ins=self.Inputs()
        ino=20+m-ic*10
        ouo=20+m-oc*10

        pix=scene.addPixmap(self.icon)
        pix.setPos((self.windowRect.width()-self.icon.width())*0.5,20+(self.windowRect.height()-20-self.icon.height())*0.5)
        pix.setParentItem(self.item)
        
        for i in range(0, ic):
            ti = scene.addText(ins[i])
            ti.setPos(0,i*20+ino)
            ti.setParentItem(self.item)
            ie = scene.addEllipse(QRectF(QPointF(-5,i*20+5+ino),QSizeF(10,10)))
            ie.setParentItem(self.item)
            ie.setFlag(QGraphicsItem.ItemNegativeZStacksBehindParent)
            ie.setZValue(-1)

        ins=self.Outputs()
        for i in range(0, oc):
            ti = scene.addText(ins[i])
            ti.setPos(QPointF(self.windowRect.width()-ti.boundingRect().width(),i*20+ouo))
            ti.setParentItem(self.item)
            ie = scene.addEllipse(QRectF(QPointF(self.windowRect.width()-5,i*20+5+ouo),QSizeF(10,10)));
            ie.setParentItem(self.item)
            ie.setFlag(QGraphicsItem.ItemNegativeZStacksBehindParent)
            ie.setZValue(-1);

        self.inputs = [];
        for i in range (0,ic):
            self.inputs.append(NodeConnector())
            
# user node
class Mul(Node):
    ilabel = ["A","B"]
    olabel = ["Result"]

    def __init__(self):
        super(Mul, self).__init__("../Icons/icons8-Mul.png")
    
    def GetTitle(self):
        return "Mul"

    def Inputs(self):
        return Mul.ilabel

    def Outputs(self):
        return Mul.olabel

# user node
class Generate(Node):
    ilabel = ["Count","Type"]
    olabel = ["A","B","C"]

    def __init__(self):
        super(Generate, self).__init__("../Icons/icons8-Chip.png")
    
    def GetTitle(self):
        return "Generate"

    def Inputs(self):
        return Generate.ilabel

    def Outputs(self):
        return Generate.olabel

# user node
class Many(Node):
    ilabel = ["In2","In3","In4","In5"]
    olabel = ["A","B","C","D"]

    def __init__(self):
        super(Many, self).__init__("../Icons/icons8-Integr.png")
    
    def GetTitle(self):
        return "Many"

    def Inputs(self):
        return Many.ilabel

    def Outputs(self):
        return Many.olabel
    

class NodesManager(QDockWidget):
    def __init__(self, text, parent: QWidget = None, flags = Qt.WindowFlags()):
       super(NodesManager, self).__init__(text,parent,flags)
       self.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)

       self.nodesTree = QTreeWidget(self)
       self.setWidget(self.nodesTree)
       self.nodesTree.setDragDropMode(QAbstractItemView.DragOnly);
       self.nodesTree.header().hide();

# user nodes tree
       sub1 = QTreeWidgetItem(self.nodesTree)
       sub1.setText(0,"Some1")
       tMul=QTreeWidgetItem(sub1)
       tMul.setText(0,"Mul")
       sub2 = QTreeWidgetItem(self.nodesTree)
       sub2.setText(0,"Some2")
       tGenerate=QTreeWidgetItem(sub2)
       tGenerate.setText(0,"Generate")
       tMany=QTreeWidgetItem(sub2)
       tMany.setText(0,"Many")


class NodesEditor(QDockWidget):
    nodeCreator = {}##dict()
    
    def __init__(self, text, parent: QWidget = None, flags = Qt.WindowFlags()):
        super(NodesEditor, self).__init__(text,parent,flags)
        
        self.nodesViewArea = QGraphicsView(self)
        self.nodesScene = QGraphicsScene(self)
        self.setWidget(self.nodesViewArea)
        self.nodesViewArea.setAcceptDrops(True)
        self.nodesViewArea.setDragMode(QGraphicsView.RubberBandDrag)
        
        self.nodesViewArea.setScene(self.nodesScene)
        self.nodesScene.installEventFilter(self)

        self.nodes = []
        self.connectFrom = None
        self.connectTo = None
        self.connectItem = None
        self.idconnect = -1

    def addNode(self,node:Node):
        self.nodes.append(node)
        self.InitNodeItems(node)
        return node;

    def DeleteNode(self,node:Node):
        #if node is None:
        #    print("null delete")
        #    return
        self.DeleteConnections(node)
        idn = -1
        for i in range(0,len(self.nodes)):
            if node==self.nodes[i]:
                idn=i
        self.nodesScene.removeItem(node.item)
        node.item=0
        if idn >=0:
            del self.nodes[idn]

    def DeleteConnections(self,node:Node):
        for i in range(0,len(self.nodes)) :
            inputs = self.nodes[i].inputs
            for j in range(0,len(inputs)) :
                if inputs[j].node == node or self.nodes[i]==node :
                    if inputs[j].connector is not None :
                        self.nodesScene.removeItem(inputs[j].connector)
                    inputs[j].connector = None
                    inputs[j].node = None
                    inputs[j].out = 0

    def DeleteConnection(self, item:QGraphicsItem):
        for i in range(0,len(self.nodes)) :
            inputs = self.nodes[i].inputs
            for j in range(0,len(inputs)) :
                if inputs[j].connector == item :
                    if inputs[j].connector is not None :
                        self.nodesScene.removeItem(inputs[j].connector)
                    inputs[j].connector = None
                    inputs[j].node = None
                    inputs[j].out = 0

    def GetNodeItem(self, item:QGraphicsItem):
        node = None
        if item is not None :
            if item.parentItem() is not None:
                item = item.parentItem()
            for i in range(0,len(self.nodes)):
                if self.nodes[i].item == item :
                    node = self.nodes[i]
        return node

    def GetNode(self, pos:QPointF):
        item = self.nodesScene.itemAt(pos,self.nodesViewArea.transform())
        node = None
        if item is not None :
            if item.parentItem() is not None:
                item = item.parentItem()
            for i in range(0,len(self.nodes)):
                if self.nodes[i].item == item :
                    node = self.nodes[i]
        return node

    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyRelease and event.key()==Qt.Key_Delete :
            lists = self.nodesScene.selectedItems()
            for i in range(0,len(lists)):
                if lists[i].type()== 2:#QGraphicsPathItem.Type:
                    self.DeleteConnection(lists[i])
                if lists[i].type()== 3:#QGraphicsRectItem.Type
                    self.DeleteNode(self.GetNodeItem(lists[i]))
            self.clearConnection()
            self.updateConnectors()

        if event.type() == QEvent.GraphicsSceneMousePress and (self.connectTo or self.connectFrom):
            if (event.buttons() & Qt.LeftButton)!=0 :
                p = event.scenePos()
                node = self.GetNode(p)
                if node is not None :
                    if self.connectTo is not None:
                        ido=node.GetOutputId(p)
                        if ido>=0 :
                            self.connectTo.inputs[self.idconnect].node=node
                            self.connectTo.inputs[self.idconnect].out=ido
                    if self.connectFrom is not None:
                        idi=node.GetInputId(p)
                        if idi>=0 :
                            node.inputs[idi].node=self.connectFrom
                            node.inputs[idi].out=self.idconnect
                self.clearConnection()
                self.updateConnectors()
        
        if event.type() == QEvent.GraphicsSceneContextMenu :
            p = event.scenePos()
            node = self.GetNode(p)
            self.clearConnection()
            if node is not None :
                idi=node.GetInputId(p)
                ido=node.GetOutputId(p)
                if idi>=0 :
                    self.connectTo = node
                    self.idconnect = idi;
                elif ido>=0 :
                    self.connectFrom = node
                    self.idconnect = ido;
                if idi>=0 or ido>=0:
                    return True

        if event.type() == QEvent.GraphicsSceneDragEnter or event.type() == QEvent.GraphicsSceneDragMove or event.type() == QEvent.GraphicsSceneDrop:
            event.acceptProposedAction()
            if event.type() == QEvent.GraphicsSceneDrop:
                bytearray = event.mimeData().data(event.mimeData().formats()[0])
                data_items = self.decode_data(bytearray)
                text = data_items[0][Qt.DisplayRole].value()
                if text in NodesEditor.nodeCreator:
                    node = NodesEditor.nodeCreator[text]()
                    self.addNode(node).setPos(event.scenePos())
            return True;
        
        if event.type() == QEvent.GraphicsSceneMouseMove:
            if (event.buttons() & Qt.LeftButton)!=0:
                self.updateConnectors()
            if self.connectTo is not None or self.connectFrom is not None:
                p1 = QPointF(0,0)
                p2 = QPointF(0,0)
                if self.connectFrom is not None :
                    p1 = event.scenePos()
                    p2 = self.connectFrom.GetOutputPoint(self.idconnect)
                if self.connectTo is not None :
                    p1 = self.connectTo.GetInputPoint(self.idconnect)
                    p2 = event.scenePos()
                self.connectItem = self.updateConnector(self.connectItem,p1,p2,False)
                
        return QDockWidget.eventFilter(self, obj, event)

    def decode_data(self, bytearray):
        data = []
        item = {}
        
        ds = QDataStream(bytearray)
        while not ds.atEnd():
        
            row = ds.readInt32()
            column = ds.readInt32()
            
            map_items = ds.readInt32()
            for i in range(map_items):
            
                key = ds.readInt32()
                
                value = QVariant()
                ds >> value
                item[Qt.ItemDataRole(key)]=value
             
            data.append(item)
        
        return data
    
    def InitNodeItems(self,node:Node):
        wr = node.windowRect
        winRect = self.nodesScene.addRect(wr,QPen(),QBrush(QColor(0xFFFFFFFF)))
        winRect.setFlag(QGraphicsItem.ItemIsMovable)
        winRect.setFlag(QGraphicsItem.ItemIsSelectable)
        titleRect = self.nodesScene.addRect(wr.x(),wr.y(),wr.width(),20)
        titleRect.setParentItem(winRect)
        title = self.nodesScene.addText(node.GetTitle())
        title.setParentItem(titleRect)
    
        node.item = winRect
        node.ItemsInit(self.nodesScene)
        winRect.setRect(node.windowRect)

    def updateConnectors(self):
        for i in range (0,len(self.nodes)):
            c = self.nodes[i].inputs
            if len(c)<=0:
                continue
            for j in range (0,len(c)):
                if c[j].node is not None:
                    p1 = self.nodes[i].GetInputPoint(j)
                    p2 = c[j].node.GetOutputPoint(c[j].out)
                    c[j].connector = self.updateConnector(c[j].connector,p1,p2)
                elif (c[j].connector!=None):
                    self.nodesScene.removeItem(c[j].connector)
                    c[j].connector = None
                    
    def updateConnector(self,item:QGraphicsPathItem, p1:QPointF, p2:QPointF, select:bool = True):
        path = QPainterPath(p1)
        path.quadTo(p1+QPointF(-15,0),(p1+p2)*0.5)
        path.quadTo(p2+QPointF(15,0),p2)
        if item is None:
            item = self.nodesScene.addPath(path)
        else :
            item.setPath(path)
        item.setZValue(-1)
        if select:
            item.setFlag(QGraphicsItem.ItemIsSelectable)
        return item

    def clearConnection(self):
        self.connectFrom = None
        self.connectTo = None
        if self.connectItem is not None :
            self.nodesScene.removeItem(self.connectItem)
            self.connectItem = None

    def indexOfNode(self,node):
        for i in range(0,len(self.nodes)):
            if self.nodes[i] == node:
                return i
        return -1

    def SeveToFile(self, path):
        #print(path)
        f = QFile(path)
        if f.open(QIODevice.WriteOnly):
            stream = QTextStream(f)
            stream.setCodec("UTF-8")
            doc = QDomDocument()
            xmlInstruct = doc.createProcessingInstruction ("xml", "version=\"1\" encoding=\"UTF-8\"")
            doc.appendChild(xmlInstruct)
            mainEl = doc.createElement("Nodes")
            doc.appendChild(mainEl)

            for i in range(0,len(self.nodes)):
                objectEl = doc.createElement("Node")
                mainEl.appendChild(objectEl)

                idoEl = doc.createElement("Id")
                objectEl.appendChild(idoEl)
                idoEltext = doc.createTextNode(str(i))
                idoEl.appendChild(idoEltext)

                typeEl = doc.createElement("Type")
                objectEl.appendChild(typeEl)
                nameEltext = doc.createTextNode(type(self.nodes[i]).__name__)
                typeEl.appendChild(nameEltext)

                posEl = doc.createElement("Pos")
                objectEl.appendChild(posEl)
                pos = self.nodes[i].item.scenePos()
                posEltext = doc.createTextNode(str(pos.x()) + "," + str(pos.y()))
                posEl.appendChild(posEltext)

                liksEl = doc.createElement("Links")
                objectEl.appendChild(liksEl)
                inputs = self.nodes[i].inputs;
                for j in range(0,len(inputs)):
                    if inputs[j].node is not None:
                        linkEl = doc.createElement("Link")
                        liksEl.appendChild(linkEl)

                        idEl = doc.createElement("Id")
                        linkEl.appendChild(idEl)
                        idEltext = doc.createTextNode(str(j))
                        idEl.appendChild(idEltext)

                        nodeEl = doc.createElement("Node")
                        linkEl.appendChild(nodeEl)
                        idnEltext = doc.createTextNode(str(self.indexOfNode(inputs[j].node)))
                        nodeEl.appendChild(idnEltext)

                        outEl = doc.createElement("Out")
                        linkEl.appendChild(outEl)
                        outEltext = doc.createTextNode(str(inputs[j].out))
                        outEl.appendChild(outEltext)

            doc.save(stream, 4)
            f.close()
            return True
        return False

    def LoadFromFile(self, path):
        #print(path)
        self.nodes=[]
        self.nodesScene.clear()
        file = QFile(path)
        if not file.open(QIODevice.ReadOnly):
            return False

        doc = QDomDocument()
        if not doc.setContent(file):
            file.close()
            return False
        file.close()

        docElem = doc.documentElement()
        n = docElem.firstChild()
        while not n.isNull():
            en = n.toElement()
            #print(en.tagName())
            if en.tagName() == "Node":
                np = n.firstChild()
                typeN = ""
                pos = QPointF(0,0)
                links = None
                while not np.isNull():
                    enp = np.toElement()
                    if enp.tagName() == "Type":
                        typeN = enp.text()
                    if enp.tagName() == "Pos":
                        post = enp.text().split(",")
                        pos = QPointF(float(post[0]),float(post[1]))
                    if enp.tagName() == "Links":
                        links = np;
                    np = np.nextSibling()
                if typeN in NodesEditor.nodeCreator:
                    node = NodesEditor.nodeCreator[typeN]()
                    self.addNode(node).setPos(pos)
                    if links is not None:
                        nl = links.firstChild()
                        while not nl.isNull():
                            npl = nl.firstChild()
                            idl = -1
                            idn = -1
                            out = -1
                            while not npl.isNull():
                                enpl = npl.toElement()
                                if enpl.tagName() == "Id":
                                    idl = int(enp.text());
                                if enpl.tagName() == "Node":
                                    idn = int(enp.text());
                                if enpl.tagName() == "Out":
                                    out = int(enp.text());
                                npl = npl.nextSibling()
                            if idl>=0:
                                node.inputs[idl].node = self.nodes[idn]
                                node.inputs[idl].out = out
                            nl = nl.nextSibling()
            n = n.nextSibling()
        self.updateConnectors()
        return True

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()

        self.nm = NodesManager("NodesManager",self)
        self.ne = NodesEditor("NodesEditor",self)

        self.addDockWidget(Qt.TopDockWidgetArea, self.nm)
        self.addDockWidget(Qt.TopDockWidgetArea, self.ne)

        self.fileName = ""

        fileMenu = self.menuBar().addMenu("&File")
        openAction = fileMenu.addAction("&Open")
        openAction.setShortcut("Ctrl+O")
        saveAction = fileMenu.addAction("&Save")
        saveAction.setShortcut("Ctrl+S")
        saveAsAction = fileMenu.addAction("&Save as ..")
        exitAction = fileMenu.addAction("E&xit")
        exitAction.setShortcut("Ctrl+Q")
        
        openAction.triggered.connect(self.openNodes)
        saveAction.triggered.connect(self.saveNodes)
        saveAsAction.triggered.connect(self.saveAsNodes)
        exitAction.triggered.connect(self.quitApp)

#register user node
        NodesEditor.nodeCreator["Mul"]=Mul
        NodesEditor.nodeCreator["Generate"]=Generate
        NodesEditor.nodeCreator["Many"]=Many

    def RegisterNode(path,name,cname):
        NodesEditor.nodeCreator[name]=cname
        #self.nm.addPath(path);

    def openNodes(self):
        #print("openNodes")
        format = 'nod'
        initialPath = self.fileName
        if initialPath == "":
            initialPath = QDir.currentPath() + "/untitled." + format

        initialPath, _ = QFileDialog.getOpenFileName(self, "Save As", initialPath,"%s Files (*.%s);;All Files (*)" % (format.upper(), format))
        if initialPath:
            self.fileName = initialPath
            self.ne.LoadFromFile(initialPath)

    def saveNodes(self):
        #print("saveNodes")
        if self.fileName == "":
            self.saveAsNodes()
        else:
            self.ne.SeveToFile(self.fileName)

    def saveAsNodes(self):
        #print("saveAsNodes")
        format = 'nod'
        initialPath = self.fileName
        if initialPath == "":
            initialPath = QDir.currentPath() + "/untitled." + format

        initialPath, _ = QFileDialog.getSaveFileName(self, "Save As", initialPath,"%s Files (*.%s);;All Files (*)" % (format.upper(), format))
        if initialPath:
            self.fileName = initialPath
            self.ne.SeveToFile(initialPath)
        
    def quitApp(self):
        #print("quitApp")
        #close()
        QApplication.instance().quit()

if __name__ == '__main__':

    import sys

    app = QApplication(sys.argv)
    main = MainWindow()
    main.show()
    sys.exit(app.exec_())
