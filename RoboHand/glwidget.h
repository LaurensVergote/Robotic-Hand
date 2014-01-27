/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <vector>
#include <fstream>
#include <sstream>
#include <vector>
#include "pickingray.h"
#include "hand.h"
#include "serialcommunicator.h"

using namespace std;

class QtLogo;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    Hand* getHand(){
        return hand;
    }

    QNode* getSelectedNode(){
        return this->selectedNode;
    }

    SerialCommunicator* getSerialCom(){
        return this->serialCom;
    }

    void setHandLock(bool handLock);

    void setupView(bool orthographic);
    void bendFingerTest();

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void serializeAll();
    void changeComPort(int port);
    void changeComPort(QAction* action);
    void changeBaudRate(int rate);
    void changeBaudRate(QAction* action);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void jointSelected();
    void COMPORTChanged(bool change);
    void baudRateChanged(bool change);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void selectJoint();
    PickingRay constructRay(float xPos, float yPos);

private:
    SerialCommunicator* serialCom;
    bool orthographicView;
    bool handLocked;
    bool dragMode;
    Hand *hand;
    QNode* selectedNode;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastMousePressPos, lastMouseReleasePos, lastMousePos;
    QColor qtGreen;
    QColor qtPurple;
    int widgetWidth, widgetHeight;

    static const bool debug = true;
};

#endif
