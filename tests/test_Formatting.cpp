#include <QTest>

#include "../src/util/Formatting.h"

using namespace taskmanager::util;

class TestFormatting : public QObject {
    Q_OBJECT

private slots:
    void formatBytes_bytes() { QCOMPARE(QString::fromStdString(formatBytes(500)), QStringLiteral("500 B")); }
    void formatBytes_kilobytes() { QCOMPARE(QString::fromStdString(formatBytes(1536)), QStringLiteral("1.5 KB")); }
    void formatBytes_gigabytes() {
        QCOMPARE(QString::fromStdString(formatBytes(2ULL * 1024 * 1024 * 1024)), QStringLiteral("2.0 GB"));
    }

    void formatDurationSeconds_underADay() {
        QCOMPARE(QString::fromStdString(formatDurationSeconds(3725)), QStringLiteral("01:02:05"));
    }
    void formatDurationSeconds_overADay() {
        QCOMPARE(QString::fromStdString(formatDurationSeconds(90000)), QStringLiteral("1:01:00:00"));
    }

    void formatPercent_clampsToRange() {
        QCOMPARE(QString::fromStdString(formatPercent(150.0)), QStringLiteral("100%"));
        QCOMPARE(QString::fromStdString(formatPercent(-10.0)), QStringLiteral("0%"));
        QCOMPARE(QString::fromStdString(formatPercent(42.4)), QStringLiteral("42%"));
    }
};

QTEST_APPLESS_MAIN(TestFormatting)
#include "test_Formatting.moc"
