/*
 * MyListModel.hpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef MyListModel_HPP_
#define MyListModel_HPP_

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMetaType>
#include <bb/cascades/QListDataModel>
#include <bb/data/JsonDataAccess>

/*!
 * @brief Mutable list model implementation
 */
class MyListModel: public bb::cascades::QVariantListDataModel {
Q_OBJECT
public:
	/*!
	 * @brief Convenience method for loading data from JSON file.
	 *
	 * @param file_name  file to load
	 */
	Q_INVOKABLE
	void load(const QString& file_name);

private:
	void removeHeaderJS(const QString& file_name);

public:
	MyListModel(QObject* parent = 0);
	virtual ~MyListModel();
};

#endif /* MyListModel_HPP_ */
