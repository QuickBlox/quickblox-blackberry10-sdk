/*
 * QBChat.hpp
 *
 *  Created on: 27.02.2013
 *      Author: Alexey Grankov
 */

#ifndef QBCHAT_HPP_
#define QBCHAT_HPP_

#include "QBGlobal.hpp"
#include <QObject>
#include "QBChatMessage.hpp"

//todo for error in slot section, move errors to our stucture and remove this include
#include "QXmppClient.h"
#include "QXmppMucManager.h"

class QBUUser;
class QBChatPrivate;
//class QXmppPresence;
//class QXmppPresence;
//class QXmppIq;

class QB_EXPORT QBChat: public QObject
{
	Q_OBJECT
public:
	explicit QBChat(QObject *parent = 0);
	~QBChat();

	static QBChat* instance();

	Q_INVOKABLE bool login(QString username, QString password);
	Q_INVOKABLE bool loginWithUser(QBUUser* user);
	Q_INVOKABLE void logout();
	Q_INVOKABLE bool sendMessage(QString destination, QString message);
	Q_INVOKABLE void sendPresence();
	Q_INVOKABLE QString currentUserId();

	Q_INVOKABLE void createRoom(QString name, bool isOnlyMembers = false, bool isPersistent = false);
	Q_INVOKABLE void requestAllRooms();
	Q_INVOKABLE void joinToRoom(QString roomId);
	Q_INVOKABLE bool sendRoomMessage(QString destination, QString message);
	Q_INVOKABLE void leaveRoom(QString roomId);

	bool isLoggedIn();

	static const QString QB_CHAT_SERVICE_MUC_DOMAIN;
	static const QString QB_CHAT_SERVICE_DOMAIN;

Q_SIGNALS:
	void loggedIn();

	void messageSended(QBChatMessage* message);
	void messageReceived(QBChatMessage* message);

	void roomAdded(QString roomId, QString roomName);
	void roomMessageReceived(QBChatMessage* message);

	void presenceReceived(QBUUser* user);

public Q_SLOTS:
	void onChatConnected();
	void onChatDisconnected();
	void onChatError(QXmppClient::Error);
	void onMessageReceived(const QXmppMessage &message);
    void onPresenceReceived(const QXmppPresence &presence);
    void onIqReceived(const QXmppIq &iq);

    void onRoomAdded(QXmppMucRoom*);
    void onJoined();
    void onConfigurationReceived(const QXmppDataForm &);
    void onPermissionsReceived(const QList<QXmppMucItem> &);
    void onRoomMessageReceived(const QXmppMessage&);

    void onItemsReceived(const QXmppDiscoveryIq&);

private:
	QString userIDfromJid(QString jid);
	QString jidFromUserID(QString userID);
	QString roomIDfromJid(QString jid);
	QString userIdfromRoomMessage(const QXmppMessage &message);

private:
	QBChatPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QBChat);
    Q_DISABLE_COPY(QBChat);
};

#endif /* QBCHAT_HPP_ */
