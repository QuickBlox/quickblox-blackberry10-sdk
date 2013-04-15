/*
 * QBCustomObject.cpp
 *
 *  Created on: 12.03.2013
 *      Author: kirill
 */

#include "QBCustomObject.hpp"
#include <bb/data/JsonDataAccess>

using namespace bb::data;

QString QBCustomObject::className() {
	return m_className;
}

QVariantMap QBCustomObject::fields() {
	return m_fields;
}

uint QBCustomObject::userId() {
	return m_userId;
}

QString QBCustomObject::id() {
	return m_id;
}

QString QBCustomObject::parentId() {
	return m_parentId;
}

QDateTime QBCustomObject::createdAt() {
	return m_createdAt;
}

QDateTime QBCustomObject::updatedAt() {
	return m_updatedAt;
}

QString QBCustomObject::createdAtString() {
	return m_createdAt.toString(Qt::ISODate).replace("T", " ");
}

QString QBCustomObject::updatedAtString() {
	return m_updatedAt.toString(Qt::ISODate).replace("T", " ");
}

void QBCustomObject::setClassName(QString className)
{
	m_className = className;
}

void QBCustomObject::setUserId(uint userId)
{
	m_userId = userId;
}

void QBCustomObject::setId(QString id)
{
	m_id = id;
}

void QBCustomObject::setParentId(QString parentId)
{
	m_parentId = parentId;
}

void QBCustomObject::setCreatedAt(QDateTime createdAt)
{
	m_createdAt = createdAt;
}

void QBCustomObject::setUpdatedAt(QDateTime updatedAt)
{
	m_updatedAt = updatedAt;
}

void QBCustomObject::addFieldValue(QString key, QVariant value)
{
	m_fields[key] = value;
}

void QBCustomObject::addValueToArrayField(QString key, QVariant value)
{
	if (!m_fields.contains(key)) {
		m_fields[key] = QVariantList();
	}

	if (m_fields[key].canConvert<QVariantList>())
	{
		QVariantList list = m_fields[key].toList();
		list << value;
		m_fields[key] = list;
	}
}

QBCustomObject* QBCustomObject::parse(QString data) {
	JsonDataAccess jda;
	QVariant coData = jda.loadFromBuffer(data);

	return parse(coData.toMap());
}

QBCustomObject* QBCustomObject::parse(QVariantMap map) {
	QBCustomObject* result = new QBCustomObject;

	foreach (const QVariant& key, map.keys())
	{
		QString keyString = key.toString();
		if (keyString == "_id") {
			result->setId(map["_id"].toString());
		} else if (keyString == "_parent_id") {
			if (map["_parent_id"].toString() != "")
				result->setParentId(map["_parent_id"].toString());
		} else if (keyString == "user_id") {
			result->setUserId(map["user_id"].toUInt());
		} else if (keyString == "created_at") {
			result->setCreatedAt(QDateTime::fromTime_t(map["created_at"].toUInt()));
		} else if (keyString == "updated_at") {
			result->setUpdatedAt(QDateTime::fromTime_t(map["updated_at"].toUInt()));
		} else {
			result->addFieldValue(keyString, map[keyString]);
		}
	}

	return result;
}

QList<QBCustomObject*> QBCustomObject::parseFromJson(QString data)
{
	QList<QBCustomObject*> result;

	JsonDataAccess jda;
	QVariant coData = jda.loadFromBuffer(data);
	foreach (const QVariant& item, coData.toMap()["items"].toList())
	{
		QBCustomObject* co = parse(item.toMap());
		co->setClassName(coData.toMap()["class_name"].toString());
		result << co;
	}

	return result;
}

QString QBCustomObject::toDebugString()
{
	//TODO i think not best use QString there
	QString result;
	result += "className=\"" + className() + "\" ";
	result += "id=\"" + id() + "\" ";
	result += "parentId=\"" + parentId() + "\" ";
	result += "userId=\"" + QString::number(userId()) + "\" ";
	result += "updatedAt=\"" + updatedAt().toString() + "\" ";
	result += "createdAt=\"" + createdAt().toString() + "\" ";
	result += "fields=\"" + QVariant(fields()).toString() + "\"";
	return result;
}

QBCustomObject::QBCustomObject(QObject* parent) : QObject(parent),
									m_fields(QVariantMap()),
									m_userId(0),
									m_parentId(QString::null),
									m_createdAt(QDateTime::currentDateTimeUtc()),
									m_updatedAt(QDateTime::currentDateTimeUtc()) {
}

QBCustomObject::~QBCustomObject() {
}

