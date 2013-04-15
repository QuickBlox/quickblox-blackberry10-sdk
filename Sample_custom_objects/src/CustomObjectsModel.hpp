/*
 * CustomObjectsModel.hpp
 *
 *  Created on: 14.03.2013
 *      Author: kirill
 */

#ifndef CUSTOMOBJECTSMODEL_HPP_
#define CUSTOMOBJECTSMODEL_HPP_

#include <QObject>
#include <bb/cascades/GroupDataModel>

#include "QBCustomObject.hpp"

class CustomObjectsModel: public bb::cascades::GroupDataModel {
Q_OBJECT
public:
	CustomObjectsModel(QObject* parent = 0);
	virtual ~CustomObjectsModel();

	Q_INVOKABLE void init(QString data);
	Q_INVOKABLE void addCO(QString data, QString className = QString::null);
	Q_INVOKABLE void deleteCO(QString id);

	Q_INVOKABLE QBCustomObject* lastAdded();

private:
	QVariantList found(QString id);
	QBCustomObject* m_lastAdded;
};

#endif /* CUSTOMOBJECTSMODEL_HPP_ */
