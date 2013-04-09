/*
 * ImageLoader.cpp
 *
 * Created on: 20.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */


#include "ImageLoader.hpp"

/**
 *  Constructor
 */
ImageLoader::ImageLoader(const QString &blob_uid, QObject* parent) :
		QObject(parent), m_loading(false), m_blob_uid(blob_uid) {
}

/**
 * Destructor
 */
ImageLoader::~ImageLoader() {
}

/**
 * ImageLoader::load()
 *
 * Instruct the QNetworkAccessManager to initialize a network request in asynchronous manner.
 *
 * Also, setup the signal handler to receive the event indicating the network response.
 */
void ImageLoader::load() {
	m_networkAccessManager = new QNetworkAccessManager(this);

	m_loading = true;
	emit loadingChanged();

	requestDownloadFile();
}

/**
 * Request: download file
 */
void ImageLoader::requestDownloadFile() {
	const QUrl url(
			QString("http://api.quickblox.com/") + "blobs/" + m_blob_uid
					+ ".json");

	QNetworkRequest request(url);
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", QBAuth::m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(responseDownloadFile()));
}

void ImageLoader::responseDownloadFile() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {

		QFile sourceFile(
				"app/native/assets/JDataDownloadFile_" + m_blob_uid + ".xml");
		if (!sourceFile.open(QIODevice::WriteOnly))
			return;
		sourceFile.write(reply->readAll());
		sourceFile.close();

		QDomDocument domDoc;
		QFile file(
				"app/native/assets/JDataDownloadFile_" + m_blob_uid + ".xml");
		if (file.open(QIODevice::ReadOnly)) {
			if (domDoc.setContent(&file)) {
				QDomElement domElement = domDoc.documentElement();
				traverseNode(domElement);
			} else {
				qDebug()
						<< "--------------------------------- ERROR ---------------------- ";
			}
		}

		if (reply->error() == QNetworkReply::NoError) {
			//showError("Download was successful");
		} else {
			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			//another error
			showError("QBlox Server Error = " + response);
			return;
		}
	}
	reply->deleteLater();

	QNetworkAccessManager* netManager = new QNetworkAccessManager(this);

	const QUrl url(m_href);
	QNetworkRequest request(url);

	QNetworkReply *replyImage = netManager->get(request);
	connect(replyImage, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

/**
 * Traverse teams
 */
void ImageLoader::traverseNode(const QDomNode& node) {
	QDomNode domNode = node.firstChild();
	while (!domNode.isNull()) {
		if (domNode.isElement()) {
			QDomElement domElement = domNode.toElement();
			if (!domElement.isNull()) {
				if (domElement.tagName() == "a") {
					if (domElement.attribute("href") != NULL) {
						m_href = QString(
								QUrl::fromPercentEncoding(
										domElement.attribute("href").toUtf8()));
						return;
					}
				}
			}
		}
		traverseNode(domNode);
		domNode = domNode.nextSibling();
	}
}

/**
 * ImageLoader::onReplyFinished()
 *
 * Handler for the signal indicating the response for the previous network request.
 *
 * If the result was a success, it will start the thread of constructing the QImage object.
 */
void ImageLoader::onReplyFinished() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {

			const QByteArray data(reply->readAll());

			// Setup the image processing thread
			ImageProcessor *imageProcessor = new ImageProcessor(data);

			connect(imageProcessor, SIGNAL(finished(QImage)), this,
					SLOT(onImageProcessingFinished(QImage)),
					Qt::QueuedConnection);
			imageProcessor->start();

		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}
			m_label =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			emit labelChanged();

			m_loading = false;
			emit loadingChanged();
		}

		reply->deleteLater();
	} else {
		if (reply->error() < 100) {
			showError("Please check your internet connection");
			return;
		}
		m_label = tr("Download failed. Check internet connection");
		emit labelChanged();

		m_loading = false;
		emit loadingChanged();
	}
}

/**
 * ImageLoader::onImageProcessingFinished(const QImage&)
 *
 * Handler for the signal indicating the result of the image processing.
 */
void ImageLoader::onImageProcessingFinished(const QImage &image) {
	const QImage swappedImage = image.rgbSwapped();
	const bb::ImageData imageData = bb::ImageData::fromPixels(
			swappedImage.bits(), bb::PixelFormat::RGBX, swappedImage.width(),
			swappedImage.height(), swappedImage.bytesPerLine());

	m_image = bb::cascades::Image(imageData);
	emit imageChanged();

	m_label.clear();
	emit labelChanged();

	m_loading = false;
	emit loadingChanged();
}

void ImageLoader::showError(const QString &message) {
	toast = new bb::system::SystemToast(this);
	toast->setBody(message);
	toast->show();
}

QVariant ImageLoader::image() const {
	return QVariant::fromValue(m_image);
}

QString ImageLoader::label() const {
	return m_label;
}

bool ImageLoader::loading() const {
	return m_loading;
}
