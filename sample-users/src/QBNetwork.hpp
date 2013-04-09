/*
 * QBNetwork.hpp
 *
 * Created on: 19.02.2013
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

class QNetworkAccessManager;

class QBNetwork: public QObject {
	Q_OBJECT
	Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
	Q_PROPERTY(bool unauthorized READ unauthorized NOTIFY unauthorizedChanged)

public:
	QBNetwork();
	Q_INVOKABLE void requestSessionWithLogin();
	Q_INVOKABLE void requestSessionRegister();
	Q_INVOKABLE void registrationNewUser();
	Q_INVOKABLE void removeSession();

	Q_INVOKABLE
	bool loading() const;
	Q_INVOKABLE
	bool unauthorized() const;
	Q_INVOKABLE
	void setCredentials(const QString &login, const QString &password);
	Q_INVOKABLE

Q_SIGNALS:
	void loadingChanged();
	void unauthorizedChanged();
	void completeLogin();
	void error(const QString &info);

public Q_SLOTS:
	void onRequestSessionWithLogin();
	void onRequestSessionRegister();
	void onRegistrationNewUser();
	void onRequestUsers();
	void onRequestLogin();
	void onSingOut();
	void slotTimerDelay();

private:
	void requestUsers();
	void requestLogin();
	QString hmac_sha1(const QString &message, const QString &key);

	void showError(const QString &message);
	QString getTimestampUTC();

private:
	QNetworkAccessManager* m_networkAccessManager;
	QString m_token;
	QString m_userId;
	QString m_userName;

	QTimer *timerDelay;

	QString QBLOX_LOGIN;
	QString QBLOX_PASSWORD;
	QString QBLOX_API_SERVER;
	QString QBLOX_APP_ID;
	QString QBLOX_AUTH_KEY;
	QString QBLOX_AUTH_SECRET;

	int m_type;
	static bool successLoad;
	static bool successShow;
};

#endif /* QBNETWORK_HPP_ */
