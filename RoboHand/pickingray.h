/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef PICKINGRAY_H
#define PICKINGRAY_H

#include <QVector3D>
#include "qmesh.h"

class PickingRay : public QMesh
{
public:
    PickingRay(Geometry *g, QVector3D start, QVector3D end);
    PickingRay(Geometry *g, float startX, float startY, float startZ,
               float endX, float endY, float endZ);
    ~PickingRay();
    bool intersects(const QMesh* qmesh, const aiMatrix4x4 transform, float& distanceToCamera);
    void draw();
private:
    void transformVertex(const QVector3D vertex, const aiMatrix4x4 transform, QVector3D& transformedVertex);
    bool checkIntersection(QVector3D start, QVector3D direction, QVector3D v0, QVector3D v1, QVector3D v2);
    void createRayMesh();
    QVector3D startPoint;
    QVector3D endPoint;
    QVector3D directionVector;
};

#endif // PICKINGRAY_H
