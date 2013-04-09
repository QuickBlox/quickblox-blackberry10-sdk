/*
 * QBRatings.cpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "QBRatings.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

using namespace bb::cascades;
using namespace bb::system;

QBRatings::QBRatings(bb::cascades::Application *app) :
		QObject(app), m_model(new QListDataModel<QObject*>()) {
	// Register custom type to QML
	qmlRegisterType<RatingsProcessor>();
	qmlRegisterType<QBAuth>("com.example.QBRatings.QBAuth", 1, 0, "QBAuth");

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

void QBRatings::loadImages() {
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

	// Call the load() method for each ImageLoader instance inside the model
	for (int row = 0; row < m_model->size(); ++row) {
		qobject_cast<RatingsProcessor*>(m_model->value(row))->load();
	}
}

bb::cascades::DataModel* QBRatings::model() const {
	return m_model;
}

void QBRatings::createModel() {
	m_model = new QListDataModel<QObject*>();
}

/**
 * Set ratings
 */
void QBRatings::setRating(QString id, QString value, QString index,
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

void QBRatings::onReplyFinished() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (reply) {
		if (reply->error() == QNetworkReply::NoError) {
			qbauth->requestCreateScore(m_id.toInt(), m_value.toInt());
			qobject_cast<RatingsProcessor*>(m_model->value(m_index.toInt()))->refresh(m_id);
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
void QBRatings::singout() {
	qbauth->removeSession();
}

/**
 * Set object
 */
void QBRatings::setObjectQBNetwork(QBAuth *qbauth_) {
	qbauth = qbauth_;
}

/**
 * Get display width
 */
int QBRatings::displayWidth() {
	bb::device::DisplayInfo display;
	int width = display.pixelSize().width();
	return width;
}

/**
 * Get display height
 */
int QBRatings::displayHeight() {
	bb::device::DisplayInfo display;
	int height = display.pixelSize().height();
	return height;
}

int QBRatings::convertVariant(QVariantList indexPath) {
	QString number = QString::number(indexPath[0].toInt());
	return number.toInt();
}
