/*
 * ApplicationUI.cpp
 *
 * Created on: 11.06.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#include "applicationui.hpp"

#include "PushContentController.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bb/cascades/Sheet>

#include <bb/network/PushErrorCode>

#include <bb/platform/Notification>

#include <bb/system/InvokeRequest>
#include <bb/system/SystemDialog>
#include <bb/system/SystemUiButton>

#define BB_OPEN_INVOCATION_ACTION "bb.action.OPEN"
#define NOTIFICATION_PREFIX "com.example.Sample_messages"

using namespace bb::network;
using namespace bb::cascades;
using namespace bb::system;
using namespace bb::platform;

ApplicationUI::ApplicationUI() :
		m_invokeManager(new InvokeManager(this)), m_registerPrompt(
				new SystemCredentialsPrompt(this)), m_unregisterPrompt(
				new SystemCredentialsPrompt(this)), m_shouldRegisterToLaunch(
				false), m_shouldUnregisterFromLaunch(false), m_hasBeenInForeground(
				false), m_configSaveAction(false), m_pushContentController(
				new PushContentController(this)), m_successRegister(false), m_model(
				new GroupDataModel(this)) {
	qmlRegisterType<bb::system::SystemUiButton>("bb.system", 1, 0,
			"SystemUiButton");
	qmlRegisterType<SystemDialog>("bb.system", 1, 0, "SystemDialog");
	qmlRegisterType<PushContentController>();

// We set up the application Organization and name, this is used by QSettings
// when saving values to the persistent store.
	QCoreApplication::setOrganizationName("Example");
	QCoreApplication::setApplicationName("QB Push Service");

	m_model->setSortingKeys(QStringList() << "pushdate" << "pushtime");
	m_model->setGrouping(ItemGrouping::ByFullValue);
	m_model->setSortedAscending(false);

	connect(m_model, SIGNAL(itemAdded(QVariantList)),
			SIGNAL(modelIsEmptyChanged()));
	connect(m_model, SIGNAL(itemUpdated(QVariantList)),
			SIGNAL(modelIsEmptyChanged()));
	connect(m_model, SIGNAL(itemRemoved(QVariantList)),
			SIGNAL(modelIsEmptyChanged()));
	connect(m_model,
			SIGNAL(itemsChanged(bb::cascades::DataModelChangeType::Type, QSharedPointer<bb::cascades::DataModel::IndexMapper>)),
			SIGNAL(modelIsEmptyChanged()));

// load the configuration settings
	loadConfiguration();

// connect the push notification service signals and slots
	QObject::connect(&m_pushNotificationService,
			SIGNAL(createSessionCompleted(const bb::network::PushStatus&)),
			this,
			SLOT(onCreateSessionCompleted(const bb::network::PushStatus&)));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(createChannelCompleted(const bb::network::PushStatus&, const QString)),
			this,
			SLOT(onCreateChannelCompleted(const bb::network::PushStatus&, const QString)));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(destroyChannelCompleted(const bb::network::PushStatus&)),
			this,
			SLOT(onDestroyChannelCompleted(const bb::network::PushStatus&)));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(registerToLaunchCompleted(const bb::network::PushStatus&)),
			this,
			SLOT(onRegisterToLaunchCompleted(const bb::network::PushStatus&)));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(unregisterFromLaunchCompleted(const bb::network::PushStatus&)),
			this,
			SLOT(onUnregisterFromLaunchCompleted(const bb::network::PushStatus&)));
	QObject::connect(&m_pushNotificationService, SIGNAL(simChanged()), this,
			SLOT(onSimChanged()));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(pushTransportReady(bb::network::PushCommand::Type)), this,
			SLOT(onPushTransportReady(bb::network::PushCommand::Type)));
	QObject::connect(&m_pushNotificationService,
			SIGNAL(noPushServiceConnection()), this,
			SLOT(onNoPushServiceConnection()));
	QObject::connect(&m_pushNotificationService, SIGNAL(allPushesRemoved()),
			this, SLOT(onAllPushesRemoved()));

	QmlDocument *qml = QmlDocument::create("asset:///main.qml");
	qml->setContextProperty("_pushAPIHandler", this);
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	Application::instance()->setScene(root);

// populate the model with the pushes that are stored in the database
	m_model->insertList(m_pushNotificationService.pushes());

// connect to InvokeManager "invoked" signal to handle incoming push notifications.
// We will ignore non-push invoke requests.
	connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
			SLOT(onInvoked(const bb::system::InvokeRequest&)));

	initializePushSession();

	m_authQB = new QBNetwork;
}

void ApplicationUI::onFullscreen() {
	m_hasBeenInForeground = true;
}

void ApplicationUI::saveConfiguration() {
	// First, load the previous configuration so we can determine whether or not to register to launch
	// or unregister from launch
	if (m_configurationService.hasConfiguration()) {
		Configuration storedConfig = m_configurationService.configuration();

		if (!storedConfig.launchApplicationOnPush()
				&& m_configuration.launchApplicationOnPush()) {
			m_shouldRegisterToLaunch = true;
			m_shouldUnregisterFromLaunch = false;
		} else if (storedConfig.launchApplicationOnPush()
				&& !m_configuration.launchApplicationOnPush()) {
			m_shouldRegisterToLaunch = false;
			m_shouldUnregisterFromLaunch = true;
		} else {
			m_shouldRegisterToLaunch = false;
			m_shouldUnregisterFromLaunch = false;
		}

	} else {
		m_shouldRegisterToLaunch = m_configuration.launchApplicationOnPush();

		// There is no configuration currently stored, so register to launch was never previously called
		// so we wouldn't need to call unregister
		m_shouldUnregisterFromLaunch = false;
	}

	if (m_usePublicPpg || m_useSdk)
		m_configuration.setProviderApplicationId(m_appId);
	else
		m_configuration.setProviderApplicationId(QString());

	if (m_usePublicPpg)
		m_configuration.setPpgUrl(m_ppgUrl);
	else
		m_configuration.setPpgUrl(QString());

	m_configuration.setLaunchApplicationOnPush(m_launchApplicationOnPush);
	m_configuration.setUsingPublicPushProxyGateway(m_usePublicPpg);
	m_configuration.setSuccessRegister(false);

	openActivityDialog(tr("Configuration"), tr("Saving configuration..."));
	m_configurationService.save(m_configuration);
	m_configSaveAction = true;
	m_pushNotificationService.createSession();
}

void ApplicationUI::loadConfiguration() {

	m_configuration = m_configurationService.configuration();

	m_appId = m_configuration.providerApplicationId();
	m_ppgUrl = m_configuration.ppgUrl();
	m_useSdk = false;
	m_launchApplicationOnPush = m_configuration.launchApplicationOnPush();
	m_usePublicPpg = m_configuration.usingPublicPushProxyGateway();
	m_successRegister = m_configuration.successRegister();

	emit appIdChanged();
	emit ppgUrlChanged();
	emit useSdkChanged();
	emit launchApplicationOnPushChanged();
	emit usePublicPpgChanged();
	emit isSuccessRegisterChanged();
}

bool ApplicationUI::validateConfiguration() {
	bool isValid = false;

	// Trim the values
	m_appId = m_appId.trimmed();
	m_ppgUrl = m_ppgUrl.trimmed();

	if ((m_usePublicPpg || m_useSdk) && m_appId.isEmpty()) {
		showDialog(tr("Configuration"),
				tr("Error: No provider application ID was specified."));
	} else if ((m_usePublicPpg || m_useSdk) && m_appId.indexOf("||") != -1) {
		showDialog(tr("Configuration"),
				tr(
						"Error: A provider application ID is not allowed to contain '||'."));
	} else if (m_usePublicPpg && m_ppgUrl.isEmpty()) {
		showDialog(tr("Configuration"), tr("Error: No PPG URL was specified."));
	} else if (m_usePublicPpg && !m_ppgUrl.startsWith("http://")) {
		showDialog(tr("Configuration"),
				tr("Error: The PPG URL must start with http://."));
	} else if (m_usePublicPpg && !QUrl(m_ppgUrl).isValid()) {
		showDialog(tr("Configuration"), tr("Error: The PPG URL is invalid."));
	} else if (m_usePublicPpg && m_ppgUrl.endsWith("/")) {
		showDialog(tr("Configuration"),
				tr(
						"Error: The PPG URL should not end with a /. One will be automatically added to the end."));
	} else {
		isValid = true;
	}

	return isValid;
}

void ApplicationUI::onNoPushServiceConnection() {
	emit closeActivityDialog();

	if (m_configSaveAction) {
		showDialog(tr("Configuration"),
				tr(
						"Configuration was saved, but was unable to create push session. Error: Push Service could not connect to the Push Agent)"));
	} else {
		showDialog(tr("Push Collector"),
				tr("Error: Push Service could not connect to the Push Agent"));
	}

	m_configSaveAction = false;
}

void ApplicationUI::onCreateSessionCompleted(
		const bb::network::PushStatus &status) {
	emit closeActivityDialog();

	if (status.code() == PushErrorCode::NoError) {
		if (m_shouldRegisterToLaunch) {
			m_pushNotificationService.registerToLaunch();
		} else if (m_shouldUnregisterFromLaunch) {
			m_pushNotificationService.unregisterFromLaunch();
		}

		if (m_configSaveAction) {
			showDialog(tr("Configuration"),
					tr("Configuration was saved. Please register now."));
		}
	} else {
		if (m_configSaveAction) {
			showDialog(tr("Configuration"),
					m_notificationBody
							+ tr(
									"Configuration was saved, but was unable to create push session. (Error code: %0)").arg(
									status.code()));
		} else {
			// Typically in your own application you wouldn't want to display this error to your users
			showDialog(tr("Push Collector"),
					m_notificationBody
							+ tr(
									"Error: unable to create push session. (Error code: %0)").arg(
									status.code()));
		}
	}

	m_configSaveAction = false;
}

void ApplicationUI::createChannel() {

	m_authQB->requestSessionWithLogin();

	if (!m_configurationService.hasConfiguration()) {
		showDialog(tr("Push Collector"),
				tr("Please fill in the config before attempting to register."));
	} else {
		openActivityDialog(tr("Register"), tr("Creating push channel..."));
		m_pushNotificationService.createChannel();
	}
}

void ApplicationUI::onCreateChannelCompleted(
		const bb::network::PushStatus &status, const QString &token) {
	qDebug() << "creatChannelComplete status: " << status.code();
	qDebug() << "createChannelComplete token: " << token;

	emit closeActivityDialog();

	// Typically in your own application you wouldn't want to display this error to your users
	QString message = QString("Cannot create a channel. Please, check the configuration settings").arg(
			status.code());

	switch (status.code()) {
	case PushErrorCode::NoError:
		message = tr("Register succeeded.");
		m_successRegister = true;
		if (m_usePublicPpg || m_useSdk)
			m_configuration.setProviderApplicationId(m_appId);
		else
			m_configuration.setProviderApplicationId(QString());

		if (m_usePublicPpg)
			m_configuration.setPpgUrl(m_ppgUrl);
		else
			m_configuration.setPpgUrl(QString());

		m_configuration.setLaunchApplicationOnPush(m_launchApplicationOnPush);
		m_configuration.setUsingPublicPushProxyGateway(m_usePublicPpg);
		m_configuration.setSuccessRegister(m_successRegister);

		m_configurationService.save(m_configuration);

		emit isSuccessRegisterChanged();
		break;
	case PushErrorCode::TransportFailure:
		message =
				tr(
						"Create channel failed as the push transport is unavailable. "
								"Verify your mobile network and/or Wi-Fi are turned on. "
								"If they are on, you will be notified when the push transport is available again.");
		m_successRegister = false;
		break;
	case PushErrorCode::SubscriptionContentNotAvailable:
		message =
				tr(
						"Create channel failed as the PPG is currently returning a server error. "
								"You will be notified when the PPG is available again.");
		m_successRegister = false;
		break;
	}

	showDialog(tr("Register"), message);
}


void ApplicationUI::destroyChannel() {
	m_authQB->requestRemoveSubscriptions();
	if (!m_configurationService.hasConfiguration()) {
		showDialog(tr("Push Collector"),
				tr(
						"Please fill in the config before attempting to unregister."));
	} else {
			// No username and password is needed when the
			// Push Service SDK is not being used
			// Jump straight to destroying a push channel
			openActivityDialog(tr("Unregister"),
					tr("Destroying push channel..."));
			m_pushNotificationService.destroyChannel();
	}
}

void ApplicationUI::onDestroyChannelCompleted(
		const bb::network::PushStatus &status) {
	qDebug() << "onDestroyChannelCompleted: " << status.code();

	emit closeActivityDialog();

	// Typically in your own application you wouldn't want to display this error to your users
	QString message = QString("Destroy channel failed with error code: %0").arg(
			status.code());

	switch (status.code()) {
	case PushErrorCode::NoError:
		message = tr("Unregister succeeded.");
		m_successRegister = false;
		if (m_usePublicPpg || m_useSdk)
			m_configuration.setProviderApplicationId(m_appId);
		else
			m_configuration.setProviderApplicationId(QString());

		if (m_usePublicPpg)
			m_configuration.setPpgUrl(m_ppgUrl);
		else
			m_configuration.setPpgUrl(QString());

		m_configuration.setLaunchApplicationOnPush(m_launchApplicationOnPush);
		m_configuration.setUsingPublicPushProxyGateway(m_usePublicPpg);
		m_configuration.setSuccessRegister(m_successRegister);

		m_configurationService.save(m_configuration);

		emit isSuccessRegisterChanged();

		break;
	case PushErrorCode::TransportFailure:
		message =
				tr(
						"Destroy channel failed as the push transport is unavailable. "
								"Verify your mobile network and/or Wi-Fi are turned on. "
								"If they are on, you will be notified when the push transport is available again.");
		break;
	case PushErrorCode::SubscriptionContentNotAvailable:
		message =
				tr(
						"Destroy channel failed as the PPG is currently returning a server error. "
								"You will be notified when the PPG is available again.");
		break;
	}

	showDialog(tr("Unregister"), message);
}

void ApplicationUI::onRegisterToLaunchCompleted(
		const bb::network::PushStatus &status) {
	if (status.code() != PushErrorCode::NoError) {
		// Typically in your own application you wouldn't want to display this error to your users
		showDialog(tr("Configuration"),
				m_notificationBody
						+ tr("Register to launch failed with error code: %0.").arg(
								status.code()));
	}
}

void ApplicationUI::onUnregisterFromLaunchCompleted(
		const bb::network::PushStatus &status) {
	if (status.code() != PushErrorCode::NoError) {
		// Typically in your own application you wouldn't want to display this error to your users
		showDialog(tr("Configuration"),
				m_notificationBody
						+ tr(
								"Unregister from launch failed with error code: %0.").arg(
								status.code()));
	}
}

void ApplicationUI::onInvoked(const InvokeRequest &request) {
	if (m_configurationService.hasConfiguration()) {
		// The underlying PushService instance might not have been
		// initialized when an invoke first comes in
		// Make sure that we initialize it here if it hasn't been already
		// It requires an application ID (for consumer applications) so we have to check
		// that configuration settings have already been stored
		m_pushNotificationService.initializePushService();

		if (request.action().compare(BB_PUSH_INVOCATION_ACTION) == 0) {
			qDebug() << "Received push action";
			// Received an incoming push
			// Extract it from the invoke request and then process it
			PushPayload payload(request);
			if (payload.isValid()) {
				pushNotificationHandler(payload);
			}
		} else if (request.action().compare(BB_OPEN_INVOCATION_ACTION) == 0) {
			qDebug() << "Received open action";
			// Received an invoke request to open an existing push (ie. from a notification in the BlackBerry Hub)
			// The payload from the open invoke is the seqnum for the push in the database
			openPush(request.data().toInt());
		}
	}
}

void ApplicationUI::pushNotificationHandler(
		bb::network::PushPayload &pushPayload) {

	// Check for a duplicate push
	PushHistoryItem pushHistoryItem(pushPayload.id());

	if (m_pushNotificationService.checkForDuplicatePush(pushHistoryItem)) {
		// A duplicate was found, stop processing. Silently discard this push from the user
		qWarning()
				<< QString("Duplicate push was found with ID: %0.").arg(
						pushPayload.id());

		// Exit the application if it has not been brought to the foreground
		if (!m_hasBeenInForeground) {
			Application::instance()->requestExit();
		}
		return;
	}

	// Convert from PushPayload to Push so that it can be stored in the database
	Push push(pushPayload);

	// Save the push and set the sequence number (ID) of the push
	push.setSeqNum(m_pushNotificationService.savePush(push));

	// Create a notification for the push that will be added to the BlackBerry Hub
	Notification *notification = new Notification(
			NOTIFICATION_PREFIX + QString::number(push.seqNum()), this);
	notification->setTitle("QB Push Service");
	notification->setBody(
			QString("New %0 push received").arg(push.fileExtension()));

	// Add an invoke request to the notification
	// This invoke will contain the seqnum of the push.
	// When the notification in the BlackBerry Hub is selected, this seqnum will be used to lookup the push in
	// the database and display it
	InvokeRequest invokeRequest;
	invokeRequest.setTarget(INVOKE_TARGET_KEY_OPEN);
	invokeRequest.setAction(BB_OPEN_INVOCATION_ACTION);
	invokeRequest.setMimeType("text/plain");
	invokeRequest.setData(QByteArray::number(push.seqNum()));
	notification->setInvokeRequest(invokeRequest);

	// Add the notification for the push to the BlackBerry Hub
	// Calling this method will add a "splat" to the application icon, indicating that a new push has been received
	notification->notify();

	m_model->insert(push.toMap());

	// If an acknowledgement of the push is required (that is, the push was sent as a confirmed push
	// - which is equivalent terminology to the push being sent with application level reliability),
	// then you must either accept the push or reject the push
	if (pushPayload.isAckRequired()) {
		// In our sample, we always accept the push, but situations might arise where an application
		// might want to reject the push (for example, after looking at the headers that came with the push
		// or the data of the push, we might decide that the push received did not match what we expected
		// and so we might want to reject it)
		m_pushNotificationService.acceptPush(pushPayload.id());
	}

	SystemDialog* getDialog = new SystemDialog;
	getDialog->setTitle("New Push");
	getDialog->setBody(QString::fromUtf8(push.content().data()));
	getDialog->confirmButton()->setLabel("Open");
	getDialog->show();

	if (getDialog->exec() == SystemUiResult::ConfirmButtonSelection) {
		QVariantList indexPath = m_model->findExact(push.toMap());
		updatePushContent(push, indexPath);
		m_currentIndex = indexPath;
		m_currentPush = push;
		m_currentItem = push.toMap();
	}
}

void ApplicationUI::deletePush(const QVariantMap &item) {
	SystemDialog deleteDialog;
	deleteDialog.setTitle("Delete");
	deleteDialog.setBody("Are you sure you want to delete this item?");
	deleteDialog.confirmButton()->setLabel("Delete");

	if (deleteDialog.exec() == SystemUiResult::ConfirmButtonSelection) {
		Push push(item);
		m_pushNotificationService.removePush(push.seqNum());
		m_model->remove(item);

		// The push has been deleted, so delete the notification
		Notification::deleteFromInbox(
				NOTIFICATION_PREFIX + QString::number(push.seqNum()));
	}
}

void ApplicationUI::deleteCurrentPush() {
	SystemDialog deleteDialog;
	deleteDialog.setTitle("Delete");
	deleteDialog.setBody("Are you sure you want to delete this item?");
	deleteDialog.confirmButton()->setLabel("Delete");

	if (deleteDialog.exec() == SystemUiResult::ConfirmButtonSelection) {
		m_pushNotificationService.removePush(m_currentPush.seqNum());
		m_model->remove(m_currentItem);

		// The push has been deleted, so delete the notification
		Notification::deleteFromInbox(
				NOTIFICATION_PREFIX + QString::number(m_currentPush.seqNum()));
	}
}

void ApplicationUI::deleteAllPushes() {
	SystemDialog deleteAllDialog;
	deleteAllDialog.setTitle("Delete ALL");
	deleteAllDialog.setBody("Are you sure you want to delete all items?");
	deleteAllDialog.confirmButton()->setLabel("Delete");

	if (deleteAllDialog.exec() == SystemUiResult::ConfirmButtonSelection) {
		m_pushNotificationService.removeAllPushes();
	}
}

void ApplicationUI::markAllPushesAsRead() {
	if (m_model->size() > 0) {
		// All the pushes have been marked as open/read, so delete all the notifications for the app
		Notification::deleteAllFromInbox();

		m_pushNotificationService.markAllPushesAsRead();

		for (QVariantList indexPath = m_model->first(); !indexPath.isEmpty();
				indexPath = m_model->after(indexPath)) {
			QVariantMap item = m_model->data(indexPath).toMap();
			item["unread"] = false;
			m_model->updateItem(indexPath, item);
		}
	}
}

void ApplicationUI::selectPush(const QVariantList &indexPath) {
	m_currentIndex = indexPath;
	const QVariantMap item = m_model->data(indexPath).toMap();
	Push push(item);
	m_currentPush = push;
	updatePushContent(push, indexPath);
}

void ApplicationUI::selectItem(const QVariantMap &item) {
	m_currentItem = item;
}

Push ApplicationUI::currentPush() {
	return m_currentPush;
}

QVariantList ApplicationUI::currentIndex() {
	return m_currentIndex;
}

void ApplicationUI::openPush(int pushSeqNum) {
	Push push = m_pushNotificationService.push(pushSeqNum);
	QVariantList indexPath = m_model->findExact(push.toMap());
	updatePushContent(push, indexPath);
}

void ApplicationUI::updatePushContent(Push &push,
		const QVariantList &indexPath) {
	push.setUnread(false);
	m_pushNotificationService.markPushAsRead(push.seqNum());

	m_model->updateItem(indexPath, push.toMap());

	// The push has been opened, so delete the notification
	Notification::deleteFromInbox(
			NOTIFICATION_PREFIX + QString::number(push.seqNum()));

	m_pushContentController->setCurrentPush(push);
}

void ApplicationUI::onAllPushesRemoved() {
	// All the pushes have been deleted, so delete all the notifications for the app
	Notification::deleteAllFromInbox();
	m_model->clear();
}

QString ApplicationUI::convertToUtf8String(const QVariant &pushContent) {
	return QString::fromUtf8(pushContent.toByteArray().data());
}

void ApplicationUI::onSimChanged() {
	// Remove the currently registered user (if there is one)
	// and unsubscribe the user from the Push Initiator since
	// switching SIMs might indicate we are dealing with
	// a different user. Also, remove all pushes and push
	// history from the database.
	m_pushNotificationService.handleSimChange();

	SystemDialog simChangeDialog;
	simChangeDialog.setTitle("QB Push Service");
	simChangeDialog.setBody(
			"The SIM card was changed and, as a result, the current user has been unregistered. Would you like to re-register?");
	simChangeDialog.confirmButton()->setLabel("Yes");
	simChangeDialog.cancelButton()->setLabel("No");

	if (simChangeDialog.exec() == SystemUiResult::ConfirmButtonSelection) {
		createChannel();
	}
}

void ApplicationUI::onPushTransportReady(
		bb::network::PushCommand::Type command) {
	QString message = "The push transport is now available. Please try ";
	if (command == PushCommand::CreateChannel) {
		message += "registering ";
	} else {
		message += "unregistering ";
	}

	message += "again.";

	showDialog(tr("Push Collector"), message);
}

bool ApplicationUI::isSuccessRegister() const {
	//return m_configurationService.isSuccessRegister();
	return m_successRegister;
}

void ApplicationUI::initializePushSession() {
	// If the app already has config info saved then create the session
	if (m_configurationService.hasConfiguration()) {

		m_shouldRegisterToLaunch = m_configuration.launchApplicationOnPush();
		m_shouldUnregisterFromLaunch =
				!m_configuration.launchApplicationOnPush();
		m_pushNotificationService.createSession();

	} else {

		Sheet* configSheet = Application::instance()->findChild<Sheet*>(
				"configurationSheet");
		if (configSheet) {
			configSheet->open();

			m_hasBeenInForeground = true;
		}

		showDialog(tr("Push Collector"),
				tr(
						"No configuration settings were found. Please fill them in."));

	}
}

void ApplicationUI::showDialog(const QString &title, const QString &message) {
	m_notificationTitle = title;
	m_notificationBody = message;

	emit notificationChanged();
}

void ApplicationUI::openActivityDialog(const QString& title,
		const QString &message) {
	m_activityDialogTitle = title;
	m_activityDialogBody = message;
	emit activityDialogChanged();

	emit openActivityDialog();
}

QString ApplicationUI::appId() const {
	return m_appId;
}

void ApplicationUI::setAppId(const QString &appId) {
	if (m_appId == appId)
		return;

	m_appId = appId;
	emit appIdChanged();
}

QString ApplicationUI::ppgUrl() const {
	return m_ppgUrl;
}

void ApplicationUI::setPpgUrl(const QString &ppgUrl) {
	if (m_ppgUrl == ppgUrl)
		return;

	m_ppgUrl = ppgUrl;
	emit ppgUrlChanged();
}

bool ApplicationUI::launchApplicationOnPush() const {
	return m_launchApplicationOnPush;
}

void ApplicationUI::setLaunchApplicationOnPush(bool launchAppOnPush) {
	if (m_launchApplicationOnPush == launchAppOnPush)
		return;

	m_launchApplicationOnPush = launchAppOnPush;
	emit launchApplicationOnPushChanged();
}

bool ApplicationUI::useSdk() const {
	return m_useSdk;
}

void ApplicationUI::setUseSdk(bool value) {
	if (m_useSdk == value)
		return;

	m_useSdk = value;
	emit useSdkChanged();
}

bool ApplicationUI::usePublicPpg() const {
	return m_usePublicPpg;
}

void ApplicationUI::setUsePublicPpg(bool usingPublicPpg) {
	if (m_usePublicPpg == usingPublicPpg)
		return;

	m_usePublicPpg = usingPublicPpg;
	emit usePublicPpgChanged();
}

bb::cascades::GroupDataModel* ApplicationUI::model() const {
	return m_model;
}

bool ApplicationUI::modelIsEmpty() const {
	return (m_model->size() == 0);
}

QString ApplicationUI::notificationTitle() const {
	return m_notificationTitle;
}

QString ApplicationUI::notificationBody() const {
	return m_notificationBody;
}

QString ApplicationUI::activityDialogTitle() const {
	return m_activityDialogTitle;
}

QString ApplicationUI::activityDialogBody() const {
	return m_activityDialogBody;
}

PushContentController* ApplicationUI::currentPushContent() const {
	return m_pushContentController;
}
