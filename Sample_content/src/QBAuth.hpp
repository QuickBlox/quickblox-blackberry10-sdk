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
#include <bb/cascades/Image>
#include <QtXml>
#include <bb/device/HardwareInfo>

class QNetworkAccessManager;

class QBAuth: public QObject {
	Q_OBJECT
	Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
	Q_PROPERTY(bool unauthorized READ unauthorized NOTIFY unauthorizedChanged)
	Q_PROPERTY(bool loadGameMode READ loadGameMode NOTIFY loadGameModeChanged)
	Q_PROPERTY(QVariant image READ image NOTIFY imageChanged)

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
	void requestCreateFile(QString path);
	Q_INVOKABLE
	void requestUploadFile();
	Q_INVOKABLE
	void requestDeclaringFile();
	Q_INVOKABLE
	void requestTaggedList();

	Q_INVOKABLE
	bool loading() const;Q_INVOKABLE
	bool unauthorized() const;Q_INVOKABLE
	bool loadGameMode() const;Q_INVOKABLE
	void setCredentials(const QString &login, const QString &password);Q_INVOKABLE
	static QString getToken();

	static QString m_token;
	static QString m_userId;

Q_SIGNALS:
	void loadingChanged();
	void unauthorizedChanged();
	void loadGameModeChanged();
	void completeLogin();
	void error(const QString &info);
	void imageChanged();

/*public Q_SLOTS:
	int responseScoreAverage(int id);*/

private Q_SLOTS:
	void onRequestSessionWithLogin();
	void onRequestSessionRegister();
	void onRegistrationNewUser();
	void onRequestLogin();
	void onSingOut();
	void slotTimerDelay();
	void slotTimerDelayAmazon();
	void responseCreateFile() ;
	void responseUploadFile();
	void responseDeclaringFile();
	void responseTaggedList();

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
	bool successLoad;
	bool successShow;
	bool successloadGameMode;

	QTimer *timerDelay;
	QTimer *timerDelayAmazon;

	QString m_nameFile, m_path_file;

	QString m_AWSAccessKeyId;
	QString m_content_type;
	QString m_policy;
	QString m_signature;
	QString m_acl;
	QString m_key;
	QString m_success_action_status;
	QString m_params;
	QString m_blob_id;
	QString m_href;

	QVariant image() const;
	bb::cascades::Image m_image;
};

#endif /* QBAuth_HPP_ */
