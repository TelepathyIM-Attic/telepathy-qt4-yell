
#include <tests/lib/glib-helpers/test-conn-helper.h>
#include <tests/lib/glib/contactlist/conn.h>
#include <QtCore/QEventLoop>
#include <QtTest/QtTest>

#define TP_QT4_ENABLE_LOWLEVEL_API

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingAccount>
#include <TelepathyQt4/PendingVoid>

#include <TelepathyQt4Yell/Models/AccountsModel>

#include <telepathy-glib/debug.h>

#include <tests/lib/test.h>

using namespace Tp;

class TestAccountsModelBasics : public Test
{
    Q_OBJECT

public:
    TestAccountsModelBasics(QObject *parent = 0)
        : Test(parent),
          mAccountCountChanged(false),
          mRowsAboutToBeInsertedStart(-1),
          mRowsAboutToBeInsertedEnd(-1),
          mRowsInsertedStart(-1),
          mRowsInsertedEnd(-1),
          mRowsAboutToBeRemovedStart(-1),
          mRowsAboutToBeRemovedEnd(-1),
          mRowsRemovedStart(-1),
          mRowsRemovedEnd(-1),
          mDataChangedRow(-1),
          mContactsInserted(0)
    { }

protected Q_SLOTS:
    void onNewAccountItem(const QString &id);
    void onAccountCountChanged();
    void onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onRowsInserted(const QModelIndex &parent, int start, int end);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onRowsRemoved(const QModelIndex &parent, int start, int end);
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private Q_SLOTS:
    void initTestCase();
    void init();

    void testBasics();

    void cleanup();
    void cleanupTestCase();

private:
    Tp::AccountManagerPtr mAM;

    QString mConnName, mConnPath;
    TestConnHelper *mConn;
    Tpy::AccountsModel *mAccountsModel;

    QString mNewAccountId;
    bool mAccountCountChanged;
    int mRowsAboutToBeInsertedStart;
    int mRowsAboutToBeInsertedEnd;
    int mRowsInsertedStart;
    int mRowsInsertedEnd;
    int mRowsAboutToBeRemovedStart;
    int mRowsAboutToBeRemovedEnd;
    int mRowsRemovedStart;
    int mRowsRemovedEnd;
    int mDataChangedRow;
    int mContactsInserted;
};

void TestAccountsModelBasics::onNewAccountItem(const QString &id)
{
    qDebug() << "Got new account" << id;
    mNewAccountId = id;
}

void TestAccountsModelBasics::onAccountCountChanged()
{
    qDebug() << "Account count changed!";
    mAccountCountChanged = true;
}

void TestAccountsModelBasics::onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    qDebug() << "Rows about to be inserted" << parent << start << end;
    Q_UNUSED(parent)
    mRowsAboutToBeInsertedStart = start;
    mRowsAboutToBeInsertedEnd = end;
}

void TestAccountsModelBasics::onRowsInserted(const QModelIndex &parent, int start, int end)
{
    qDebug() << "Rows inserted" << parent << start << end;
    Q_UNUSED(parent)
    mRowsInsertedStart = start;
    mRowsInsertedEnd = end;
}

void TestAccountsModelBasics::onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    qDebug() << "Rows about to be removed" << parent << start << end;
    Q_UNUSED(parent)
    mRowsAboutToBeRemovedStart = start;
    mRowsAboutToBeRemovedEnd = end;
}

void TestAccountsModelBasics::onRowsRemoved(const QModelIndex &parent, int start, int end)
{
    qDebug() << "Rows removed" << parent << start << end;
    Q_UNUSED(parent)
    mRowsRemovedStart = start;
    mRowsRemovedEnd = end;
}

void TestAccountsModelBasics::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QCOMPARE(topLeft.row(), bottomRight.row());
    mDataChangedRow = topLeft.row();
}

void TestAccountsModelBasics::initTestCase()
{
    initTestCaseImpl();

    mAM = AccountManager::create(
              AccountFactory::create(QDBusConnection::sessionBus(), Account::FeatureCore | Account::FeatureCapabilities),
              ConnectionFactory::create(QDBusConnection::sessionBus(), Connection::FeatureCore | Connection::FeatureRoster),
              ChannelFactory::create(QDBusConnection::sessionBus()),
              ContactFactory::create(Contact::FeatureAlias | Contact::FeatureCapabilities)
                                 );
    QCOMPARE(mAM->isReady(), false);

    g_type_init();
    g_set_prgname("accounts-model-basics");
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

void TestAccountsModelBasics::init()
{
    initImpl();
}

void TestAccountsModelBasics::testBasics()
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

    qDebug() << "creating an account before creating the model";
    // this is to make sure the model is loading existent accounts correctly
    QVariantMap parameters;
    parameters[QLatin1String("account")] = QLatin1String("existent");
    PendingAccount *pacc = mAM->createAccount(QLatin1String("foo"),
            QLatin1String("bar"), QLatin1String("foobar"), parameters);
    QVERIFY(connect(pacc,
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(pacc->account());

    mAccountsModel = new Tpy::AccountsModel(mAM, this);

    QCOMPARE(mAccountsModel->columnCount(), 1);

    QVERIFY(connect(mAccountsModel,
                    SIGNAL(newAccountItem(const QString&)),
                    SLOT(onNewAccountItem(const QString&))));

    // check if the existent accounts are loaded before connecting to the other signals
    while (mNewAccountId.isNull()) {
        mLoop->processEvents();
    }

    // check if the roles in the model have the correct name
    QHash<int, QByteArray> roles = mAccountsModel->roleNames();
    QCOMPARE(roles[Tpy::AccountsModel::ItemRole], QByteArray("item"));
    QCOMPARE(roles[Tpy::AccountsModel::IdRole], QByteArray("id"));
    QCOMPARE(roles[Tpy::AccountsModel::ValidRole], QByteArray("valid"));
    QCOMPARE(roles[Tpy::AccountsModel::EnabledRole], QByteArray("enabled"));
    QCOMPARE(roles[Tpy::AccountsModel::ConnectionManagerNameRole], QByteArray("connectionManager"));
    QCOMPARE(roles[Tpy::AccountsModel::ProtocolNameRole], QByteArray("protocol"));
    QCOMPARE(roles[Tpy::AccountsModel::ServiceNameRole], QByteArray("service"));
    QCOMPARE(roles[Tpy::AccountsModel::DisplayNameRole], QByteArray("displayName"));
    QCOMPARE(roles[Tpy::AccountsModel::IconRole], QByteArray("icon"));
    QCOMPARE(roles[Tpy::AccountsModel::NicknameRole], QByteArray("nickname"));
    QCOMPARE(roles[Tpy::AccountsModel::ConnectsAutomaticallyRole], QByteArray("connectsAutomatically"));
    QCOMPARE(roles[Tpy::AccountsModel::ChangingPresenceRole], QByteArray("changingPresence"));
    QCOMPARE(roles[Tpy::AccountsModel::AutomaticPresenceRole], QByteArray("automaticStatus"));
    QCOMPARE(roles[Tpy::AccountsModel::AutomaticPresenceTypeRole], QByteArray("automaticStatusType"));
    QCOMPARE(roles[Tpy::AccountsModel::AutomaticPresenceStatusMessageRole], QByteArray("automaticStatusMessage"));
    QCOMPARE(roles[Tpy::AccountsModel::CurrentPresenceRole], QByteArray("status"));
    QCOMPARE(roles[Tpy::AccountsModel::CurrentPresenceTypeRole], QByteArray("statusType"));
    QCOMPARE(roles[Tpy::AccountsModel::CurrentPresenceStatusMessageRole], QByteArray("statusMessage"));
    QCOMPARE(roles[Tpy::AccountsModel::RequestedPresenceRole], QByteArray("requestedStatus"));
    QCOMPARE(roles[Tpy::AccountsModel::RequestedPresenceTypeRole], QByteArray("requestedStatusType"));
    QCOMPARE(roles[Tpy::AccountsModel::RequestedPresenceStatusMessageRole], QByteArray("requestedStatusMessage"));
    QCOMPARE(roles[Tpy::AccountsModel::ConnectionStatusRole], QByteArray("connectionStatus"));
    QCOMPARE(roles[Tpy::AccountsModel::ConnectionStatusReasonRole], QByteArray("connectionStatusReason"));
    QCOMPARE(roles[Tpy::AccountsModel::ContactListStateRole], QByteArray("contactListState"));
    QCOMPARE(roles[Tpy::AccountsModel::AliasRole], QByteArray("aliasName"));
    QCOMPARE(roles[Tpy::AccountsModel::AvatarRole], QByteArray("avatar"));
    QCOMPARE(roles[Tpy::AccountsModel::PresenceStatusRole], QByteArray("presenceStatus"));
    QCOMPARE(roles[Tpy::AccountsModel::PresenceTypeRole], QByteArray("presenceType"));
    QCOMPARE(roles[Tpy::AccountsModel::PresenceMessageRole], QByteArray("presenceMessage"));
    QCOMPARE(roles[Tpy::AccountsModel::SubscriptionStateRole], QByteArray("subscriptionState"));
    QCOMPARE(roles[Tpy::AccountsModel::PublishStateRole], QByteArray("publishState"));
    QCOMPARE(roles[Tpy::AccountsModel::BlockedRole], QByteArray("blocked"));
    QCOMPARE(roles[Tpy::AccountsModel::GroupsRole], QByteArray("groups"));
    QCOMPARE(roles[Tpy::AccountsModel::TextChatCapabilityRole], QByteArray("textChat"));
    QCOMPARE(roles[Tpy::AccountsModel::StreamedMediaCallCapabilityRole], QByteArray("streamedMediaCall"));
    QCOMPARE(roles[Tpy::AccountsModel::StreamedMediaAudioCallCapabilityRole], QByteArray("streamedMediaAudioCall"));
    QCOMPARE(roles[Tpy::AccountsModel::StreamedMediaVideoCallCapabilityRole], QByteArray("streamedMediaVideoCall"));
    QCOMPARE(roles[Tpy::AccountsModel::StreamedMediaVideoCallWithAudioCapabilityRole], QByteArray("streamedMediaVideoCallWithAudio"));
    QCOMPARE(roles[Tpy::AccountsModel::StreamedMediaUpgradeCallCapabilityRole], QByteArray("streamedMediaUpgradeCall"));
    QCOMPARE(roles[Tpy::AccountsModel::MediaCallCapabilityRole], QByteArray("mediaCall"));
    QCOMPARE(roles[Tpy::AccountsModel::AudioCallCapabilityRole], QByteArray("audioCall"));
    QCOMPARE(roles[Tpy::AccountsModel::VideoCallCapabilityRole], QByteArray("videoCall"));
    QCOMPARE(roles[Tpy::AccountsModel::VideoCallWithAudioCapabilityRole], QByteArray("videoCallWithAudio"));
    QCOMPARE(roles[Tpy::AccountsModel::UpgradeCallCapabilityRole], QByteArray("upgradeCall"));
    QCOMPARE(roles[Tpy::AccountsModel::FileTransferCapabilityRole], QByteArray("fileTransfer"));

    QVERIFY(connect(mAccountsModel,
                    SIGNAL(accountCountChanged()),
                    SLOT(onAccountCountChanged())));

    QVERIFY(connect(mAccountsModel,
                    SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
                    SLOT(onRowsAboutToBeInserted(const QModelIndex&, int, int))));
    QVERIFY(connect(mAccountsModel,
                    SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                    SLOT(onRowsInserted(const QModelIndex&, int, int))));

    QVERIFY(connect(mAccountsModel,
                    SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
                    SLOT(onRowsAboutToBeRemoved(const QModelIndex&, int, int))));
    QVERIFY(connect(mAccountsModel,
                    SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                    SLOT(onRowsRemoved(const QModelIndex&, int, int))));
    QVERIFY(connect(mAccountsModel,
                    SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                    SLOT(onDataChanged(const QModelIndex&, const QModelIndex&))));

    parameters.clear();
    parameters[QLatin1String("account")] = QLatin1String("foobar");
    pacc = mAM->createAccount(QLatin1String("foo"),
            QLatin1String("bar"), QLatin1String("foobar"), parameters);
    QVERIFY(connect(pacc,
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(pacc->account());

    // check if the account count signal is being emitted correctly
    while (!mAccountCountChanged && mAccountsModel->accountCount() <= 1) {
        mLoop->processEvents();
    }
 
    // Verify the account was added to the model
    QCOMPARE(mAccountsModel->rowCount(), 2);
    QCOMPARE(mAccountsModel->accountCount(), 2);

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


    // check the accountItemForId method
    QObject *accObject = mAccountsModel->accountItemForId(acc->uniqueIdentifier());
    Tpy::AccountsModelItem *accItem = qobject_cast<Tpy::AccountsModelItem*>(accObject);
    QVERIFY(accItem);
    QCOMPARE(accItem->data(Tpy::AccountsModel::IdRole).toString(), acc->uniqueIdentifier());

    // check the index(TreeNode*) method
    QModelIndex accountIndex = mAccountsModel->index(accItem);
    QVERIFY(accountIndex.isValid());
    QCOMPARE(accountIndex.data(Tpy::AccountsModel::IdRole).toString(), acc->uniqueIdentifier());

    // check if the item is enabled
    QVERIFY(mAccountsModel->flags(accountIndex) & Qt::ItemIsEnabled);

    // check the accountForIndex method
    Tp::AccountPtr accountPtr = mAccountsModel->accountForIndex(mAccountsModel->index(0,0));
    QCOMPARE(accountPtr->uniqueIdentifier(), acc->uniqueIdentifier());

    // check the setData method
    QVERIFY(mAccountsModel->setData(mAccountsModel->index(0,0), false, Tpy::AccountsModel::EnabledRole));

    // check if the dataChanged signal is emitted
    while (mDataChangedRow < 0) {
        mLoop->processEvents();
    }
    QCOMPARE(mDataChangedRow, 0);


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
        qDebug() << "Account is still null";
        mLoop->processEvents();
    }

    QVERIFY(connect(accountPtr->connection()->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    while (!accountPtr->connection()->isReady()) {
        mLoop->processEvents();
    }


    Tp::Contacts contacts = contactManager->allKnownContacts();
    QVERIFY(contacts.count());

    ContactPtr contact = *contacts.begin();

    // check the contactItemForId function
    QObject *contactObject = mAccountsModel->contactItemForId(acc->uniqueIdentifier(), contact->id());
    Tpy::ContactModelItem *contactItem = qobject_cast<Tpy::ContactModelItem*>(contactObject);
    QVERIFY(contactItem);
    QCOMPARE(contactItem->data(Tpy::AccountsModel::IdRole).toString(), contact->id());

    // get an index for a contact
    QModelIndex contactIndex = mAccountsModel->index(contactItem);
    QCOMPARE(contactIndex.data(Tpy::AccountsModel::IdRole).toString(), contact->id());

    // check the contactForIndex method
    Tp::ContactPtr contactPtr = mAccountsModel->contactForIndex(contactIndex);
    QVERIFY(!contactPtr.isNull());
    QCOMPARE(contactPtr->id(), contact->id());

    // check the accountForContactIndex method
    accountPtr = mAccountsModel->accountForContactIndex(contactIndex);
    QVERIFY(!accountPtr.isNull());
    QCOMPARE(accountPtr->uniqueIdentifier(), acc->uniqueIdentifier());

    // check the accountForContactItem method
    accountPtr = mAccountsModel->accountForContactItem(contactItem);
    QVERIFY(!accountPtr.isNull());
    QCOMPARE(accountPtr->uniqueIdentifier(), acc->uniqueIdentifier());


    // check the parent() method
    QModelIndex parentIndex = mAccountsModel->parent(accountIndex);
    QVERIFY(!parentIndex.isValid());

    parentIndex = mAccountsModel->parent(contactIndex);
    QVERIFY(parentIndex.isValid());
    QCOMPARE(parentIndex.data(Tpy::AccountsModel::IdRole).toString(), acc->uniqueIdentifier());

    qDebug() << "Going to remove account " << acc->uniqueIdentifier();
    // check for the rowsAboutToBeRemoved and rowsRemoved signals
    QVERIFY(connect(acc->remove(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));

    while (mRowsRemovedStart < 0) {
        mLoop->processEvents();
    }

    QCOMPARE(mRowsRemovedStart, mRowsRemovedEnd);
    QCOMPARE(mRowsAboutToBeRemovedStart, mRowsRemovedStart);
    QCOMPARE(mRowsAboutToBeRemovedEnd, mRowsRemovedEnd);
}

void TestAccountsModelBasics::cleanup()
{
    cleanupImpl();
}

void TestAccountsModelBasics::cleanupTestCase()
{
    if (mConn) {
        QCOMPARE(mConn->disconnect(), true);
        delete mConn;
    }

    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAccountsModelBasics)
#include "_gen/accounts-model-basics.cpp.moc.hpp"
