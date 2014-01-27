/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef HAND_H
#define HAND_H

#include <QMatrix4x4>
#include <iostream>
#include <QObject>
#include <QColor>
#include <QVector3D>
#include <qmath.h>

#include <qgl.h>

#include <math.h>

#include "geometry.h"
#include "qmesh.h"
#include "qnode.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//===============================
//
// Joint Enum & Helpers
//
//===============================
enum Joint{
    Wrist,
    Palm,
    Thumb,
    Thumb0, Thumb1, Thumb2, Thumb3, Thumb4, Thumb5, Thumb6,
    Index,
    Index0, Index1, Index2, Index3,
    Middle,
    Middle0, Middle1, Middle2, Middle3,
    Ring,
    Ring0, Ring1, Ring2, Ring3,
    Little,
    Little0, Little1, Little2, Little3
};

static const int JointAmount = 30; // amount of Joints
static const std::string JointStrings[] = {
    "n0",
    "n1",
    "n2",
    "j0", "n3", "j1", "n4", "n5", "j2", "j3",
    "j4",
    "n6", "j5", "j6", "d6",
    "j7",
    "n7", "j8", "j9", "d9",
    "j10",
    "n8", "j11", "j12", "d12",
    "j13",
    "n9", "j14", "j15", "d15"
};

static const bool SelectableJoints[] = {
    false,
    false,
    false,
    true, false, true, false, false, true, true,
    true,
    false, true, true, true,
    true,
    false, true, true, true,
    true,
    false, true, true, true,
    true,
    false,
    true, true, true, true
};

//===============================
//
// Hand
//
//===============================

class Hand : public QObject
{
    Q_OBJECT
public:

    Hand(QObject *parent, QString modelPath, qreal scale = 1.0);
    ~Hand();
    void draw() const;
    void rotateFinger(QNode* selectedNode, float deg, QVector3D rotationAxis);

    const QVector3D getMaximum() const
    {
        return max;
    }

    QNode* getModelRoot() const{
        return modelRoot;
    }
    Geometry* getGeometry() const{
        return geom;
    }

private:
    QString modelPath;

    void buildGeometry(qreal scale);
    void ExtractDataFromScene(aiNode* node, const aiScene* scene, Geometry* geom);
    float *vertexArray;
    float *normalArray;
    float *uvArray;
    int numTriangles;

    QVector3D max;
    QNode* modelRoot;
    QList<QMesh*> handparts;
    Geometry *geom;
};

#endif // HAND_H
