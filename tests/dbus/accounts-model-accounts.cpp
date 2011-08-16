
#include <tests/lib/glib-helpers/test-conn-helper.h>
#include <tests/lib/glib/echo2/conn.h>
#include <QtCore/QEventLoop>
#include <QtTest/QtTest>

#define TP_QT4_ENABLE_LOWLEVEL_API

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/ConnectionLowlevel>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingAccount>
#include <TelepathyQt4/PendingVoid>

#include <TelepathyQt4Yell/Models/AccountsModel>
#include <TelepathyQt4Yell/Models/AccountsModelItem>

#include <telepathy-glib/debug.h>

#include <tests/lib/test.h>

using namespace Tp;

class TestAccountsModelAccounts : public Test
{
    Q_OBJECT

public:
    TestAccountsModelAccounts(QObject *parent = 0)
        : Test(parent),
          mAccountCountChanged(false),
          mAccountItemCreated(false),
          mAllSignalsExecuted(false)
    { }

protected Q_SLOTS:
    void onAccountCountChanged();
    void onNewAccountItem(const QString &accountId);
    void onConnectionStatusChanged(const QString &accountId, const int status);
    void onItemChanged();

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

    bool mAccountCountChanged;
    bool mAccountItemCreated;
    bool mAccountItemConnected;
    bool mAllSignalsExecuted;

};

void TestAccountsModelAccounts::onAccountCountChanged()
{
    qDebug() << "Account count changed!";
    mAccountCountChanged = true;
}

void TestAccountsModelAccounts::onNewAccountItem(const QString &accountId)
{
    qDebug() << "Account model item created!";
    mAccountItemCreated = true;
}

void TestAccountsModelAccounts::onConnectionStatusChanged(const QString &accountId, const int status)
{
    qDebug() << "Account changed status: " << status;
    if(status == Tp::ConnectionStatusConnected) {
        mAccountItemConnected = true;
    }
}

void TestAccountsModelAccounts::onItemChanged()
{
    qDebug() << "Account item changed signal received";
    mAllSignalsExecuted = true;

    Tpy::AccountsModelItem* accountItem = qobject_cast<Tpy::AccountsModelItem *>(sender());
    Tp::AccountPtr account = accountItem->account();

    //QCOMPARE(accountItem->data(Tpy::AccountsModel::ItemRole), static_cast<QObject *>(accountItem));
    QCOMPARE(accountItem->data(Tpy::AccountsModel::IdRole).toString(), account->uniqueIdentifier());
    // This should be tested with AvatarImageProvider-specific tests
    //QCOMPARE(accountItem->data(Tpy::AccountsModel::AvatarRole).toString(), account->iconName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ValidRole).toBool(), account->isValid());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::EnabledRole).toBool(), account->isEnabled());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ConnectionManagerNameRole).toString(), account->cmName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ProtocolNameRole).toString(), account->protocolName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ServiceNameRole).toString(), account->serviceName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::DisplayNameRole).toString(), account->displayName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::IconRole).toString(), account->iconName());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::NicknameRole).toString(), account->nickname());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ConnectsAutomaticallyRole).toBool(), account->connectsAutomatically());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::ChangingPresenceRole).toBool(), account->isChangingPresence());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::AutomaticPresenceRole).toString(), account->automaticPresence().status());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::AutomaticPresenceTypeRole).toUInt(), static_cast<uint>(account->automaticPresence().type()));
    QCOMPARE(accountItem->data(Tpy::AccountsModel::AutomaticPresenceStatusMessageRole).toString(), account->automaticPresence().statusMessage());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::RequestedPresenceRole).toString(), account->requestedPresence().status());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::RequestedPresenceTypeRole).toUInt(), static_cast<uint>(account->requestedPresence().type()));
    QCOMPARE(accountItem->data(Tpy::AccountsModel::RequestedPresenceStatusMessageRole).toString(), account->requestedPresence().statusMessage());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::CurrentPresenceRole).toString(), account->currentPresence().status());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::CurrentPresenceTypeRole).toUInt(), static_cast<uint>(account->currentPresence().type()));
    QCOMPARE(accountItem->data(Tpy::AccountsModel::CurrentPresenceStatusMessageRole).toString(), account->currentPresence().statusMessage());
    if (!account->connection().isNull()) {
        QCOMPARE(accountItem->data(Tpy::AccountsModel::ConnectionStatusRole).toUInt(), static_cast<uint>(account->connection()->status()));
        QCOMPARE(accountItem->data(Tpy::AccountsModel::ConnectionStatusReasonRole).toUInt(), static_cast<uint>(account->connection()->statusReason()));
        QCOMPARE(accountItem->data(Tpy::AccountsModel::ContactListStateRole).toUInt(), static_cast<uint>(account->connection()->contactManager()->state()));
    }
    QCOMPARE(accountItem->data(Tpy::AccountsModel::TextChatCapabilityRole).toBool(), account->capabilities().textChats());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::StreamedMediaCallCapabilityRole).toBool(), account->capabilities().streamedMediaCalls());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::StreamedMediaAudioCallCapabilityRole).toBool(), account->capabilities().streamedMediaAudioCalls());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::StreamedMediaVideoCallCapabilityRole).toBool(), account->capabilities().streamedMediaVideoCalls());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::StreamedMediaVideoCallWithAudioCapabilityRole).toBool(), account->capabilities().streamedMediaVideoCallsWithAudio());
    QCOMPARE(accountItem->data(Tpy::AccountsModel::StreamedMediaUpgradeCallCapabilityRole).toBool(), account->capabilities().upgradingStreamedMediaCalls());
}

void TestAccountsModelAccounts::initTestCase()
{
    initTestCaseImpl();

    mAM = AccountManager::create(AccountFactory::create(QDBusConnection::sessionBus(),
                Account::FeatureCore | Account::FeatureCapabilities));
    QCOMPARE(mAM->isReady(), false);

    g_type_init();
    g_set_prgname("accounts-model-basics");
    tp_debug_set_flags("all");
    dbus_g_bus_get(DBUS_BUS_STARTER, 0);

    mConn = new TestConnHelper(this,
            EXAMPLE_TYPE_ECHO_2_CONNECTION,
            "account", "me@example.com",
            "protocol", "echo2",
            NULL);
    QCOMPARE(mConn->connect(), true);
}

void TestAccountsModelAccounts::init()
{
    initImpl();
}

void TestAccountsModelAccounts::testBasics()
{
    QVERIFY(connect(mAM->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mAM->isReady(), true);

    mAccountsModel = new Tpy::AccountsModel(mAM, this);

    QCOMPARE(mAccountsModel->columnCount(), 1);

    QVERIFY(connect(mAccountsModel,
                    SIGNAL(accountCountChanged()),
                    SLOT(onAccountCountChanged())));
    QVERIFY(connect(mAccountsModel,
                    SIGNAL(newAccountItem(QString)),
                    SLOT(onNewAccountItem(QString))));

    QVariantMap parameters;
    parameters[QLatin1String("account")] = QLatin1String("foobar");
    PendingAccount *pacc = mAM->createAccount(QLatin1String("foo"),
            QLatin1String("bar"), QLatin1String("foobar"), parameters);
    QVERIFY(connect(pacc,
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(pacc->account());

    // check if the account count signal is being emitted correctly
    while (!mAccountCountChanged && mAccountsModel->accountCount() != 1) {
        mLoop->processEvents();
    }
 
    // Verify the account was added to the model
    QCOMPARE(mAccountsModel->rowCount(), 1);
    QCOMPARE(mAccountsModel->accountCount(), 1);

    AccountPtr acc = Account::create(mAM->dbusConnection(), mAM->busName(),
            QLatin1String("/org/freedesktop/Telepathy/Account/foo/bar/Account0"),
            mAM->connectionFactory(), mAM->channelFactory(), mAM->contactFactory());


    QVERIFY(mAccountsModel->index(0, 0).isValid());

    QCOMPARE(mAccountsModel->index(0, 0).data(Tpy::AccountsModel::IdRole).toString(),
             acc->uniqueIdentifier());

    QVERIFY(connect(acc->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));

    while (!acc->isReady()) {
        mLoop->processEvents();
    }

    // set values

    Tp::AccountPtr accountPtr = mAccountsModel->accountForIndex(mAccountsModel->index(0, 0));
    Tpy::AccountsModelItem* accountItem = qobject_cast<Tpy::AccountsModelItem *>(mAccountsModel->accountItemForId(accountPtr->uniqueIdentifier()));

    QVERIFY(connect(accountItem, SIGNAL(connectionStatusChanged(QString,int)),
                    SLOT(onConnectionStatusChanged(QString,int))));
    QVERIFY(connect(accountItem, SIGNAL(nicknameChanged(QString)),
                    SLOT(onItemChanged())));

    // simulate that the account has a connection
    Client::DBus::PropertiesInterface *accPropertiesInterface =
        acc->interface<Client::DBus::PropertiesInterface>();

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
        QCOMPARE(mLoop->exec(), 0);
    }

    QVERIFY(connect(accountPtr->connection()->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    while (!accountPtr->connection()->isReady()) {
        mLoop->processEvents();
    }

    accountItem->setRequestedPresence(Tp::ConnectionStatusConnected, QLatin1String("available"), QLatin1String("available"));

    accountItem->setData(Tpy::AccountsModel::EnabledRole, true);
    accountItem->setData(Tpy::AccountsModel::RequestedPresenceRole, QLatin1String("available"));
    accountItem->setData(Tpy::AccountsModel::RequestedPresenceStatusMessageRole, QLatin1String("message changed while testing"));
    accountItem->setData(Tpy::AccountsModel::NicknameRole, QLatin1String("tp qt test nickname user"));

    QVERIFY(connect(acc->remove(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));

    while (!mAllSignalsExecuted) {
        mLoop->processEvents();
    }

    QCOMPARE(mAccountItemCreated, true);
    QCOMPARE(mAccountItemConnected, true);
}

void TestAccountsModelAccounts::cleanup()
{
    cleanupImpl();
}

void TestAccountsModelAccounts::cleanupTestCase()
{
    if (mConn) {
        QCOMPARE(mConn->disconnect(), true);
        delete mConn;
    }

    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAccountsModelAccounts)
#include "_gen/accounts-model-accounts.cpp.moc.hpp"
