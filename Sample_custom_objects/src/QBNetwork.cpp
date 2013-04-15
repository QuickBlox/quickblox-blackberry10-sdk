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

#include "QBCustomObject.hpp"

using namespace bb::system;
using namespace bb::data;

QBNetwork::QBNetwork() {
	m_networkAccessManager = new QNetworkAccessManager(this);
	connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(onNetworkManagerReply(QNetworkReply *)));
	void finished(QNetworkReply *reply);
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

void QBNetwork::registerUser(QString userName, QString userPassword) {
	QBLOX_LOGIN = userName;
	QBLOX_PASSWORD = userPassword;

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

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestSessionRegister()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestSessionRegister() {
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
				const DataAccessError err = jda.error();
				emit error(
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

				if (m_token != NULL) {
					//register after we have session
					emit sessionEstablished();
					registerNewUser();
				} else {
					emit error("Wrong reply for register session request");
				}
			}
		} else {
			emit error("Wrong reply for register session request");
		}
		reply->deleteLater();
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
		emit error("Wrong reply for register new user request");
	}

	disconnectFromAll();
}

void QBNetwork::login(QString userName, QString userPassword) {
	QBLOX_LOGIN = userName;
	QBLOX_PASSWORD = userPassword;

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
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestSessionWithLogin()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestSessionWithLogin() {
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

			if (m_token != NULL && m_userId != NULL) {
				emit sessionEstablished();
				requestLogin();
			}
		} else {
			emit error("Wrong reply for login session request");
		}
		reply->deleteLater();
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
	if (reply) {
		emit loggedIn();
		reply->deleteLater();
	} else {
		emit error("Wrong reply for login request");
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

void QBNetwork::requestCO(QString className)
{
	const QUrl url(QBLOX_API_SERVER + "data/" + className + ".json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(onRequestCO()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onRequestCO() {
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
				emit coReceived(response);
			}
		} else {
			emit error("Wrong reply for request CO request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for request CO request");
	}

	disconnectFromAll();
}

void QBNetwork::addCO(QBCustomObject* co) {
	const QUrl url(QBLOX_API_SERVER + "data/" + co->className() + ".json");

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
//	qSettings["login"] = QBLOX_LOGIN;
//	qSettings["password"] = QBLOX_PASSWORD;
	if (co->parentId() != QString::null) {
		qSettings["_parent_id"] = co->parentId();
	}

	QVariantMap fieldsMap = co->fields();

	foreach (const QVariant& key, co->fields().keys()) {
		QString keyString = key.toString();
		if (fieldsMap[keyString].canConvert<QVariantList>())
		{
			QString result;
			foreach (const QVariant& arrItem, fieldsMap[keyString].toList()) {
				result += arrItem.toString() + ",";
			}
			qSettings[keyString] = result.left(result.length() - 1);
		}
		else
		{
			qSettings[keyString] = fieldsMap[keyString].toString();
		}
	}


	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->post(request, outRequest.toAscii());

	connect(reply, SIGNAL(finished()), this, SLOT(onAddCO()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onAddCO() {
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
				emit coAdded(response);
			}
		} else {
			emit error("Wrong reply for add CO request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for add CO request");
	}

	disconnectFromAll();
}

void QBNetwork::deleteCO(QBCustomObject* co) {
	const QUrl url(QBLOX_API_SERVER + "data/" + co->className() + "/" + co->id() + ".json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->deleteResource(request);

	connect(reply, SIGNAL(finished()), this, SLOT(onDeleteCO()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onDeleteCO() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply && reply->error() == QNetworkReply::NoError) {
		//get id from url string
		QString idString = reply->url().toString();
		idString = idString.right(idString.length() - idString.lastIndexOf("/") - 1);
		idString = idString.left(idString.lastIndexOf("."));
		emit coDeleted(idString);

		reply->deleteLater();
	} else {
//		emit error("Wrong reply for delete CO request");
	}

	disconnectFromAll();
}

void QBNetwork::updateCO(QBCustomObject* co) {
	QUrl url(QBLOX_API_SERVER + "data/" + co->className() + "/" + co->id() + ".json");

	QList<QPair<QString,QString> > queryItems;

	if (co->parentId() != QString::null) {
		queryItems << QPair<QString,QString>("_parent_id", co->parentId());
	}

	QVariantMap fieldsMap = co->fields();

	foreach (const QVariant& key, co->fields().keys()) {
		QString keyString = key.toString();
		if (fieldsMap[keyString].canConvert<QVariantList>())
		{
			QString result;
			foreach (const QVariant& arrItem, fieldsMap[keyString].toList()) {
				result += arrItem.toString() + ",";
			}
			queryItems << QPair<QString,QString>(keyString, result.left(result.length() - 1));
		}
		else
		{
			queryItems << QPair<QString,QString>(keyString, fieldsMap[keyString].toString());
		}
	}

	url.setQueryItems(queryItems);

	qDebug() << "final update request " << url.toString();

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());

	QNetworkReply* reply = m_networkAccessManager->put(request, QByteArray());

	connect(reply, SIGNAL(finished()), this, SLOT(onUpdateCO()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
			SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void QBNetwork::onUpdateCO() {
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
				emit coUpdated(response);
			}
		} else {
			emit error("Wrong reply for update CO request");
		}
		reply->deleteLater();
	} else {
//		emit error("Wrong reply for update CO request");
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
		else {
			//haven't QB error response
			handleError(reply);
		}
	}
}

void QBNetwork::disconnectFromAll() {
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestSessionRegister()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRegistrationNewUser()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestSessionWithLogin()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestLogin()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onRequestUsers()));
	disconnect(sender(), SIGNAL(finished()), this, SLOT(onSignOut()));
	disconnect(sender(), SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
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
