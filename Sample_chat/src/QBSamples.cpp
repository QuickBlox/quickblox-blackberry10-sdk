// Default empty project template
#include "QBSamples.hpp"

#include "MyListmodel.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

#include "QBNetwork.hpp"
#include "QBChat.hpp"
#include "QBChatMessage.hpp"
#include "QBUUser.hpp"

using namespace bb::device;
using namespace bb::cascades;

QBSamples::QBSamples(bb::cascades::Application *app) : QObject(app)
{
	// register the MyListModel C++ type to be visible in QML
	qmlRegisterType<MyListModel>("com.rim.example.custom", 1, 0, "MyListModel");
	qmlRegisterType<QBChatMessage>("com.rim.example.custom", 1, 0, "QBChatMessage");
	qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");
	qmlRegisterType<QBUUser>("com.rim.example.custom", 1, 0, "QBUUser");
	qmlRegisterType<QBNetwork>("com.rim.example.custom", 1, 0, "QBNetwork");

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("_app", this);
	qml->setContextProperty("_chat", QBChat::instance());
	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
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
