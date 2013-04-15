#include "MapController.h"

#include <bb/data/JsonDataAccess>

#include <QObject>
#include <QVariant>
#include <QFile>

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/XmlDataModel>
#include <bb/cascades/Page>
#include <bb/cascades/WebView>

using namespace bb::data;
using namespace bb::cascades;

/*
 *********************************************************************************
 * Constructor
 *********************************************************************************
 */

MapController::MapController(QObject *parent) : QObject(parent) {

}

MapController::~MapController() {

}

/*
 *********************************************************************************
 * Setters
 *********************************************************************************
 */

void MapController::setWebView( QObject* webView ) {
	mWebView = qobject_cast<WebView *>(webView);

	if ( mWebView ) {
		mWebView->evaluateJavaScript("setPinsClickable(true)");
	}
}

void MapController::zoomOut() {
	if ( mWebView ) {
		mWebView->evaluateJavaScript("zoomOut()");
	}
}

void MapController::zoomIn() {
	if ( mWebView ) {
		mWebView->evaluateJavaScript("zoomIn()");
	}
}

void MapController::setZoomLevel( int zoomLevel ) {
	if ( mWebView ) {
		mWebView->evaluateJavaScript("setZoomLevel(" + QString::number(zoomLevel) + ")");
	}
}

void MapController::setMapPins( QString pins ) {
	if ( mWebView ) {
		//Serializer serializer;
		//QString mapPinsString( serializer.serialize(pins) );
		QString setPins;

		setPins.append( "addMapPins(" + pins + ")" );
		qDebug() << "Map Pins fn: " << setPins;

		mWebView->evaluateJavaScript( setPins );
	}
}


void MapController::setPinsClickable( bool clickable ) {

	if ( mWebView ) {
		if ( clickable )
			mWebView->evaluateJavaScript("setPinsClickable(true)");
		else
			mWebView->evaluateJavaScript("setPinsClickable(false)");
	}
}

WebView* MapController::getWebView() const{
	    return mWebView;
}

void MapController::setCenter(double lat, double lon, int zoom)
{
	QString slat = QString::number(lat);
	QString slon = QString::number(lon);
	QString szoom = QString::number(zoom);
	QString request = "setCenter(" + slat + ", " + slon + ", " + szoom + ")";

	if ( mWebView ) {
		mWebView->evaluateJavaScript(request);
	}
}
