#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage> // For storing original images

// Forward declarations
class QHBoxLayout;
class QWidget;
class ImageViewer;

/**
 * @brief The MainWindow class is the main application window.
 *
 * It sets up the UI for displaying two input images and their difference.
 * It uses ImageViewer widgets for each display area, allowing zoom and scroll.
 * It also handles loading images from command-line arguments and generating
 * the difference image.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructs the main application window.
     * @param parent The parent widget, typically null for the main window.
     */
    MainWindow(QWidget *parent = nullptr);

    // Destructor can be default: ~MainWindow() {}

    /**
     * @brief Loads the two specified images from file paths and displays them.
     *        Also triggers the generation and display of their difference.
     * @param path1 File path to the first image.
     * @param path2 File path to the second image.
     */
    void loadAndDisplayImages(const QString &path1, const QString &path2);

private:
    /**
     * @brief Generates an image highlighting the differences between
     *        currentOriginalImage1 and currentOriginalImage2, then displays it.
     *        Handles cases like images not being loaded or having different dimensions by
     *        displaying appropriate messages as images in the difference viewer.
     */
    void generateAndDisplayDifference();

    /**
     * @brief Helper function to create a UI group for an ImageViewer.
     * This group includes the ImageViewer within a QScrollArea and zoom control buttons.
     * @param viewer Pointer to the ImageViewer instance.
     * @param title The title for the group box.
     * @return A QWidget* representing the created group.
     */
    QWidget* createImageViewGroup(ImageViewer* viewer, const QString& title);

    // UI Elements
    QWidget *mainCentralWidget;     // The central widget of the main window.
    QHBoxLayout *mainLayout;        // Main horizontal layout for the three image groups.

    ImageViewer *imgViewer1;        // Viewer for the first input image.
    ImageViewer *imgViewer2;        // Viewer for the second input image.
    ImageViewer *imgViewerDiff;     // Viewer for the difference image.

    // Image Data
    QImage currentOriginalImage1;   // Stores the loaded first original image.
    QImage currentOriginalImage2;   // Stores the loaded second original image.
};

#endif // MAINWINDOW_H
