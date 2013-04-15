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

#include "QBChatMessage.hpp"
#include "QBUUser.hpp"

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
	Q_INVOKABLE void load(QString data);
	Q_INVOKABLE void update(QString data);
	Q_INVOKABLE void newChatMessage(QBChatMessage* message);
	Q_INVOKABLE void newRoom(QString roomId, QString roomName);
	Q_INVOKABLE void newRoomMessage(QBChatMessage* message);
	Q_INVOKABLE void setUnreadedMessageCount(QString userId, int count);
	Q_INVOKABLE void presenceReceived(QBUUser* user);
	Q_INVOKABLE void loggedIn(QString userId);

	Q_INVOKABLE void setFilter(QString filter);

    int	childCount (const QVariantList &indexPath);
    bool hasChildren (const QVariantList &indexPath);
    QString	itemType (const QVariantList &indexPath);
    QVariant data (const QVariantList &indexPath);

Q_SIGNALS:
	void newMessageForUser(QString userId, QVariantMap chatMessage);
	void newMessageForRoom(QString roomId, QVariantMap chatMessage);

public:
	MyListModel(QObject* parent = 0);
	virtual ~MyListModel();

private:
	QVariantList parseUserData(QString data);

	QVariantList roomsIndexPath();
	QVariantList usersIndexPath();
	bool isHaveRoom(const QVariantList &indexPath);
	bool isFiltered(QVariantMap item);
	void appendUsers(QVariantList usersList);
	void appendRoom(QVariantMap roomData);

	int findUser(QString userId);
	int findFilteredUser(QString userId);
	int findRoom(QString roomId);
	int findFilteredRoom(QString roomId);

	void storeMessage(QVariantMap message);
	void restoreMessages();

	QString userNickFromId(QString id);

	QString m_currentUserId;

	QString m_filter;

	QVariantList* m_roomsList;
	QVariantList* m_usersList;
	QVariantList* m_filteredRoomsList;
	QVariantList* m_filteredUsersList;
};

#endif /* MyListModel_HPP_ */
