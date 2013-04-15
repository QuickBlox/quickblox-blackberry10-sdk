/*
 * QBChatMessage.cpp
 *
 *  Created on: 28.02.2013
 *      Author: Angryv
 */

#include "QBChatMessage.hpp"
#include "QBUtils.hpp"

#include <QDebug>

QBChatMessage::QBChatMessage(QObject *parent) :
    QObject(parent),
    m_time(QBUtils::getTimestampUTC()),
    m_timestamp(QBUtils::getTimestampUTCMs())
{

}

QBChatMessage::~QBChatMessage()
{
}

QString QBChatMessage::getFrom() const
{
	return m_from;
}

QString QBChatMessage::getTo() const
{
	return m_to;
}

QString QBChatMessage::getText() const
{
	return m_text;
}

QString QBChatMessage::getTime() const
{
	return m_time;
}

uint QBChatMessage::getTimestamp() const
{
	return m_timestamp;
}

void QBChatMessage::setFrom(QString from)
{
	m_from = from;
}

void QBChatMessage::setTo(QString to)
{
	m_to = to;
}

void QBChatMessage::setText(QString text)
{
	m_text = text;
}

void QBChatMessage::setTime(QString time)
{
	m_time = time;
}

void QBChatMessage::setTimestamp(uint timestamp)
{
	m_timestamp = timestamp;
}

void QBChatMessage::fromQVariantMap(QVariantMap map)
{
	setFrom(map["from"].toString());
	setTo(map["to"].toString());
	setText(map["text"].toString());
	setTime(map["time"].toString());
	setTimestamp(map["timestamp"].toUInt());
}

QVariantMap QBChatMessage::toQVariantMap() const
{
	QVariantMap result;

	result["from"] = getFrom();
	result["to"] = getTo();
	result["text"] = getText();
	result["time"] = QDateTime::fromTime_t(getTimestamp()).toString();
	result["timestamp"] = getTimestamp();

	return result;
}

