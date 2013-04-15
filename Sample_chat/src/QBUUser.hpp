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

class QB_EXPORT QBUUser: public QObject
{
	Q_OBJECT
public:
	QBUUser(QObject *parent = 0);
	virtual ~QBUUser();

	QString getId();
	QString getPassword();
	int getPresence();
	QDateTime getLastActivity();

	void setId(QString id);
	void setPassword(QString password);
	void setPresence(int presence);

private:
	QString m_id;
	QString m_password;
	int m_presence;
	QDateTime m_lastActivity;
};

#endif /* QBUUSER_HPP_ */
