/*
 * CustomObjectsModel.cpp
 *
 *  Created on: 14.03.2013
 *      Author: kirill
 */

#include "CustomObjectsModel.hpp"

using namespace bb::cascades;

CustomObjectsModel::CustomObjectsModel(QObject* parent) :
		GroupDataModel(), m_lastAdded(
				NULL) {
	setParent(parent);
}

CustomObjectsModel::~CustomObjectsModel() {
	qDebug() << "~CustomObjectsModel()";
}

void CustomObjectsModel::init(QString data) {
	qDebug() << "custom objects model initialization";
	foreach (QBCustomObject* item, QBCustomObject::parseFromJson(data)){
	insert(item);
}
}

void CustomObjectsModel::addCO(QString data, QString className) {
	qDebug() << "custom objects model add co " << data;
	QBCustomObject* co = QBCustomObject::parse(data);
	co->setClassName(className);

	QVariantList indexPath = found(co->id());
	if (!indexPath.isEmpty()) {
		qDebug() << "founded co with same id, replacing";
		updateItem(indexPath, co);
	} else {
		m_lastAdded = co;
		insert(co);
	}
}

void CustomObjectsModel::deleteCO(QString id) {
	qDebug() << "custom objects model delete co " << id;
	QVariantList indexPath = found(id);
	if (!indexPath.isEmpty()) {
		removeAt(indexPath);
	}
}

QBCustomObject* CustomObjectsModel::lastAdded() {
	return m_lastAdded;
}

QVariantList CustomObjectsModel::found(QString id) {
	for (QVariantList indexPath = first(); !indexPath.isEmpty(); indexPath = after(indexPath)) {
		QBCustomObject* dataCO = qobject_cast<QBCustomObject*>(data(indexPath).value<QObject*>());
		if (dataCO && dataCO->id() == id) {
			return indexPath;
		}
	}
	return QVariantList();
}
