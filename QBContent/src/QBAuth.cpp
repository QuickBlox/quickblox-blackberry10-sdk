/*
 * QBAuth.cpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBAuth.hpp"
#include <QHttp>

using namespace bb::system;
using namespace bb::data;

QString QBAuth::m_token = "";
QString QBAuth::m_userId = "";

QBAuth::QBAuth() {
	m_networkAccessManager = new QNetworkAccessManager(this);

	QBLOX_API_SERVER = "http://api.quickblox.com/"; // QBlox server
	QBLOX_APP_ID = "1956"; // add here your Application id
	QBLOX_AUTH_KEY = "AwbBs9fbH8s2-Gx"; // add here your Authorization key
	QBLOX_AUTH_SECRET = "ArzRA5uC9pepyUF"; // add here your Authorization secret

	successloadGameMode = false;
}

void QBAuth::setCredentials(const QString &login, const QString &password) {
	m_networkAccessManager = new QNetworkAccessManager(this);

	successLoad = false;
	successShow = false;
	emit loadingChanged();
	emit unauthorizedChanged();
	QBLOX_LOGIN = login; // login
	QBLOX_PASSWORD = password; // password

	timerDelay = new QTimer(this);
	QObject::connect(timerDelay, SIGNAL(timeout()), this,
			SLOT(slotTimerDelay()));

	timerDelayAmazon = new QTimer(this);
	QObject::connect(timerDelayAmazon, SIGNAL(timeout()), this,
			SLOT(slotTimerDelayAmazon()));
}

void QBAuth::slotTimerDelay() {
	requestSessionWithLogin();
	timerDelay->stop();
}

void QBAuth::slotTimerDelayAmazon() {
	requestUploadFile();
	timerDelayAmazon->stop();
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
 * Request: create file
 */
void QBAuth::requestCreateFile(QString path) {
	successLoad = false;
	emit loadingChanged();
	QStringList filePathList = path.split('/');
	m_nameFile = filePathList.at(filePathList.count() - 1);
	m_path_file = path;

	const QUrl url(QBLOX_API_SERVER + "blobs.json");

	QVariantMap param;
	param["content_type"] = "image/jpeg";
	param["name"] = m_nameFile;
	param["tag_list"] = "man,ContentBB10";
	param["public"] = "true";

	QVariantMap qSettings;
	qSettings["blob"] = param;

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
	connect(reply, SIGNAL(finished()), this, SLOT(responseCreateFile()));
}

void QBAuth::responseCreateFile() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			const int available = reply->bytesAvailable();
			if (available > 0) {
				const QByteArray buffer(reply->readAll());
				response = QString::fromUtf8(buffer);

				QFile sourceFile("app/native/assets/JDataCreateFile.json");
				if (!sourceFile.open(QIODevice::WriteOnly))
					return;
				sourceFile.write(response.toAscii());
				sourceFile.close();

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
						if (it.key() == "blob") {
							QVariantMap sessObject = it.value().toMap();
							QMapIterator<QString, QVariant> sit(sessObject);
							while (sit.hasNext()) {
								sit.next();
								if (sit.key() == "blob_object_access") {
									QVariantMap sessObject =
											sit.value().toMap();
									QMapIterator<QString, QVariant> it(
											sessObject);
									while (it.hasNext()) {
										it.next();
										if (it.key() == "params") {
											m_params = it.value().toString();
										}
									}
								}
								if (sit.key() == "id") {
									m_blob_id = sit.value().toString();
								}
							}
						}
					}
					for (int i = 0; i < m_params.length(); i++) {
						if (m_params.at(i) == '?') {
							m_params[i] = '"';
							m_params = m_params.mid(i + 1, m_params.length());
						}
					}

					m_params.replace("=", "\":\"");
					m_params.replace("&", "\",\"");

					QFile resultFile("app/native/assets/JDataResult.json");
					if (!resultFile.open(QIODevice::WriteOnly))
						return;
					resultFile.write(("{\"" + m_params + "\"}").toAscii());
					resultFile.close();

					JsonDataAccess jda;
					QVariant qtData = jda.loadFromBuffer(
							("{\"" + m_params + "\"}").toAscii());
					if (jda.hasError()) {
						const DataAccessError err = jda.error();
						showError(
								tr("Error converting JSON data: %1").arg(
										err.errorMessage()));
						qDebug() << "-------------------- error";
					} else {
						const QVariantMap object = qtData.value<QVariantMap>();
						QMapIterator<QString, QVariant> it(object);
						while (it.hasNext()) {
							it.next();
							if (it.key() == "AWSAccessKeyId") {
								m_AWSAccessKeyId =
										QString(
												QUrl::fromPercentEncoding(
														it.value().toString().toUtf8()));
							} else if (it.key() == "Content-Type") {
								m_content_type = it.value().toString();
							} else if (it.key() == "Policy") {
								m_policy =
										QString(
												QUrl::fromPercentEncoding(
														it.value().toString().toUtf8()));
							} else if (it.key() == "Signature") {
								m_signature =
										QString(
												QUrl::fromPercentEncoding(
														it.value().toString().toUtf8()));
							} else if (it.key() == "acl") {
								m_acl = it.value().toString();
							} else if (it.key() == "key") {
								m_key =
										QString(
												QUrl::fromPercentEncoding(
														it.value().toString().toUtf8()));
							} else if (it.key() == "success_action_status") {
								m_success_action_status = it.value().toString();
							}
						}
					}
				}
			}
		} else {
			successLoad = true;
			emit loadingChanged();
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
			} else {
				//another error
				showError("QBlox Server Error = " + response);
				return;
			}
		}
		reply->deleteLater();
	}

	timerDelayAmazon->start(1000);
}

/**
 * Request: upload the file
 */
void QBAuth::requestUploadFile() {

	QString boundary = "--"
			+ QString::number(
					qrand() * (90000000000) / (RAND_MAX + 1) + 10000000000, 16);

	QNetworkRequest request(QUrl("http://qbprod.s3.amazonaws.com/"));

	request.setHeader(QNetworkRequest::ContentTypeHeader,
			"multipart/form-data; boundary=" + boundary);

	QHttpMultiPart *multiPart = new QHttpMultiPart(
			QHttpMultiPart::FormDataType);
	multiPart->setBoundary(boundary.toAscii());

	QHttpPart textTokenPart1;
	textTokenPart1.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"key\""));
	textTokenPart1.setBody(QByteArray(m_key.toAscii()));

	QHttpPart textTokenPart2;
	textTokenPart2.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"AWSAccessKeyId\""));
	textTokenPart2.setBody(QByteArray(m_AWSAccessKeyId.toAscii()));

	QHttpPart textTokenPart3;
	textTokenPart3.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"policy\""));
	textTokenPart3.setBody(QByteArray(m_policy.toAscii()));

	QHttpPart textTokenPart4;
	textTokenPart4.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"signature\""));
	textTokenPart4.setBody(QByteArray(m_signature.toAscii()));

	QHttpPart textTokenPart5;
	textTokenPart5.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"acl\""));
	textTokenPart5.setBody(QByteArray(m_acl.toAscii()));

	QHttpPart textTokenPart6;
	textTokenPart6.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"success_action_status\""));
	textTokenPart6.setBody(QByteArray(m_success_action_status.toAscii()));

	QHttpPart textTokenPart7;
	textTokenPart7.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant("form-data; name=\"Content-Type\""));
	textTokenPart7.setBody(QByteArray(m_content_type.toAscii()));

	multiPart->append(textTokenPart1);
	multiPart->append(textTokenPart2);
	multiPart->append(textTokenPart3);
	multiPart->append(textTokenPart4);
	multiPart->append(textTokenPart5);
	multiPart->append(textTokenPart6);
	multiPart->append(textTokenPart7);

	QHttpPart imagePart;
	QFile *file = new QFile(m_path_file);
	if (!file->open(QIODevice::ReadOnly)) {
		return;
	}
	QFileInfo fi(file->fileName());

	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant(
					"form-data; name=\"file\"; filename=\"" + m_nameFile
							+ "\""));
	imagePart.setBodyDevice(file);
	file->setParent(multiPart);

	imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
			QVariant("image/jpeg"));

	multiPart->append(imagePart);
	QNetworkReply *reply = m_networkAccessManager->post(request, multiPart);
	connect(reply, SIGNAL(finished()), this, SLOT(responseUploadFile()));
}

void QBAuth::responseUploadFile() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {

		QFile sourceFile("app/native/assets/JDataUploadFile.json");
		if (!sourceFile.open(QIODevice::WriteOnly))
			return;
		sourceFile.write(reply->readAll());
		sourceFile.close();

		if (reply->error() == QNetworkReply::NoError) {
			requestDeclaringFile();
		} else {
			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			//another error
			showError("QBlox Server Error = " + response);
		}
		reply->deleteLater();
	}
}

/**
 * Request: declaring the file
 */
void QBAuth::requestDeclaringFile() {
	const QUrl url(QBLOX_API_SERVER + "blobs/" + m_blob_id + "/complete.json");

	QVariantMap param;
	param["size"] = "10000000";

	QVariantMap qSettings;
	qSettings["blob"] = param;

	QVariant setData = QVariant(qSettings);
	QString outRequest = "";
	JsonDataAccess jda;
	jda.saveToBuffer(setData, &outRequest);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->put(request,
			outRequest.toAscii());
	connect(reply, SIGNAL(finished()), this, SLOT(responseDeclaringFile()));
}

void QBAuth::responseDeclaringFile() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		QFile sourceFile("app/native/assets/JDataDeclaringFile.json");
		if (!sourceFile.open(QIODevice::WriteOnly))
			return;
		sourceFile.write(reply->readAll());
		sourceFile.close();
		if (reply->error() == QNetworkReply::NoError) {
			//showError("Declaring was successful");

			requestTaggedList();

			return;
		} else {
			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			//another error
			showError("QBlox Server Error = " + response);
		}
		reply->deleteLater();
	}
}

/**
 * Request: Get Tagged Files List
 */
void QBAuth::requestTaggedList() {
	const QUrl url("http://api.quickblox.com/blobs.json?per_page=50");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("QuickBlox-REST-API-Version", "0.1.0");
	request.setRawHeader("QB-Token", m_token.toUtf8());
	QNetworkReply* reply = m_networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(responseTaggedList()));
}

void QBAuth::responseTaggedList() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QString response;
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			QByteArray data(reply->readAll());

			QFile sourceFile("app/native/assets/JDataTaggedList.json");
			QFileInfo fi(sourceFile);
			if (!sourceFile.open(QIODevice::WriteOnly))
				return;
			sourceFile.write(data);
			sourceFile.close();

			successLoad = true;
			successShow = true;
			emit unauthorizedChanged();
			emit loadingChanged();
			emit completeLogin();

			return;
		} else {
			response =
					tr("Error: %1 status: %2").arg(reply->errorString(),
							reply->attribute(
									QNetworkRequest::HttpStatusCodeAttribute).toString());
			//another error
			showError("QBlox Server Error = " + response);
			successLoad = true;
			emit loadingChanged();
		}
		reply->deleteLater();
	}
}

QVariant QBAuth::image() const {
	return QVariant::fromValue(m_image);
}

/*
 * Logout
 */
void QBAuth::removeSession() {
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
	postParam += "man,ContentBB10";
	postParam += "&token=";
	postParam += m_token;

	QString signature = hmac_sha1(postParam, QBLOX_AUTH_SECRET);

	QVariantMap qUser;
	qUser["login"] = QBLOX_LOGIN;
	qUser["password"] = QBLOX_PASSWORD;
	qUser["tag_list"] = "ContentBB10";

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
			//////////////////////////////
			successLoad = true;
			emit loadingChanged();
			emit unauthorizedChanged();
			emit completeLogin();
			//////////////////////////////

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
			//////////////////////////////
			successLoad = true;
			emit loadingChanged();
			emit unauthorizedChanged();
			emit completeLogin();
			//////////////////////////////

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
			requestTaggedList();
			return;
		} else {
			//////////////////////////////
			successShow = false;
			successLoad = true;
			emit unauthorizedChanged();
			emit loadingChanged();
			emit completeLogin();
			//////////////////////////////
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
	//////////////////////////////
	successShow = false;
	successLoad = true;
	emit unauthorizedChanged();
	emit loadingChanged();
	emit completeLogin();
	//////////////////////////////
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
