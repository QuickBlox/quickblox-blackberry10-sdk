/*
 * QBCustomObject.hpp
 *
 *  Created on: 12.03.2013
 *      Author: kirill
 */

#ifndef QBCUSTOMOBJECT_HPP_
#define QBCUSTOMOBJECT_HPP_

#include <QObject>
#include <QVariantMap>
#include <QDateTime>

class QBCustomObject: public QObject {
	Q_OBJECT

	Q_PROPERTY(QString className READ className WRITE setClassName)
	Q_PROPERTY(QVariantMap fields READ fields CONSTANT)
	Q_PROPERTY(uint userId READ userId WRITE setUserId)
	Q_PROPERTY(QString id READ id WRITE setId)
	Q_PROPERTY(QString parentId READ parentId WRITE setParentId)
	Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt)
	Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt)
	Q_PROPERTY(QString createdAtString READ createdAtString CONSTANT)
	Q_PROPERTY(QString updatedAtString READ updatedAtString CONSTANT)

public:
	QBCustomObject(QObject* parent = 0);
	virtual ~QBCustomObject();

	static QBCustomObject* parse(QString data);
	static QBCustomObject* parse(QVariantMap map);
	static QList<QBCustomObject*> parseFromJson(QString data);

	Q_INVOKABLE QString toDebugString();

public:
	QString className();
	QVariantMap fields();
	uint userId();
	QString id();
	QString parentId();
	QDateTime createdAt();
	QDateTime updatedAt();
	QString createdAtString();
	QString updatedAtString();

	void setClassName(QString className);
	void setUserId(uint userId);
	void setId(QString id);
	void setParentId(QString parentId);
	void setCreatedAt(QDateTime createdAt);
	void setUpdatedAt(QDateTime updatedAt);

	Q_INVOKABLE void addFieldValue(QString key, QVariant value);
	Q_INVOKABLE void addValueToArrayField(QString key, QVariant value);

private:
	QString m_className;
	QVariantMap m_fields;
	uint m_userId;
	QString m_id;
	QString m_parentId;
	QDateTime m_createdAt;
	QDateTime m_updatedAt;
};
Q_DECLARE_METATYPE(QBCustomObject*);

#endif /* QBCUSTOMOBJECT_HPP_ */
