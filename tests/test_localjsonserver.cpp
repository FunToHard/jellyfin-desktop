#include <QtTest/QtTest>
#include "shared/LocalJsonServer.h"
#include "shared/LocalJsonClient.h"
#include "shared/Paths.h"

class TestLocalJsonServer : public QObject
{
  Q_OBJECT

private slots:
  void testListen();
  void testRetryLimitOnAddressCollision();
  void testClientSendReceive();
};

void TestLocalJsonServer::testListen()
{
  LocalJsonServer server("test_unit_listen");
  QVERIFY(server.listen());
}

void TestLocalJsonServer::testRetryLimitOnAddressCollision()
{
  LocalJsonServer primaryServer("test_unit_collision");
  QVERIFY(primaryServer.listen());

  // A secondary server trying to listen on the exact same name
  // Must return and NOT hang indefinitely in an infinite loop
  QElapsedTimer timer;
  timer.start();

  LocalJsonServer secondaryServer("test_unit_collision");
  bool res = secondaryServer.listen();
  Q_UNUSED(res);

  // Must complete in under 5 seconds
  QVERIFY(timer.elapsed() < 5000);
}

void TestLocalJsonServer::testClientSendReceive()
{
  LocalJsonServer server("test_unit_client");
  QVERIFY(server.listen());

  bool messageReceived = false;
  QVariantMap receivedMap;
  connect(&server, &LocalJsonServer::messageReceived, [&](const QVariantMap& map) {
    messageReceived = true;
    receivedMap = map;
  });

  LocalJsonClient client("test_unit_client");
  client.connectToServer();
  QVERIFY(client.waitForConnected(2000));

  QVariantMap sendMap;
  sendMap.insert("type", "ping");
  sendMap.insert("value", 42);
  QVERIFY(client.sendMessage(sendMap));

  QTRY_VERIFY_WITH_TIMEOUT(messageReceived, 2000);
  QCOMPARE(receivedMap.value("type").toString(), QString("ping"));
  QCOMPARE(receivedMap.value("value").toInt(), 42);

  client.close();
}

QTEST_MAIN(TestLocalJsonServer)
#include "test_localjsonserver.moc"
