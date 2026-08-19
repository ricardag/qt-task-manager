#include <QTest>

#include "../src/datasource/MemoryStats.h"

using namespace taskmanager::datasource;

class TestMemoryStats : public QObject {
    Q_OBJECT

private slots:
    void sample_parsesMeminfoFields() {
        MemoryStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const MemorySample sample = stats.sample();

        QCOMPARE(sample.totalKb, std::uint64_t(8000000));
        QCOMPARE(sample.freeKb, std::uint64_t(2000000));
        QCOMPARE(sample.availableKb, std::uint64_t(4000000));
        QCOMPARE(sample.buffersKb, std::uint64_t(100000));
        QCOMPARE(sample.cachedKb, std::uint64_t(1500000));
        QCOMPARE(sample.swapTotalKb, std::uint64_t(2000000));
        QCOMPARE(sample.swapFreeKb, std::uint64_t(1900000));
    }

    void usedKb_isTotalMinusAvailable() {
        MemoryStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const MemorySample sample = stats.sample();
        QCOMPARE(sample.usedKb(), std::uint64_t(4000000));
    }

    void swapUsedKb_isSwapTotalMinusSwapFree() {
        MemoryStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const MemorySample sample = stats.sample();
        QCOMPARE(sample.swapUsedKb(), std::uint64_t(100000));
    }

    void sample_missingFileReturnsZeroed() {
        MemoryStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "does-not-exist");
        const MemorySample sample = stats.sample();
        QCOMPARE(sample.totalKb, std::uint64_t(0));
    }
};

QTEST_APPLESS_MAIN(TestMemoryStats)
#include "test_MemoryStats.moc"
