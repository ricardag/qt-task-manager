#include <QTest>

#include "../src/datasource/CpuStats.h"

using namespace taskmanager::datasource;

class TestCpuStats : public QObject {
    Q_OBJECT

private slots:
    void sampleTicks_parsesAggregateAndPerCoreLines() {
        CpuStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "cpu_sample1");
        const auto ticks = stats.sampleTicks();

        QCOMPARE(ticks.size(), std::size_t(3)); // aggregate "cpu" + cpu0 + cpu1
        QCOMPARE(QString::fromStdString(ticks[0].label), QStringLiteral("cpu"));
        QCOMPARE(ticks[0].user, std::uint64_t(100));
        QCOMPARE(ticks[0].idle, std::uint64_t(800));
    }

    void logicalCoreCount_countsProcessorLines() {
        CpuStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "cpu_sample1");
        QCOMPARE(stats.logicalCoreCount(), 2);
    }

    void modelName_readsFirstModelNameLine() {
        CpuStats stats(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "cpu_sample1");
        QCOMPARE(stats.modelName(), QStringLiteral("Test CPU Model X1"));
    }

    void usagePercent_computesDeltaBetweenTwoSamples() {
        CpuStats sample1(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "cpu_sample1");
        CpuStats sample2(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR) / "cpu_sample2");

        const auto previous = sample1.sampleTicks();
        const auto current = sample2.sampleTicks();

        // sample1 aggregate total = 100+10+50+800+5+0+2+0 = 967, idle+iowait = 805
        // sample2 aggregate total = 200+10+100+900+5+0+2+0 = 1217, idle+iowait = 905
        // totalDelta = 250, idleDelta = 100, busyDelta = 150 -> 60%
        const double usage = CpuStats::usagePercent(previous[0], current[0]);
        QVERIFY(qFuzzyCompare(usage, 60.0));
    }

    void usagePercent_zeroDeltaReturnsZero() {
        CpuCoreTicks a;
        a.user = 10;
        a.idle = 90;
        QCOMPARE(CpuStats::usagePercent(a, a), 0.0);
    }
};

QTEST_APPLESS_MAIN(TestCpuStats)
#include "test_CpuStats.moc"
