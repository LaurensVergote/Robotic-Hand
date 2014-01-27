/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef SAVENODE_H
#define SAVENODE_H

#include <string>
#include <vector>
#include "qnode.h"
#include "hand.h"

class SaveNode
{
friend class Gesture;
public:
    SaveNode(QNode* qnode, SaveNode* parent = NULL);
    ~SaveNode();

    std::string nodeName;
    aiMatrix4x4 nodeTransform;
    SaveNode *parent;
    std::vector<SaveNode*> children;
private:

};

#endif // SAVENODE_H
