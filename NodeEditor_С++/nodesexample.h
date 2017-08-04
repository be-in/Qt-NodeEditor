//
// Panteleymonov Aleksandr Konstantinovich 03.08.2017
//

#ifndef NODESEXAMPLE_H
#define NODESEXAMPLE_H

#include "nodeseditor.h"

namespace NodesType
{

#define CONNECTORS(i,o) \
    static const char* ilabel[];\
    static const char* olabel[]; \
    int InputsCount(); \
    int OutputsCount(); \
    char** Inputs(); \
    char** Outputs(); \

class Mul:public NodesEditor::Node
{
public:
    QString GetTitle() { return "Mul"; }

    CONNECTORS(2,1)
};

class Generate:public NodesEditor::Node
{
public:
    QString GetTitle() { return "Generate"; }

    CONNECTORS(2,3)
};

class Int:public NodesEditor::Node
{
public:
    QString GetTitle() { return "Int"; }

    CONNECTORS(0,1)
};

class Many:public NodesEditor::Node
{
public:
    QString GetTitle() { return "Many"; }

    CONNECTORS(4,4)
};

}

#endif // NODESEXAMPLE_H
