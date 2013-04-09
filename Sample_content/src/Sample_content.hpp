/*
 * Sample_content.hpp
 *
 * Created on: 20.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef Sample_content_HPP_
#define Sample_content_HPP_

#include <QObject>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <bb/system/CardDoneMessage>
#include <bb/cascades/QListDataModel>
#include <QtCore/QObject>
#include <bb/data/JsonDataAccess>
#include <QDebug>

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/pickers/FilePickerMode>
#include <bb/cascades/pickers/FilePickerSortFlag>
#include <bb/cascades/pickers/FilePickerSortOrder>
#include <bb/cascades/pickers/FileType>
#include <bb/cascades/pickers/FilePickerViewMode>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/device/DisplayInfo>

#include "QBAuth.hpp"
#include "ImageLoader.hpp"

using namespace bb::cascades;
using namespace bb::system;

namespace bb {
namespace cascades {
class Application;
}
}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class Sample_content: public QObject {
Q_OBJECT
Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)
public:
	Sample_content(bb::cascades::Application *app);
	virtual ~Sample_content() {
	}

	/**
	 * uses Invokation Framework to Invoke embedded Card
	 * as Previewer
	 */
	Q_INVOKABLE
	void uploadImage(QString data);
	Q_INVOKABLE
	void singout();
	Q_INVOKABLE
	void setObjectQBNetwork(QBAuth *qbauth);
	Q_INVOKABLE
	QBAuth* getObjectQBNetwork();
	Q_INVOKABLE
	void updateList(QString path);
	Q_INVOKABLE
	void loadImages();
	Q_INVOKABLE
	void createModel();

public slots:
	// get device displayWidth
	int displayWidth();
	// get device displayHeight
	int displayHeight();

private:
	// The accessor method for the property
	bb::cascades::DataModel* model() const;

	// The model that contains the progress and image data
	bb::cascades::QListDataModel<QObject*>* m_model;

	QString m_blob_uid, m_blob_name;

	QTimer *timerDelay;
	static int counter;

	QBAuth *qbauth;
};

#endif /* Sample_content_HPP_ */
