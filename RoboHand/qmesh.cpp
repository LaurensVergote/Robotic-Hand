/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "qmesh.h"


//=======================
//
// QMesh
//
//=======================

QMesh::QMesh(Geometry *g, aiMesh *mesh, aiNode *node)
    : start(g->faces.count())
    , count(0)
    , initv(g->vertices.count())
    , sm(QMesh::Faceted)
    , geom(g)
{
    qSetColor(QColor(231,158,109));
    this->node = node;
    for(unsigned int i = 0 ; i < mesh->mNumFaces;i++)
    {
        const aiFace& face = mesh->mFaces[i];

        QVector3D& vertex0 = QVector3D(mesh->mVertices[face.mIndices[0]].x,
                                         mesh->mVertices[face.mIndices[0]].y,
                                         mesh->mVertices[face.mIndices[0]].z);
        QVector3D& vertex1 = QVector3D(mesh->mVertices[face.mIndices[1]].x,
                                         mesh->mVertices[face.mIndices[1]].y,
                                         mesh->mVertices[face.mIndices[1]].z);
        QVector3D& vertex2 = QVector3D(mesh->mVertices[face.mIndices[2]].x,
                                         mesh->mVertices[face.mIndices[2]].y,
                                         mesh->mVertices[face.mIndices[2]].z);
        QVector3D& normal0 = QVector3D(mesh->mNormals[face.mIndices[0]].x,
                mesh->mNormals[face.mIndices[0]].y,
                mesh->mNormals[face.mIndices[0]].z);

        QVector3D& normal1 = QVector3D(mesh->mNormals[face.mIndices[1]].x,
                mesh->mNormals[face.mIndices[1]].y,
                mesh->mNormals[face.mIndices[1]].z);

        QVector3D& normal2 = QVector3D(mesh->mNormals[face.mIndices[2]].x,
                mesh->mNormals[face.mIndices[2]].y,
                mesh->mNormals[face.mIndices[2]].z);


        this->addTri(vertex0, vertex1, vertex2, normal0, normal1, normal2);
    }
}

QMesh::QMesh(Geometry *g)
    : start(g->faces.count())
    , count(0)
    , initv(g->vertices.count())
    , sm(QMesh::Smooth)
    , geom(g)
{

}

void QMesh::rotate(qreal deg, QVector3D axis)
{
    mat.rotate(deg, axis);
}

void QMesh::translate(const QVector3D &t)
{
    mat.translate(t);
}

void QMesh::draw(aiMatrix4x4 transform) const
{
    glPushMatrix();
    aiMatrix4x4 mult;
    aiMatrix4x4::Translation(aiVector3D(0,-100,0), mult);
    transform = mult * transform;
    transform.Transpose(); //Transpose for some reason
    glMultMatrixf(static_cast<GLfloat*>(transform[0]));
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);

    const GLuint *indices = geom->faces.constData();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices + start);
    glPopMatrix();
}

void QMesh::addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &an, const QVector3D &bn, const QVector3D &cn)
{
    QVector3D anorm = an.isNull() ? QVector3D::normal(a, b, c) : an;
    QVector3D bnorm = bn.isNull() ? QVector3D::normal(a, b, c) : bn;
    QVector3D cnorm = cn.isNull() ? QVector3D::normal(a, b, c) : cn;

    if (sm == Smooth) {
        geom->appendSmooth(a, anorm, initv);
        geom->appendSmooth(b, bnorm, initv);
        geom->appendSmooth(c, cnorm, initv);
    } else {
        geom->appendFaceted(a, anorm);
        geom->appendFaceted(b, bnorm);
        geom->appendFaceted(c, cnorm);
    }

    count += 3;
}

void QMesh::addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d)
{
    QVector3D norm = QVector3D::normal(a, b, c);

    if (sm == Smooth) {
        addTri(a, b, c, norm, norm, norm);
        addTri(a, c, d, norm, norm, norm);
    } else {
        // If faceted share the two common vertices
        addTri(a, b, c, norm, norm, norm);
        int k = geom->vertices.count();
        geom->appendSmooth(a, norm, k);
        geom->appendSmooth(c, norm, k);
        geom->appendFaceted(d, norm);
        count += 3;
    }
}

void QMesh::qSetColor(QColor c)
{
    faceColor[0] = c.redF();
    faceColor[1] = c.greenF();
    faceColor[2] = c.blueF();
    faceColor[3] = c.alphaF();
}
