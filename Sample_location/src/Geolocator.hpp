/*
 * Geolocator.hpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#ifndef GEOLOCATOR_HPP_
#define GEOLOCATOR_HPP_

#include <QObject>
#include <QtLocationSubset/QGeoPositionInfo>
#include <QtLocationSubset/QGeoPositionInfoSource>

using namespace QtMobilitySubset;

class Geolocator: public QObject {
	Q_OBJECT

	Q_PROPERTY(double latitude READ latitude CONSTANT)
	Q_PROPERTY(double longitude READ longitude CONSTANT)

public:
	Geolocator(QObject* parent = 0);
	virtual ~Geolocator();

    double latitude();
    double longitude();

Q_SIGNALS:
	void positionUpdated(double latitude, double longitude);
	void positionUpdatedTimeout();

private Q_SLOTS:
    void positionUpdated(const QGeoPositionInfo &);
    void positionUpdateTimeout();

private:
    double m_latitude;
    double m_longitude;
    QtMobilitySubset::QGeoPositionInfoSource* m_src;
};

#endif /* GEOLOCATOR_HPP_ */
