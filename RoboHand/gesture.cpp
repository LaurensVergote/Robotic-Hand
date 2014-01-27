/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "gesture.h"
#include "queue"

Gesture::Gesture(Hand* hand, QString name, QImage img)
{
    this->name = name;
    this->hand = hand;
    this->img = img;
    this->rootNode = new SaveNode(hand->getModelRoot());

}

Gesture::~Gesture(){
    this->rootNode->~SaveNode();
}

void Gesture::load()
{
    std::queue<SaveNode*> q;
    q.push(rootNode);
    while(!q.empty())
    {
        SaveNode* snode = q.front();
        q.pop();
        for (std::vector<SaveNode*>::iterator it = snode->children.begin() ; it != snode->children.end() ; it++)
        {
            q.push(*it);
        }

        //Functional Code
        QNode* qnode = hand->getModelRoot()->FindNode(snode->nodeName);
        qnode->setTransform(snode->nodeTransform);
    }
}
