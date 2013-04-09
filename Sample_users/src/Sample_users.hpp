/*
 * Sample_users.hpp
 *
 * Created on: 18.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef Sample_users_HPP_
#define Sample_users_HPP_

#include "QBNetwork.hpp"
#include "MyListmodel.hpp"
#include "QBNetwork.hpp"

#include <QObject>

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
class Sample_users: public QObject {
	Q_OBJECT
public:
	Sample_users(bb::cascades::Application *app);Q_INVOKABLE
	void singout();Q_INVOKABLE
	void setObjectQBNetwork(QBNetwork *qbnetwork);
	virtual ~Sample_users() {
	}

public slots:
	int displayWidth();
	int displayHeight();
private:
	QBNetwork *qbnetwork;
};

#endif /* Sample_users_HPP_ */
