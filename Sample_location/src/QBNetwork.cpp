/*
 * QBNetwork.cpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBNetwork.hpp"

#include "QBGlobal.hpp"

#include "QBUtils.hpp"

#include "QBLGeoData.hpp"

using namespace bb::system;
using namespace bb::data;

QBNetwork::QBNetwork() :
		QBLOX_LOGIN(QString::null), QBLOX_PASSWORD(QString::null), m_userId(
				QString::null) {
	m_networkAccessManager = new QNetworkAccessManager(this);
	connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(onNetworkManagerReply(QNetworkReply *)));
}

QBNetwork::~QBNetwork() {
	disconnect(m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(onNetworkManagerReply(QNetworkReply *)));
	delete m_networkAccessManager;
}

QString QBNetwork::userId() {
	return m_userId;
}

QString QBNetwork::userPassword() {
	return QBLOX_PASSWORD;
}

QNetworkReply* QBNetwork::getCreateSessionRequest(QString userName,
		QString userPassword) {
	const QUrl url(QBLOX_API_SERVER + "session.json");

	QString timestamp = QString::number(QBUtils::getTimestampUTCMs());

	QString postParam = "application_id=";
	postParam += QBLOX_APP_ID;
	postParam += "&auth_key=";
	postParam += QBLOX_AUTH_KEY;
	postParam += "&nonce=";
	postParam += timestamp;
	postParam += "&timestamp=";
	postParam += timestamp;

	if (userName != QString::null && userPassword != QString::null) {
		postParam += "&user[login]=";
		postParam += userName;
		postParam += "&user[password]=";
		postParam += userPassword;
	}

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qSettings;
	qSettings["application_id"] = QBLOX_APP_ID;
	qSettings["auth_key"] = QBLOX_AUTH_KEY;
	qSettings["nonce"] = timestamp;
	qSettings["timestamp"] = timestamp;
	qSettings["signature"] = signature;

	if (userName != QString::null && userPassword != QString::null) {
		QVariantMap qUser;
		qUser["login"] = userName;
		qUser["password"] = userPassword;
		qSettings["user"] = qUser;
	}

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	qDebug() << "url " << request.url();
	qDebug() << "rawHeaderList " << request.rawHeaderList();
	qDebug() << "data " << QString(outRequest);

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());

	return reply;
}

bool QBNetwork::handleSessionResponse() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
//				const DataAccessError err = jda.error();
//				emit error(
//						tr("Error converting JSON data: %1").arg(
//								err.errorMessage()));
				return false;
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

				return true;
			}
		} else {
			return false;
		}
		reply->deleteLater();
	} else {
		return false;
	}

	return false;
}

void QBNetwork::registerUser(QString userName, QString userPassword) {
	QBLOX_LOGIN = userName;
	QBLOX_PASSWORD = userPassword;

	if (m_token == QString::null) {
		//creating session

		QNetworkReply* reply = getCreateSessionRequest();

		connect(reply, SIGNAL(finished()), this,
				SLOT(onRequestSessionRegister()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
				SLOT(onNetworkError(QNetworkReply::NetworkError)));
	} else {
		registerNewUser();
	}
}

void QBNetwork::onRequestSessionRegister() {
	if (handleSessionResponse()) {
		if (m_token != NULL) {
			//register after we have session
			emit sessionEstablished();
			registerNewUser();
		} else {
			emit error("Wrong reply for register session request");
		}
	} else {
		emit error("Wrong reply for register session request");
	}
	disconnectFromAll();
}

void QBNetwork::registerNewUser() {
	const QUrl url(QBLOX_API_SERVER + "users.json");

	QString timestamp = QString::number(QBUtils::getTimestampUTCMs());

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
	connect(reply, SIGNAL(finished()), this, SLOT(onRegistrationNewUser()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRegistrationNewUser() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	if (reply && reply->error() == QNetworkReply::NoError) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
				const DataAccessError err = jda.error();
				emit error(
						tr("Error converting JSON data: %1").arg(
								err.errorMessage()));
			} else {
				QVariantMap userDetails = qtData.toMap()["user"].toMap();
				if (userDetails.contains("id")) {
					m_userId = userDetails["id"].toString();
				}
			}

			if (m_token != NULL && m_userId != NULL) {
				emit registered();
			}
		} else {
			emit error("Wrong reply for register new user request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for register new user request");
	}
	disconnectFromAll();
}

void QBNetwork::login(QString userName, QString userPassword) {
	QBLOX_LOGIN = userName;
	QBLOX_PASSWORD = userPassword;

	if (m_token == QString::null || m_userId == QString::null) {
		//creating session

		QNetworkReply* reply = getCreateSessionRequest(userName, userPassword);

		connect(reply, SIGNAL(finished()), this,
				SLOT(onRequestSessionWithLogin()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
				SLOT(onNetworkError(QNetworkReply::NetworkError)));
	} else {
		requestLogin();
	}
}

void QBNetwork::onRequestSessionWithLogin() {
	if (handleSessionResponse()) {
		if (m_token != NULL && m_userId != NULL) {
			emit sessionEstablished();
			requestLogin();
		} else {
			emit error("Wrong reply for login session request");
		}
	} else {
		emit error("Wrong reply for login session request");
	}
	disconnectFromAll();
}

void QBNetwork::requestLogin() {
	const QUrl url(QBLOX_API_SERVER + "login.json");

	QString timestamp = QString::number(QBUtils::getTimestampUTCMs());

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
	qSettings["login"] = QBLOX_LOGIN;
	qSettings["password"] = QBLOX_PASSWORD;

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
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestLogin()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestLogin() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	QString response;
	if (reply && reply->error() == QNetworkReply::NoError) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
				const DataAccessError err = jda.error();
				emit error(
						tr("Error converting JSON data: %1").arg(
								err.errorMessage()));
			} else {
				emit loggedIn(response);
			}
		} else {
			emit error("Wrong reply for login request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for login request");
	}
	disconnectFromAll();
}

void QBNetwork::requestUsers() {
	const QUrl url(QBLOX_API_SERVER + "users.json?per_page=200");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(onRequestUsers()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestUsers() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
				const DataAccessError err = jda.error();
				emit error(
						tr("Error converting JSON data: %1").arg(
								err.errorMessage()));
			} else {
				emit usersReceived(response);
			}
		} else {
			emit error("Wrong reply for request users request");
		}
		reply->deleteLater();
	} else {
		emit error("Wrong reply for request users request");
	}
	disconnectFromAll();
}

void QBNetwork::logout() {
	const QUrl url(QBLOX_API_SERVER + "login.json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->deleteResource(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onSignOut()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onSignOut() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	if (reply) {
		emit loggedOut();
		reply->deleteLater();
	} else {
		emit error("Wrong reply for sign out request");
	}
	disconnectFromAll();
}

void QBNetwork::getGeoData() {
	if (m_token == QString::null) {
		//creating session

		QNetworkReply* reply = getCreateSessionRequest();

		connect(reply, SIGNAL(finished()), this,
				SLOT(onGetGeoDataSessionRequest()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
				SLOT(onNetworkError(QNetworkReply::NetworkError)));
	} else {
		requestGeoData();
	}
}

void QBNetwork::onGetGeoDataSessionRequest() {
	if (handleSessionResponse()) {
		if (m_token != NULL) {
			//register after we have session
			emit sessionEstablished();
			requestGeoData();
		} else {
			emit error("Wrong reply for geo data session request");
		}
	} else {
//		emit error("Wrong reply for geo data session request");
	}
	disconnectFromAll();
}

void QBNetwork::createGeoData(double latitude, double longitude, QString status) {
	QBLGeoData* geoData = new QBLGeoData();
	geoData->setLatitude(latitude);
	geoData->setLongitude(longitude);
	geoData->setStatus(status);
	createGeoData(geoData);
	delete geoData;
}

void QBNetwork::createGeoData(QBLGeoData* data) {
	const QUrl url(QBLOX_API_SERVER + "geodata.json");

	QVariantMap coordObject;
	coordObject["latitude"] = QString("%1").arg(data->latitude());
	coordObject["longitude"] = QString("%1").arg(data->longitude());
	coordObject["status"] = data->status();

	QVariantMap qData;
	qData["geo_data"] = coordObject;

	QVariant setData = QVariant(qData);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestCreateGeoData()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestCreateGeoData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply && reply->error() == QNetworkReply::NoError) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
				const DataAccessError err = jda.error();
				emit error(
						tr("Error converting JSON data: %1").arg(
								err.errorMessage()));
			} else {
				emit geoDataCreated(response);
			}
		} else {
			emit error("Wrong reply for request create geo data");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for request CO request");
	}
	disconnectFromAll();
}

void QBNetwork::requestGeoData() {
	const QUrl url(
			QBLOX_API_SERVER
					+ "geodata/find.json?per_page=70&last_only=1&sort_by=created_at");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestGeoData()));
}

void QBNetwork::onRequestGeoData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply && reply->error() == QNetworkReply::NoError) {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);
			if (jda.hasError()) {
				const DataAccessError err = jda.error();
				emit error(
						tr("Error converting JSON data: %1").arg(
								err.errorMessage()));
			} else {
				emit geoDataReceived(response);
			}
		} else {
			emit error("Wrong reply for request geo data request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for request CO request");
	}
	disconnectFromAll();
}

void QBNetwork::handleError(QNetworkReply* reply) {
	if (reply->error() < 100) {
//		emit error("Network error\n" + reply->errorString());
		emit error("Please check your internet connection");
		return;
	}

	emit error(reply->errorString());
}

void QBNetwork::onNetworkError(QNetworkReply::NetworkError er) {
	disconnectFromAll();
}

void QBNetwork::onNetworkManagerReply(QNetworkReply *reply) {
	if (reply->error() == QNetworkReply::NoError) {
//		qDebug() << "success reply";
	} else {
		const int available = reply->bytesAvailable();
		if (available > 0) {
			const QByteArray buffer(reply->readAll());
			QString response = QString::fromUtf8(buffer);

			JsonDataAccess jda;
			QVariant qtData = jda.loadFromBuffer(response);

			//TODO simplest way for change default server responses
			if (response.contains("login")
					&& response.contains("has already been taken")) {
				emit error("Login has already been taken");
			} else if (response.contains("Unauthorized")) {
				emit error("login or password is incorrect");
			} else {
//			if (jda.hasError()) {
				emit error("Error response from QB " + response);
				//			} else {
				//				QString result = "Error response from QB";
				//				QVariantMap errorsMap =  qtData.toMap()["errors"].toMap();
				//				foreach(const QVariant& errorKey, errorsMap.keys())
				//				{
				//					result += "\n" + errorKey.toString() + ":";
				//					foreach(const QVariant& errorValue, errorsMap[errorKey.toString()].toList())
				//					{
				//						result += errorValue.toString() + " ";
				//					}
				//				}
				//				emit error(result);
				//			}
			}

		} else {
			//haven't QB error response
			handleError(reply);
		}
	}
}

void QBNetwork::disconnectFromAll() {
	disconnect(sender(), SIGNAL(finished()), this,
			SLOT(onRequestSessionRegister()));
	disconnect(sender(), SIGNAL(finished()), this,
			SLOT(onRegistrationNewUser()));
	disconnect(sender(), SIGNAL(finished()), this,
			SLOT(onRequestSessionWithLogin()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestLogin()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestUsers()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onSignOut()));
	disconnect(sender(), SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
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
