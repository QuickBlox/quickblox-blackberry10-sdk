/*
 * QBUsers.hpp
 *
 * Created on: 18.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef QBUsers_HPP_
#define QBUsers_HPP_

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
class QBUsers: public QObject {
	Q_OBJECT
public:
	QBUsers(bb::cascades::Application *app);Q_INVOKABLE
	void singout();Q_INVOKABLE
	void setObjectQBNetwork(QBNetwork *qbnetwork);
	virtual ~QBUsers() {
	}

public slots:
	int displayWidth();
	int displayHeight();
private:
	QBNetwork *qbnetwork;
};

#endif /* QBUsers_HPP_ */
