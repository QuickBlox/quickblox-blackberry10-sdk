/*
 * Geolocator.cpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#include "Geolocator.hpp"

#include <bb/platform/geo/GeoLocation.hpp>
#include <bb/platform/geo/Point.hpp>
#include <bb/platform/geo/BoundingBox.hpp>
#include <bb/location/PositionErrorCode>

using namespace bb::platform::geo;

using namespace bb::location;

Geolocator::Geolocator(QObject* parent) : QObject(parent), m_latitude(0.0), m_longitude(0.0) {
	//Set up the position info source.
	m_src = QGeoPositionInfoSource::createDefaultSource(this);

	// Connect the updateTimeout() signal to a
	// slot that handles timeouts.
	bool updateTimeoutConnected = connect(m_src, SIGNAL(updateTimeout()), this,
			SLOT(positionUpdateTimeout()));

	if (updateTimeoutConnected)
	{
		qDebug() << "updateTimeout CONNECTED!";
	}
	else
	{
		 qDebug() << "positionUpdated FAILED to connect!";
	}

//Object::connect: No such slot Geolocator::positionUpdated(const QGeoPositionInfo &) in ../src/Geolocator.cpp:42
//Object::connect: No such signal bb::qtplugins::position::GeoPositionInfoSourceBb::positionUpdated(const QtMobilitySubset::QGeoPositionInfo &) in ../src/Geolocator.cpp:40
//positionUpdated FAILED to connect!

	// Connect the positionUpdated() signal to a
	// slot that handles position updates.
	bool positionUpdatedConnected = connect(m_src, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(positionUpdated(const QGeoPositionInfo &)));
	if (positionUpdatedConnected)
	{
		m_src->setProperty("canRunInBackground", true);
		m_src->setProperty("provider", "hybrid");
		m_src->setProperty("period", 1.0);
		m_src->startUpdates();

	    qDebug() << "positionUpdated CONNECTED!";
	}
	else
	{
		qDebug() << "positionUpdated FAILED to connect!";
	}
}

Geolocator::~Geolocator() {
}

void Geolocator::positionUpdated (const QGeoPositionInfo & pos)
{
	QGeoCoordinate	coordinates = pos.coordinate();
	m_latitude = coordinates.latitude();
	m_longitude = coordinates.longitude();

	m_src->stopUpdates();

	emit positionUpdated(m_latitude, m_longitude);
}

void Geolocator::positionUpdateTimeout()
{
    // A timeout occurred, no position update available.
	if ( m_src->property("replyErrorCode").isValid()  )
	{
	    bb::location::PositionErrorCode::Type errorCode = m_src->property("replyErrorCode").value<PositionErrorCode::Type>();
	    qDebug() << "LM Error Code: ";
	    switch ( errorCode )
	    {
	        case PositionErrorCode::None:
	        	qDebug() << "None";
	            break;

	        case PositionErrorCode::FatalDisabled:
	        	qDebug() << "Fatal - disabled (turn on location services!)";
	            break;

	        case PositionErrorCode::FatalNoLastKnownPosition:
	        	qDebug() << "Fatal - no last known position";
	            break;

	        case PositionErrorCode::FatalInsufficientProviders:
	        	qDebug() << "Fatal - insufficient providers";
	            break;

	        case PositionErrorCode::FatalInvalidRequest:
	        	qDebug() << "Fatal - invalid request";
	            break;

	        case PositionErrorCode::WarnTimeout:
	        	qDebug() << "Warning - timeout";
	            break;

	        case PositionErrorCode::WarnLostTracking:
	        	qDebug() << "Warning - lost tracking";
	            break;

	        default:
	        	qDebug() << "Unknown (bad enum value)";
	            break;
	    }
	}

	emit positionUpdatedTimeout();
}

double Geolocator::latitude() {
	return m_latitude;
}

double Geolocator::longitude() {
	return m_longitude;
}

