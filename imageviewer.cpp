#include "imageviewer.h"
#include <QPainter>
#include <QDebug> // For qWarning, qInfo, etc.
#include <QWheelEvent>
#include <QColorConstants>

/**
 * @brief Constructs an ImageViewer widget.
 * Initializes scaleFactor to 1.0.
 * @param parent The parent widget.
 */
ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent), scaleFactor(1.0) {
    // Widget properties like size policy can be set here if needed,
    // but QScrollArea often manages this well.
}

/**
 * @brief Sets the image to be displayed in the viewer.
 * This will replace the current image. The zoom level is reset to 1.0 (100%).
 * If the provided image is null, the viewer will display a "No hay imagen cargada" message.
 * @param image The QImage to display.
 */
void ImageViewer::setImage(const QImage &image) {
    originalImage = image;
    scaleFactor = 1.0;     // Reset scale for a new image

    updateScaledImage();   // Generate the displayable pixmap from the new image and scale

    // Adjust the widget's preferred size based on the new pixmap.
    // This is crucial for QScrollArea to correctly show scrollbars.
    adjustSize();
    update();              // Schedule a repaint of the widget.
}

/**
 * @brief Sets the zoom scale factor for displaying the image.
 * The factor is clamped between 0.01 (1%) and 100.0 (10000%).
 * A factor of 1.0 means the image is displayed at its original size.
 * @param factor The new scale factor.
 */
void ImageViewer::setScaleFactor(qreal factor) {
    // Clamp the factor to a reasonable range
    if (factor < 0.01) factor = 0.01;
    if (factor > 100.0) factor = 100.0;

    // Avoid unnecessary updates if the scale factor hasn't significantly changed
    if (qFuzzyCompare(scaleFactor, factor))
        return;

    scaleFactor = factor;

    updateScaledImage(); // Re-generate the scaled pixmap

    adjustSize(); // Update widget's preferred size for QScrollArea
    update();     // Repaint the widget
    emit zoomChanged(scaleFactor); // Notify listeners about the zoom change
}

/**
 * @brief Updates the internal QPixmap (`displayPixmap`) by scaling the `originalImage`
 * according to the current `scaleFactor`. Handles cases where `originalImage` is null
 * or scaling results in an invalid size.
 */
void ImageViewer::updateScaledImage() {
    if (originalImage.isNull()) {
        displayPixmap = QPixmap(); // Clear pixmap if no original image
        return;
    }

    QSize newScaledSize = originalImage.size() * scaleFactor;

    // Ensure scaled dimensions are at least 1x1 if the original image was valid but scale is very small
    if (newScaledSize.width() < 1 && originalImage.width() > 0) newScaledSize.setWidth(1);
    if (newScaledSize.height() < 1 && originalImage.height() > 0) newScaledSize.setHeight(1);

    if (newScaledSize.isValid() && newScaledSize.width() > 0 && newScaledSize.height() > 0) {
        displayPixmap = QPixmap::fromImage(originalImage.scaled(
            newScaledSize,
            Qt::KeepAspectRatio,        // Preserve aspect ratio during scaling
            Qt::SmoothTransformation    // Use high-quality scaling
        ));
    } else {
        // If scaling results in an invalid size (e.g., original image itself was 0x0)
        displayPixmap = QPixmap();
    }
}

/**
 * @brief Handles paint events for the widget.
 * Draws the background, then the scaled image (if available), or a message
 * indicating "No hay imagen cargada" or "Vista no disponible".
 * The image is centered within the widget's bounds if it's smaller.
 * @param event The paint event.
 */
void ImageViewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event); // Mark event as unused as we repaint the whole widget
    QPainter painter(this);

    // Fill background (e.g., light gray to see image boundaries)
    painter.fillRect(rect(), QColorConstants::Svg::gainsboro);

    if (displayPixmap.isNull()) {
        painter.setPen(QColorConstants::Svg::black);
        QString message;
        if (originalImage.isNull()) {
            message = "No hay imagen cargada";
        } else {
            // This case implies originalImage is valid, but displayPixmap couldn't be made
            message = "Vista no disponible (ajuste zoom)";
        }
        painter.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, message);
        return;
    }

    // Calculate top-left point to center the pixmap if it's smaller than the widget area
    int x = (this->width() - displayPixmap.width()) / 2;
    int y = (this->height() - displayPixmap.height()) / 2;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    painter.drawPixmap(x, y, displayPixmap);
}

/**
 * @brief Provides a size hint based on the current scaled pixmap's size.
 * If no pixmap is available, a default size is returned.
 * @return The preferred QSize for this widget.
 */
QSize ImageViewer::sizeHint() const {
    if (!displayPixmap.isNull()) {
        return displayPixmap.size();
    }
    return QSize(200, 200); // Default size for "No Image" text area
}

/**
 * @brief Zooms in on the image by a predefined factor (1.25x).
 * This is a public slot that can be connected to UI elements.
 */
void ImageViewer::zoomIn() {
    setScaleFactor(scaleFactor * 1.25);
}

/**
 * @brief Zooms out of the image by a predefined factor (1.25x).
 * This is a public slot that can be connected to UI elements.
 */
void ImageViewer::zoomOut() {
    setScaleFactor(scaleFactor / 1.25);
}

/**
 * @brief Resets the zoom level to 100% (original image size).
 * Does nothing if no image is currently loaded.
 * This is a public slot.
 */
void ImageViewer::resetZoom() {
    if (!originalImage.isNull()) {
       setScaleFactor(1.0);
    }
}

/**
 * @brief Handles mouse wheel events to implement zooming.
 * Zooms in if the wheel delta is positive (scrolled up/away),
 * and zooms out if the delta is negative (scrolled down/towards).
 * The event is accepted if an image is loaded and zooming occurs.
 * @param event The QWheelEvent.
 */
void ImageViewer::wheelEvent(QWheelEvent *event) {
    if (originalImage.isNull() || (event->angleDelta().y() == 0) ) {
        event->ignore(); // Pass event to parent if no image or no vertical scroll
        return;
    }

    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
    event->accept(); // Event handled
}
