#include "mainwindow.h"
#include "imageviewer.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <QRgb>
#include <QColorConstants>
#include <QFont>
// #include <QMessageBox> // Could be used for more critical error popups if desired

/**
 * @brief Helper function to create a UI group (QGroupBox) for displaying an image.
 * The group contains a QScrollArea (hosting the provided ImageViewer) and
 * zoom control buttons (+, -, 100%). This function encapsulates the common UI
 * structure for each of the three image display areas.
 * @param viewer The ImageViewer instance to be placed in this group.
 * @param title The title for the QGroupBox.
 * @return A QWidget pointer to the created QGroupBox, or nullptr if viewer is null.
 */
QWidget* MainWindow::createImageViewGroup(ImageViewer* viewer, const QString& title) {
    if (!viewer) {
        qWarning() << "createImageViewGroup called with null viewer for title:" << title;
        return nullptr;
    }

    QGroupBox *groupBox = new QGroupBox(title, this);
    groupBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);

    QScrollArea* scrollArea = new QScrollArea(groupBox);
    // setWidgetResizable(false) means the scroll area respects the widget's sizeHint.
    // The ImageViewer's sizeHint is based on the scaled image size, so scrollbars appear correctly.
    scrollArea->setWidgetResizable(false);
    scrollArea->setWidget(viewer);
    scrollArea->setAlignment(Qt::AlignCenter); // Center viewer if it's smaller than scroll area viewport
    scrollArea->setMinimumSize(250, 250);

    QWidget *zoomControlsWidget = new QWidget(groupBox);
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomControlsWidget);
    zoomLayout->setContentsMargins(5,2,5,2);

    QPushButton *zoomInButton = new QPushButton("+", zoomControlsWidget);
    QPushButton *zoomOutButton = new QPushButton("-", zoomControlsWidget);
    QPushButton *resetButton = new QPushButton("100%", zoomControlsWidget);

    zoomInButton->setToolTip("Acercar (Rueda Arriba)");
    zoomOutButton->setToolTip("Alejar (Rueda Abajo)");
    resetButton->setToolTip("Restablecer Zoom a Tamaño Original (100%)");

    QFont buttonFont = zoomInButton->font();
    buttonFont.setPixelSize(18);
    buttonFont.setBold(true);
    zoomInButton->setFont(buttonFont);
    zoomOutButton->setFont(buttonFont);

    QFont resetFont = resetButton->font();
    resetFont.setPixelSize(14);
    resetButton->setFont(resetFont);

    // Fixed sizes for a consistent button appearance
    zoomInButton->setFixedSize(40, 30);
    zoomOutButton->setFixedSize(40, 30);
    resetButton->setMinimumWidth(60); // Allow text "100%" to fit
    resetButton->setFixedHeight(30);

    zoomLayout->addStretch(); // Push buttons towards the center/right depending on other elements
    zoomLayout->addWidget(zoomInButton);
    zoomLayout->addWidget(zoomOutButton);
    zoomLayout->addWidget(resetButton);
    zoomLayout->addStretch();
    zoomControlsWidget->setLayout(zoomLayout);

    groupBoxLayout->addWidget(scrollArea, 1); // ScrollArea takes most of the vertical space
    groupBoxLayout->addWidget(zoomControlsWidget); // Zoom controls below the scroll area

    groupBox->setLayout(groupBoxLayout);

    // Connect signals from buttons to slots in ImageViewer
    connect(zoomInButton, &QPushButton::clicked, viewer, &ImageViewer::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, viewer, &ImageViewer::zoomOut);
    connect(resetButton, &QPushButton::clicked, viewer, &ImageViewer::resetZoom);

    return groupBox;
}

/**
 * @brief Constructs the MainWindow.
 * Sets up the main application window title, central widget, layout,
 * and initializes the three image view groups.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Comparador de Imágenes Avanzado");
    mainCentralWidget = new QWidget(this);
    setCentralWidget(mainCentralWidget);

    mainLayout = new QHBoxLayout(mainCentralWidget); // Main layout for the three groups

    // Create ImageViewer instances, parented to MainWindow for auto-cleanup
    imgViewer1 = new ImageViewer(this);
    imgViewer2 = new ImageViewer(this);
    imgViewerDiff = new ImageViewer(this);

    // Create the grouped view for each ImageViewer
    QWidget *group1 = createImageViewGroup(imgViewer1, "Imagen Original 1");
    QWidget *group2 = createImageViewGroup(imgViewer2, "Imagen Original 2");
    QWidget *groupDiff = createImageViewGroup(imgViewerDiff, "Resultado de la Comparación");

    // Add groups to the main layout, with a stretch factor of 1 to share space equally
    if (group1) mainLayout->addWidget(group1, 1);
    if (group2) mainLayout->addWidget(group2, 1);
    if (groupDiff) mainLayout->addWidget(groupDiff, 1);

    mainCentralWidget->setLayout(mainLayout);
    resize(1200, 750); // Set a generous initial window size
    setMinimumSize(800, 600); // Set a reasonable minimum window size
}

/**
 * @brief Loads two images from the given file paths, displays them in their respective
 * ImageViewer instances, and then calls generateAndDisplayDifference.
 * If an image fails to load, its corresponding ImageViewer will show a "No Image" message.
 * Error messages for failed loads are printed to the console via qWarning.
 * @param path1 Path to the first image file.
 * @param path2 Path to the second image file.
 */
void MainWindow::loadAndDisplayImages(const QString &path1, const QString &path2) {
    QImage loadedImg1(path1);
    QImage loadedImg2(path2);

    bool img1SuccessfullyLoaded = false;
    if (loadedImg1.isNull()) {
        qWarning() << "Error al cargar imagen 1 desde la ruta:" << path1;
        currentOriginalImage1 = QImage(); // Ensure member is null
    } else {
        currentOriginalImage1 = loadedImg1;
        img1SuccessfullyLoaded = true;
        qInfo() << "Imagen 1 cargada exitosamente:" << path1;
    }
    imgViewer1->setImage(currentOriginalImage1); // ImageViewer handles null image by showing message

    bool img2SuccessfullyLoaded = false;
    if (loadedImg2.isNull()) {
        qWarning() << "Error al cargar imagen 2 desde la ruta:" << path2;
        currentOriginalImage2 = QImage();
    } else {
        currentOriginalImage2 = loadedImg2;
        img2SuccessfullyLoaded = true;
        qInfo() << "Imagen 2 cargada exitosamente:" << path2;
    }
    imgViewer2->setImage(currentOriginalImage2);

    // Generate difference only if both images were loaded successfully
    if (img1SuccessfullyLoaded && img2SuccessfullyLoaded) {
        generateAndDisplayDifference();
    } else {
        // If one or both images failed, clear the diff viewer (it will show "No Image")
        imgViewerDiff->setImage(QImage());
        qInfo() << "Generación de diferencias omitida: una o ambas imágenes no pudieron cargarse.";
    }
}

/**
 * @brief Generates a difference image between currentOriginalImage1 and currentOriginalImage2.
 * - If images are not loaded, or dimensions mismatch, displays an appropriate message image in imgViewerDiff.
 * - If images are identical, displays a message image indicating so.
 * - Otherwise, creates an image where differing pixels are marked in semi-transparent red
 *   and displays it in imgViewerDiff.
 * Operational messages are logged via qInfo/qWarning.
 */
void MainWindow::generateAndDisplayDifference() {
    // Pre-condition check: ensure both original images are loaded.
    if (currentOriginalImage1.isNull() || currentOriginalImage2.isNull()) {
        qWarning() << "generateAndDisplayDifference: Al menos una de las imágenes originales no está cargada. No se generará la diferencia.";
        QImage msgImage(300, 150, QImage::Format_ARGB32_Premultiplied);
        msgImage.fill(QColorConstants::Svg::whitesmoke); // Light background for the message
        QPainter painter(&msgImage);
        painter.setPen(QColorConstants::Svg::crimson); // Red text for error
        painter.setFont(QFont("Arial", 11));
        painter.drawText(msgImage.rect(), Qt::AlignCenter | Qt::TextWordWrap,
                         "No se pueden generar diferencias.\nUna o ambas imágenes originales no se cargaron correctamente.");
        imgViewerDiff->setImage(msgImage);
        return;
    }

    // Check for dimensional equality.
    if (currentOriginalImage1.size() != currentOriginalImage2.size()) {
        qWarning() << "generateAndDisplayDifference: Las imágenes tienen dimensiones diferentes. No se puede generar la diferencia.";
        QImage errorMsgImage(320, 150, QImage::Format_ARGB32_Premultiplied); // Slightly wider for text
        errorMsgImage.fill(QColorConstants::Svg::lightyellow); // Yellowish background for warning
        QPainter painter(&errorMsgImage);
        painter.setPen(QColorConstants::Svg::darkorange); // Dark orange text for warning
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(errorMsgImage.rect(), Qt::AlignCenter | Qt::TextWordWrap,
                         "¡Atención!\nLas imágenes tienen dimensiones diferentes.\nNo se puede realizar una comparación píxel a píxel.");
        imgViewerDiff->setImage(errorMsgImage);
        return;
    }

    // Convert images to a standard format for reliable pixel comparison.
    QImage img1Comparable = currentOriginalImage1.convertToFormat(QImage::Format_ARGB32);
    QImage img2Comparable = currentOriginalImage2.convertToFormat(QImage::Format_ARGB32);

    QImage visualDiffImage(img1Comparable.size(), QImage::Format_ARGB32_Premultiplied);
    visualDiffImage.fill(Qt::transparent); // Start with a transparent canvas for the diff

    int differingPixels = 0;
    // Iterate over each pixel to find differences.
    for (int y = 0; y < img1Comparable.height(); ++y) {
        for (int x = 0; x < img1Comparable.width(); ++x) {
            if (img1Comparable.pixel(x, y) != img2Comparable.pixel(x, y)) {
                visualDiffImage.setPixel(x, y, qRgba(255, 0, 0, 180)); // Semi-transparent red for differences
                differingPixels++;
            }
            // Identical pixels remain transparent on the visualDiffImage.
        }
    }

    if (differingPixels == 0) {
        qInfo() << "Comparación de imágenes: Las imágenes son idénticas.";
        QImage identicalMsgImage(300, 150, QImage::Format_ARGB32_Premultiplied);
        identicalMsgImage.fill(QColorConstants::Svg::honeydew); // Light green background for success
        QPainter painter(&identicalMsgImage);
        painter.setPen(QColorConstants::Svg::darkgreen); // Dark green text
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(identicalMsgImage.rect(), Qt::AlignCenter | Qt::TextWordWrap, "¡Excelente!\nLas imágenes son idénticas.");
        imgViewerDiff->setImage(identicalMsgImage);
    } else {
        qInfo() << QString("Comparación de imágenes: Se encontraron %1 píxeles diferentes.").arg(differingPixels);
        imgViewerDiff->setImage(visualDiffImage); // Display the image with red highlighted differences.
    }
}
