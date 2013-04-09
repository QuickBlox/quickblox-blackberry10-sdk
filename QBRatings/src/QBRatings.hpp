/*
 * QBRatings.hpp
 *
 * Created on: 10.03.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef QBRatings_HPP_
#define QBRatings_HPP_

#include "RatingsProcessor.hpp"
#include "QBAuth.hpp"

#include <QObject>
#include <bb/cascades/QListDataModel>

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
class QBRatings: public QObject {
Q_OBJECT

	// The model that contains the progress and image data
Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)

public:
	QBRatings(bb::cascades::Application *app);

	// This method is called to start the loading of all images.
	Q_INVOKABLE
	void loadImages();
	Q_INVOKABLE
	void singout();
	Q_INVOKABLE
	void createModel();
	Q_INVOKABLE
	void setObjectQBNetwork(QBAuth *qbauth);
	Q_INVOKABLE
	void setRating(QString id, QString value, QString index, QString urlImage);
	Q_INVOKABLE
	int convertVariant(QVariantList indexPath);

public slots:
	// get device displayWidth
	int displayWidth();
	// get device displayHeight
	int displayHeight();

private Q_SLOTS:
    /*
     * Response handler for the network operation.
     */
    void onReplyFinished();

private:
	QBAuth *qbauth;
	QString m_id, m_value, m_index, m_urlImage;

	// The accessor method for the property
	bb::cascades::DataModel* model() const;

	// The model that contains the progress and image data
	bb::cascades::QListDataModel<QObject*>* m_model;
};

#endif /* QBRatings_HPP_ */
