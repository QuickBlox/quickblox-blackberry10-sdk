/*
 * ApplicationUI.hpp
 *
 * Created on: 11.06.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "service/ConfigurationService.hpp"
#include "service/PushNotificationService.hpp"

#include "service/QBNetwork.hpp"

#include <bb/cascades/GroupDataModel>

#include <bb/system/InvokeManager>
#include <bb/system/SystemCredentialsPrompt>

#include <bb/cascades/Container>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>

class PushContentController;

class ApplicationUI : public QObject
{
    Q_OBJECT

    // The data model that contains all received pushes
    Q_PROPERTY(bb::cascades::GroupDataModel* model READ model CONSTANT)
    Q_PROPERTY(bool modelIsEmpty READ modelIsEmpty NOTIFY modelIsEmptyChanged)

    // The title and body text for the notification dialog
    Q_PROPERTY(QString notificationTitle READ notificationTitle NOTIFY notificationChanged)
    Q_PROPERTY(QString notificationBody READ notificationBody NOTIFY notificationChanged)

    // The title and body text for the activity dialog
    Q_PROPERTY(QString activityDialogTitle READ activityDialogTitle NOTIFY activityDialogChanged)
    Q_PROPERTY(QString activityDialogBody READ activityDialogBody NOTIFY activityDialogChanged)

    // The controller object for the push content page
    Q_PROPERTY(PushContentController* currentPushContent READ currentPushContent CONSTANT)

    // The configuration provider application ID.
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)

    // The configuration Push Proxy Gateway(PPG) URL.
    Q_PROPERTY(QString ppgUrl READ ppgUrl WRITE setPpgUrl NOTIFY ppgUrlChanged)

    // The configuration 'useSdk' value.
    Q_PROPERTY(bool useSdk READ useSdk WRITE setUseSdk NOTIFY useSdkChanged)

    // The configuration 'launchApplicationOnPush' value.
    Q_PROPERTY(bool launchApplicationOnPush READ launchApplicationOnPush WRITE setLaunchApplicationOnPush NOTIFY launchApplicationOnPushChanged)

    // The configuration 'usingPublicPpg' value.
    Q_PROPERTY(bool usePublicPpg READ usePublicPpg WRITE setUsePublicPpg NOTIFY usePublicPpgChanged)

    Q_PROPERTY(bool isSuccessRegister READ isSuccessRegister NOTIFY isSuccessRegisterChanged)

public:
    ApplicationUI();

    /**
     * Saves the Configuration to the persistent store.
     */
    Q_INVOKABLE void saveConfiguration();

    /**
     * Loads the Configuration from the persistent store.
     */
    Q_INVOKABLE void loadConfiguration();

    /**
     * Returns a value that indicates whether or not the Configuration settings are valid.
     *
     * @return true if valid; false otherwise
     */
    Q_INVOKABLE bool validateConfiguration();

    /**
     * Calls the push service create channel
     */
    Q_INVOKABLE void createChannel();

    /**
     * Calls the push service destroy channel
     */
    Q_INVOKABLE void destroyChannel();

    Q_INVOKABLE void deletePush(const QVariantMap &item);

    Q_INVOKABLE void deleteCurrentPush();

    Q_INVOKABLE void deleteAllPushes();

    Q_INVOKABLE void markAllPushesAsRead();

    /**
     * Marks the passed push as current one and prepares the controller
     * object of the PushContentPage.
     */
    Q_INVOKABLE void selectPush(const QVariantList &indexPath);

    Q_INVOKABLE QString convertToUtf8String(const QVariant &pushContent);

    Q_INVOKABLE bool isSuccessRegister() const;

    Q_INVOKABLE Push currentPush();

    Q_INVOKABLE QVariantList currentIndex();

    Q_INVOKABLE void selectItem(const QVariantMap &item);

public Q_SLOTS:
    void onCreateSessionCompleted(const bb::network::PushStatus &status);
    void onCreateChannelCompleted(const bb::network::PushStatus &status, const QString &token);
    void onDestroyChannelCompleted(const bb::network::PushStatus &status);
    void onRegisterToLaunchCompleted(const bb::network::PushStatus &status);
    void onUnregisterFromLaunchCompleted(const bb::network::PushStatus &status);
    void onInvoked(const bb::system::InvokeRequest &request);
    void onSimChanged();
    void onPushTransportReady(bb::network::PushCommand::Type command);
    void onNoPushServiceConnection();
    void onFullscreen();
    void onAllPushesRemoved();

Q_SIGNALS:
    void modelIsEmptyChanged();
    void notificationChanged();
    void activityDialogChanged();

    void appIdChanged();
    void ppgUrlChanged();
    void useSdkChanged();
    void launchApplicationOnPushChanged();
    void usePublicPpgChanged();

    void openActivityDialog();
    void closeActivityDialog();

    void isSuccessRegisterChanged();

private:
    // A helper function to initialize the push session
    void initializePushSession();
    void pushNotificationHandler(bb::network::PushPayload &pushPayload);
    void showDialog(const QString &title, const QString &message);
    void openActivityDialog(const QString &title, const QString &message);

    // used to open and display a push when a notification is selected in the BlackBerry Hub
    void openPush(int pushSeqNum);

    // a helper function which marks the push as read, and updates the displayed push content
    void updatePushContent(Push &push, const QVariantList &indexPath);

    // The accessor methods of the properties
    bb::cascades::GroupDataModel* model() const;
    bool modelIsEmpty() const;

    QString notificationTitle() const;
    QString notificationBody() const;
    QString activityDialogTitle() const;
    QString activityDialogBody() const;

    PushContentController* currentPushContent() const;

    QString appId() const;
    void setAppId(const QString &appId);
    QString ppgUrl() const;
    void setPpgUrl(const QString &ppgUrl);
    bool useSdk() const;
    void setUseSdk(bool value);
    bool launchApplicationOnPush() const;
    void setLaunchApplicationOnPush(bool launchAppOnPush);
    bool usePublicPpg() const;
    void setUsePublicPpg(bool usingPublicPpg);

    // The manager object to react to invocations
    bb::system::InvokeManager *m_invokeManager;

    // The credentials dialog to register to the push service
    bb::system::SystemCredentialsPrompt *m_registerPrompt;

    // The credentials dialog to unregister from the push service
    bb::system::SystemCredentialsPrompt *m_unregisterPrompt;

    // The wrapper classes for loading/storing configuration values
    ConfigurationService m_configurationService;
    Configuration m_configuration;

    PushNotificationService m_pushNotificationService;

    bool m_shouldRegisterToLaunch;
    bool m_shouldUnregisterFromLaunch;

    // Whether or not the application has at some point in time been running in the foreground
    bool m_hasBeenInForeground;

    // Whether or not the Configuration is in the process of being saved
    bool m_configSaveAction;

    // The controller object for the push content page
    PushContentController* m_pushContentController;

    // The property values
    bb::cascades::GroupDataModel *m_model;
    QString m_notificationTitle;
    QString m_notificationBody;
    QString m_activityDialogTitle;
    QString m_activityDialogBody;
    QString m_appId;
    QString m_ppgUrl;
    bool m_successRegister;
    bool m_useSdk;
    bool m_launchApplicationOnPush;
    bool m_usePublicPpg;
    QVariantList m_currentIndex;
    Push m_currentPush;

    QVariantMap m_currentItem;

    QBNetwork* m_authQB;
};

#endif /* ApplicationUI_HPP_ */
