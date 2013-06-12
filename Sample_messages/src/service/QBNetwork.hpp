/*
 * QBNetwork.hpp
 *
 * Created on: 27.05.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef QBNETWORK_HPP_
#define QBNETWORK_HPP_

#include <QtCore/QObject>
#include <bb/data/JsonDataAccess>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QUrl>
#include <QDateTime>
#include <bb/system/SystemToast>
#include <QObject>
#include <bb/device/HardwareInfo>

class QNetworkAccessManager;

class QBNetwork: public QObject {
	Q_OBJECT

public:
	QBNetwork();
	virtual ~QBNetwork();

	Q_INVOKABLE void requestSessionWithLogin();
	Q_INVOKABLE void requestRemoveSubscriptions();

Q_SIGNALS:
	void failInternet();
	void error(const QString &info);
	void successSubscription();
	void successRemoveSubscription();

private:
	void requestRegisterUser();
	void requestSession();
	void requestCreatePushToken();
	void requestCreateSubscriptions();

	QString hmac_sha1(const QString &message, const QString &key);
	void showError(const QString &message);
	QString getTimestampUTC();

private Q_SLOTS:
	void responseSessionWithLogin();
	void responseSession();
	void responseRegisterUser();
	void responseCreatePushToken();
	void responseCreateSubscriptions();
	void responseRemoveSubscriptions();

private:
	QNetworkAccessManager* m_networkAccessManager;
	QString m_token;
	QString m_userId;
	QString m_userName;

	QString QBLOX_LOGIN;
	QString QBLOX_PASSWORD;
	QString QBLOX_API_SERVER;
	QString QBLOX_APP_ID;
	QString QBLOX_AUTH_KEY;
	QString QBLOX_AUTH_SECRET;
	QString m_PIN;

	int m_type;
};

#endif /* QBNETWORK_HPP_ */
