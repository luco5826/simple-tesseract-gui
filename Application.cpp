#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
    MainWindowTess *window = new MainWindowTess();
    window->setWindowTitle("Prova tesseract");
    window->setMinimumSize(800, 600);
    window->show();
    app->exec();
}