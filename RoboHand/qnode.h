/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef QNODE_H
#define QNODE_H

#include <assimp/scene.h>
#include <vector>
#include "qmesh.h"
#include "assimp/matrix4x4.h"
#include <string>

class QNode
{
public:
    QNode(aiNode *node, const aiScene *scene, QNode *parent, Geometry *g);
    ~QNode();
    std::string mName() const{
        return this->qName;
    }
    void draw() const;

    void setTransform(aiMatrix4x4 transform){
        this->qTransform = transform;
    }

    void setTranslation(float x, float y, float z);
    aiVector3D getTranslation() const { return m_vecTranslation; }
    void translate(const QVector3D &t);

    void setRotation(float deg, QVector3D axis);
    void setRotation(float x,float y,float z,float w);
    aiQuaternion getRotation() const { return m_quaRotation; }
    int getRotationAngle() const;
    void rotate(float rad, QVector3D axis);

    void setScale(float x, float y, float z);
    aiVector3D getScale() { return m_vecScale; }

    void setParent(QNode *parent){m_parent = parent;}
    void addChild(QNode *child);

    aiMatrix4x4 getLocalTransform() const {
        return this->qTransform;
    }
    aiMatrix4x4 getWorldTransform() const {
		if(m_parent != NULL)
            return m_parent->getWorldTransform()*m_parent->getLocalTransform()*node->mTransformation;
        return node->mTransformation;
	}

    QNode* getParent() const{
        return m_parent;
    }

    std::vector<QNode*> m_children;

    const QMesh* getMesh(int index) const{
        if(index < meshes.size() && index >= 0)
            return &(meshes[index]);
        return NULL;
    }
    const int getNumMeshes() const{
        return meshes.size();
    }

    bool isSelectable(){
        return selectable;
    }

    QNode* FindNode(std::string name);

    void changeColor(QColor c);



private:
    void calculateTransform();
    std::string qName;
    bool selectable;
    aiNode *node;
    QNode *m_parent;
    std::vector<QMesh> meshes;
    aiMatrix4x4 qTransform;

    aiVector3D m_vecTranslation;
    aiQuaternion m_quaRotation;
    aiVector3D m_vecScale;

};

#endif // QNODE_H
