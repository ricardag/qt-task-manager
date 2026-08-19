#include <QTest>

#include "../src/datasource/ProcessReader.h"

using namespace taskmanager::datasource;

class TestProcessReader : public QObject {
    Q_OBJECT

private slots:
    void readOne_parsesNameWithParenthesesAndSpaces() {
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto info = reader.readOne(1234);

        QVERIFY(info.has_value());
        QCOMPARE(QString::fromStdString(info->name), QStringLiteral("weird (name) proc"));
        QCOMPARE(info->pid, 1234);
        QCOMPARE(info->ppid, 1);
        QCOMPARE(info->state, 'S');
        QCOMPARE(info->utimeTicks, std::uint64_t(500));
        QCOMPARE(info->stimeTicks, std::uint64_t(200));
        QCOMPARE(info->priority, 20);
        QCOMPARE(info->threadCount, 4); // overwritten by status's Threads field
    }

    void readOne_mergesStatusFields() {
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto info = reader.readOne(1234);

        QVERIFY(info.has_value());
        QCOMPARE(info->vmRssKb, std::uint64_t(45678));
        QCOMPARE(info->vmSizeKb, std::uint64_t(123456));
        QCOMPARE(info->uid, 1000);
    }

    void readOne_joinsNullSeparatedCmdline() {
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto info = reader.readOne(1234);

        QVERIFY(info.has_value());
        QCOMPARE(QString::fromStdString(info->cmdline), QStringLiteral("weird-proc --flag value"));
    }

    void readOne_parsesIoCounters() {
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto info = reader.readOne(1234);

        QVERIFY(info.has_value());
        QCOMPARE(info->readBytes, std::uint64_t(4096));
        QCOMPARE(info->writeBytes, std::uint64_t(8192));
    }

    void readOne_missingProcessReturnsNullopt() {
        // 9999/ exists as a directory (simulating a PID that was listed by
        // readdir but exited before its files could be read) but has no
        // stat file inside.
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        QVERIFY(!reader.readOne(9999).has_value());
        QVERIFY(!reader.readOne(424242).has_value());
    }

    void readAll_skipsNonNumericAndIncompleteEntries() {
        ProcessReader reader(std::filesystem::path(TASKMANAGER_TEST_FIXTURES_DIR));
        const auto all = reader.readAll();

        QCOMPARE(all.size(), std::size_t(1));
        QCOMPARE(all[0].pid, 1234);
    }
};

QTEST_APPLESS_MAIN(TestProcessReader)
#include "test_ProcessReader.moc"
