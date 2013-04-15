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
class QBLGeoData;

class QBNetwork: public QObject {
	Q_OBJECT
public:
	QBNetwork();
	~QBNetwork();

	//Authorization
	Q_INVOKABLE void registerUser(QString userName, QString userPassword);
	Q_INVOKABLE void login(QString userName, QString userPassword);
	Q_INVOKABLE void requestUsers();
	Q_INVOKABLE void logout();

	//Location
	Q_INVOKABLE void getGeoData();
	Q_INVOKABLE void createGeoData(QBLGeoData* data);
	Q_INVOKABLE void createGeoData(double latitude, double longitude, QString status);

	//Retrieving current user info
	Q_INVOKABLE QString userId();
	Q_INVOKABLE QString userPassword();

Q_SIGNALS:
	void error(const QString &info);
	void sessionEstablished();
	void loggedIn(QString userData);
	void loggedOut();
	void registered();
	void usersReceived(QString usersData);

	void geoDataReceived(QString geoData);
	void geoDataCreated(QString geoData);

private Q_SLOTS:
	void onRequestSessionRegister();
	void onRegistrationNewUser();
	void onRequestSessionWithLogin();
	void onRequestLogin();
	void onRequestUsers();
	void onSignOut();

	void onGetGeoDataSessionRequest();
	void onRequestGeoData();
	void onRequestCreateGeoData();

	void onNetworkError(QNetworkReply::NetworkError);
	void onNetworkManagerReply(QNetworkReply *);

private:
	void handleError(QNetworkReply* reply);
	void disconnectFromAll();

	bool handleSessionResponse();

	void registerNewUser();
	void requestLogin();
	void requestGeoData();

	QNetworkReply* getCreateSessionRequest(QString userName = QString::null, QString userPassword = QString::null);

	QString hmac_sha1(const QString &message, const QString &key);

private:
	QNetworkAccessManager* m_networkAccessManager;
	QString m_token;
	QString m_userId;
	QString m_userName;

	QString QBLOX_LOGIN;
	QString QBLOX_PASSWORD;
};

#endif /* QBNETWORK_HPP_ */
