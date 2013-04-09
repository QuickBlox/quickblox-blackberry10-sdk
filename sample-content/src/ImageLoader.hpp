/*
 * ImageLoader.hpp
 *
 * Created on: 20.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "ImageProcessor.hpp"
#include "QBAuth.hpp"

#include <QtXml>
#include <bb/cascades/Image>
#include <bb/ImageData>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

/*
 * This class retrieves an image from the web, then converts the binary
 * data into a bb::cascades::Image and makes it available through a property.
 */
class ImageLoader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant image READ image NOTIFY imageChanged)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /*
     * Creates a new image loader.
     *
     * @param imageUrl The url to load the image from.
     */
    ImageLoader(const QString &blod_uid, QObject* parent = 0);

    /*
     * Destroys the image loader.
     */
    ~ImageLoader();

    /*
     * Loads the image.
     */
    void load();

	Q_INVOKABLE
	void requestDownloadFile();

public Q_SLOTS:
	void responseDownloadFile();

Q_SIGNALS:
    // The change notification signals of the properties
    void imageChanged();
    void labelChanged();
    void loadingChanged();

private Q_SLOTS:
    /*
     * Response handler for the network operation.
     */
    void onReplyFinished();

    /*
     * Response handler for the image process operation.
     */
    void onImageProcessingFinished(const QImage &image);

private:
    void traverseNode(const QDomNode& node);
    void showError(const QString &message);

    QNetworkAccessManager* m_networkAccessManager;

    // The accessor methods of the properties
    QVariant image() const;
    QString label() const;
    bool loading() const;

    // The property values
    bb::cascades::Image m_image;
    bb::system::SystemToast *toast;
    QString m_label;
    bool m_loading;

    // The URL of the image that should be loaded
    QString m_blob_uid, m_href;

    // The thread context that processes the image
    QPointer<QThread> m_thread;
};

#endif
