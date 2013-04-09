/*
 * ImageProcessor.cpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "ImageProcessor.hpp"

ImageProcessor::ImageProcessor(const QByteArray &imageData, QObject *parent)
    : QObject(parent)
    , m_data(imageData)
{
}

void ImageProcessor::start()
{
    QImage image;

    image.loadFromData(m_data);

    image = image.scaled(450, 450, Qt::KeepAspectRatioByExpanding);

    // Image processing goes here, example could be adding water mark to the downloaded image

    emit finished(image);
}
