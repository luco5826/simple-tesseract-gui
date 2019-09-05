#ifndef MY_MAIN_WINDOW_HPP
#define MY_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QScrollArea>
#include <QLabel>
#include <QScrollBar>

#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include <QGuiApplication>
#include <QScreen>

#include <QPainter>
#include <QRubberBand>

#include "Recognition.hpp"

class MainWindowTess : public QMainWindow
{
    Q_OBJECT
private:
    QStringList acceptedExtensions = {"jpg", "jpeg", "png"};
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QRubberBand* rubberBand;

    double scaleFactor;
    bool imageLoaded;
    bool cropMode;
    QPoint oldMousePos;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

    void createActions();

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

public:
    MainWindowTess(/* args */);
    ~MainWindowTess();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


public slots:
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
};

#endif