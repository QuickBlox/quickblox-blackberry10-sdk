/*
 * RatingsProcessor.hpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef RatingsProcessor_HPP
#define RatingsProcessor_HPP

#include "QBAuth.hpp"
#include "ImageProcessor.hpp"

#include <QByteArray>

#include <bb/cascades/Image>

class RatingsProcessor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant image READ image NOTIFY imageChanged)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(int idRat READ idRat NOTIFY idRatChanged)
    Q_PROPERTY(int ratValue READ ratValue NOTIFY ratValueChanged)
    Q_PROPERTY(QString urlImage READ urlImage NOTIFY urlImageChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool showing READ showing NOTIFY showingChanged)

public:
    /*
     * Creates a new image loader.
     *
     * @param imageUrl The url to load the image from.
     */
    RatingsProcessor(const QString &imageUrl, QObject* parent = 0);

    /*
     * Destroys the image loader.
     */
    ~RatingsProcessor();

    /*
     * Loads the image.
     */
    void load();

    void refresh(QString idRat);

Q_SIGNALS:
    // The change notification signals of the properties
    void imageChanged();
    void labelChanged();
    void loadingChanged();
    void ratValueChanged();
    void idRatChanged();
    void urlImageChanged();
    void showingChanged();

private Q_SLOTS:
    /*
     * Response handler for the network operation.
     */
    void onReplyFinished();

    /*
     * Response handler for the image process operation.
     */
    void onImageProcessingFinished(const QImage &image);

    void responseCreateGameMode();
    void responseScoreAverage();
    void responseGameModes();
    void slotTimerDelay();

private:
    // The accessor methods of the properties
    QVariant image() const;
    QString label() const;
    bool loading() const;
    int ratValue() const;
    int idRat() const;
    QString urlImage() const;
    bool showing() const;

    int getScore(int id);
    int getId(QString title, QString response);
    void requestCreateGameMode(QString title);
    void requestScoreAverage();
    void requestGameModes();

    void showError(const QString &message);

    // The property values
    bb::cascades::Image m_image;
    QString m_label;
    bool m_loading;
    int m_ratValue;
    int m_idRat;
    bool m_showing;
    QString m_urlImage;

    // The URL of the image that should be loaded
    QString m_imageUrl;

    // The thread context that processes the image
    QPointer<QThread> m_thread;

    QNetworkAccessManager* m_networkAccessManager;
    QString QBLOX_API_SERVER;
    QString fileName;
    QBAuth *auth;

	QTimer *timerDelay;
};

#endif
