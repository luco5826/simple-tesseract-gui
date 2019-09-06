#include <QMainWindow>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QScrollArea>
#include <QLabel>
#include <QDir>
#include <QFile>

#include <QAction>
#include <QMenu>

#include <QGuiApplication>
#include <QScreen>

#include "MainWindow.hpp"

MainWindowTess::MainWindowTess() : imageLabel(new QLabel),
                                   scrollArea(new QScrollArea),
                                   textWidget(new QDockWidget),
                                   textEditor(new QTextEdit),
                                   scaleFactor(1),
                                   imageLoaded(false),
                                   cropMode(false)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setText("Drop image here!");
    imageLabel->setFont(QFont("Arial", 24, 2));

    rubberBand = new QRubberBand(QRubberBand::Rectangle, imageLabel);
    rubberBand->hide();

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setVisible(true);

    textEditor->setFont(QFont("Arial", 24, 2));
    textWidget->setWidget(textEditor);

    textWidget->setMinimumSize(QGuiApplication::primaryScreen()->availableSize() * 1 / 5);

    setCentralWidget(scrollArea);
    addDockWidget(Qt::LeftDockWidgetArea, textWidget);

    createActions();
    setAcceptDrops(true);

    resize(QGuiApplication::primaryScreen()->availableSize());
}

MainWindowTess::~MainWindowTess()
{
}

void MainWindowTess::createActions()
{
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindowTess::zoomIn, QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindowTess::zoomOut, QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindowTess::normalSize, tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindowTess::fitToWindow, tr("Ctrl+F"));
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));

    QMenu *languageSubmenu = new QMenu("Language");
    toolsMenu->addMenu(languageSubmenu);

    QActionGroup *langActionGroup = new QActionGroup(languageSubmenu);

    itaLangAct = languageSubmenu->addAction(tr("Italian"), this, &MainWindowTess::changeLanguage);
    itaLangAct->setData(QVariant("ita"));
    selectedLanguage = "ita";
    itaLangAct->setCheckable(true);
    itaLangAct->setChecked(true);
    langActionGroup->addAction(itaLangAct);

    engLangAct = languageSubmenu->addAction(tr("English"), this, &MainWindowTess::changeLanguage);
    engLangAct->setData(QVariant("eng"));
    engLangAct->setCheckable(true);
    langActionGroup->addAction(engLangAct);

    langActionGroup->setExclusive(true);
}

void MainWindowTess::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QFileInfo file = QFileInfo(event->mimeData()->urls().at(0).path());
        if (acceptedExtensions.contains(file.suffix()))
        {
            qDebug() << file.suffix() << " is a valid image format";
            event->acceptProposedAction();
        }
    }
}

void MainWindowTess::dropEvent(QDropEvent *event)
{
    // When user drops the image, we load it as a pixmap and activate actions
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
    // Hide the rubberband while scrolling
    if (rubberBand->isVisible())
    {
        rubberBand->hide();
    }

    // Zoom in and out with CTRL + scroll wheel
    if (event->modifiers() == Qt::Modifier::CTRL)
    {
        event->delta() > 0 ? zoomIn() : zoomOut();
    }
}
void MainWindowTess::mousePressEvent(QMouseEvent *event)
{
    // Store mouse press position
    oldMousePos = event->pos();
    rubberBand->hide();
    // If the click happened above the image, start a rubberband
    if (imageLoaded && imageLabel->underMouse())
    {
        if (rubberBand != nullptr)
        {
            delete rubberBand;
        }
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->hide(); // We show the rubberband only when user starts dragging
    }
}
void MainWindowTess::mouseMoveEvent(QMouseEvent *event)
{
    // Move image with mid-click + drag
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
        rubberBand->show();
        cropMode = true;
    }
}

void MainWindowTess::mouseReleaseEvent(QMouseEvent *event)
{
    if (cropMode && imageLoaded)
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

        // Crop out the selected rectangle and save to file
        // TODO: Remove file save/load overhead by passing directly the image to the Recognition::recognize method
        QString savePath = QDir::home().filePath(".mytesseract/cropped.jpeg");
        OriginalPix.toImage().copy(myRect).save(savePath);

        textEditor->setText(Recognition::recognize(savePath.toStdString().data(), selectedLanguage.toStdString().data()));
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
    {
        normalSize();
    }
}

void MainWindowTess::changeLanguage()
{
    QAction *actionSender = qobject_cast<QAction *>(sender());
    selectedLanguage = actionSender->data().toString();
}