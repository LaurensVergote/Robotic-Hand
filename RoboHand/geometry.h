/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QVector>
#include <qgl.h>
#include <QVector3D>

class Geometry
{
public:
    Geometry();
    QVector<GLuint> faces;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;

    void appendSmooth(const QVector3D &a, const QVector3D &n, int from);
    void appendFaceted(const QVector3D &a, const QVector3D &n);
    void finalize();
    void loadArrays() const;
};

#endif // GEOMETRY_H
