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
class QBCustomObject;

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

	//Custom objects
	Q_INVOKABLE void requestCO(QString className);
	Q_INVOKABLE void addCO(QBCustomObject* co);
	Q_INVOKABLE void deleteCO(QBCustomObject* co);
	Q_INVOKABLE void updateCO(QBCustomObject* co);

	//Retrieving current user info
	Q_INVOKABLE QString userId();
	Q_INVOKABLE QString userPassword();

Q_SIGNALS:
	void error(const QString &info);
	void sessionEstablished();
	void loggedIn();
	void loggedOut();
	void registered();
	void usersReceived(QString usersData);

	void coReceived(QString coData);
	void coAdded(QString coData);
	void coDeleted(QString id);
	void coUpdated(QString coData);

private Q_SLOTS:
	void onRequestSessionRegister();
	void onRegistrationNewUser();
	void onRequestSessionWithLogin();
	void onRequestLogin();
	void onRequestUsers();
	void onSignOut();

	void onRequestCO();
	void onAddCO();
	void onDeleteCO();
	void onUpdateCO();

	void onNetworkError(QNetworkReply::NetworkError);
	void onNetworkManagerReply(QNetworkReply *);

private:
	void handleError(QNetworkReply* reply);
	void disconnectFromAll();

	void registerNewUser();
	void requestLogin();

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
