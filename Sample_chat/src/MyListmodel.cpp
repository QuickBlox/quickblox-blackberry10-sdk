/*
 * MyListModel.cpp
 *
 * Created on: 19.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "mylistmodel.hpp"
#include <QSettings>
#include "QBNetwork.hpp"

using namespace bb::cascades;

static const QString storageKey = QString("chat_messages_storage");

MyListModel::MyListModel(QObject* parent) :
		bb::cascades::QVariantListDataModel(), m_filter(""), m_roomsList(
				new QVariantList()), m_usersList(new QVariantList()), m_filteredRoomsList(
				new QVariantList()), m_filteredUsersList(new QVariantList()) {
	setParent(parent);
}

MyListModel::~MyListModel() {
	qDebug() << "~MyListModel()";

	delete m_roomsList;
	delete m_usersList;
	delete m_filteredRoomsList;
	delete m_filteredUsersList;
}

bool MyListModel::isFiltered(QVariantMap item) {
	return (item["login"].toString().contains(m_filter, Qt::CaseInsensitive)
			|| item["name"].toString().contains(m_filter, Qt::CaseInsensitive));
}

QVariantList MyListModel::roomsIndexPath() {
	QVariantList indexPath;
	indexPath << QVariant(0);
	return indexPath;
}

QVariantList MyListModel::usersIndexPath() {
	QVariantList indexPath;
	if (!isHaveRoom(indexPath))
		indexPath << QVariant(0);
	else
		indexPath << QVariant(1);
	return indexPath;
}

bool MyListModel::isHaveRoom(const QVariantList &indexPath) {
	return ((indexPath.length() == 0 || indexPath[0].toInt() == 0)
			&& m_filteredRoomsList->size() != 0);
}

int MyListModel::childCount(const QVariantList &indexPath) {
	if (indexPath.length() == 0) {
		int result = 0;
		if (m_filteredRoomsList->size() != 0)
			result++;
		if (m_filteredUsersList->size() != 0)
			result++;
		return result;
	} else if (indexPath.length() == 1) {
		if (isHaveRoom(indexPath))
			return m_filteredRoomsList->size();
		return m_filteredUsersList->size();
	}
	return 0;
}

bool MyListModel::hasChildren(const QVariantList &indexPath) {
	if (indexPath.length() == 0) {
		return (m_filteredRoomsList->size() != 0
				|| m_filteredUsersList->size() != 0);
	} else if (indexPath.length() == 1) {
		if (isHaveRoom(indexPath))
			return (m_filteredRoomsList->size() > 0);
		return (m_filteredUsersList->size() > 0);
	}
	return false;
}

QString MyListModel::itemType(const QVariantList &indexPath) {
	if (indexPath.length() == 0) {
		return QString::null;
	} else if (indexPath.length() == 1) {
		return QString("header");
	} else {
		if (isHaveRoom(indexPath))
			return QString("room");
		return QString("user");
	}
}

QVariant MyListModel::data(const QVariantList &indexPath) {
	if (indexPath.length() == 1) {
		if (isHaveRoom(indexPath))
			return QVariant(QString("Rooms"));
		return QVariant(QString("Users"));
	} else if (indexPath.length() > 1) {
		if (isHaveRoom(indexPath)) {
			return m_filteredRoomsList->at(indexPath[1].toInt());
		}
		return m_filteredUsersList->at(indexPath[1].toInt());
	}
	return QVariant();
}

void MyListModel::setFilter(QString filter) {
	if (m_filter == filter)
		return;

	m_filter = filter;
	//add filtered rooms
	for (int i = 0; i < m_roomsList->size(); i++) {
		if (isFiltered(m_roomsList->at(i).toMap())
				&& !m_filteredRoomsList->contains(m_roomsList->at(i).toMap())) {
			m_filteredRoomsList->append(m_roomsList->at(i).toMap());
			if (m_filteredRoomsList->size() == 1)
				emit itemAdded(roomsIndexPath());
			else
				emit itemAdded(
						roomsIndexPath() << (m_filteredRoomsList->size() - 1));
		}
	}
	//add filtered users
	for (int i = 0; i < m_usersList->size(); i++) {
		if (isFiltered(m_usersList->at(i).toMap())
				&& !m_filteredUsersList->contains(m_usersList->at(i).toMap())) {
			m_filteredUsersList->append(m_usersList->at(i).toMap());
			if (m_filteredUsersList->size() == 1)
				emit itemAdded(usersIndexPath());
			else
				emit itemAdded(
						usersIndexPath() << (m_filteredUsersList->size() - 1));
		}
	}
	//remove not filtered rooms
	for (int i = 0; i < m_filteredRoomsList->size(); i++) {
		if (!isFiltered(m_filteredRoomsList->at(i).toMap())) {
			m_filteredRoomsList->removeAt(i);
			if (m_filteredRoomsList->size() == 0)
				emit itemRemoved(roomsIndexPath());
			else
				emit itemRemoved(roomsIndexPath() << i);
			i--;
		}
	}
	//remove not filtered users
	for (int i = 0; i < m_filteredUsersList->size(); i++) {
		if (!isFiltered(m_filteredUsersList->at(i).toMap())) {
			m_filteredUsersList->removeAt(i);
			if (m_filteredUsersList->size() == 0)
				emit itemRemoved(usersIndexPath());
			else
				emit itemRemoved(usersIndexPath() << i);
			i--;
		}
	}
}

void MyListModel::appendUsers(QVariantList usersList) {
	QVariantList filteredUsers;

	foreach(const QVariant& itemData, usersList){
	if (isFiltered(itemData.toMap())) {
		filteredUsers.append(itemData);
	}
	m_usersList->append(itemData);
}

	if (filteredUsers.size() > 0) {
		if (m_filteredUsersList->size() == 0) {
			m_filteredUsersList->append(filteredUsers);
			emit itemAdded(usersIndexPath());
		} else {
			m_filteredUsersList->append(filteredUsers);
			emit itemUpdated(usersIndexPath());
		}
	}
}

void MyListModel::appendRoom(QVariantMap roomData) {
	QVariantList filteredRooms;

	if (isFiltered(roomData)) {
		filteredRooms.append(roomData);
	}
	m_roomsList->append(roomData);

	if (filteredRooms.size() > 0) {
		if (m_filteredRoomsList->size() == 0) {
			m_filteredRoomsList->append(filteredRooms);
			emit itemAdded(roomsIndexPath());
		} else {
			m_filteredRoomsList->append(filteredRooms);
			emit itemUpdated(roomsIndexPath());
		}
	}
}

void MyListModel::storeMessage(QVariantMap message) {
	QSettings settings;
	QVariantList currentChatHistory =
			settings.value(storageKey, QVariantList()).toList();
	currentChatHistory.append(message);
	settings.setValue(storageKey, currentChatHistory);
}

void MyListModel::restoreMessages() {
	qDebug() << "restore messages process started";
	QSettings settings;
	QVariantList currentChatHistory =
			settings.value(storageKey, QVariantList()).toList();
	foreach (const QVariant& item, currentChatHistory){
//	qDebug() << "restore message " << item.toMap();
	QBChatMessage* message = new QBChatMessage();
	message->fromQVariantMap(item.toMap());

	int idx = findUser(message->getFrom());
	if (idx >= 0 && message->getTo() == m_currentUserId) {
		QVariantMap item = m_usersList->at(idx).toMap();

		QList<QVariant> chatList = item["chat"].toList();
		chatList.append(message->toQVariantMap());
		item["chat"] = chatList;

		m_usersList->replace(idx, item);

		int fidx = findFilteredUser(item["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, item);
		}
	}

	idx = findUser(message->getTo());
	if (idx >= 0 && message->getFrom() == m_currentUserId) {
		QVariantMap item = m_usersList->at(idx).toMap();

		QList<QVariant> chatList = item["chat"].toList();
		chatList.append(message->toQVariantMap());
		item["chat"] = chatList;

		m_usersList->replace(idx, item);

		int fidx = findFilteredUser(item["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, item);
		}
	}
}

	qDebug() << "restore messages process finished";
}

QList<QVariant> MyListModel::parseUserData(QString data) {
	bb::data::JsonDataAccess jda;
	QVariantMap usersResultData = jda.loadFromBuffer(data).toMap();

	QList<QVariant> lt;

	if (jda.hasError()) {
		bb::data::DataAccessError err = jda.error();
		qDebug() << "Error converting JSON data: " << err.errorMessage();
		return lt;
	}

	foreach(const QVariant& itemData, usersResultData["items"].toList()){
	QVariantMap itemMapData = itemData.toMap();
	QVariantMap userMapData = itemMapData["user"].toMap();
	QList<QVariant> chatList;
	userMapData["chat"] = chatList;
	userMapData["unreadedCount"] = QVariant(0);
	// if user didn't do anything last 5 minutes - he is offline
	qint64 msec = userMapData["last_request_at"].toDateTime().msecsTo(QDateTime::currentDateTimeUtc());
	if (msec >= 0 && msec < 300000) {
		userMapData["online"] = true;
	} else {
		userMapData["online"] = false;
	}
//		qDebug() << userMapData;
	lt.append(userMapData);
}

	return lt;
}

void MyListModel::load(QString data) {

	QList<QVariant> parsedData = parseUserData(data);

	appendUsers(parsedData);
}

void MyListModel::update(QString data) {
	qDebug() << "update process started";
	QList<QVariant> parsedData = parseUserData(data);

	foreach(const QVariant& itemData, parsedData){
	QVariantMap itemMap = itemData.toMap();
//	qDebug() << itemMap;
	int idx = findUser(itemMap["id"].toString());

	if (idx >= 0) {
		QVariantMap item = m_usersList->at(idx).toMap();

		itemMap["chat"] = item["chat"];
		itemMap["unreadedCount"] = item["unreadedCount"];

		m_usersList->replace(idx, itemMap);

		int fidx = findFilteredUser(item["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, itemMap);

			for (int i = 0; i < item.keys().count(); i++) {
				if (item.keys().at(i) != "chat") {
					if (item[item.keys().at(i)] != itemMap[item.keys().at(i)]) {
						emit itemUpdated(usersIndexPath() << fidx);
						break;
					}
				}
			}
		}
	}
	else {
		// Adding new user if it's not me
		if (itemMap["id"].toString() != m_currentUserId) {
			m_usersList->append(itemMap);

			if (isFiltered(itemMap)) {
				m_filteredUsersList->append(itemMap);
				emit itemAdded(usersIndexPath() << m_filteredUsersList->size() - 1);
			}
		}
	}
}
	qDebug() << "update process finished";
}

void MyListModel::newRoom(QString roomId, QString roomName) {
	QVariantMap room;
	room["id"] = roomId;
	room["name"] = (roomName != "") ? roomName : roomId;
	if (!room.contains("chat")) {
		QList<QVariant> chatList;
		room["chat"] = chatList;
	}
	appendRoom(room);
}

void MyListModel::newChatMessage(QBChatMessage* message) {
	qDebug() << "new chat message " << message->toQVariantMap();
	storeMessage(message->toQVariantMap());

	int idx = findUser(message->getFrom());
	if (idx < 0) {
		idx = findUser(message->getTo());
	}

	if (idx >= 0) {
		QVariantMap item = m_usersList->at(idx).toMap();

		QList<QVariant> chatList = item["chat"].toList();
		chatList.append(message->toQVariantMap());
		item["chat"] = chatList;

		m_usersList->replace(idx, item);

		int fidx = findFilteredUser(item["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, item);

//			emit itemUpdated(usersIndexPath() << fidx);
		}

		emit newMessageForUser(item["id"].toString(), message->toQVariantMap());
	}
}

void MyListModel::newRoomMessage(QBChatMessage* message) {
	int idx = findRoom(message->getTo());

	if (idx >= 0) {
		QVariantMap item = m_roomsList->at(idx).toMap();

		QList<QVariant> chatList = item["chat"].toList();

		//check if we already have if
		foreach(const QVariant& m, chatList){
		QVariantMap map = m.toMap();
		if ((abs(map["timestamp"].toInt() - (int)message->getTimestamp()) <= 2) && map["text"].toString() == message->getText())
		{
			return;
		}
	}

		QVariantMap messageMap = message->toQVariantMap();
		messageMap["nick"] = userNickFromId(message->getFrom());

		chatList.append(messageMap);
		item["chat"] = chatList;

		m_roomsList->replace(idx, item);

		int fidx = findFilteredRoom(item["id"].toString());
		if (fidx >= 0) {
			m_filteredRoomsList->replace(fidx, item);

			emit itemUpdated(roomsIndexPath() << fidx);
		}

		emit newMessageForRoom(item["id"].toString(), messageMap);
	}
}

void MyListModel::setUnreadedMessageCount(QString userId, int count) {
	int idx = findUser(userId);

	if (idx >= 0) {
		QVariantMap* item = new QVariantMap(m_usersList->at(idx).toMap());
		qDebug() << item;

		(*item)["unreadedCount"] = QVariant(count);

		m_usersList->replace(idx, *item);

		int fidx = findFilteredUser((*item)["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, *item);

			emit itemUpdated(usersIndexPath() << fidx);
		}
	}
}

void MyListModel::presenceReceived(QBUUser* user) {
	int idx = findUser(user->getId());

	if (idx >= 0) {
		QVariantMap item = m_usersList->at(idx).toMap();

		item["online"] = (user->getPresence() == 1);
		item["lastActivity"] = user->getLastActivity().toString();

		m_usersList->replace(idx, item);

		int fidx = findFilteredUser(item["id"].toString());
		if (fidx >= 0) {
			m_filteredUsersList->replace(fidx, item);

			emit itemUpdated(usersIndexPath() << fidx);
		}
	}
}

void MyListModel::loggedIn(QString userId) {
	m_currentUserId = userId;
	int idx = findUser(userId);

	if (idx >= 0) {
		m_usersList->removeAt(idx);

		int fidx = findFilteredUser(userId);
		if (fidx >= 0) {
			m_filteredUsersList->removeAt(fidx);

			emit itemRemoved(usersIndexPath() << fidx);
		}
	}

	restoreMessages();
}

int MyListModel::findUser(QString userId) {
	for (int i = 0; i < m_usersList->size(); i++) {
		QVariantMap item = m_usersList->at(i).toMap();

		if (item["id"].toString() == userId) {
			return i;
		}
	}
	return -1;
}

int MyListModel::findFilteredUser(QString userId) {
	for (int i = 0; i < m_filteredUsersList->size(); i++) {
		QVariantMap item = m_filteredUsersList->at(i).toMap();

		if (item["id"].toString() == userId) {
			return i;
		}
	}
	return -1;
}

int MyListModel::findRoom(QString roomId) {
	for (int i = 0; i < m_roomsList->size(); i++) {
		QVariantMap item = m_roomsList->at(i).toMap();

		if (item["id"] == roomId) {
			return i;

		}
	}
	return -1;
}

int MyListModel::findFilteredRoom(QString roomId) {
	for (int i = 0; i < m_filteredRoomsList->size(); i++) {
		QVariantMap item = m_filteredRoomsList->at(i).toMap();

		if (item["id"] == roomId) {
			return i;

		}
	}
	return -1;
}

QString MyListModel::userNickFromId(QString id) {

	for (int i = 0; i < m_usersList->size(); i++) {
		QVariantMap item = m_usersList->at(i).toMap();

		if (item["id"].toString() == id) {
			return item["login"].toString();
		}
	}

	return QString::null;
}
