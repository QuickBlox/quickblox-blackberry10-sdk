/*
 * QBAuth.hpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef QBAuth_HPP_
#define QBAuth_HPP_

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

class QBAuth: public QObject {
	Q_OBJECT
	Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
	Q_PROPERTY(bool unauthorized READ unauthorized NOTIFY unauthorizedChanged)
	Q_PROPERTY(bool loadGameMode READ loadGameMode NOTIFY loadGameModeChanged)

public:
	QBAuth();
	Q_INVOKABLE
	void requestSessionWithLogin();
	Q_INVOKABLE
	void requestSessionRegister();
	Q_INVOKABLE
	void registrationNewUser();
	Q_INVOKABLE
	void removeSession();
	Q_INVOKABLE
	void requestCreateScore(int id, int value);
	Q_INVOKABLE
	void requestGameModes();

	Q_INVOKABLE
	bool loading() const;Q_INVOKABLE
	bool unauthorized() const;Q_INVOKABLE
	bool loadGameMode() const;Q_INVOKABLE
	void setCredentials(const QString &login, const QString &password);Q_INVOKABLE
	static QString getToken();

	static QString m_token;

Q_SIGNALS:
	void loadingChanged();
	void unauthorizedChanged();
	void loadGameModeChanged();
	void completeLogin();
	void error(const QString &info);

private Q_SLOTS:
	void onRequestSessionWithLogin();
	void onRequestSessionRegister();
	void onRegistrationNewUser();
	void onRequestLogin();
	void onSingOut();
	void responseCreateScore();
	void responseGameModes();
	void slotTimerDelay();

private:
	void requestLogin();
	QString hmac_sha1(const QString &message, const QString &key);

	void showError(const QString &message);
	QString getTimestampUTC();

private:
	QNetworkAccessManager* m_networkAccessManager;

	QString QBLOX_LOGIN;
	QString QBLOX_PASSWORD;
	QString QBLOX_API_SERVER;
	QString QBLOX_APP_ID;
	QString QBLOX_AUTH_KEY;
	QString QBLOX_AUTH_SECRET;

	int m_type;
	QString m_userId;
	bool successLoad;
	bool successShow;
	bool successloadGameMode;

	QTimer *timerDelay;
};

#endif /* QBAuth_HPP_ */
