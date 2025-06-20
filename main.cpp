#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>      // For qDebug, qCritical, qInfo
#include <QStringList> // For positional arguments
#include <QTranslator> // For QCoreApplication::translate (optional here, but good practice)
#include <QLibraryInfo> // To load Qt's own translations (e.g., for standard dialogs, not used directly here)

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("ComparadorImagenes");
    QCoreApplication::setApplicationVersion("1.1"); // Updated version example

    // --- Command Line Argument Parsing ---
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QCoreApplication::translate("main",
            "Aplicación de escritorio para comparar dos imágenes.\n"
            "Muestra ambas imágenes y resalta visualmente sus diferencias.\n"
            "Permite zoom y desplazamiento en cada vista de imagen.")
    );
    // Standard options: -h, --help, -v, --version
    parser.addHelpOption();
    parser.addVersionOption();

    // Define positional arguments for the two image paths
    // These are mandatory for the application to run.
    parser.addPositionalArgument("imagen1", QCoreApplication::translate("main", "Ruta al primer archivo de imagen (ej: imagenA.png)."));
    parser.addPositionalArgument("imagen2", QCoreApplication::translate("main", "Ruta al segundo archivo de imagen (ej: imagenB.jpg)."));

    // Process the actual command line arguments provided by the user
    parser.process(a);

    const QStringList positionalArgs = parser.positionalArguments();

    // Validate that exactly two image paths are provided
    if (positionalArgs.count() != 2) {
        // qCritical is suitable for fatal errors that prevent the app from running.
        // Output messages to stderr.
        qCritical().noquote() << QCoreApplication::translate("main", "Error: Se deben proporcionar exactamente dos rutas de archivos de imagen.");
        qCritical().noquote() << QCoreApplication::translate("main", "Uso: %1 <ruta_imagen1> <ruta_imagen2>").arg(QCoreApplication::applicationName());
        qCritical().noquote() << QCoreApplication::translate("main", "Use la opción --help para más detalles.");
        // parser.showHelp(1); // showHelp exits the application. Can be used instead of manual exit.
        return 1; // Exit with an error code
    }

    QString imagePath1 = positionalArgs.at(0);
    QString imagePath2 = positionalArgs.at(1);

    qInfo().noquote() << QString("Procesando Imagen 1: %1").arg(imagePath1);
    qInfo().noquote() << QString("Procesando Imagen 2: %1").arg(imagePath2);

    // --- Application Setup and Execution ---
    MainWindow w;
    w.loadAndDisplayImages(imagePath1, imagePath2); // Load images passed from command line
    w.show();                                       // Display the main window

    return a.exec(); // Enter the Qt event loop
}
