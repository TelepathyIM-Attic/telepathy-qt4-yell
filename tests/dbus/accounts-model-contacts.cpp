
#include <tests/lib/glib-helpers/test-conn-helper.h>
#include <tests/lib/glib/contactlist/conn.h>
#include <QtCore/QEventLoop>
#include <QtGui/QImage>
#include <QtTest/QtTest>

#define TP_QT4_ENABLE_LOWLEVEL_API

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/ContactCapabilities>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingAccount>
#include <TelepathyQt4/PendingVoid>

#include <TelepathyQt4Yell/ContactCapabilities>
#include <TelepathyQt4Yell/Models/AccountsModel>

#include <telepathy-glib/debug.h>

#include <tests/lib/test.h>

using namespace Tp;

class TestAccountsModelContacts : public Test
{
    Q_OBJECT

public:
    TestAccountsModelContacts(QObject *parent = 0)
        : Test(parent),
          mContactItemChanged(false)
    { }

protected Q_SLOTS:
    void onContactItemChanged();
    
private Q_SLOTS:
    void initTestCase();
    void init();

    void testBasics();

    void cleanup();
    void cleanupTestCase();

private:
    Tp::AccountManagerPtr mAM;

    TestConnHelper *mConn;
    Tpy::AccountsModel *mAccountsModel;
    bool mContactItemChanged;
};

void TestAccountsModelContacts::onContactItemChanged()
{
    mContactItemChanged = true;
}

void TestAccountsModelContacts::initTestCase()
{
    initTestCaseImpl();

    mAM = AccountManager::create(
              AccountFactory::create(QDBusConnection::sessionBus(), Account::FeatureCore | Account::FeatureCapabilities),
              ConnectionFactory::create(QDBusConnection::sessionBus(), Connection::FeatureCore | Connection::FeatureRoster),
              ChannelFactory::create(QDBusConnection::sessionBus()),
              ContactFactory::create(Contact::FeatureAlias | Contact::FeatureCapabilities | Contact::FeatureSimplePresence
                                     | Contact::FeatureAvatarData)
                                 );
    QCOMPARE(mAM->isReady(), false);

    g_type_init();
    g_set_prgname("accounts-model-contacts");
    tp_debug_set_flags("all");
    dbus_g_bus_get(DBUS_BUS_STARTER, 0);

    mConn = new TestConnHelper(this,
            ChannelFactory::create(QDBusConnection::sessionBus()),
            ContactFactory::create(Contact::FeatureAlias),
            EXAMPLE_TYPE_CONTACT_LIST_CONNECTION,
            "account", "me@example.com",
            "protocol", "contactlist",
            NULL);
    QCOMPARE(mConn->connect(), true);
}

void TestAccountsModelContacts::init()
{
    initImpl();
}

void TestAccountsModelContacts::testBasics()
{
    Features features = Features() << Connection::FeatureRoster;
    QCOMPARE(mConn->enableFeatures(features), true);

    Tp::ContactManagerPtr contactManager = mConn->client()->contactManager();
    QCOMPARE(contactManager->state(), ContactListStateSuccess);

    QVERIFY(connect(mAM->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mAM->isReady(), true);

    mAccountsModel = new Tpy::AccountsModel(mAM, this);

    QVariantMap parameters;
    parameters[QLatin1String("account")] = QLatin1String("foobar");
    Tp::PendingAccount *pacc = mAM->createAccount(QLatin1String("foo"),
            QLatin1String("bar"), QLatin1String("foobar"), parameters);
    QVERIFY(connect(pacc,
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(pacc->account());

    // wait for the account to be added
    while (mAccountsModel->accountCount() < 1) {
        qDebug() << "Account count is" << mAccountsModel->accountCount();
        mLoop->processEvents();
    }

    AccountPtr accountPtr = Account::create(mAM->dbusConnection(), mAM->busName(),
            QLatin1String("/org/freedesktop/Telepathy/Account/foo/bar/Account0"),
            mAM->connectionFactory(), mAM->channelFactory(), mAM->contactFactory());

    QVERIFY(connect(accountPtr->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));

    while (!accountPtr->isReady()) {
        mLoop->processEvents();
    }

    // simulate that the account has a connection
    Client::DBus::PropertiesInterface *accPropertiesInterface =
        accountPtr->interface<Client::DBus::PropertiesInterface>();

    QVERIFY(connect(new PendingVoid(
                        accPropertiesInterface->Set(
                            QLatin1String(TELEPATHY_INTERFACE_ACCOUNT),
                            QLatin1String("Connection"),
                            QDBusVariant(mConn->objectPath())),
                        accountPtr),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    // wait for the connection to be built in Account
    while (accountPtr->connection().isNull()) {
        qDebug() << "Account is still null";
        mLoop->processEvents();
    }

    QVERIFY(connect(accountPtr->connection()->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    // wait for the connection to be ready and for the contact rows to be added
    while (!accountPtr->connection()->isReady() || mAccountsModel->rowCount(mAccountsModel->index(0,0)) == 0) {
        mLoop->processEvents();
    }

    QModelIndex accountIndex = mAccountsModel->index(0, 0);
    QModelIndex contactIndex = mAccountsModel->index(0, 0, accountIndex);
    QObject *contactObject = mAccountsModel->contactItemForId(accountIndex.data(Tpy::AccountsModel::IdRole).toString(),
                                                              contactIndex.data(Tpy::AccountsModel::IdRole).toString());
    Tpy::ContactModelItem *contactItem = qobject_cast<Tpy::ContactModelItem*>(contactObject);
    QVERIFY(contactItem);

    QVERIFY(connect(contactItem,
                    SIGNAL(changed(Tpy::TreeNode*)),
                    SLOT(onContactItemChanged())));

    Tp::ContactPtr contactPtr = contactItem->contact();
    QVERIFY(!contactPtr.isNull());


    QCOMPARE(contactItem->data(Tpy::AccountsModel::ItemRole).value<QObject*>(), contactObject);
    QCOMPARE(contactItem->data(Tpy::AccountsModel::IdRole).toString(), contactPtr->id());
    QCOMPARE(contactItem->data(Qt::DisplayRole).toString(), contactPtr->alias());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::AliasRole).toString(), contactPtr->alias());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::PresenceStatusRole).toString(), contactPtr->presence().status());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::PresenceTypeRole).toInt(), (int)contactPtr->presence().type());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::PresenceMessageRole).toString(), contactPtr->presence().statusMessage());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::SubscriptionStateRole).toInt(), (int)contactPtr->subscriptionState());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::PublishStateRole).toInt(), (int)contactPtr->publishState());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::BlockedRole).toBool(), contactPtr->isBlocked());

    // compare the groups
    QStringList dataGroups = contactItem->data(Tpy::AccountsModel::GroupsRole).toStringList();
    QStringList itemGroups = contactPtr->groups();
    QCOMPARE(dataGroups.count(), itemGroups.count());
    Q_FOREACH(const QString &group, dataGroups) {
        QVERIFY(itemGroups.removeOne(group));
    }

    QCOMPARE(contactItem->data(Tpy::AccountsModel::AvatarRole).toString(), contactPtr->avatarData().fileName);
    QCOMPARE(contactItem->data(Qt::DecorationRole).value<QImage>(), QImage(contactPtr->avatarData().fileName));
    QCOMPARE(contactItem->data(Tpy::AccountsModel::TextChatCapabilityRole).toBool(), contactPtr->capabilities().textChats());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::StreamedMediaCallCapabilityRole).toBool(),
             contactPtr->capabilities().streamedMediaCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::StreamedMediaAudioCallCapabilityRole).toBool(),
             contactPtr->capabilities().streamedMediaAudioCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::StreamedMediaVideoCallCapabilityRole).toBool(),
             contactPtr->capabilities().streamedMediaVideoCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::StreamedMediaVideoCallWithAudioCapabilityRole).toBool(),
             contactPtr->capabilities().streamedMediaVideoCallsWithAudio());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::StreamedMediaUpgradeCallCapabilityRole).toBool(),
             contactPtr->capabilities().upgradingStreamedMediaCalls());

    Tpy::ContactCapabilities capabilities(contactPtr->capabilities());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::MediaCallCapabilityRole).toBool(), capabilities.mediaCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::AudioCallCapabilityRole).toBool(), capabilities.audioCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::VideoCallCapabilityRole).toBool(), capabilities.videoCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::VideoCallWithAudioCapabilityRole).toBool(),
             capabilities.videoCallsWithAudio());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::UpgradeCallCapabilityRole).toBool(), capabilities.upgradingCalls());
    QCOMPARE(contactItem->data(Tpy::AccountsModel::FileTransferCapabilityRole).toBool(), capabilities.fileTransfers());

    // set the publish state and check if the value changed accordingly
    // TODO: check if the values are being set correctly
    QVERIFY(contactItem->setData(Tpy::AccountsModel::PublishStateRole, Tp::Contact::PresenceStateNo));
    QVERIFY(contactItem->setData(Tpy::AccountsModel::PublishStateRole, Tp::Contact::PresenceStateYes));
    QVERIFY(contactItem->setData(Tpy::AccountsModel::SubscriptionStateRole, Tp::Contact::PresenceStateAsk));
    QVERIFY(contactItem->setData(Tpy::AccountsModel::SubscriptionStateRole, Tp::Contact::PresenceStateNo));

    QCOMPARE(mLoop->exec(), 0);
}

void TestAccountsModelContacts::cleanup()
{
    cleanupImpl();
}

void TestAccountsModelContacts::cleanupTestCase()
{
    if (mConn) {
        QCOMPARE(mConn->disconnect(), true);
        delete mConn;
    }

    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAccountsModelContacts)
#include "_gen/accounts-model-contacts.cpp.moc.hpp"
