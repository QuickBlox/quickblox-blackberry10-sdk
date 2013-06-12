/*
 * QBNetwork.cpp
 *
 * Created on: 27.05.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBNetwork.hpp"

#include <bps/deviceinfo.h>

using namespace bb::system;
using namespace bb::data;

QBNetwork::QBNetwork() {
	m_networkAccessManager = new QNetworkAccessManager(this);

	deviceinfo_identifying_details_t *deviceDetails;
	if (deviceinfo_get_identifying_details(&deviceDetails) == BPS_SUCCESS) {
		int pinValue = deviceinfo_identifying_details_get_pin(deviceDetails);
		m_PIN = QString::number(pinValue, 16);
		deviceinfo_free_identifying_details(&deviceDetails);
	}

	qDebug() << "m_PIN" << m_PIN;

	QBLOX_API_SERVER = "http://api.quickblox.com/"; // QBlox server
	QBLOX_APP_ID = "1770"; // add here your Application id
	QBLOX_AUTH_KEY = "FYahbAzmtUwsKAB"; // add here your Authorization key
	QBLOX_AUTH_SECRET = "EjyJeV9r78v3cOg"; // add here your Authorization secret
	QBLOX_LOGIN = m_PIN; // login
	QBLOX_PASSWORD = m_PIN; // password
}

QBNetwork::~QBNetwork() {
}

/**
 * request session with Login
 */
void QBNetwork::requestSessionWithLogin() {

	qDebug() << "--- requestSessionWithLogin";
	const QUrl url(QBLOX_API_SERVER + "session.json");

	QString timestamp = getTimestampUTC();

	QString postParam = "application_id=";
	postParam += QBLOX_APP_ID;
	postParam += "&auth_key=";
	postParam += QBLOX_AUTH_KEY;
	postParam += "&nonce=";
	postParam += timestamp;
	postParam += "&timestamp=";
	postParam += timestamp;
	postParam += "&user[login]=";
	postParam += QBLOX_LOGIN;
	postParam += "&user[password]=";
	postParam += QBLOX_PASSWORD;

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qUser;
	qUser["login"] = QBLOX_LOGIN;
	qUser["password"] = QBLOX_PASSWORD;

	QVariantMap qSettings;
	qSettings["application_id"] = QBLOX_APP_ID;
	qSettings["auth_key"] = QBLOX_AUTH_KEY;
	qSettings["nonce"] = timestamp;
	qSettings["timestamp"] = timestamp;
	qSettings["signature"] = signature;
	qSettings["user"] = qUser;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(responseSessionWithLogin()));
}

/**
 * response session with Login
 */
void QBNetwork::responseSessionWithLogin() {
	qDebug() << "--- responseRequestSessionWithLogin";
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				JsonDataAccess jda;
				QVariant qtData = jda.loadFromBuffer(response);
				if (jda.hasError()) {
					const DataAccessError err = jda.error();
					showError(
							tr("Error converting JSON data: %1").arg(
									err.errorMessage()));
				} else {
					const QVariantMap object = qtData.value<QVariantMap>();
					QMapIterator<QString, QVariant> it(object);
					while (it.hasNext()) {
						it.next();
						if (it.key() == "session") {
							QVariantMap sessObject = it.value().toMap();
							QMapIterator<QString, QVariant> sit(sessObject);
							while (sit.hasNext()) {
								sit.next();
								if (sit.key() == "user_id") {
									m_userId = sit.value().toString();
								} else if (sit.key() == "token") {
									m_token = sit.value().toString();
								}
							}
						}
					}
				}
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				emit failInternet();
			}
			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "401") {
				requestSession();
			}
		}
		reply->deleteLater();
	}
	requestCreatePushToken();
}

/**
 * request session
 */
void QBNetwork::requestSession() {
	qDebug() << "--- requestSession";
	const QUrl url(QBLOX_API_SERVER + "session.json");

	QString timestamp = getTimestampUTC();

	QString postParam = "application_id=";
	postParam += QBLOX_APP_ID;
	postParam += "&auth_key=";
	postParam += QBLOX_AUTH_KEY;
	postParam += "&nonce=";
	postParam += timestamp;
	postParam += "&timestamp=";
	postParam += timestamp;

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qSettings;
	qSettings["application_id"] = QBLOX_APP_ID;
	qSettings["auth_key"] = QBLOX_AUTH_KEY;
	qSettings["nonce"] = timestamp;
	qSettings["timestamp"] = timestamp;
	qSettings["signature"] = signature;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	qDebug() << "--- request : " << qSettings;

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(responseSession()));
}

/**
 * response session
 */
void QBNetwork::responseSession() {
	qDebug() << "--- responseSession";
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				JsonDataAccess jda;
				QVariant qtData = jda.loadFromBuffer(response);
				if (jda.hasError()) {
					const DataAccessError err = jda.error();
					showError(
							tr("Error converting JSON data: %1").arg(
									err.errorMessage()));
				} else {
					const QVariantMap object = qtData.value<QVariantMap>();
					QMapIterator<QString, QVariant> it(object);
					while (it.hasNext()) {
						it.next();
						if (it.key() == "session") {
							QVariantMap sessObject = it.value().toMap();
							QMapIterator<QString, QVariant> sit(sessObject);
							while (sit.hasNext()) {
								sit.next();
								if (sit.key() == "user_id") {
									m_userId = sit.value().toString();
								}
								if (sit.key() == "token") {
									m_token = sit.value().toString();
								}
							}
						}
					}
				}
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				emit failInternet();
				return;
			}
		}
		reply->deleteLater();
	}
	requestRegisterUser();
}

/**
 * request register user
 */
void QBNetwork::requestRegisterUser() {
	qDebug() << "--- requestRegisterUser";
	qDebug() << "--- token = " << m_token;

	const QUrl url(QBLOX_API_SERVER + "users.json");

	QString timestamp = getTimestampUTC();

	QString postParam = "&user[login]=";
	postParam += QBLOX_LOGIN;
	postParam += "&user[password]=";
	postParam += QBLOX_PASSWORD;
	postParam += "&token=";
	postParam += m_token;

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qUser;
	qUser["login"] = QBLOX_LOGIN;
	qUser["password"] = QBLOX_PASSWORD;

	QVariantMap qSettings;
	qSettings["user"] = qUser;
	qSettings["token"] = m_token;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(responseRegisterUser()));
}

/**
 * response register user
 */
void QBNetwork::responseRegisterUser() {
	qDebug() << "--- responseRegisterUser";
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				qDebug() << "--- response : " << response;

				if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
						== "201") {
					qDebug() << "--- Ok";
				}
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				emit failInternet();
				return;
			}
		}
		reply->deleteLater();
	}
	requestSessionWithLogin();
}

/**
 * request create push token
 */
void QBNetwork::requestCreatePushToken() {
	qDebug() << "--- requestCreatePushToken";
	qDebug() << "--- token = " << m_token;

	const QUrl url(QBLOX_API_SERVER + "push_tokens.json");

	QVariantMap mapPushToken;
	mapPushToken["environment"] = "development";
	mapPushToken["client_identification_sequence"] = m_PIN;

	QVariantMap mapDevice;
	mapDevice["platform"] = "blackberry";
	mapDevice["udid"] = m_PIN;

	QVariantMap qSettings;
	qSettings["push_token"] = mapPushToken;
	qSettings["device"] = mapDevice;
	//qSettings["signature"] = signature;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());

	qDebug() << "--- outRequest.toAscii() = " << outRequest.toAscii();

	connect(reply, SIGNAL(finished()), this, SLOT(responseCreatePushToken()));
}

/**
 * response create push token
 */
void QBNetwork::responseCreatePushToken() {
	qDebug() << "--- responseCreatePushToken";
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	const QByteArray buffer(reply->readAll());
	response = QString::fromUtf8(buffer);
	qDebug() << "--- reply = " << response;

	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				qDebug() << "--- response : " << response;
			}
		} else {
			if (reply->error() < 100) {
				showError("Please check your internet connection");
				emit failInternet();
				return;
			}
		}
		reply->deleteLater();
	}
	requestCreateSubscriptions();
}

/*
 * request create subscriptions
 */
void QBNetwork::requestCreateSubscriptions() {
	qDebug() << "--- requestCreateSubscriptions";
	qDebug() << "--- token = " << m_token;

	const QUrl url(QBLOX_API_SERVER + "subscriptions.json");

	QVariantMap qSettings;
	qSettings["notification_channels"] = "bbps";

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());

	qDebug() << "--- outRequest.toAscii() = " << outRequest.toAscii();

	connect(reply, SIGNAL(finished()), this,
			SLOT(responseCreateSubscriptions()));
}

/*
 * response create subscriptions
 */
void QBNetwork::responseCreateSubscriptions() {
	qDebug() << "--- responseCreateSubscriptions";

	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	const QByteArray buffer(reply->readAll());
	response = QString::fromUtf8(buffer);
	qDebug() << "--- reply = " << response;
	qDebug() << "--- emit response ";

	emit successSubscription();

	reply->deleteLater();
}

/*
 * request remove subscriptions
 */
void QBNetwork::requestRemoveSubscriptions() {
	qDebug() << "--- requestRemoveSubscriptions";

	const QUrl url(QBLOX_API_SERVER + "subscriptions/263396.json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->deleteResource(request);

	connect(reply, SIGNAL(finished()), this,
			SLOT(responseRemoveSubscriptions()));
}

/*
 * response remove subscriptions
 */
void QBNetwork::responseRemoveSubscriptions() {
	qDebug() << "--- responseRemoveSubscriptions";

	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	const QByteArray buffer(reply->readAll());
	response = QString::fromUtf8(buffer);
	qDebug() << "--- reply = " << response;
}

void QBNetwork::showError(const QString &message) {
	m_userName = "";
	emit error(message);
	qDebug() << message;
	SystemToast *toast = new SystemToast(this);
	toast->setBody(message);
	toast->show();
}

QString QBNetwork::getTimestampUTC() {
#if QT_VERSION >= 0x040700
	return QString::number(QDateTime::currentDateTimeUtc().toTime_t());
#else
	return QString::number(QDateTime::currentDateTime().toUTC().toTime_t());
#endif
}

QString QBNetwork::hmac_sha1(const QString &message, const QString &key) {
	QByteArray keyBytes = key.toAscii();
	int keyLength; // Lenght of key word
	const int blockSize = 64; // Both MD5 and SHA-1 have a block size of 64.

	keyLength = keyBytes.size();
	// If key is longer than block size, we need to hash the key
	if (keyLength > blockSize) {
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(keyBytes);
		keyBytes = hash.result();
	}

	// Create the opad and ipad for the hash function.
	QByteArray ipad;
	QByteArray opad;

	ipad.fill(0, blockSize);
	opad.fill(0, blockSize);

	ipad.replace(0, keyBytes.length(), keyBytes);
	opad.replace(0, keyBytes.length(), keyBytes);

	for (int i = 0; i < 64; i++) {
		ipad[i] = ipad[i] ^ 0x36;
		opad[i] = opad[i] ^ 0x5c;
	}

	QByteArray workArray;
	workArray.clear();

	workArray.append(ipad, 64);
	workArray.append(message.toAscii());

	QByteArray sha1 = QCryptographicHash::hash(workArray,
			QCryptographicHash::Sha1);

	workArray.clear();
	workArray.append(opad, 64);
	workArray.append(sha1);

	sha1.clear();

	sha1 = QCryptographicHash::hash(workArray, QCryptographicHash::Sha1);

	return sha1.toHex();
}
