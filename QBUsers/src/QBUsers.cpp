/*
 * Users.cpp
 *
 * Created on: 18.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBUsers.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

using namespace bb::device;
using namespace bb::cascades;

QBUsers::QBUsers(bb::cascades::Application *app) :
		QObject(app) {
	// register the MyListModel C++ type to be visible in QML
	qmlRegisterType<MyListModel>("com.rim.example.custom", 1, 0, "MyListModel");
	qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");
	qmlRegisterType<QBNetwork>("com.example.QBUsers.QBNetwork", 1, 0,
			"QBNetwork");

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///HomePage.qml").parent(
			this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	qml->setContextProperty("_app", this);
	// set created root object as a scene
	app->setScene(root);
}

// sing in
void QBUsers::singout() {
	qbnetwork->removeSession();
}

/**
 * Set object
 */
void QBUsers::setObjectQBNetwork(QBNetwork *qbnetwork_) {
	qbnetwork = qbnetwork_;
}

/**
 * Get display width
 */
int QBUsers::displayWidth() {
	bb::device::DisplayInfo display;
	int width = display.pixelSize().width();
	return width;
}

/**
 * Get display height
 */
int QBUsers::displayHeight() {
	bb::device::DisplayInfo display;
	int height = display.pixelSize().height();
	return height;
}
