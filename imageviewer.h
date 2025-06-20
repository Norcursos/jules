#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <qreal>

class QWheelEvent;

/**
 * @brief The ImageViewer class provides a widget for displaying QImage objects
 * with support for zooming and panning (via QScrollArea).
 *
 * It handles scaling of the image and provides methods for zoom control,
 * which can be triggered by UI buttons or mouse wheel events.
 */
class ImageViewer : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs an ImageViewer widget.
     * @param parent The parent widget.
     */
    explicit ImageViewer(QWidget *parent = nullptr);

    /**
     * @brief Sets the image to be displayed.
     * @param image The QImage to display. If the image is null,
     *              the viewer will display a "No hay imagen cargada" message.
     *              Setting a new image resets the zoom level to 100%.
     */
    void setImage(const QImage &image);

    /**
     * @brief Sets the current zoom scale factor for the image.
     * @param factor The new scale factor. Must be greater than 0.
     *               The factor is clamped between 0.01 (1%) and 100.0 (10000%).
     */
    void setScaleFactor(qreal factor);

    /**
     * @brief Returns the current scale factor.
     * @return The current zoom scale factor.
     */
    qreal getScaleFactor() const { return scaleFactor; }

    /**
     * @brief Returns a pointer to the original, unscaled image.
     * @return Const pointer to the original QImage. Can be null if no image is set.
     */
    const QImage* getOriginalImage() const { return &originalImage; }

public slots:
    /**
     * @brief Zooms in on the image by a predefined factor (1.25x).
     */
    void zoomIn();

    /**
     * @brief Zooms out of the image by a predefined factor (1.25x).
     */
    void zoomOut();

    /**
     * @brief Resets the zoom level to 100% (original image size).
     *        Does nothing if no image is loaded.
     */
    void resetZoom();

signals:
    /**
     * @brief Emitted when the zoom scale factor changes.
     * @param newScaleFactor The new zoom scale factor.
     */
    void zoomChanged(qreal newScaleFactor);

protected:
    /**
     * @brief Handles paint events to draw the scaled image or messages.
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Handles mouse wheel events for zooming.
     * @param event The mouse wheel event.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Provides a size hint based on the current scaled pixmap.
     * @return The preferred size for the widget.
     */
    QSize sizeHint() const override;


private:
    /**
     * @brief Updates the internal QPixmap used for display, by scaling the
     *        original image according to the current scale factor.
     */
    void updateScaledImage();

    QImage originalImage;   // Stores the full-resolution original image.
    QPixmap displayPixmap;  // Stores the scaled pixmap that is actually drawn.
    qreal scaleFactor;      // Current zoom scale factor.
};

#endif // IMAGEVIEWER_H
