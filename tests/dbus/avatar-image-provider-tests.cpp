
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
#include <TelepathyQt4Yell/Models/AvatarImageProvider>

#include <telepathy-glib/debug.h>

#include <tests/lib/test.h>

using namespace Tp;

class TestAvatarImageProvider : public Test
{
    Q_OBJECT

public:
    TestAvatarImageProvider(QObject *parent = 0)
        : Test(parent)
    { }

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
};

void TestAvatarImageProvider::initTestCase()
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
    g_set_prgname("avatar-image-provider-tests");
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

void TestAvatarImageProvider::init()
{
    initImpl();
}

void TestAvatarImageProvider::testBasics()
{

    QVERIFY(connect(mAM->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mAM->isReady(), true);

    qDebug() << "Initializing engine";
    QDeclarativeEngine* engine = new QDeclarativeEngine();
    Tpy::AvatarImageProvider::registerProvider(engine, mAM);

    qDebug() << "requesting image";
    QDeclarativeImageProvider* provider = engine->imageProvider(QString::fromLatin1("avatars"));
    qDebug() << "provider acquired";
    QImage voidImage = provider->requestImage(QString::fromLatin1("aaaa"), new QSize(), QSize());
    qDebug() << "image retrieved";
    QCOMPARE(voidImage.size().width(), 0);
    QCOMPARE(voidImage.size().height(), 0);
}

void TestAvatarImageProvider::cleanup()
{
    cleanupImpl();
}

void TestAvatarImageProvider::cleanupTestCase()
{
    if (mConn) {
        QCOMPARE(mConn->disconnect(), true);
        delete mConn;
    }

    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAvatarImageProvider)
#include "_gen/avatar-image-provider-tests.cpp.moc.hpp"
