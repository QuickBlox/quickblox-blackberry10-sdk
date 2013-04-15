/*
 * GeoDataModel.hpp
 *
 *  Created on: 21.03.2013
 *      Author: kirill
 */

#ifndef GEODATAMODEL_HPP_
#define GEODATAMODEL_HPP_

#include <QObject>
#include <bb/cascades/GroupDataModel>

#include "QBLGeoData.hpp"

class GeoDataModel: public bb::cascades::GroupDataModel {
	Q_OBJECT
public:
	GeoDataModel(QObject* parent = 0);
	virtual ~GeoDataModel();

	Q_INVOKABLE void init(QString data);
	Q_INVOKABLE void itemReceived(QString data);
	Q_INVOKABLE void updateCurrentUser(QString userData);
	Q_INVOKABLE QList<QObject*> getAllData();

private:
	QVariantList find(uint userId);
	QBUUser* m_currentUser;
};

#endif /* GEODATAMODEL_HPP_ */
