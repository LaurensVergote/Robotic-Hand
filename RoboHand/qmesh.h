/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef QMESH_H
#define QMESH_H

#include <qgl.h>
#include <QMatrix4x4>

#include "geometry.h"
#include "assimp/scene.h"

class QMesh
{
public:
    QMesh(Geometry *g, aiMesh *mesh, aiNode *node);
    QMesh(Geometry *g);
    enum Smoothing { Faceted, Smooth };
    void setSmoothing(Smoothing s) { sm = s; }
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    void draw(aiMatrix4x4 transform) const;
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &an, const QVector3D &bn, const QVector3D &cn);
    void addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d);
    void qSetColor(QColor c);

    GLuint start;
    GLuint count;
    GLuint initv;

    GLfloat faceColor[4];
    QMatrix4x4 mat;
    aiNode *node;
    Smoothing sm;
    Geometry *geom;
};

#endif // QMESH_H
