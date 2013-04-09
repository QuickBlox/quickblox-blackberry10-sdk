/*
 * ImageProcessor.hpp
 *
 * Created on: 20.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef IMAGESCALER_HPP
#define IMAGESCALER_HPP

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtGui/QImage>

/**
 * @short A class to convert raw image data into a QImage and process it further.
 *
 * The class is designed to be used inside a QThread by providing a separated
 * start() slot and a finished() signal.
 */
class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    /*
     * Creates a new image processor.
     *
     * @param imageData The raw image data.
     * @param parent The parent object.
     */
    ImageProcessor(const QByteArray &imageData, QObject *parent = 0);

//public Q_SLOTS:
    /*
     * Starts the actual operation.
     */
    void start();

Q_SIGNALS:
    /*
     * This signal is emitted after the operation has finished.
     *
     * @param image The processed image.
     */
    void finished(const QImage &image);

private:
    // The raw image data
    QByteArray m_data;
};

#endif
