/*
 * QBUUser.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#include "QBUUser.hpp"
#include "QBUtils.hpp"
#include <bb/data/JsonDataAccess>

using namespace bb::data;

QBUUser::QBUUser(QObject *parent) :
    QObject(parent),
	m_externalUserId(0),
	m_blobId(0),
	m_facebookId(QString::null),
	m_twitterId(QString::null),
	m_fullName(QString::null),
	m_email(QString::null),
	m_login(QString::null),
	m_phone(QString::null),
	m_website(QString::null),
	m_password(QString::null),
	m_oldPassword(QString::null),
	m_lastRequestAt(QDateTime::currentDateTimeUtc()),
	m_tags(QVariantList())
{

}

QBUUser::~QBUUser()
{
}

uint QBUUser::externalUserId() {
	return m_externalUserId;
}

uint QBUUser::blobId() {
	return m_blobId;
}

QString QBUUser::facebookId() {
	return m_facebookId;
}

QString QBUUser::twitterId() {
	return m_twitterId;
}

QString QBUUser::fullName() {
	return m_fullName;
}

QString QBUUser::email() {
	return m_email;
}

QString QBUUser::login() {
	return m_login;
}

QString QBUUser::phone() {
	return m_phone;
}

QString QBUUser::website() {
	return m_website;
}

QString QBUUser::password() {
	return m_password;
}

QString QBUUser::oldPassword() {
	return m_oldPassword;
}

QDateTime QBUUser::lastRequestAt() {
	return m_lastRequestAt;
}

QVariantList QBUUser::tags() {
	return m_tags;
}

void QBUUser::setExternalUserId(uint externalUserId) {
	m_externalUserId = externalUserId;
}

void QBUUser::setBlobId(uint blobId) {
	m_blobId = blobId;
}

void QBUUser::setFacebookId(QString facebookId) {
	m_facebookId = facebookId;
}

void QBUUser::setTwitterId(QString twitterId) {
	m_twitterId = twitterId;
}

void QBUUser::setFullName(QString fullName) {
	m_fullName = fullName;
}

void QBUUser::setEmail(QString email) {
	m_email = email;
}

void QBUUser::setLogin(QString login) {
	m_login = login;
}

void QBUUser::setPhone(QString phone) {
	m_phone = phone;
}

void QBUUser::setWebsite(QString website) {
	m_website = website;
}

void QBUUser::setPassword(QString password) {
	m_password = password;
}

void QBUUser::setOldPassword(QString oldPassword) {
	m_oldPassword = oldPassword;
}

void QBUUser::setLastRequestAt(QDateTime lastRequestAt) {
	m_lastRequestAt = lastRequestAt;
}

QBUUser* QBUUser::parse(QString data) {
	JsonDataAccess jda;
	QVariant userData = jda.loadFromBuffer(data);

	if (userData.toMap().contains("user")) {
		return parse(userData.toMap()["user"].toMap());
	}

	return parse(userData.toMap());
}

QBUUser* QBUUser::parse(QVariantMap map) {
	QBUUser* result = new QBUUser;

	result->setExternalUserId(map["external_user_id"].toUInt());
	result->setBlobId(map["blob_id"].toUInt());
	result->setFacebookId(map["facebook_id"].toString());
	result->setTwitterId(map["twitter_id"].toString());
	result->setFullName(map["full_name"].toString());
	result->setEmail(map["email"].toString());
	result->setLogin(map["login"].toString());
	result->setPhone(map["phone"].toString());
	result->setWebsite(map["website"].toString());
	result->setPassword(map["password"].toString());
	result->setOldPassword(map["old_password"].toString());
	result->setLastRequestAt(map["last_request_at"].toDateTime());

//	result->setCreatedAt(map["created_at"].toDateTime());
//	result->setId(map["id"].toString());
//	result->setOwnerId(map["owner_id"].toString());
//	m_tags(QVariantList())

	return result;
}

QList<QBUUser*> QBUUser::parseFromJson(QString data)
{
	QList<QBUUser*> result;

	JsonDataAccess jda;
	QVariant userData = jda.loadFromBuffer(data);
	foreach (const QVariant& item, userData.toMap()["items"].toList())
	{
		QBUUser* user = parse(item.toMap());
		result << user;
	}

	return result;
}
