/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QPushButton>
#include <map>
#include "gesture.h"
#include <QtWidgets>
#include <QtGui>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE
class GLWidget;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();
    QString dirpath;

public slots:


protected:

signals:
    void handLockPressed(bool checked);

private slots:
    /*
    void fileMenuNew();
    void fileMenuOpen();
    void fileMenuSave();
    void fileMenuClose();

    void lockHand(bool checked);
    */
    bool saveGesture();
    bool loadGesture();
    bool open();

    void perspectiveViewClicked();
    void orthographicViewClicked();

    void toolBarAddClicked();
    void gestureNameChanged(QString name);
    void rotationAngleEntered();
    void rotationAngleChanged();
    void gestureLoad();
    void showTimeLine(bool checked);

    void lockHand(bool checked);
    void indexChanged(int index);
    void bendFinger();
    void about();
    void showControls();

private:
    //Functions
    void writeNode(SaveNode* snode);
    aiMatrix4x4t<float> readTransform();
    void readNode();
    void readChildren();

    //FileParsers
    QXmlStreamWriter xmlWriter;
    QXmlStreamReader xmlReader;

    //Not UI elements
    std::vector<Gesture*> gestures;
    //std::map<Gesture*, QPushButton*> gestures;
    GLWidget *glWidget;

    //UI elements
    QComboBox *history;
    const QIcon *unlockIcon;
    const QIcon *lockIcon;
    const QIcon *saveIcon;
    const QIcon *addIcon;
    QPushButton *toolbarLock;
    QPushButton *toolbarSave;
    QPushButton *toolbarAdd;
    QLineEdit *gestureName;
    QLineEdit *rotationAngle;

    QHBoxLayout *sequenceLayout;
    QWidget *sequenceWidget;

};

#endif
