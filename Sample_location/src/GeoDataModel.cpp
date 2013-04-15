/*
 * GeoDataModel.cpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#include "GeoDataModel.hpp"
#include "QBUUser.hpp"

using namespace bb::cascades;

GeoDataModel::GeoDataModel(QObject* parent) :
		GroupDataModel(),
		m_currentUser(NULL) {
	setParent(parent);
}

GeoDataModel::~GeoDataModel() {
	qDebug() << "~GeoDataModel()";
	if (m_currentUser) {
		delete m_currentUser;
	}
}

void GeoDataModel::init(QString data) {
	qDebug() << "geo data model initialization";
	foreach (QBLGeoData* item, QBLGeoData::parseFromJson(data)){
	insert(item);
}
}

void GeoDataModel::itemReceived(QString data) {
	qDebug() << "geo data model receive new item";

	QBLGeoData* item = QBLGeoData::parse(data);
	item->setUser(m_currentUser);
	QVariantList prevIdx = find(item->userId());
	if (prevIdx.length() > 0) {
		qDebug() << "successfully found exists item, replacing";
		updateItem(prevIdx, item);
	} else {
		insert(item);
	}
}

void GeoDataModel::updateCurrentUser(QString userData) {
	qDebug() << "geo data model update current user";
	m_currentUser = QBUUser::parse(userData);
}

QList<QObject*> GeoDataModel::getAllData() {
	return toListOfObjects();
}

QVariantList GeoDataModel::find(uint userId) {
	for (QVariantList indexPath = first(); !indexPath.isEmpty(); indexPath = after(indexPath)) {
		QBLGeoData* geoData = qobject_cast<QBLGeoData*>(data(indexPath).value<QObject*>());
		if (geoData && geoData->userId() == userId) {
			return indexPath;
		}
	}
	return QVariantList();
}

