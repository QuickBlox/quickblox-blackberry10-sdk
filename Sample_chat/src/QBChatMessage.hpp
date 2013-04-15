/*
 * QBChatMessage.hpp
 *
 *  Created on: 28.02.2013
 *      Author: Angryv
 */

#ifndef QBCHATMESSAGE_HPP_
#define QBCHATMESSAGE_HPP_

#include "QBGlobal.hpp"
#include <QObject>
#include <QVariant>

class QB_EXPORT QBChatMessage: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString from READ getFrom WRITE setFrom)
	Q_PROPERTY(QString to READ getTo WRITE setTo)
	Q_PROPERTY(QString text READ getText WRITE setText)
	Q_PROPERTY(QString time READ getTime WRITE setTime)

public:
	QBChatMessage(QObject *parent = 0);
	virtual ~QBChatMessage();

	QString getFrom() const;
	QString getTo() const;
	QString getText() const;
	QString getTime() const;
	uint getTimestamp() const;

	void setFrom(QString from);
	void setTo(QString to);
	void setText(QString text);
	void setTime(QString time);
	void setTimestamp(uint timestamp);

	void fromQVariantMap(QVariantMap map);
	QVariantMap toQVariantMap() const;

private:
	QString m_from;
	QString m_to;
	QString m_text;
	QString m_time;
	uint m_timestamp;
};

#endif /* QBCHATMESSAGE_HPP_ */
