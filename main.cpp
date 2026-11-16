/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * Initialises the Main ECU Qt application and launches the live ECU window.
 */

#include <QApplication>
#include "MainWindow.h"

/**
 * @brief Application entry point.
 *
 * Creates the QApplication instance,
 * constructs and shows the MainWindow, then enters the Qt event loop.
 *
 * @param argc  Argument count passed from the OS.
 * @param argv  Argument vector passed from the OS.
 * @return      Exit code returned by QApplication::exec().
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

