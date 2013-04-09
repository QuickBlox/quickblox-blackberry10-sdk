/*
 * Sample_content.cpp
 *
 * Created on: 20.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "Sample_content.hpp"

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::data;

int Sample_content::counter = -1;

Sample_content::Sample_content(bb::cascades::Application *app) :
		QObject(app), m_model(
				new QListDataModel<QObject*>()) {

	qmlRegisterType<QBAuth>("com.example.Sample_content.QBAuth", 1, 0, "QBAuth");

	// Register some classes for Filepicker for QML
	qmlRegisterType<bb::cascades::pickers::FilePicker>("bb.cascades.pickers", 1,
			0, "FilePicker");
	qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerMode>(
			"bb.cascades.pickers", 1, 0, "FilePickerMode", "");
	qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerSortFlag>(
			"bb.cascades.pickers", 1, 0, "FilePickerSortFlag", "");
	qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerSortOrder>(
			"bb.cascades.pickers", 1, 0, "FilePickerSortOrder", "");
	qmlRegisterUncreatableType<bb::cascades::pickers::FileType>(
			"bb.cascades.pickers", 1, 0, "FileType", "");
	qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerViewMode>(
			"bb.cascades.pickers", 1, 0, "FilePickerViewMode", "");

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///HomePage.qml").parent(
			this);

	qml->setContextProperty("_app", this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app->setScene(root);

	timerDelay = new QTimer(this);
	QObject::connect(timerDelay, SIGNAL(timeout()), this,
			SLOT(slotTimerDelay()));
}

void Sample_content::uploadImage(QString data) {
	qbauth->requestCreateFile(data.mid(data.indexOf("shared"), data.length()));
}

void Sample_content::createModel() {
	m_model = new QListDataModel<QObject*>();
}

void Sample_content::updateList(QString path) {
	if (path == "") {
		return;
	}
	m_blob_name = "";
	QStringList filePathList = path.split('/');
	QString m_nameFile = filePathList.at(filePathList.count() - 1);

	QFile sourceFile("app/native/assets/JDataTaggedList.json");
	if (!sourceFile.open(QIODevice::ReadOnly)) {
		return;
	}

	JsonDataAccess jda;
	QVariant qtData = jda.loadFromBuffer(sourceFile.readAll());
	sourceFile.close();

	if (jda.hasError()) {
		const DataAccessError err = jda.error();
	} else {
		const QVariantMap header = qtData.value<QVariantMap>();
		QMapIterator<QString, QVariant> iterHeader(header);
		while (iterHeader.hasNext()) {
			iterHeader.next();
			if (iterHeader.key() == "items") {
				QVariantList object1 = iterHeader.value().toList();
				QListIterator<QVariant> iter1(object1);
				while (iter1.hasNext()) {
					//QVariant v = iter1.next();
					const QVariantMap object2 = iter1.next().toMap();
					QMapIterator<QString, QVariant> iter2(object2);
					while (iter2.hasNext()) {
						iter2.next();
						const QVariantMap object3 = iter2.value().toMap();
						QMapIterator<QString, QVariant> iter3(object3);
						while (iter3.hasNext()) {
							iter3.next();
							if (iter3.key() == "name"
									&& iter3.value() == m_nameFile) {
								m_blob_name = iter3.value().toString();
							}
							if (iter3.key() == "uid" && m_blob_name != "") {
								m_blob_uid = iter3.value().toString();
								m_model->append(
										new ImageLoader(m_blob_uid, this));
								qobject_cast<ImageLoader*>(
										m_model->value(m_model->size()-1))->load();
								return;
							}
						}
					}
				}
			}
		}
	}
}

void Sample_content::loadImages() {
	m_model->clear();

	QFile sourceFile("app/native/assets/JDataTaggedList.json");
	if (!sourceFile.open(QIODevice::ReadOnly)) {
		return;
	}

	JsonDataAccess jda;
	QVariant qtData = jda.loadFromBuffer(sourceFile.readAll());
	sourceFile.close();

	if (jda.hasError()) {
		const DataAccessError err = jda.error();
		qDebug()
				<< "-----------------------------Error converting JSON data:--------------------------";
	} else {
		const QVariantMap header = qtData.value<QVariantMap>();
		QMapIterator<QString, QVariant> iterHeader(header);
		while (iterHeader.hasNext()) {
			iterHeader.next();
			if (iterHeader.key() == "items") {
				QVariantList object1 = iterHeader.value().toList();
				QListIterator<QVariant> iter1(object1);
				while (iter1.hasNext()) {
					//QVariant v = iter1.next();
					const QVariantMap object2 = iter1.next().toMap();
					QMapIterator<QString, QVariant> iter2(object2);
					while (iter2.hasNext()) {
						iter2.next();
						const QVariantMap object3 = iter2.value().toMap();
						QMapIterator<QString, QVariant> iter3(object3);
						while (iter3.hasNext()) {
							iter3.next();
							if (iter3.key() == "uid") {
								m_blob_uid = iter3.value().toString();
								counter = 0;
								m_model->append(
										new ImageLoader(m_blob_uid, this));
							}
							if (iter3.key() == "name") {
								m_blob_name = iter3.value().toString();
							}
						}
					}
				}
			}
		}
	}
	if (counter == 0) {
    for (int row = 0; row < m_model->size(); ++row) {
        qobject_cast<ImageLoader*>(m_model->value(row))->load();
    }
	}
}

bb::cascades::DataModel* Sample_content::model() const {
	return m_model;
}

/**
 * Sing in
 */
void Sample_content::singout() {
	qbauth->removeSession();
}

/**
 * Set object
 */
void Sample_content::setObjectQBNetwork(QBAuth *qbauth_) {
	qbauth = qbauth_;
}

/**
 * Get object
 */
QBAuth* Sample_content::getObjectQBNetwork() {
	return qbauth;
}

/**
 * Get display width
 */
int Sample_content::displayWidth() {
	bb::device::DisplayInfo display;
	int width = display.pixelSize().width();
	return width;
}

/**
 * Get display height
 */
int Sample_content::displayHeight() {
	bb::device::DisplayInfo display;
	int height = display.pixelSize().height();
	return height;
}
