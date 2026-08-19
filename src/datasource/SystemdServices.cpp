#include "SystemdServices.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QProcess>
#include <QStringList>

namespace taskmanager::datasource {

namespace {

// One row of the array returned by Manager.ListUnits: (ssssssouso)
// name, description, load_state, active_state, sub_state, followed_unit,
// object_path, job_id, job_type, job_object_path.
struct RawUnit {
    QString name;
    QString description;
    QString loadState;
    QString activeState;
    QString subState;
};

RawUnit extractUnit(const QDBusArgument& arg) {
    RawUnit unit;
    QString followed;
    QDBusObjectPath objectPath;
    uint jobId = 0;
    QString jobType;
    QDBusObjectPath jobObjectPath;

    arg.beginStructure();
    arg >> unit.name >> unit.description >> unit.loadState >> unit.activeState >> unit.subState >> followed >>
        objectPath >> jobId >> jobType >> jobObjectPath;
    arg.endStructure();

    return unit;
}

} // namespace

std::vector<ServiceUnit> SystemdServices::listUnits() const {
    std::vector<ServiceUnit> units = listUnitsViaDBus();
    if (!units.empty()) {
        return units;
    }
    return listUnitsViaSystemctl();
}

std::vector<ServiceUnit> SystemdServices::listUnitsViaDBus() const {
    std::vector<ServiceUnit> result;

    QDBusInterface manager(QStringLiteral("org.freedesktop.systemd1"), QStringLiteral("/org/freedesktop/systemd1"),
                           QStringLiteral("org.freedesktop.systemd1.Manager"), QDBusConnection::systemBus());
    if (!manager.isValid()) {
        return result;
    }

    QDBusReply<QDBusArgument> reply = manager.call(QStringLiteral("ListUnits"));
    if (!reply.isValid()) {
        return result;
    }

    const QDBusArgument arg = reply.value();
    arg.beginArray();
    while (!arg.atEnd()) {
        const RawUnit raw = extractUnit(arg);
        if (!raw.name.endsWith(QStringLiteral(".service"))) {
            continue; // Task Manager's Services tab only cares about services
        }
        ServiceUnit unit;
        unit.name = raw.name;
        unit.description = raw.description;
        unit.loadState = raw.loadState;
        unit.activeState = raw.activeState;
        unit.subState = raw.subState;
        result.push_back(std::move(unit));
    }
    arg.endArray();

    return result;
}

std::vector<ServiceUnit> SystemdServices::listUnitsViaSystemctl() const {
    std::vector<ServiceUnit> result;

    QProcess process;
    process.start(QStringLiteral("systemctl"),
                   {QStringLiteral("list-units"), QStringLiteral("--type=service"), QStringLiteral("--all"),
                    QStringLiteral("--no-legend"), QStringLiteral("--no-pager"), QStringLiteral("--plain")});
    if (!process.waitForFinished(5000)) {
        return result;
    }

    const QString output = QString::fromUtf8(process.readAllStandardOutput());
    for (const QString& line : output.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
        const QStringList fields = line.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
        // UNIT LOAD ACTIVE SUB DESCRIPTION...
        if (fields.size() < 5) continue;

        ServiceUnit unit;
        unit.name = fields[0];
        unit.loadState = fields[1];
        unit.activeState = fields[2];
        unit.subState = fields[3];
        unit.description = fields.mid(4).join(QLatin1Char(' '));
        result.push_back(std::move(unit));
    }

    return result;
}

} // namespace taskmanager::datasource
