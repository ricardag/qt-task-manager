#include <QTest>

#include "../src/datasource/NetworkStats.h"

using namespace taskmanager::datasource;

class TestNetworkStats : public QObject {
    Q_OBJECT

private slots:
    void sample_parsesInterfacesSkippingHeaderLines() {
        NetworkStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto samples = stats.sample();

        QCOMPARE(samples.size(), std::size_t(2));
        QCOMPARE(QString::fromStdString(samples[0].name), QStringLiteral("lo"));
        QCOMPARE(samples[0].rxBytes, std::uint64_t(123456));
        QCOMPARE(samples[0].txBytes, std::uint64_t(123456));

        QCOMPARE(QString::fromStdString(samples[1].name), QStringLiteral("eth0"));
        QCOMPARE(samples[1].rxBytes, std::uint64_t(987654321));
        QCOMPARE(samples[1].txBytes, std::uint64_t(123456789));
    }
};

QTEST_APPLESS_MAIN(TestNetworkStats)
#include "test_NetworkStats.moc"
