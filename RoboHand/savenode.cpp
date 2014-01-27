/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "savenode.h"

SaveNode::SaveNode(QNode* qnode, SaveNode* parent){
    this->parent = parent;

    this->nodeName = qnode->mName();

    this->nodeTransform = qnode->getLocalTransform();

    for (unsigned int i = 0 ; i < qnode->m_children.size() ; i++){
        SaveNode* child = new SaveNode(qnode->m_children[i], this);
        children.push_back(child);
    }
}

SaveNode::~SaveNode(){
    for(std::vector<SaveNode*>::iterator it = children.begin() ; it != children.end() ; it++){
        (*it)->~SaveNode();
        delete (*it);
    }
    delete parent;
}
