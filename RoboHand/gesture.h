/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef GESTURE_H
#define GESTURE_H

#include <string>
#include "hand.h"
#include "savenode.h"

class Gesture : public QObject
{
    Q_OBJECT
public:
    Gesture(Hand* hand, QString name, QImage img);
    ~Gesture();
    SaveNode* getRootNode() const{
        return rootNode;
    }
    QString getName() const{
        return name;
    }

public slots:
    void load();

private:
    QImage img;
    Hand* hand;
    QString name;
    SaveNode* rootNode;
};

#endif // GESTURE_H
