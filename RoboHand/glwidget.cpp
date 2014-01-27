/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include <QtWidgets>
#include <QtOpenGL>
#include <math.h>
#include <string>
#include <qgl.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glwidget.h"
#include <queue>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    serialCom = new SerialCommunicator();
    orthographicView = true;
    handLocked = false;
    dragMode = false;
    selectedNode = 0;
    hand = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}



GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::initializeGL()
{
    if(debug){
        const GLubyte* version = glGetString(GL_VERSION);
        cout << "OpenGL version: " << version << endl;
    }
    qglClearColor(qtPurple.dark());

    QString modelPath = qApp->applicationDirPath() + "/Model/" + "hand.3ds";
    hand = new Hand(this, modelPath);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0, 100, 100, 10.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void GLWidget::paintGL()
{
    if (debug)
        cout << "paintGL" << endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -200.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    hand->draw();
    //if(pRay != 0)
    //    pRay->draw();
}

void GLWidget::setupView(bool orthographic)
{
    this->orthographicView = orthographic;
    resizeGL(widgetWidth,widgetHeight);
    updateGL();
}

PickingRay GLWidget::constructRay(float xPos, float yPos){
    //Ray Casting
    double matModelView[16], matProjection[16];
    int viewport[4];
    double startX, startY, startZ;
    double endX, endY, endZ;
    glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
    glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    double winX = (double)xPos;
    double winY = viewport[3] - (double)yPos;
    gluUnProject(winX, winY, 0.0, matModelView, matProjection,
                 viewport, &startX, &startY, &startZ);
    gluUnProject(winX, winY, 1.0, matModelView, matProjection,
                 viewport, &endX, &endY, &endZ);
    return PickingRay(hand->getGeometry(), startX, startY, startZ, endX, endY, endZ);
}

void GLWidget::selectJoint(){
    //Ray Casting
    PickingRay pRay = constructRay(lastMouseReleasePos.x(), lastMouseReleasePos.y());

    //Iterate nodes and find collision with picking ray
    std::queue<QNode*> q;
    q.push(hand->getModelRoot());
    QNode* lastNode = NULL;
    float lastDistance = FLT_MAX;
    while(!q.empty())
    {
        QNode* qnode = q.front();
        qnode->changeColor(QColor(231,158,109));//To default color
        q.pop();
        for (std::vector<QNode*>::const_iterator it = qnode->m_children.begin() ; it != qnode->m_children.end() ; it++)
        {
            q.push(*it);
        }

        //Functional Code
        for(int i = 0 ; i < qnode->getNumMeshes() ; i++)
        {
            const QMesh *qmesh = qnode->getMesh(i);
            if(qmesh != NULL)
            {
                float curDistance = FLT_MAX;
                aiMatrix4x4 translation;
                aiMatrix4x4::Translation(aiVector3D(0,-100,0), translation);
                aiMatrix4x4 transform = translation * qnode->getWorldTransform() * qnode->getLocalTransform();
                if(pRay.intersects(qmesh, transform, curDistance) && curDistance < lastDistance)
                {
                    //Only select the closest mesh to the camera
                    while(qnode->getParent() != NULL && !qnode->isSelectable()){
                        qnode = qnode->getParent();
                    }
                    if(qnode->isSelectable()){
                        lastDistance = curDistance;
                        lastNode = qnode;
                    }
                }
            }
        }
    }

    if(lastNode != NULL){
        lastNode->changeColor(QColor(Qt::green));//To selected color
    }
    if(this->selectedNode != NULL && lastNode == NULL)
    {
        this->serialCom->sendCommand(this->selectedNode->mName(), this->selectedNode->getRotationAngle());
    }
    this->selectedNode = lastNode;
    emit jointSelected();

    updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event){
    float numSteps = (float)event->delta() / (8.0*15.0);
    float rotateRadians = (M_PI/180.0) * numSteps;
    if(selectedNode !=NULL)
    {
        selectedNode->rotate(rotateRadians,QVector3D(0,0,1));
        emit jointSelected();
    }

    updateGL();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!this->handLocked)
        selectJoint();
}

void GLWidget::resizeGL(int width, int height)
{
    if (debug)
        cout << "resizeGL" << endl;

    widgetWidth = width;
    widgetHeight = height;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective
    if(!orthographicView)
        gluPerspective(80.0, aspect, 0.1, 1000.0);

    //Orthographic
    if(orthographicView)
    {
        float max = hand->getMaximum().z()*1.3;
    #ifdef QT_OPENGL_ES_1
        if (width <= height){
            glOrthof(-max, max, -max / aspect, max / aspect, 0.0, 1000.0);
        } else {
            glOrthof(-max * aspect, max * aspect, -max, max, 0.0, 1000.0);
        }
    #else
        if (width <= height){
            glOrtho(-max, max, -max / aspect, max / aspect, 0.0, 1000.0);
        } else {
            glOrtho(-max * aspect, max * aspect, -max, max, 0.0, 1000.0);
        }
    #endif
    }

    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePressPos = event->pos();
    lastMousePos = event->pos();
    if(selectedNode != NULL) //If there is a node selected
    {
        PickingRay pRay = constructRay(lastMousePressPos.x(), lastMouseReleasePos.y());
        aiMatrix4x4 translation;
        aiMatrix4x4::Translation(aiVector3D(0,-100,0), translation);
        aiMatrix4x4 transform = translation * selectedNode->getWorldTransform() * selectedNode->getLocalTransform();
        float distanceToCamera;
        for(int i = 0 ; i < selectedNode->getNumMeshes() ; i++)
        {
            if(pRay.intersects(selectedNode->getMesh(i), transform, distanceToCamera))//If the mouse is currently over the selected node
            {
                dragMode = true;
            }
        }
    }
    //if (debug)
    //    cout << "lastMousePressPos: x = " << lastMousePressPos.x() << " & y = " << lastMousePressPos.y() << std::endl;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    lastMouseReleasePos = event->pos();
    lastMousePos = event->pos();
    dragMode = false;
    if(lastMouseReleasePos.x() == lastMousePressPos.x() &&
       lastMouseReleasePos.y() == lastMousePressPos.y() &&
       !this->handLocked)
        selectJoint();
    //if (debug)
    //    cout << "lastMouseReleasePos: x = " << lastMouseReleasePos.x() << " & y = " << lastMouseReleasePos.y() << std::endl;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();
    if(dragMode)//We are dragging a mesh
    {
        if (event->buttons() & Qt::LeftButton) {
            selectedNode->rotate(dx*M_PI/180.0,QVector3D(0,0,1));
            selectedNode->rotate(dy*M_PI/180.0,QVector3D(0,0,1));
            emit jointSelected();
        }
        updateGL();
    }
    else
    {
        if (event->buttons() & Qt::LeftButton) {
            setXRotation(xRot + 8 * dy);
            setYRotation(yRot + 8 * dx);
        }
        else if (event->buttons() & Qt::RightButton) {
            setXRotation(xRot + 8 * dy);
            setZRotation(zRot + 8 * dx);
        }
    }

    lastMousePos = event->pos();
    //if (debug)
    //    cout << "mousePos: x = " << lastMousePos.x() << " & y = " << lastMousePos.y() << std::endl;
}

void GLWidget::bendFingerTest()
{
    /*
    if(selectedNode == NULL)
        return;
    QVector3D rotationAxis(1,0,0);
    hand->rotateFinger(selectedNode,0.2,rotationAxis);
    */

    updateGL();
}

void GLWidget::setHandLock(bool handLock)
{
    handLocked = handLock;
    if(handLock && selectedNode != NULL){
        selectedNode->changeColor(QColor(231,158,109));
        selectedNode = NULL;
    }
    updateGL();
}

void GLWidget::serializeAll()
{
    std::queue<QNode*> q;
    q.push(hand->getModelRoot());
    while(!q.empty())
    {
        QNode* qnode = q.front();
        q.pop();
        for (std::vector<QNode*>::const_iterator it = qnode->m_children.begin() ; it != qnode->m_children.end() ; it++)
        {
            q.push(*it);
        }

        //Functional Code
        for(int i = 0 ; i < JointAmount; i++)
        {
            if(qnode->mName() == JointStrings[i] && SelectableJoints[i])
            {
                serialCom->sendCommand(qnode->mName(), qnode->getRotationAngle());
                Sleep(2); //Just in case (this is for 9600 baud rate default)
                break;
            }
        }
    }
}

void GLWidget::changeComPort(int port)
{
    serialCom->setComPort(port-1);
    serialCom->openConnection();
}

void GLWidget::changeComPort(QAction *action)
{
    changeComPort(action->data().toInt());
    emit COMPORTChanged(false);
    action->setChecked(true);
}

void GLWidget::changeBaudRate(int rate)
{
    serialCom->setBaudRate(rate);
}

void GLWidget::changeBaudRate(QAction *action)
{
    changeBaudRate(action->data().toInt());
    emit baudRateChanged(false);
    action->setChecked(true);
}
