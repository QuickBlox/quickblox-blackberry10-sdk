/*
 * MyListModel.cpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "mylistmodel.hpp"

using namespace bb::cascades;

MyListModel::MyListModel(QObject* parent) :
		bb::cascades::QVariantListDataModel() {
	setParent(parent);
}

MyListModel::~MyListModel() {
}

void MyListModel::load(const QString& file_name) {
	removeHeaderJS(file_name);
	bb::data::JsonDataAccess jda;
	QVariant list = jda.load(file_name);

    QVariantList feed = list.toList();

    QList<QVariant> lt;

    // for each object in the array, push the variantmap in its raw form
    // into the ListView
    for (QList<QVariant>::iterator it = feed.begin(); it != feed.end(); it++)
    {
        QVariantMap tweet = it->toMap();
        QMapIterator<QString, QVariant> i(tweet);
        while (i.hasNext()) {
            i.next();
            QVariantMap tw = i.value().toMap();
            lt.append(tw);
        }
    }

	if (jda.hasError()) {
		bb::data::DataAccessError error = jda.error();
		qDebug() << file_name << "JSON loading error: " << error.errorType()
				<< ": " << error.errorMessage();
	} else {
		this->append(lt);
	}
}

void MyListModel::removeHeaderJS(const QString& file_name) {
		QFile file(file_name);
		QString process_line_str = "";
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		// read the file
		while (!file.atEnd()) {
			QString line = file.readLine();
			process_line_str += line;
		}
		file.close();

		int startPosition = 0;
		int endPosition = process_line_str.length()-1;

		// remove header from file
		for (int i = 0; i < process_line_str.length(); i++) {
			if (process_line_str.at(i) == '[') {
				startPosition = i;
				break;
			}
		}

		process_line_str = process_line_str.mid(startPosition, endPosition-startPosition);

		QFile fileResult(file_name);
		fileResult.open(QIODevice::WriteOnly | QIODevice::Text);
		fileResult.write((const char *) process_line_str.toAscii().data());
		fileResult.close();
}
