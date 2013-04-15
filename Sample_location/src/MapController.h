#ifndef MAPCONTROLLER_H_
#define MAPCONTROLLER_H_

#include <bb/cascades/WebView>
#include <bb/cascades/QmlDocument>

class MapController: public QObject {
	Q_OBJECT

public:

	// Constructor
	MapController(QObject *parent = 0);
	virtual ~MapController();

	Q_INVOKABLE void setWebView(QObject* webView);
	Q_INVOKABLE void zoomIn();
	Q_INVOKABLE void zoomOut();
	Q_INVOKABLE void setZoomLevel( int zoomLevel );
	Q_INVOKABLE void setMapPins(QString pins);
	Q_INVOKABLE void setPinsClickable( bool clickable );
	Q_INVOKABLE void setCenter(double lat, double lon, int zoom);

//	signals:

//private slots:

private:
	// Network manager
	bb::cascades::WebView* getWebView() const;
	QPointer<bb::cascades::WebView> mWebView;
	bb::cascades::QmlDocument* mQmlDocument;
};

#endif /* MAPCONTROLLER_H_ */
