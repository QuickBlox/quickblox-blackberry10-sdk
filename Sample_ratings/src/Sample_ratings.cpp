/*
 * Sample_ratings.cpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "Sample_ratings.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

using namespace bb::cascades;
using namespace bb::system;

int Sample_ratings::index = 0;

Sample_ratings::Sample_ratings(bb::cascades::Application *app) :
		QObject(app), m_model(new QListDataModel<QObject*>()) {
	// Register custom type to QML
	qmlRegisterType<RatingsProcessor>();
	qmlRegisterType<QBAuth>("com.example.Sample_ratings.QBAuth", 1, 0,
			"QBAuth");

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

void Sample_ratings::loadImages() {
	m_model->clear();
	// Fill the model with data
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/blackberry_sdk.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/android_sdk.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/ios_sdk.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/javascript.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/rest_api.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/web.png",
					this));
	m_model->append(
			new RatingsProcessor(
					"http://quickblox.com/developers/skins/quickblox/img/sdk/wp7_sdk.png",
					this));

	qobject_cast<RatingsProcessor*>(m_model->value(index))->load();
	index++;

	timerDelay = new QTimer(this);
	QObject::connect(timerDelay, SIGNAL(timeout()), this, SLOT(slotTimerDelay()));
	timerDelay->start(2000);
}

void Sample_ratings::slotTimerDelay() {
	if (index == m_model->size()-1) {
		timerDelay->stop();
	}
	qobject_cast<RatingsProcessor*>(m_model->value(index))->load();
	index++;
}

bb::cascades::DataModel* Sample_ratings::model() const {
	return m_model;
}

void Sample_ratings::createModel() {
	m_model = new QListDataModel<QObject*>();
}

/**
 * Set ratings
 */
void Sample_ratings::setRating(QString id, QString value, QString index,
		QString urlImage) {
	m_id = id;
	m_value = value;
	m_index = index;
	m_urlImage = urlImage;

	QNetworkAccessManager* netManager = new QNetworkAccessManager(this);
	QNetworkRequest request(QUrl("http://google.com"));

	QNetworkReply* reply = netManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void Sample_ratings::onReplyFinished() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			qbauth->requestCreateScore(m_id.toInt(), m_value.toInt());
			qobject_cast<RatingsProcessor*>(m_model->value(m_index.toInt()))->refresh(
					m_id);
		} else {
			if (reply->error() < 100) {
				SystemToast *toast = new SystemToast(this);
				toast->setBody("Please check your internet connection");
				toast->show();
			}
		}
	}
}

// sing in
void Sample_ratings::singout() {
	qbauth->removeSession();
}

/**
 * Set object
 */
void Sample_ratings::setObjectQBNetwork(QBAuth *qbauth_) {
	qbauth = qbauth_;
}

/**
 * Get display width
 */
int Sample_ratings::displayWidth() {
	bb::device::DisplayInfo display;
	int width = display.pixelSize().width();
	return width;
}

/**
 * Get display height
 */
int Sample_ratings::displayHeight() {
	bb::device::DisplayInfo display;
	int height = display.pixelSize().height();
	return height;
}

int Sample_ratings::convertVariant(QVariantList indexPath) {
	QString number = QString::number(indexPath[0].toInt());
	return number.toInt();
}
