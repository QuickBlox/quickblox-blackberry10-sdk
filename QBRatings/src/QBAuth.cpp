/*
 * QBAuth.cpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBAuth.hpp"

using namespace bb::system;
using namespace bb::data;

QString QBAuth::m_token = "";

QBAuth::QBAuth() {
	m_networkAccessManager = new QNetworkAccessManager(this);

	QBLOX_API_SERVER = "http://api.quickblox.com/"; // QBlox server
	QBLOX_APP_ID = "1897"; // add here your Application id
	QBLOX_AUTH_KEY = "QQLFF5VBbWUTdCh"; // add here your Authorization key
	QBLOX_AUTH_SECRET = "yWLrGWLeD23Unrk"; // add here your Authorization secret

	successloadGameMode = false;
}

void QBAuth::setCredentials(const QString &login, const QString &password) {
	successLoad = false;
	successShow = false;
	emit loadingChanged();
	emit unauthorizedChanged();
	QBLOX_LOGIN = login; // login
	QBLOX_PASSWORD = password; // password
	timerDelay = new QTimer(this);
	QObject::connect(timerDelay, SIGNAL(timeout()), this, SLOT(slotTimerDelay()));
}

void QBAuth::slotTimerDelay() {
	requestSessionWithLogin();
	timerDelay->stop();
}


/**
 * Stop activityIndicator
 */
bool QBAuth::loading() const {
	if (successLoad) {
		return false;
	}
	return true;
}

bool QBAuth::loadGameMode() const {
	if (successloadGameMode) {
		return false;
	}
	return true;
}

/**
 * Show error
 */
bool QBAuth::unauthorized() const {
	if (successShow) {
		return false;
	}
	return true;
}

/**
 * Request: create score
 */
void QBAuth::requestCreateScore(int id, int value) {
	m_networkAccessManager = new QNetworkAccessManager(this);
	const QUrl url(QBLOX_API_SERVER + "scores.json");

	QVariantMap param;
	param["game_mode_id"] = id;
	param["value"] = value;

	QVariantMap qSettings;
	qSettings["score"] = param;

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
	connect(reply, SIGNAL(finished()), this, SLOT(responseCreateScore()));
}

/**
 * Response: create score
 */
void QBAuth::responseCreateScore() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile("app/native/assets/JDataRatingsScore.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());
			}
		} else {
			return;
		}
		reply->deleteLater();
	}
}

/*
 * Logout
 */
void QBAuth::removeSession() {
	successLoad = false;
	successShow = false;
	emit loadingChanged();
	emit unauthorizedChanged();

	const QUrl url(QBLOX_API_SERVER + "login.json");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->deleteResource(request);

	connect(reply, SIGNAL(finished()), this, SLOT(onSingOut()));
}

/**
 * Registration the new user
 */
void QBAuth::registrationNewUser() {
	const QUrl url(QBLOX_API_SERVER + "users.json");

	QString timestamp = getTimestampUTC();

	QString postParam = "&user[login]=";
	postParam += QBLOX_LOGIN;
	postParam += "&user[password]=";
	postParam += QBLOX_PASSWORD;
	postParam += "&user[tag_list]=";
	postParam += "man,RatingsBB10";
	postParam += "&token=";
	postParam += m_token;

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qUser;
	qUser["login"] = QBLOX_LOGIN;
	qUser["password"] = QBLOX_PASSWORD;
	qUser["tag_list"] = "RatingsBB10";

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
}

/**
 * Request session with Login
 */
void QBAuth::requestSessionWithLogin() {
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
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestSessionWithLogin()));
}

/**
 * Request session for registration
 */
void QBAuth::requestSessionRegister() {
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

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");

	QNetworkReply* reply = m_networkAccessManager->post(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestSessionRegister()));
}

/**
 * Request Login
 */
void QBAuth::requestLogin() {
	const QUrl url(QBLOX_API_SERVER + "login.json");

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
}

void QBAuth::onSingOut() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		reply->deleteLater();
	}
}

void QBAuth::onRegistrationNewUser() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile("app/native/assets/JDataRegistr.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());

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

				if (m_token != NULL && m_userId != NULL) {
					requestLogin();
				}
			}
		} else {
			successLoad = true;
			emit loadingChanged();
			emit unauthorizedChanged();
			emit completeLogin();

			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}

			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "401") {
				showError("Incorrect login or password");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "422") {
				showError("Login has already been taken");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "404") {
				showError("The requested resource could not be found");
				return;
			} else {
				//another error
				showError("QBlox Server Error = " + response);
				return;
			}
		}
		reply->deleteLater();
	}
	showError("Registration was successful");
	timerDelay->start(3000);
}

//SLOTS
void QBAuth::onRequestSessionRegister() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile("app/native/assets/JDataSession.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());

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
			successLoad = true;
			emit loadingChanged();
			emit unauthorizedChanged();
			emit completeLogin();

			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}

			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "401") {
				showError("Incorrect login or password");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "422") {
				showError("Login has already been taken");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "404") {
				showError("The requested resource could not be found");
				return;
			} else {
				//another error
				showError("QBlox Server Error = " + response);
				return;
			}
		}
		reply->deleteLater();
	}

	registrationNewUser();
}

void QBAuth::onRequestSessionWithLogin() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {

			successShow = true;
			emit unauthorizedChanged();

			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				JsonDataAccess jda;
				QVariant qtData = jda.loadFromBuffer(response);

				QFile sourceFile("app/native/assets/SessionWithLogin.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());

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

				if (m_token != NULL && m_userId != NULL) {
					requestLogin();
				}
			}
		} else {
			successShow = false;
			successLoad = true;
			emit unauthorizedChanged();
			emit loadingChanged();
			emit completeLogin();

			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}

			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "401") {
				showError("Incorrect login or password");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "422") {
				showError("User is not registered");
				return;
			} else if (reply->attribute(
					QNetworkRequest::HttpStatusCodeAttribute).toString()
					== "404") {
				showError("The requested resource could not be found");
				return;
			} else {
				//another error
				showError("QBlox Server Error = " + response);
				return;
			}
		}
		reply->deleteLater();
	}
	successLoad = true;
	emit loadingChanged();
	emit completeLogin();
}

void QBAuth::onRequestLogin() {
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
						if (it.key() == "user") {
							QVariantMap sessObject = it.value().toMap();
							QMapIterator<QString, QVariant> sit(sessObject);
							while (sit.hasNext()) {
								sit.next();
								if (sit.key() == "id") {
									break;
								}
							}
						}
					}
				}
			}
		} else {

			if (reply->error() < 100) {
				showError("Please check your internet connection");
				return;
			}

			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
		}
		reply->deleteLater();
	}
}

void QBAuth::showError(const QString &message) {
	emit error(message);
	qDebug() << message;
	SystemToast *toast = new SystemToast(this);
	toast->setBody(message);
	toast->show();
}

QString QBAuth::getTimestampUTC() {
#if QT_VERSION >= 0x040700
	return QString::number(QDateTime::currentDateTimeUtc().toTime_t());
#else
	return QString::number(QDateTime::currentDateTime().toUTC().toTime_t());
#endif
}

QString QBAuth::hmac_sha1(const QString &message, const QString &key) {
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

QString QBAuth::getToken() {
	return m_token;
}
