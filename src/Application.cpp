#include <QApplication>
#include <QFile>
#include <QDir>
#include <QStyleFactory>
#include "MainWindow.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (!QDir::home().exists(".mytesseract"))
    {
        qDebug() << QDir::home().mkdir(".mytesseract");
    }

    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Set CSS style for this
    QFile cssFile("src/ui/tesseract.css");
    if (cssFile.open(QFile::ReadOnly | QFile::Text))
    {
        // Read in entire CSS from file and set as the application stylesheet
        QTextStream cssTxtStream(&cssFile);
        app.setStyleSheet(cssTxtStream.readAll());
        cssFile.close();
    }
    else
    {
        qDebug() << cssFile.errorString();
    }

    MainWindowTess *window = new MainWindowTess();
    window->setWindowTitle("Prova tesseract");
    window->setMinimumSize(800, 600);
    window->show();
    app.exec();
}