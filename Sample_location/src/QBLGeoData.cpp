/*
 * QBLGeoData.cpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#include "QBLGeoData.hpp"
#include <bb/data/JsonDataAccess>

#include "QBUUser.hpp"

using namespace bb::data;

QBLGeoData::QBLGeoData(QObject* parent) : QObject(parent),
		m_latitude(0),
		m_longitude(0),
		m_status(QString::null),
		m_userId(0),
		m_createdAt(QDateTime::currentDateTimeUtc()),
		m_updatedAt(QDateTime::currentDateTimeUtc()),
		m_id(QString::null),
		m_applicationId(QString::null),
		m_user(NULL) {

}

QBLGeoData::~QBLGeoData() {

}

double QBLGeoData::latitude() {
	return m_latitude;
}

double QBLGeoData::longitude() {
	return m_longitude;
}

QString QBLGeoData::status() {
	return m_status;
}

uint QBLGeoData::userId() {
	return m_userId;
}

QDateTime QBLGeoData::createdAt() {
	return m_createdAt;
}

QDateTime QBLGeoData::updatedAt() {
	return m_updatedAt;
}

QString QBLGeoData::id() {
	return m_id;
}

QString QBLGeoData::applicationId() {
	return m_applicationId;
}

QBUUser* QBLGeoData::user() {
	return m_user;
}

void QBLGeoData::setLatitude(double latitude) {
	m_latitude = latitude;
}

void QBLGeoData::setLongitude(double longitude) {
	m_longitude = longitude;
}

void QBLGeoData::setStatus(QString status) {
	m_status = status;
}

void QBLGeoData::setUserId(uint userId) {
	m_userId = userId;
}

void QBLGeoData::setCreatedAt(QDateTime createdAt) {
	m_createdAt = createdAt;
}

void QBLGeoData::setUpdatedAt(QDateTime updatedAt) {
	m_updatedAt = updatedAt;
}

void QBLGeoData::setId(QString id) {
	m_id = id;
}

void QBLGeoData::setApplicationId(QString applicationId) {
	m_applicationId = applicationId;
}

void QBLGeoData::setUser(QBUUser* user) {
	m_user = user;
}

QBLGeoData* QBLGeoData::parse(QString data) {
	JsonDataAccess jda;
	QVariant coData = jda.loadFromBuffer(data);

	return parse(coData.toMap());
}

QBLGeoData* QBLGeoData::parse(QVariantMap map) {
	QBLGeoData* result = new QBLGeoData;

	result->setApplicationId(map["geo_datum"].toMap()["application_id"].toString());
	result->setCreatedAt(map["geo_datum"].toMap()["created_at"].toDateTime());
	result->setId(map["geo_datum"].toMap()["id"].toString());
	result->setLatitude(map["geo_datum"].toMap()["latitude"].toDouble());
	result->setLongitude(map["geo_datum"].toMap()["longitude"].toDouble());
	result->setStatus(map["geo_datum"].toMap()["status"].toString());
	result->setUpdatedAt(map["geo_datum"].toMap()["updated_at"].toDateTime());
	result->setUserId(map["geo_datum"].toMap()["user_id"].toUInt());

	result->setUser(QBUUser::parse(map["geo_datum"].toMap()["user"].toMap()["user"].toMap()));

	return result;
}

QList<QBLGeoData*> QBLGeoData::parseFromJson(QString data)
{
	QList<QBLGeoData*> result;

	JsonDataAccess jda;
	QVariant coData = jda.loadFromBuffer(data);
	foreach (const QVariant& item, coData.toMap()["items"].toList())
	{
		QBLGeoData* co = parse(item.toMap());
		result << co;
	}

	return result;
}
