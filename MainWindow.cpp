#include <QMainWindow>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QScrollArea>
#include <QLabel>

#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include <QGuiApplication>
#include <QScreen>

#include "MainWindow.hpp"

void MainWindowTess::createActions()
{

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindowTess::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindowTess::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindowTess::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindowTess::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

MainWindowTess::MainWindowTess(/* args */) : imageLabel(new QLabel), scrollArea(new QScrollArea), scaleFactor(1), imageLoaded(false), cropMode(false)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->hide();

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setVisible(true);

    setCentralWidget(scrollArea);

    createActions();
    setAcceptDrops(true);

    resize(QGuiApplication::primaryScreen()->availableSize());
}

MainWindowTess::~MainWindowTess()
{
}

void MainWindowTess::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasUrls())
    {
        QUrl fileUrl = event->mimeData()->urls().at(0);
        QFileInfo file = QFileInfo(fileUrl.path());
        if (acceptedExtensions.contains(file.suffix()))
        {
            qDebug() << file.suffix() << " is a valid image format";
            event->acceptProposedAction();
        }
    }
}

void MainWindowTess::dropEvent(QDropEvent *event)
{
    QPixmap pixmap = QPixmap(event->mimeData()->urls().at(0).path());
    imageLabel->setPixmap(pixmap);
    imageLoaded = true;

    normalSize();
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    normalSizeAct->setEnabled(true);
}

void MainWindowTess::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::Modifier::CTRL)
    {
        if (event->delta() > 0)
        {
            zoomIn();
        }
        else
        {
            zoomOut();
        }
    }
}
void MainWindowTess::mousePressEvent(QMouseEvent *event)
{

    oldMousePos = event->pos();
    if (imageLabel->underMouse())
    {
        if (rubberBand != nullptr)
        {
            delete rubberBand;
        }
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->show();
    }
}
void MainWindowTess::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::MidButton)
    {
        QPoint offset = QPoint(event->pos().x() - oldMousePos.x(), event->pos().y() - oldMousePos.y());
        QPoint oldPos = imageLabel->pos();

        imageLabel->move(oldPos.x() + offset.x(), oldPos.y() + offset.y());
        oldMousePos = event->pos();
    }
    else if (event->buttons() == Qt::LeftButton)
    {
        rubberBand->setGeometry(QRect(oldMousePos, event->pos()).normalized());
        cropMode = true;
    }
}

void MainWindowTess::mouseReleaseEvent(QMouseEvent *event)
{
    if (cropMode)
    {
        // Copied from stackOverflow (https://stackoverflow.com/questions/11881989/using-qrubberband-to-crop-image-in-qt)
        QPoint a = mapToGlobal(oldMousePos);
        QPoint b = event->globalPos();

        a = imageLabel->mapFromGlobal(a);
        b = imageLabel->mapFromGlobal(b);

        QPixmap OriginalPix(*(imageLabel->pixmap()));
        double sx = imageLabel->rect().width();
        double sy = imageLabel->rect().height();
        sx = OriginalPix.width() / sx;
        sy = OriginalPix.height() / sy;
        a.setX((int)(a.x() * sx));
        b.setX((int)(b.x() * sx));
        a.setY((int)(a.y() * sy));
        b.setY((int)(b.y() * sy));

        QRect myRect(a, b);

        QImage newImage;
        newImage = OriginalPix.toImage();
        
        QImage copyImage;
        copyImage = newImage.copy(myRect);
                copyImage.save("prova.jpeg");
        qDebug() << Recognition::recognize(copyImage).data();
        cropMode = false;
    }
}

void MainWindowTess::scaleImage(double factor)
{
    if (!imageLoaded)
    {
        return;
    }
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindowTess::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void MainWindowTess::zoomIn()
{
    scaleImage(1.05);
}

void MainWindowTess::zoomOut()
{
    scaleImage(0.95);
}

void MainWindowTess::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindowTess::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
}