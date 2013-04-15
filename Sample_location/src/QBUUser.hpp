/*
 * QBUUser.hpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#ifndef QBUUSER_HPP_
#define QBUUSER_HPP_

#include "QBGlobal.hpp"
#include <QObject>
#include <QDateTime>
#include <QVariantMap>

class QB_EXPORT QBUUser: public QObject
{
	Q_OBJECT

	Q_PROPERTY(uint externalUserId READ externalUserId WRITE setExternalUserId)
	Q_PROPERTY(uint blobId READ blobId WRITE setBlobId)
	Q_PROPERTY(QString facebookId READ facebookId WRITE setFacebookId)
	Q_PROPERTY(QString twitterId READ twitterId WRITE setTwitterId)
	Q_PROPERTY(QString fullName READ fullName WRITE setFullName)
	Q_PROPERTY(QString email READ email WRITE setEmail)
	Q_PROPERTY(QString login READ login WRITE setLogin)
	Q_PROPERTY(QString phone READ phone WRITE setPhone)
	Q_PROPERTY(QString website READ website WRITE setWebsite)
	Q_PROPERTY(QString password READ password WRITE setPassword)
	Q_PROPERTY(QString oldPassword READ oldPassword WRITE setOldPassword)
	Q_PROPERTY(QDateTime lastRequestAt READ lastRequestAt WRITE setLastRequestAt)
	Q_PROPERTY(QVariantList tags READ tags CONSTANT)

public:
	QBUUser(QObject *parent = 0);
	virtual ~QBUUser();

	static QBUUser* parse(QString data);
	static QBUUser* parse(QVariantMap map);
	static QList<QBUUser*> parseFromJson(QString data);

	uint externalUserId();
	uint blobId();
	QString facebookId();
	QString twitterId();
	QString fullName();
	QString email();
	QString login();
	QString phone();
	QString website();
	QString password();
	QString oldPassword();
	QDateTime lastRequestAt();
	QVariantList tags();

	void setExternalUserId(uint externalUserId);
	void setBlobId(uint blobId);
	void setFacebookId(QString facebookId);
	void setTwitterId(QString twitterId);
	void setFullName(QString fullName);
	void setEmail(QString email);
	void setLogin(QString login);
	void setPhone(QString phone);
	void setWebsite(QString website);
	void setPassword(QString password);
	void setOldPassword(QString oldPassword);
	void setLastRequestAt(QDateTime lastRequestAt);

private:
	uint m_externalUserId;
	uint m_blobId;
	QString m_facebookId;
	QString m_twitterId;
	QString m_fullName;
	QString m_email;
	QString m_login;
	QString m_phone;
	QString m_website;
	QString m_password;
	QString m_oldPassword;
	QDateTime m_lastRequestAt;
	QVariantList m_tags;
};
Q_DECLARE_METATYPE(QBUUser*);

#endif /* QBUUSER_HPP_ */
