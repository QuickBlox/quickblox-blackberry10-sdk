/*
 * QBChat.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#include "QBChat.hpp"
#include "QBChat_p.hpp"

#include "QBUUser.hpp"
#include "QBUtils.hpp"

#include "QXmppLogger.h"
#include "QXmppPresence.h"
#include "QXmppMessage.h"
#include "QXmppIq.h"
#include "QXmppElement.h"

const QString QBChat::QB_CHAT_SERVICE_MUC_DOMAIN = QString("muc.chat.quickblox.com");
const QString QBChat::QB_CHAT_SERVICE_DOMAIN = QString("chat.quickblox.com");

QBChat* QBChatPrivate::m_instance = NULL;

////////////// Private d_ptr implementation ////////////////

QBChatPrivate::QBChatPrivate(QBChat *parent) :
		QObject(parent),
		m_qbuser(NULL),
		q_ptr(parent),
		m_xmppClient(NULL),
		m_mucManager(NULL),
		m_discoveryManager(NULL)
{

}

QBChatPrivate::~QBChatPrivate()
{
	Q_Q(QBChat);
	if (m_xmppClient)
	{
		disconnect(m_mucManager, SIGNAL(roomAdded(QXmppMucRoom*)), q, SLOT(onRoomAdded(QXmppMucRoom*)));
		delete m_mucManager;

		disconnect(m_xmppClient, SIGNAL(connected()), q, SLOT(onChatConnected()));
		disconnect(m_xmppClient, SIGNAL(disconnected()), q, SLOT(onChatDisconnected()));
		disconnect(m_xmppClient, SIGNAL(error(QXmppClient::Error)), q, SLOT(onChatError(QXmppClient::Error)));
		disconnect(m_xmppClient, SIGNAL(messageReceived(const QXmppMessage)), q, SLOT(onMessageReceived(const QXmppMessage)));
		disconnect(m_xmppClient, SIGNAL(presenceReceived(const QXmppPresence)), q, SLOT(onPresenceReceived(const QXmppPresence)));
		disconnect(m_xmppClient, SIGNAL(iqReceived(const QXmppIq)), q, SLOT(onIqReceived(const QXmppIq)));
		delete m_xmppClient;
	}
}

QXmppClient* QBChatPrivate::getXmppClient()
{
	Q_Q(QBChat);
	if (!m_xmppClient)
	{
		m_xmppClient = new QXmppClient;
		connect(m_xmppClient, SIGNAL(connected()), q, SLOT(onChatConnected()));
		connect(m_xmppClient, SIGNAL(disconnected()), q, SLOT(onChatDisconnected()));
		connect(m_xmppClient, SIGNAL(error(QXmppClient::Error)), q, SLOT(onChatError(QXmppClient::Error)));
		connect(m_xmppClient, SIGNAL(messageReceived(const QXmppMessage)), q, SLOT(onMessageReceived(const QXmppMessage)));
		connect(m_xmppClient, SIGNAL(presenceReceived(const QXmppPresence)), q, SLOT(onPresenceReceived(const QXmppPresence)));
		connect(m_xmppClient, SIGNAL(iqReceived(const QXmppIq)), q, SLOT(onIqReceived(const QXmppIq)));

		//for logging functionality
		QXmppLogger::getLogger()->setMessageTypes(QXmppLogger::AnyMessage);
		QXmppLogger::getLogger()->setLoggingType(QXmppLogger::StdoutLogging);
		m_xmppClient->setLogger(QXmppLogger::getLogger());

		//multi user chat manager
		m_mucManager = new QXmppMucManager;
		m_xmppClient->addExtension(m_mucManager);
		connect(m_mucManager, SIGNAL(roomAdded(QXmppMucRoom*)), q, SLOT(onRoomAdded(QXmppMucRoom*)));

		//discovery manager
		m_discoveryManager = m_xmppClient->findExtension<QXmppDiscoveryManager>();
		connect(m_discoveryManager, SIGNAL(itemsReceived(const QXmppDiscoveryIq&)), q, SLOT(onItemsReceived(const QXmppDiscoveryIq&)));
	}

	return m_xmppClient;
}

QXmppMucManager* QBChatPrivate::getMucManager()
{
	return m_mucManager;
}

QXmppDiscoveryManager* QBChatPrivate::getDiscoveryManager()
{
	return m_discoveryManager;
}

/////////////// Public implementation ////////////////

QBChat::QBChat(QObject *parent) :
    QObject(parent) ,
    d_ptr(new QBChatPrivate(this))
{

}

QBChat::~QBChat()
{
    delete d_ptr;
}

QBChat* QBChat::instance() {
	if(!QBChatPrivate::m_instance) {
		QBChatPrivate::m_instance = new QBChat;
	}
	return QBChatPrivate::m_instance;
}

bool QBChat::login(QString username, QString password)
{
	QBUUser* user = new QBUUser;
	user->setId(username);
	user->setPassword(password);

	return loginWithUser(user);
}

bool QBChat::loginWithUser(QBUUser* user)
{
	Q_D(QBChat);

	qDebug() << "Logging with user id " << user->getId() << " password " << user->getPassword();

	// return false and do not login if already logged in
	if (isLoggedIn())
	{
		return false;
	}

	d->m_qbuser = user;

	QXmppConfiguration conf;
	conf.setHost(QB_CHAT_SERVICE_MUC_DOMAIN);
	conf.setJid(jidFromUserID(user->getId()));
	conf.setPassword(user->getPassword());

	d->getXmppClient()->connectToServer(conf);

	return true;
}

bool QBChat::isLoggedIn()
{
	Q_D(QBChat);

	return (d->getXmppClient()->isConnected() && d->getXmppClient()->isAuthenticated());
}

void QBChat::logout()
{
	Q_D(QBChat);

	d->getXmppClient()->disconnectFromServer();
}

bool QBChat::sendMessage(QString destination, QString message)
{
	Q_D(QBChat);

	qDebug() << "Sending message to user " << destination << " message: " << message;

	QXmppMessage xmppMessage(jidFromUserID(d->m_qbuser->getId()), jidFromUserID(destination), message);
	bool result = d->getXmppClient()->sendPacket(xmppMessage);

	if (result) {
		QBChatMessage* qbmessage = new QBChatMessage;
		qbmessage->setFrom(d->m_qbuser->getId());
		qbmessage->setTo(destination);
		qbmessage->setText(message);

		emit messageSended(qbmessage);
	}

	return result;
}

void QBChat::sendPresence()
{
	Q_D(QBChat);

	qDebug() << "Sending presence";
	d->getXmppClient()->sendPacket(QXmppPresence());
}

/*
 * Convert user to jid
 */
QString QBChat::jidFromUserID(QString userID)
{
	return userID + "-" + QBLOX_APP_ID + "@" + QB_CHAT_SERVICE_DOMAIN;
}

/*
 * Convert jid to QBUser ID
 */
QString QBChat::userIDfromJid(QString jid)
{
	if (jid.indexOf("-") > 0)
	{
		return  jid.left(jid.indexOf("-"));
	}

	return 0;
}

QString QBChat::roomIDfromJid(QString jid)
{
	if (jid.indexOf("@") > 0)
	{
		return jid.left(jid.indexOf("@"));
	}

	return 0;
}

QString QBChat::userIdfromRoomMessage(const QXmppMessage &message) {
	QString messageFrom = message.from();

	return messageFrom.right(messageFrom.length() - messageFrom.lastIndexOf('/') - 1);
}

QString QBChat::currentUserId()
{
	Q_D(QBChat);
	return d->m_qbuser->getId();
}

/////////////// Rooms implementation ////////////////

void QBChat::createRoom(QString name, bool isOnlyMembers, bool isPersistent)
{
	Q_D(QBChat);

	name = name.replace(" ", "");
	name = name + "@" + QB_CHAT_SERVICE_MUC_DOMAIN;

	d->getMucManager()->addRoom(name);
}

//- (void)requestRoomUsers:(QBChatRoom *)room{
//    [room.xmppRoom requestUsers];
//}

void QBChat::requestAllRooms()
{
	Q_D(QBChat);

	foreach(QXmppMucRoom* room, d->getMucManager()->rooms())
	{
		emit roomAdded(roomIDfromJid(room->jid()), room->name());
	}

	d->getDiscoveryManager()->requestItems(QB_CHAT_SERVICE_MUC_DOMAIN);
}

void QBChat::joinToRoom(QString roomId)
{
	Q_D(QBChat);

	foreach(QXmppMucRoom* room, d->getMucManager()->rooms())
	{
		if (roomIDfromJid(room->jid()) == roomId)
		{
			room->setNickName(d->m_qbuser->getId());
			bool result = room->join();
			qDebug() << "try join to room " << roomId << " " << result;
		}
	}
}

bool QBChat::sendRoomMessage(QString destination, QString message)
{
	Q_D(QBChat);

	qDebug() << "Sending message to room " << destination << " message: " << message;

	foreach(QXmppMucRoom* room, d->getMucManager()->rooms())
	{
		if (roomIDfromJid(room->jid()) == destination)
		{
			bool result = room->sendMessage(message);
			return result;
		}
	}
	return false;
}

void QBChat::leaveRoom(QString roomId)
{
	Q_D(QBChat);

	qDebug() << "leave room " << roomId;

	foreach(QXmppMucRoom* room, d->getMucManager()->rooms())
	{
		if (roomIDfromJid(room->jid()) == roomId)
		{
			bool result = room->leave();
			qDebug() << result;
		}
	}
}

/////////////// Slots implementation ////////////////

void QBChat::onChatConnected()
{
	qDebug() << "onChatConnected()";

	emit loggedIn();
}

void QBChat::onChatDisconnected()
{
	qDebug() << "onChatDisconnected()";
}

void QBChat::onChatError(QXmppClient::Error error)
{
	qDebug() << "onChatError() " << error;
}

void QBChat::onMessageReceived(const QXmppMessage &message)
{
//	Q_D(QBChat);
	qDebug() << "onMessageReceived() " << message.body();

	QBChatMessage* qbmessage = new QBChatMessage;
	qbmessage->setFrom(userIDfromJid(message.from()));
	qbmessage->setTo(userIDfromJid(message.to()));
	qbmessage->setText(message.body());
	if (message.stamp().toString() != "")
	{
		qbmessage->setTime(message.stamp().toString());
		qbmessage->setTimestamp(message.stamp().toTime_t());
	}

	emit messageReceived(qbmessage);
}

void QBChat::onPresenceReceived(const QXmppPresence &presence)
{
	qDebug() << "onPresenceReceive() type = " << presence.type() << " status text = " << presence.statusText();

	QBUUser* user = new QBUUser;
	user->setId(userIDfromJid(presence.from()));
	user->setPresence(presence.type());

	emit presenceReceived(user);
}

void QBChat::onItemsReceived(const QXmppDiscoveryIq& items)
{
	Q_D(QBChat);

	qDebug() << "onItemsReceived()";

	foreach(const QXmppDiscoveryIq::Item &item, items.items())
	{
//		qDebug() << "room jid = " << item.jid() << " name = " << item.name() << " node = " << item.node();
		d->getMucManager()->addRoom(item.jid());
	}
}

void QBChat::onIqReceived(const QXmppIq &iq)
{
	qDebug() << "onIqReceived() type = " << iq.type();
}

// Muc
void QBChat::onRoomAdded(QXmppMucRoom *room)
{
	qDebug() << "room added";
	qDebug() << "jid " << room->jid();
	qDebug() << "name " << room->name();
	qDebug() << "nickName " << room->nickName();
	qDebug() << "participants " << room->participants();
	qDebug() << "password " << room->password();
	qDebug() << "subject " << room->subject();

	connect(room, SIGNAL(messageReceived(const QXmppMessage)), this, SLOT(onRoomMessageReceived(const QXmppMessage)));
	connect(room, SIGNAL(joined()), this, SLOT(onJoined()));
	connect(room, SIGNAL(configurationReceived(const QXmppDataForm &)), this, SLOT(onConfigurationReceived(const QXmppDataForm &)));
	connect(room, SIGNAL(permissionsReceived(const QList<QXmppMucItem> &)), this, SLOT(onPermissionsReceived(const QList<QXmppMucItem> &)));

	emit roomAdded(roomIDfromJid(room->jid()), room->name());
}

void QBChat::onConfigurationReceived(const QXmppDataForm &configuration)
{
	qDebug() << "onConfigurationReceived()";

	QString tempString;
	QXmlStreamWriter xmlWriter(&tempString);
	configuration.toXml(&xmlWriter);

	qDebug() << tempString;
}

void QBChat::onPermissionsReceived(const QList<QXmppMucItem> &permissions)
{
	qDebug() << "onPermissionsReceived()";

	foreach (const QXmppMucItem& item, permissions)
	{
		qDebug() << "permissions for " << item.actor();
		qDebug() << "jid " << item.jid();
		qDebug() << "nick " << item.nick();
		qDebug() << "reason " << item.reason();
		qDebug() << "affiliation " << (int)item.affiliation();
		qDebug() << "role " << (int)item.role();
	}
}

void QBChat::onJoined()
{
	qDebug() << "onJoined() ";

	QXmppMucRoom* room = qobject_cast<QXmppMucRoom*>(sender());
	qDebug() << "room jid " << room->jid();
	qDebug() << "isJoined " << room->isJoined();
	qDebug() << "name " << room->name();
	qDebug() << "nickName " << room->nickName();
	qDebug() << "participants " << room->participants();
	qDebug() << "password " << room->password();
	qDebug() << "subject " << room->subject();
	if (room->allowedActions() == QXmppMucRoom::NoAction)
	{
		qDebug() << "no allowed actions";
	}
	else if (room->allowedActions() & QXmppMucRoom::SubjectAction)
	{
		qDebug() << "SubjectAction was allowed";
	}
	else if (room->allowedActions() & QXmppMucRoom::ConfigurationAction)
	{
		qDebug() << "ConfigurationAction was allowed";
	}
	else if (room->allowedActions() & QXmppMucRoom::PermissionsAction)
	{
		qDebug() << "PermissionsAction was allowed";
	}
	else if (room->allowedActions() & QXmppMucRoom::KickAction)
	{
		qDebug() << "KickAction was allowed";
	}
	room->requestConfiguration();
	room->requestPermissions();
}

void QBChat::onRoomMessageReceived(const QXmppMessage &message)
{
	qDebug() << "onRoomMessageReceived() " << message.body();

	QXmppMucRoom* room = qobject_cast<QXmppMucRoom*>(sender());

	QBChatMessage* qbmessage = new QBChatMessage;
	qbmessage->setFrom(userIdfromRoomMessage(message));
	qbmessage->setTo(roomIDfromJid(room->jid()));
	qbmessage->setText(message.body());
	if (message.stamp().toString() != "")
	{
		qbmessage->setTime(message.stamp().toString());
		qbmessage->setTimestamp(message.stamp().toTime_t());
	}

	qDebug() << "qb message " << qbmessage->toQVariantMap();

	emit roomMessageReceived(qbmessage);
//	emit roomMessageReceived(roomIDfromJid(room->jid()), message)
}
