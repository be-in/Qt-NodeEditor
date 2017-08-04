#include "nodesexample.h"

namespace NodesType
{

#define NULLARRAYSTRING {NULL}
#define CONNECTORS_SRC(type,i,o) \
    int type::InputsCount() { return i; } \
    int type::OutputsCount() { return o; } \
    char** type::Inputs() { return (char**)ilabel; } \
    char** type::Outputs() { return (char**)olabel; } \

const char *Mul::ilabel[]={"A","B"};
const char *Mul::olabel[]={"Result"};
CONNECTORS_SRC(Mul,2,1)

const char *Generate::ilabel[]={"Count","Type"};
const char *Generate::olabel[]={"A","B","C"};
CONNECTORS_SRC(Generate,2,3)

const char *Int::ilabel[]=NULLARRAYSTRING;
const char *Int::olabel[]={"Int"};
CONNECTORS_SRC(Int,0,1)

const char *Many::ilabel[]={"In2","In3","In4","In5"};
const char *Many::olabel[]={"A","B","C","D"};
CONNECTORS_SRC(Many,4,4)

}
