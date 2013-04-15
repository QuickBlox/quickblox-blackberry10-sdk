/*
 * QBUUser.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#include "QBUUser.hpp"
#include "QBUtils.hpp"

QBUUser::QBUUser(QObject *parent) :
    QObject(parent),
    m_lastActivity(QBUtils::getCurrentTimeUTC())
{

}

QBUUser::~QBUUser()
{
}

QString QBUUser::getId()
{
	return m_id;
}

QString QBUUser::getPassword()
{
	return m_password;
}

int QBUUser::getPresence()
{
	return m_presence;
}

QDateTime QBUUser::getLastActivity()
{
	return m_lastActivity;
}

void QBUUser::setId(QString id)
{
	m_id = id;
}

void QBUUser::setPassword(QString password)
{
	m_password = password;
}

void QBUUser::setPresence(int presence)
{
	m_presence = presence;
	m_lastActivity = QBUtils::getCurrentTimeUTC();
}
