/*
 * QBLGeoData.hpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#ifndef QBLGEODATA_HPP_
#define QBLGEODATA_HPP_

#include <QObject>
#include <QDateTime>
#include <QVariantMap>

class QBUUser;

class QBLGeoData: public QObject {
	Q_OBJECT

	Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
	Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(uint userId READ userId WRITE setUserId)
	Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt)
	Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt)
	Q_PROPERTY(QString id READ id WRITE setId)
	Q_PROPERTY(QString applicationId READ applicationId WRITE setApplicationId)
	Q_PROPERTY(QBUUser* user READ user WRITE setUser)

public:
	QBLGeoData(QObject* parent = 0);
	virtual ~QBLGeoData();

	static QBLGeoData* parse(QString data);
	static QBLGeoData* parse(QVariantMap map);
	static QList<QBLGeoData*> parseFromJson(QString data);

public:
	double latitude();
	double longitude();
	QString status();
	uint userId();
	QDateTime createdAt();
	QDateTime updatedAt();
	QString id();
	QString applicationId();
	QBUUser* user();

	void setLatitude(double latitude);
	void setLongitude(double longitude);
	void setStatus(QString status);
	void setUserId(uint userId);
	void setCreatedAt(QDateTime createdAt);
	void setUpdatedAt(QDateTime updatedAt);
	void setId(QString id);
	void setApplicationId(QString applicationId);
	void setUser(QBUUser* user);

signals:
	void statusChanged(const QString& status);

private:
	double m_latitude;
	double m_longitude;
	QString m_status;
	uint m_userId;
	QDateTime m_createdAt;
	QDateTime m_updatedAt;
	QString m_id;
	QString m_applicationId;
	QBUUser* m_user;
};
Q_DECLARE_METATYPE(QBLGeoData*);

#endif /* QBLGEODATA_HPP_ */
