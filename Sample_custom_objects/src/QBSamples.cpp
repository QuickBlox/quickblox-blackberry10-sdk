// Default empty project template
#include "QBSamples.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

#include "QBNetwork.hpp"
#include "QBCustomObject.hpp"
#include "CustomObjectsModel.hpp"

using namespace bb::device;
using namespace bb::cascades;

QBSamples::QBSamples(bb::cascades::Application *app) : QObject(app)
{
	// register the MyListModel C++ type to be visible in QML
	qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");
	qmlRegisterType<QBNetwork>("com.rim.example.custom", 1, 0, "QBNetwork");
	qmlRegisterType<QBCustomObject>("com.rim.example.custom", 1,0, "QBCustomObject");
	qmlRegisterType<CustomObjectsModel>("com.rim.example.custom", 1,0, "CustomObjectsModel");

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	qml->setContextProperty("_app", this);
//	qml->setContextProperty("_network", QBNetwork());
	// set created root object as a scene
	app->setScene(root);
}

QBSamples::~QBSamples()
{
}

/**
 * Get display width
 */
int QBSamples::displayWidth() {
	bb::device::DisplayInfo display;
	int width = display.pixelSize().width();
	return width;
}

/**
 * Get display height
 */
int QBSamples::displayHeight() {
	bb::device::DisplayInfo display;
	int height = display.pixelSize().height();
	return height;
}

QBCustomObject* QBSamples::getNote(QString note, QString comment) {
	QBCustomObject* result = new QBCustomObject();

	result->setClassName(QString("Note"));
	result->addFieldValue(QString("note"), note);
	result->addFieldValue(QString("comments"), QVariantList() << comment);
	result->addFieldValue(QString("status"), QString("New"));

	return result;
}
