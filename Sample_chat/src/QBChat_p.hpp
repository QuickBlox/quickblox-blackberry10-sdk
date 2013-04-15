/*
 * QBChat_p.hpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#ifndef QBCHAT_P_HPP_
#define QBCHAT_P_HPP_

#include "QBGlobal.hpp"
#include "QBChat.hpp"
#include <QObject>

#include "QXmppClient.h"
#include "QXmppConfiguration.h"
#include "QXmppMucManager.h"
#include "QXmppDiscoveryManager.h"

class QBUUser;

////////////// Private d_ptr implementation ////////////////

class QB_EXPORT QBChatPrivate: public QObject
{
	Q_OBJECT
public:
	QBChatPrivate(QBChat *parent);
    ~QBChatPrivate();

    static QBChat* m_instance;

    QXmppClient* getXmppClient();
    QXmppMucManager* getMucManager();
    QXmppDiscoveryManager* getDiscoveryManager();

    QBUUser* m_qbuser;
    QBChat* const q_ptr;

    Q_DECLARE_PUBLIC(QBChat);

private:
    QXmppClient* m_xmppClient;
    QXmppMucManager* m_mucManager;
    QXmppDiscoveryManager* m_discoveryManager;
};


#endif /* QBCHAT_P_HPP_ */
