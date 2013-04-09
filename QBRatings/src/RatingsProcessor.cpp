/*
 * RatingsProcessor.cpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "RatingsProcessor.hpp"

#include <bb/ImageData>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

using namespace bb::system;
using namespace bb::data;

/**
 *  Constructor
 */
RatingsProcessor::RatingsProcessor(const QString &imageUrl, QObject* parent) :
		QObject(parent), m_loading(false), m_imageUrl(imageUrl) {
	m_networkAccessManager = new QNetworkAccessManager(this);
	QBLOX_API_SERVER = "http://api.quickblox.com/"; // QBlox server
	QStringList filePathList = m_imageUrl.split('/');
	fileName = filePathList.at(filePathList.count() - 1);
	fileName = fileName.remove('_');
	fileName = fileName.remove('.');
}

/**
 * Destructor
 */
RatingsProcessor::~RatingsProcessor() {
	if (m_thread)
		m_thread->wait();
}

/**
 * RatingsProcessor::load()
 *
 * Instruct the QNetworkAccessManager to initialize a network request in asynchronous manner.
 *
 * Also, setup the signal handler to receive the event indicating the network response.
 */
void RatingsProcessor::load() {
	m_loading = true;
	emit loadingChanged();

	m_ratValue = 0;

	QNetworkAccessManager* netManager = new QNetworkAccessManager(this);

	const QUrl url(m_imageUrl);
	QNetworkRequest request(url);

	QNetworkReply* reply = netManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void RatingsProcessor::slotTimerDelay() {
	requestScoreAverage();
	timerDelay->stop();
}

/**
 * Refresh the rating
 */
void RatingsProcessor::refresh(QString idRat) {
	m_loading = true;
	emit loadingChanged();

	m_idRat = idRat.toInt();

	timerDelay = new QTimer(this);
	QObject::connect(timerDelay, SIGNAL(timeout()), this, SLOT(slotTimerDelay()));
	timerDelay->start(1000);

	m_ratValue = 0;
}

/**
 * RatingsProcessor::onReplyFinished()
 *
 * Handler for the signal indicating the response for the previous network request.
 *
 * If the result was a success, it will start the thread of constructing the QImage object.
 */
void RatingsProcessor::onReplyFinished() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {

				const QByteArray data(reply->readAll());

				// Setup the image processing thread
				ImageProcessor *imageProcessor = new ImageProcessor(data);
				m_thread = new QThread(this);

				// Move the image processor to the worker thread
				imageProcessor->moveToThread(m_thread);

				// Invoke ImageProcessor's start() slot as soon as the worker thread has started
				connect(m_thread, SIGNAL(started()), imageProcessor,
						SLOT(start()));

				// Delete the worker thread automatically after it has finished
				connect(m_thread, SIGNAL(finished()), m_thread,
						SLOT(deleteLater()));

				/*
				 * Invoke our onProcessingFinished slot after the processing has finished.
				 * Since imageProcessor and 'this' are located in different threads we use 'QueuedConnection' to
				 * allow a cross-thread boundary invocation. In this case the QImage parameter is copied in a thread-safe way
				 * from the worker thread to the main thread.
				 */
				connect(imageProcessor, SIGNAL(finished(QImage)), this,
						SLOT(onImageProcessingFinished(QImage)),
						Qt::QueuedConnection);

				// Terminate the thread after the processing has finished
				connect(imageProcessor, SIGNAL(finished(QImage)), m_thread,
						SLOT(quit()));

				m_thread->start();
			}
		} else {
			if (reply->error() < 100) {
				m_loading = false;
				emit loadingChanged();
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

			emit ratValueChanged();
		}

		reply->deleteLater();
	} else {
		m_label = tr("Download failed.");

		emit labelChanged();

		m_loading = false;
		emit loadingChanged();

		emit ratValueChanged();
	}
}

/**
 * RatingsProcessor::onImageProcessingFinished(const QImage&)
 *
 * Handler for the signal indicating the result of the image processing.
 */
void RatingsProcessor::onImageProcessingFinished(const QImage &image) {
	const QImage swappedImage = image.rgbSwapped();
	const bb::ImageData imageData = bb::ImageData::fromPixels(
			swappedImage.bits(), bb::PixelFormat::RGBX, swappedImage.width(),
			swappedImage.height(), swappedImage.bytesPerLine());

	m_image = bb::cascades::Image(imageData);

	requestGameModes();
}

/**
 *
 */
int RatingsProcessor::getId(QString title) {
	int id;
	bool flagIn = false;
	bb::data::JsonDataAccess jda;
	QVariant vlist = jda.load("app/native/assets/JDataRatingsAllGameModes.json");

	QVariantList list = vlist.toList();

	for (QList<QVariant>::iterator it = list.begin(); it != list.end(); it++) {
		QVariantMap map = it->toMap();
		QMapIterator<QString, QVariant> iw(map);
		while (iw.hasNext()) {
			iw.next();
			QVariantMap tw = iw.value().toMap();
			if (tw["title"] == title) {
				flagIn = true;
				id = tw["id"].toInt();
			}
		}
	}
	if (jda.hasError()) {
		id = 0;
		bb::data::DataAccessError error = jda.error();
		qDebug() << "JSON loading error: " << error.errorType() << ": "
				<< error.errorMessage();
	}
	if (!flagIn) {
		return 0;
	}
	return id;
}

/**
 * Request: all gamemodes for this app
 */
void RatingsProcessor::requestGameModes() {
	const QUrl url(QBLOX_API_SERVER + "application/gamemodes.json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", QBAuth::m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(responseGameModes()));
}

/**
 * Response: all gamemodes for this app
 */
void RatingsProcessor::responseGameModes() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile(
						"app/native/assets/JDataRatingsAllGameModes.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());
				sourceFile.close();

				m_idRat = getId(fileName);
				if (m_idRat == 0) {
					requestCreateGameMode(fileName);
				} else {
					requestScoreAverage();
				}
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}

			response =
					tr("ResponseGameModes. Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			requestCreateGameMode(fileName);
		}
		reply->deleteLater();
	}
}

/**
 * Request: create GameMode
 */
void RatingsProcessor::requestCreateGameMode(QString title) {
	const QUrl url(QBLOX_API_SERVER + "gamemodes.json");

	QVariantMap param;
	param["title"] = title;

	QVariantMap qSettings;
	qSettings["gamemode"] = param;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", QBAuth::m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(responseCreateGameMode()));
}

/**
 * Response: create GameMode
 */
void RatingsProcessor::responseCreateGameMode() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile(
						"app/native/assets/JDataRatingsCreateGameMode.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());
				sourceFile.close();

				requestGameModes();
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}
			response =
					tr("ResponseCreateGameMode. Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "422") {
			}
		}
		reply->deleteLater();
	}
}

/**
 * Request: score average
 */
void RatingsProcessor::requestScoreAverage() {
	const QUrl url(
			QBLOX_API_SERVER + "gamemodes/" + QString("").setNum(m_idRat)
					+ "/average.json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", QBAuth::m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(responseScoreAverage()));
}

/**
 * Response: score average
 */
void RatingsProcessor::responseScoreAverage() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QString nameFile = "app/native/assets/JDataRatingsScoreAverage"
						+ QString("").setNum(m_idRat) + ".json";
				QFile sourceFile(nameFile);
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());
				sourceFile.close();

				QString scoreString = response.mid(10, 1);
				m_ratValue = scoreString.toInt();
			}
		} else {
			m_ratValue = 0;
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}
			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			//another error
			showError("ResponseScoreAverage. QBlox Server Error = " + response);
			return;
		}
		reply->deleteLater();
	}

	emit imageChanged();

	m_label.clear();
	emit labelChanged();

	emit idRatChanged();

	m_loading = false;
	emit loadingChanged();

	emit ratValueChanged();

	m_urlImage = m_imageUrl;
	emit urlImageChanged();

	m_showing = true;
	emit showingChanged();

	return;
}

QVariant RatingsProcessor::image() const {
	return QVariant::fromValue(m_image);
}

QString RatingsProcessor::label() const {
	return m_label;
}

QString RatingsProcessor::urlImage() const {
	return m_urlImage;
}

bool RatingsProcessor::loading() const {
	return m_loading;
}

int RatingsProcessor::ratValue() const {
	return m_ratValue;
}

int RatingsProcessor::idRat() const {
	return m_idRat;
}

bool RatingsProcessor::showing() const {
	return m_showing;
}

void RatingsProcessor::showError(const QString &message) {
	SystemToast *toast = new SystemToast(this);
	toast->setBody(message);
	toast->show();
}
