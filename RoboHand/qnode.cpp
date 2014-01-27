/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "qnode.h"
#include <queue>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "hand.h"

QNode::QNode(aiNode *node, const aiScene *scene, QNode *parent, Geometry *g)
{
    this->node = node;
    this->qName = std::string(node->mName.C_Str());
    for(int i = 0 ; i < JointAmount ; i++)
    {
        if (this->qName == JointStrings[i])
            this->selectable = SelectableJoints[i];
    }
    //parse meshes into QMeshes
    for (unsigned int i = 0 ; i < node->mNumMeshes ; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        QMesh qmesh(g, mesh, node);
        meshes.push_back(qmesh);
    }


    //set parent
    this->setParent(parent);

    //iterate children
    for (unsigned int i = 0 ; i < node->mNumChildren ; i++)
    {
        QNode *childnode = new QNode(node->mChildren[i], scene, this, g);
        addChild(childnode);
    }

    //Set Transforms
    qTransform = aiMatrix4x4t<float>();
    qTransform.Decompose(m_vecScale, m_quaRotation, m_vecTranslation);
}

int QNode::getRotationAngle() const
{
    float x = m_quaRotation.x;
    float y = m_quaRotation.y;
    float z = m_quaRotation.z;
    float w = m_quaRotation.w;


    float roll = atan2f(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    float pitch = asinf(2 * (w * y - z * y));
    float yaw = atan2f(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    int rotationAngle = (int)ceilf(-yaw * 180.0 / M_PI);

    return rotationAngle;
}

void QNode::setRotation(float deg, QVector3D axis)
{
    float rad = - M_PI * deg / 180.0;
    axis.normalize();
    float x = axis.x() * sinf(rad/2.0);
    float y = axis.y() * sinf(rad/2.0);
    float z = axis.z() * sinf(rad/2.0);
    float w = cosf(rad/2.0);
    this->setRotation(x,y,z,w);
}

void QNode::setRotation(float x, float y, float z, float w)
{
    this->m_quaRotation.x = x;
    this->m_quaRotation.y = y;
    this->m_quaRotation.z = z;
    this->m_quaRotation.w = w;
    this->calculateTransform();
}

void QNode::setScale(float x, float y, float z)
{
    this->m_vecScale.x = x;
    this->m_vecScale.y = y;
    this->m_vecScale.z = z;
    this->calculateTransform();
}

void QNode::setTranslation(float x, float y, float z)
{
    this->m_vecTranslation.x = x;
    this->m_vecTranslation.y = y;
    this->m_vecTranslation.z = z;
    this->calculateTransform();
}

void QNode::calculateTransform()
{
    aiMatrix4x4t<float> scaling(m_vecScale.x, 0.0, 0.0, 0.0,
                                0.0, m_vecScale.y, 0.0, 0.0,
                                0.0, 0.0, m_vecScale.z, 0.0,
                                0.0, 0.0, 0.0, 1.0);
    aiMatrix4x4t<float> translation(1.0, 0.0, 0.0, 0.0,
                                    0.0, 1.0, 0.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0,
                                    m_vecTranslation.x, m_vecTranslation.y, m_vecTranslation.z, 1.0);

    float x = m_quaRotation.x;
    float y = m_quaRotation.y;
    float z = m_quaRotation.z;
    float w = m_quaRotation.w;
    const float normalized = 1.0f/sqrtf(x*x+y*y+z*z+w*w);
    x *= normalized;
    y *= normalized;
    z *= normalized;
    w *= normalized;

    aiMatrix4x4t<float> rotation(
        1.0f - 2.0f*y*y - 2.0f*z*z, 2.0f*x*y - 2.0f*z*w, 2.0f*x*z + 2.0f*y*w, 0.0f,
        2.0f*x*y + 2.0f*z*w, 1.0f - 2.0f*x*x - 2.0f*z*z, 2.0f*y*z - 2.0f*x*w, 0.0f,
        2.0f*x*z - 2.0f*y*w, 2.0f*y*z + 2.0f*x*w, 1.0f - 2.0f*x*x - 2.0f*y*y, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    //rotation.Transpose();

    this->qTransform = scaling * rotation * translation;
}

void QNode::rotate(float rad, QVector3D axis)
{
    aiMatrix4x4t<float> tempRot;
    aiMatrix4x4t<float>::Rotation(rad,aiVector3t<float>(axis.x(),axis.y(),axis.z()),tempRot);
    qTransform = qTransform * tempRot;
    qTransform.Decompose(m_vecScale, m_quaRotation, m_vecTranslation);
}

void QNode::translate(const QVector3D &t)
{
    aiMatrix4x4t<float> tempTrans;
    aiMatrix4x4t<float>::Translation(aiVector3t<float>(t.x(),t.y(),t.z()),tempTrans);
    qTransform = qTransform * tempTrans;
    qTransform.Decompose(m_vecScale, m_quaRotation, m_vecTranslation);
}

void QNode::addChild(QNode *child){
    m_children.push_back(child);
}

QNode* QNode::FindNode(std::string name){
    if (m_children.size() < 1)
        return NULL;
    std::queue<QNode*> nodeQueue;
    nodeQueue.push(this);
    while (!nodeQueue.empty()){
        QNode* curNode = nodeQueue.front();
        nodeQueue.pop();
        if(curNode->mName() == name)
            return curNode;
        for (int i = 0 ; i < curNode->m_children.size() ; i++){
            nodeQueue.push(curNode->m_children[i]);
        }
    }
    return NULL;
}

void QNode::draw() const {
    for(std::vector<QMesh>::const_iterator it = meshes.begin() ; it != meshes.end() ; it++)
    {
        it->draw(getWorldTransform()*this->getLocalTransform());
    }
}

void QNode::changeColor(QColor c){
    for (int i = 0 ; i < meshes.size() ; i++)
    {
        meshes[i].qSetColor(c);
    }
    for (int i = 0 ; i < m_children.size() ; i++)
    {
        m_children[i]->changeColor(c);
    }
}

QNode::~QNode(){

}
