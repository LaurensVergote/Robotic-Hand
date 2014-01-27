/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "pickingray.h"
#include "geometry.h"


PickingRay::PickingRay(Geometry *g, QVector3D startPoint, QVector3D endPoint) : QMesh(g)
{
    qSetColor(QColor(Qt::blue));
    this->startPoint = startPoint;
    this->endPoint = endPoint;

    directionVector = endPoint - startPoint;
    //directionVector = startPoint - endPoint;
    directionVector.normalize();

    //createRayMesh();
    //g->finalize();
}

PickingRay::PickingRay(Geometry *g, float startX, float startY, float startZ, float endX, float endY, float endZ) : QMesh(g)
{
    qSetColor(QColor(Qt::blue));
    startPoint.setX(startX);
    startPoint.setY(startY);
    startPoint.setZ(startZ);

    endPoint.setX(endX);
    endPoint.setY(endY);
    endPoint.setZ(endZ);

    directionVector = endPoint - startPoint;
    //directionVector = startPoint - endPoint;
    directionVector.normalize();

    //createRayMesh();
    //g->finalize();
}

PickingRay::~PickingRay()
{
}

void PickingRay::createRayMesh()
{
    //Front quad
    QVector3D& v0 = QVector3D(startPoint.x()-3,startPoint.y()-3,startPoint.z());
    QVector3D& v1 = QVector3D(startPoint.x()+3,startPoint.y()-3,startPoint.z());
    QVector3D& v2 = QVector3D(startPoint.x()-3,startPoint.y()+3,startPoint.z());
    QVector3D& v3 = QVector3D(startPoint.x()+3,startPoint.y()+3,startPoint.z());

    //Back quad
    QVector3D& v4 = QVector3D(endPoint.x()-3,endPoint.y()-3, endPoint.z());
    QVector3D& v5 = QVector3D(endPoint.x()+3,endPoint.y()-3, endPoint.z());
    QVector3D& v6 = QVector3D(endPoint.x()-3,endPoint.y()+3, endPoint.z());
    QVector3D& v7 = QVector3D(endPoint.x()+3,endPoint.y()+3, endPoint.z());

    //Front
    addTri(v0,v1,v2,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v1,v2,v3,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));

    //Bottom
    addTri(v2,v3,v6,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v3,v6,v7,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));

    //Left
    addTri(v0,v2,v4,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v2,v4,v6,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));

    //Top
    addTri(v0,v4,v5,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v0,v1,v5,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));

    //Back
    addTri(v4,v5,v6,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v5,v6,v7,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));

    //Right
    addTri(v1,v3,v5,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
    addTri(v3,v5,v7,QVector3D(0,0,0),QVector3D(0,0,0),QVector3D(0,0,0));
}

bool PickingRay::intersects(const QMesh *qmesh, const aiMatrix4x4 transform, float& distanceToCamera)
{
    //Iterate vertices
    int faceIndex = qmesh->start;
    std::vector<float> distances;
    distanceToCamera = FLT_MAX;
    bool intersected = false;

    for(int i = 0 ; i < qmesh->count ; i+=3)
    {

        int f0 = geom->faces[faceIndex+i];
        int f1 = geom->faces[faceIndex+i+1];
        int f2 = geom->faces[faceIndex+i+2];

        //Local space coordinates
        QVector3D qV0(geom->vertices[f0].x(), geom->vertices[f0].y(), geom->vertices[f0].z());
        QVector3D qV1(geom->vertices[f1].x(), geom->vertices[f1].y(), geom->vertices[f1].z());
        QVector3D qV2(geom->vertices[f2].x(), geom->vertices[f2].y(), geom->vertices[f2].z());

        //World space coordinates
        QVector3D v0, v1, v2;
        transformVertex(qV0, transform, v0);
        transformVertex(qV1, transform, v1);
        transformVertex(qV2, transform, v2);

        distances.push_back(v0.distanceToPoint(startPoint));
        distances.push_back(v1.distanceToPoint(startPoint));
        distances.push_back(v2.distanceToPoint(startPoint));

        if(!intersected){
            //Check intersection
            intersected = checkIntersection(startPoint,directionVector,v0,v1,v2);
        }
    }

    if(intersected)
    {
        float temp = 0;
        for(int i = 0 ; i < qmesh->count ; i++)
        {
            temp += distances[i];
        }
        distanceToCamera = temp / (float)qmesh->count;
    }

    //delete[] distances;

    return intersected;
}

void PickingRay::transformVertex(const QVector3D vertex, const aiMatrix4x4 transform, QVector3D &transformedVertex)
{
    //Calculate eye coordinates
    QVector4D V0(vertex.x(), vertex.y(), vertex.z(), 1.0);
    QVector4D eyeV0(V0.x()*transform.a1 + V0.y()*transform.a2 + V0.z()*transform.a3 + V0.w()*transform.a4,
                    V0.x()*transform.b1 + V0.y()*transform.b2 + V0.z()*transform.b3 + V0.w()*transform.b4,
                    V0.x()*transform.c1 + V0.y()*transform.c2 + V0.z()*transform.c3 + V0.w()*transform.c4,
                    V0.x()*transform.d1 + V0.y()*transform.d2 + V0.z()*transform.d3 + V0.w()*transform.d4);

    transformedVertex.setX(eyeV0.x());
    transformedVertex.setY(eyeV0.y());
    transformedVertex.setZ(eyeV0.z());
}

bool PickingRay::checkIntersection(QVector3D start, QVector3D direction, QVector3D v0, QVector3D v1, QVector3D v2)
{
    QVector3D E1 = v1 - v0;
    QVector3D E2 = v2 - v0;

    QVector3D H = QVector3D::crossProduct(direction,E2);
    float A = QVector3D::dotProduct(E1, H);

    //If ray is parallel to the plane, then it does not intersect it.
    if(A > -0.00001 && A < 0.00001)
        return false;

    float F = 1.0/A;

    QVector3D S = start - v0;

    float U = F * QVector3D::dotProduct(S,H);

    //If the value of the U coordinate is outside the range of values inside the triangle, then the ray has intersected the plane outside the triangle.
    if (U < 0.0 || U > 1.0)
        return false;

    QVector3D Q = QVector3D::crossProduct(S,E1);

    float V = F * QVector3D::dotProduct(direction,Q);

    //If the value of the V coordinate is outside the range of values inside the triangle, then the ray has intersected the plane outside the triangle.
    //U + V cannot exceed 1.0 or the point is not in the triangle.
    //Imagine the triangle as half of a square. U=1 V=1 would be in the lower left hand corner which would be in the second triangle making up the square.
    if (V < 0.0 || (U+V) > 1.0)
        return false;

    //The line intersects the triangle.
    return true;

    //Note that U and V are the texture coordinates of the point intersecting the triangle.
}

void PickingRay::draw(){
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glPushMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);

    const GLuint *indices = geom->faces.constData();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices + start);
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
