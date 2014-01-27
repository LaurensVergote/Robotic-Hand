/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "hand.h"
#include <queue>

//===============================
//
// Hand
//
//===============================

Hand::Hand(QObject *parent, QString modelPath, qreal scale)
    : QObject(parent)
    , geom(new Geometry())
{
    this->modelPath = modelPath;
    max = QVector3D(0,0,0);
    buildGeometry(scale);
}

Hand::~Hand()
{
    qDeleteAll(handparts);
    delete geom;
}

void Hand::buildGeometry(qreal scale)
{
    Assimp::Importer *importer = new Assimp::Importer();
    const aiScene *scene = importer->ReadFile(modelPath.toStdString().c_str(),aiProcessPreset_TargetRealtime_Fast);

    const char* error = importer->GetErrorString();

    if (!scene)
    {
        std::printf("%s\n",error);
        return;
    }

    //Extract data
    modelRoot = new QNode(scene->mRootNode,scene,NULL,geom);
    //ExtractDataFromScene(scene->mRootNode, scene, geom);
    geom->finalize();

    for(unsigned int i = 0 ; i < geom->vertices.count() ; i++)
    {
        if(fabs(geom->vertices[i].x()) > max.x())
            max.setX(fabs(geom->vertices[i].x()));
        if(fabs(geom->vertices[i].y()) > max.y())
            max.setY(fabs(geom->vertices[i].y()));
        if(fabs(geom->vertices[i].z()) > max.z())
            max.setZ(fabs(geom->vertices[i].z()));
    }
    return;
}

void Hand::ExtractDataFromScene(aiNode* node, const aiScene* scene, Geometry* geom)
{
    for(unsigned int i = 0 ; i < node->mNumChildren ; i++)
    {
        aiNode *newnode = node->mChildren[i];
        ExtractDataFromScene(newnode, scene, geom);
    }

    if(node->mNumMeshes > 0)
    {
        for(int i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            QMesh *qmesh = new QMesh(geom,mesh,node);
            handparts << qmesh;
        }

    }
}

void Hand::draw() const
{

    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    std::queue<QNode*> q;
    q.push(modelRoot);
    while(!q.empty())
    {
        QNode* qnode = q.front();
        q.pop();

        //TEMP
        //if(qnode == allNodes[curAllNodeCounter])
            qnode->draw();

        for (std::vector<QNode*>::iterator it = qnode->m_children.begin() ; it != qnode->m_children.end() ; it++)
        {
            q.push(*it);
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void Hand::rotateFinger(QNode* selectedNode, float deg, QVector3D rotationAxis)
{
    //QNode* selectedNode = modelRoot->FindNode(JointStrings[j]);
    if (selectedNode == NULL)
        return;
    selectedNode->rotate(deg,rotationAxis);

    //Iterate nodes and color them red
    /*
    std::queue<QNode*> q;
    q.push(this->getModelRoot());
    while(!q.empty())
    {
        QNode* qnode = q.front();
        q.pop();
        for (std::vector<QNode*>::const_iterator it = qnode->m_children.begin() ; it != qnode->m_children.end() ; it++)
        {
            q.push(*it);
        }
        qnode->changeColor(QColor(Qt::red));
    }
    */
    this->draw();
}
