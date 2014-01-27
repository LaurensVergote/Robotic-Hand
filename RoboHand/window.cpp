/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
//#include <gl/GL.h>

#include "glwidget.h"
#include "window.h"
#include "serialcommunicator.h"

using namespace std;



Window::Window()
{
    this->xmlWriter.setAutoFormatting(true);
    this->xmlWriter.setAutoFormattingIndent(1);

    dirpath = qApp->applicationDirPath();
    glWidget = new GLWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QMenuBar *menuBar = new QMenuBar;
    menuBar->setMaximumHeight(25);
    QToolBar* toolBar = new QToolBar;
    //toolBar->setMaximumHeight(100);

    //-------
    //Menubar
    //-------

    //FileMenu
    QMenu *fileMenu = new QMenu("File",menuBar);
    QAction *fileMenu_New = fileMenu->addAction("New");
    QAction *fileMenu_Save = fileMenu->addAction("Save All");
    QAction *fileMenu_Open = fileMenu->addAction("Open");
    QAction *fileMenu_Close = fileMenu->addAction("Close");

    connect(fileMenu_Open,SIGNAL(triggered()),this,SLOT(open()));
    connect(fileMenu_Close,SIGNAL(triggered()),this,SLOT(close()));

    menuBar->addMenu(fileMenu);

    //ViewMenu
    QMenu *viewMenu = new QMenu("View", menuBar);
    QAction *viewMenu_Sequence = viewMenu->addAction("Sequence");
    viewMenu_Sequence->setCheckable(true);
    viewMenu_Sequence->setChecked(true);
    QAction *viewMenu_Perspective = viewMenu->addAction("Perspective View");
    QAction *viewMenu_Orthographic = viewMenu->addAction("Orthographic View");

    connect(viewMenu_Sequence,SIGNAL(triggered(bool)),this,SLOT(showTimeLine(bool)));
    connect(viewMenu_Perspective,SIGNAL(triggered()),this,SLOT(perspectiveViewClicked()));
    connect(viewMenu_Orthographic,SIGNAL(triggered()),this,SLOT(orthographicViewClicked()));

    menuBar->addMenu(viewMenu);

    //OptionsMenu
    QMenu *optionsMenu = new QMenu("Options", menuBar);

    QMenu *COMPORTMenu = new QMenu("COM Port");
    for(int i = 1 ; i < 20 ; i++)
    {
        QString s = QString("COM");
        s.append(std::to_string(i).c_str());
        QAction *COMPORTMenu_COM = COMPORTMenu->addAction(s);
        COMPORTMenu_COM->setCheckable(true);
        COMPORTMenu_COM->setData(i);
        connect(COMPORTMenu, SIGNAL(triggered(QAction*)), glWidget, SLOT(changeComPort(QAction*)));
        connect(glWidget, SIGNAL(COMPORTChanged(bool)), COMPORTMenu_COM, SLOT(setChecked(bool)));
    }

    QMenu *baudRateMenu = new QMenu("Baud Rate");
    for(int i = 0 ; i < aBRSize ; i++)
    {
        QString s = QString("");
        s.append(std::to_string(acceptedBaudRates[i]).c_str());
        QAction *baudRateMenu_rate = baudRateMenu->addAction(s);
        baudRateMenu_rate->setCheckable(true);
        baudRateMenu_rate->setData(acceptedBaudRates[i]);
        connect(baudRateMenu, SIGNAL(triggered(QAction*)), glWidget, SLOT(changeBaudRate(QAction*)));
        connect(glWidget, SIGNAL(baudRateChanged(bool)), baudRateMenu_rate, SLOT(setChecked(bool)));
    }

    optionsMenu->addMenu(COMPORTMenu);
    optionsMenu->addMenu(baudRateMenu);

    menuBar->addMenu(optionsMenu);

    //HelpMenu
    QMenu *helpMenu = new QMenu("Help", menuBar);
    QAction *helpMenu_About = helpMenu->addAction("About");
    connect(helpMenu_About, SIGNAL(triggered()), this, SLOT(about()));
    QAction *helpMenu_Controls = helpMenu->addAction("Controls");
    connect(helpMenu_Controls, SIGNAL(triggered()), this, SLOT(showControls()));

    menuBar->addMenu(helpMenu);

    //-----------------
    //Sequence timeline
    //-----------------
    sequenceWidget = new QWidget;
    sequenceWidget->setMaximumHeight(100);
    sequenceLayout = new QHBoxLayout;
    sequenceWidget->setLayout(sequenceLayout);

    //-------
    //Toolbar
    //-------

    //History
    history = new QComboBox(toolBar);
    history->setMinimumWidth(200);
    history->setMinimumHeight(30);
    connect(history, SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));

    //Toolbar buttons
    unlockIcon = new QIcon(dirpath + "/icons/" + "Unlock.png");
    lockIcon = new QIcon(dirpath + "/icons/" + "Lock.png");
    toolbarLock = new QPushButton(*unlockIcon, "", toolBar);
    toolbarLock->setCheckable(true);
    toolbarLock->setMinimumSize(QSize(30,30));
    toolbarLock->setIconSize(toolbarLock->minimumSize());
    connect(toolbarLock, SIGNAL(clicked(bool)),this,SLOT(lockHand(bool)));

    /*
    saveIcon = new QIcon(dirpath + "/icons/" + "Save_256x256.png");
    toolbarSave = new QPushButton(*saveIcon, "", toolBar);
    toolbarSave->setMinimumSize(QSize(30,30));
    toolbarSave->setIconSize(toolbarSave->minimumSize());
    connect(toolbarSave,SIGNAL(clicked()),this,SLOT(saveGesture()));
    */

    addIcon = new QIcon(dirpath + "/icons/" + "Add_256x256.png");
    toolbarAdd = new QPushButton(*addIcon, "", toolBar);
    toolbarAdd->setMinimumSize(QSize(30,30));
    toolbarAdd->setIconSize(toolbarAdd->minimumSize());
    connect(toolbarAdd,SIGNAL(clicked()),this,SLOT(toolBarAddClicked()));
    connect(toolbarAdd,SIGNAL(clicked()),this,SLOT(saveGesture()));

    gestureName = new QLineEdit("Gesture name", toolBar);
    gestureName->setMinimumHeight(30);
    connect(gestureName,SIGNAL(textChanged(QString)),this,SLOT(gestureNameChanged(QString)));

    QLabel* rotationAngleLabel = new QLabel(tr("Rotation (°):"));

    rotationAngle = new QLineEdit("0°",toolBar);
    rotationAngle->setMinimumHeight(30);
    rotationAngle->setMaximumWidth(50);
    connect(rotationAngle,SIGNAL(returnPressed()),this,SLOT(rotationAngleEntered()));
    connect(glWidget,SIGNAL(jointSelected()),this,SLOT(rotationAngleChanged()));


    toolBar->addWidget(toolbarLock);
    toolBar->addWidget(history);
    toolBar->addWidget(toolbarAdd);
    //toolBar->addWidget(toolbarSave);
    toolBar->addWidget(gestureName);
    toolBar->addWidget(rotationAngleLabel);
    toolBar->addWidget(rotationAngle);

    mainLayout->addWidget(menuBar);
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(glWidget); //Main Widget
    mainLayout->addWidget(sequenceWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Robot Hand Interface"));
}

void Window::indexChanged(int index)
{

}

void Window::lockHand(bool checked)
{
    checked ? toolbarLock->setIcon(*lockIcon): toolbarLock->setIcon(*unlockIcon);
    glWidget->setHandLock(checked);
}


void Window::bendFinger()
{
    glWidget->bendFingerTest();
}

void Window::toolBarAddClicked()
{
    lockHand(true);
    //Create the Gesture
    history->addItem(gestureName->text()); //TEMP

    QImage img = glWidget->grabFrameBuffer();
    img = img.scaledToHeight(256,Qt::SmoothTransformation);
    Gesture* g = new Gesture(glWidget->getHand(),gestureName->text(), img);

    QString gesturepath = dirpath + tr("/Gestures/") + g->getName() + tr(".png");
    img.save(gesturepath);

    const QIcon *sequenceGesIcon = new QIcon(QPixmap::fromImage(img));
    QPushButton *sequenceGes = new QPushButton(*sequenceGesIcon, "", sequenceWidget);
    sequenceGes->setMinimumHeight(64);
    sequenceGes->setIconSize(sequenceGes->size());
    sequenceLayout->addWidget(sequenceGes);

    gestures.push_back(g);
    //gestures.insert(std::pair<Gesture*,QPushButton*>(g,sequenceGes));

    connect(sequenceGes,SIGNAL(clicked()),g,SLOT(load()));
    connect(sequenceGes,SIGNAL(clicked()),this,SLOT(gestureLoad()));
    connect(sequenceGes,SIGNAL(clicked()),glWidget,SLOT(serializeAll()));

    lockHand(false);
}

void Window::gestureLoad()
{
    glWidget->updateGL();
}

void Window::gestureNameChanged(QString name)
{
    if(name.isEmpty())
        toolbarAdd->setEnabled(false);
    else
        toolbarAdd->setEnabled(true);
}

void Window::rotationAngleEntered()
{
    if(glWidget->getSelectedNode() != NULL)
    {
        int deg = rotationAngle->text().toInt();
        glWidget->getSelectedNode()->setRotation(deg, QVector3D(0,0,1));
        glWidget->updateGL();
    }
}

void Window::rotationAngleChanged()
{
    if(glWidget->getSelectedNode() != NULL)
    {
        int angle = glWidget->getSelectedNode()->getRotationAngle();
        std::string test = std::to_string(angle);
        rotationAngle->setText(tr(test.c_str()));
    }
}

void Window::showTimeLine(bool checked)
{
    if(checked){
        sequenceWidget->show();
    }
    else{
        sequenceWidget->hide();
    }
}

void Window::perspectiveViewClicked()
{
    glWidget->setupView(false);
}

void Window::orthographicViewClicked()
{
    glWidget->setupView(true);
}

void Window::about()
{
    QMessageBox::about(this, tr("About RobotInterface"),
                       tr("Icons provided by http://www.visualpharm.com/ and http://www.webdesignerdepot.com/"));
}

void Window::showControls()
{
    QMessageBox::about(this, tr("RobotInterface Controls"),
                       "Click a finger to select it \nDrag the finger or use the scrollwheel to rotate \nSet COM port and baud rate in options menu");
}

bool Window::saveGesture()
{
    QString name = gestureName->text();

    if (name.isEmpty())
        return false;

    Gesture* g = NULL;
    for(int i = 0 ; i < gestures.size() ; i++)
    {
        if(name == gestures[i]->getName())
        {
            g = gestures[i];
        }
    }

    if(g == NULL)
        return false;

    QString fileName = dirpath + "/Gestures/" + name + ".xml";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QXmlStream Gesture"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    xmlWriter.setDevice(&file);
    xmlWriter.writeStartDocument();
    //Start Gesture
    xmlWriter.writeStartElement("Gesture");
    xmlWriter.writeTextElement("GestureName",g->getName());

    //Iterate all nodes of the gestures and write them
    writeNode(g->getRootNode());

    xmlWriter.writeEndElement(); //End Gesture
    xmlWriter.writeEndDocument();
    file.close();
    return true;
}

void Window::writeNode(SaveNode *snode)
{
    //Begin Node
    xmlWriter.writeStartElement("Node");
    xmlWriter.writeTextElement("NodeName",tr(snode->nodeName.c_str()));

    //Begin Transform
    xmlWriter.writeStartElement("Transform");

    xmlWriter.writeTextElement("a1",tr(std::to_string(snode->nodeTransform.a1).c_str()));
    xmlWriter.writeTextElement("a2",tr(std::to_string(snode->nodeTransform.a2).c_str()));
    xmlWriter.writeTextElement("a3",tr(std::to_string(snode->nodeTransform.a3).c_str()));
    xmlWriter.writeTextElement("a4",tr(std::to_string(snode->nodeTransform.a4).c_str()));

    xmlWriter.writeTextElement("b1",tr(std::to_string(snode->nodeTransform.b1).c_str()));
    xmlWriter.writeTextElement("b2",tr(std::to_string(snode->nodeTransform.b2).c_str()));
    xmlWriter.writeTextElement("b3",tr(std::to_string(snode->nodeTransform.b3).c_str()));
    xmlWriter.writeTextElement("b4",tr(std::to_string(snode->nodeTransform.b4).c_str()));

    xmlWriter.writeTextElement("c1",tr(std::to_string(snode->nodeTransform.c1).c_str()));
    xmlWriter.writeTextElement("c2",tr(std::to_string(snode->nodeTransform.c2).c_str()));
    xmlWriter.writeTextElement("c3",tr(std::to_string(snode->nodeTransform.c3).c_str()));
    xmlWriter.writeTextElement("c4",tr(std::to_string(snode->nodeTransform.c4).c_str()));

    xmlWriter.writeTextElement("d1",tr(std::to_string(snode->nodeTransform.d1).c_str()));
    xmlWriter.writeTextElement("d2",tr(std::to_string(snode->nodeTransform.d2).c_str()));
    xmlWriter.writeTextElement("d3",tr(std::to_string(snode->nodeTransform.d3).c_str()));
    xmlWriter.writeTextElement("d4",tr(std::to_string(snode->nodeTransform.d4).c_str()));
    xmlWriter.writeEndElement();
    //End Transform

    //Begin Children
    xmlWriter.writeStartElement("Children");
    xmlWriter.writeAttribute("Amount", tr(std::to_string(snode->children.size()).c_str()));
    for(int i = 0 ; i < snode->children.size() ; i++)
    {
        writeNode(snode->children[i]);
    }

    xmlWriter.writeEndElement();
    //End Children

    xmlWriter.writeEndElement();
    //End Node
}

bool Window::loadGesture()
{
    QString gesName = tr("Unknown Gesture");
    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "GestureName"){
             gesName = xmlReader.readElementText();
        }
        if (xmlReader.name() == "Node")
            readNode();
    }
    gestureName->setText(gesName);
    toolBarAddClicked();
    return true;
}

void Window::readNode()
{
    Q_ASSERT(xmlReader.isStartElement() && xmlReader.name() == "Node");

    QString nodeName;
    aiMatrix4x4t<float> transform;

    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "NodeName")
            nodeName = xmlReader.readElementText();
        if(xmlReader.name() == "Transform"){
            transform = readTransform();
        }
        if(xmlReader.name() == "Children"){
            readChildren();
        }
    }

    glWidget->getHand()->getModelRoot()->FindNode(nodeName.toStdString())->setTransform(transform);
}

void Window::readChildren()
{
    Q_ASSERT(xmlReader.isStartElement() && xmlReader.name() == "Children");

    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "Node")
            readNode();
    }
}

aiMatrix4x4t<float> Window::readTransform()
{
    Q_ASSERT(xmlReader.isStartElement() && xmlReader.name() == "Transform");

    float a1, a2, a3, a4,
          b1, b2, b3, b4,
          c1, c2, c3, c4,
          d1, d2, d3, d4;

    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "a1")
            a1 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "a2")
            a2 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "a3")
            a3 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "a4")
            a4 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "b1")
            b1 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "b2")
            b2 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "b3")
            b3 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "b4")
            b4 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "c1")
            c1 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "c2")
            c2 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "c3")
            c3 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "c4")
            c4 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "d1")
            d1 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "d2")
            d2 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "d3")
            d3 = xmlReader.readElementText().toFloat();
        else if(xmlReader.name() == "d4")
            d4 = xmlReader.readElementText().toFloat();
    }

    return aiMatrix4x4t<float>(a1, a2, a3, a4,
                               b1, b2, b3, b4,
                               c1, c2, c3, c4,
                               d1, d2, d3, d4);

}

bool Window::open()
{
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Gesture File"),
                                         QDir::currentPath(),
                                         tr("XML Files (*.xml)"));
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QXmlStream Gesture"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    xmlReader.setDevice(&file);

    if (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "Gesture")
            loadGesture();
        else
            xmlReader.raiseError(QObject::tr("The file is not a Gesture XML file"));
    }

    return !xmlReader.error();
}
