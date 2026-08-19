#pragma once

#include <QString>
#include <vector>

namespace taskmanager::datasource {

struct ServiceUnit {
    QString name;         // e.g. "sshd.service"
    QString description;
    QString loadState;    // "loaded", "not-found", ...
    QString activeState;  // "active", "inactive", "failed", ...
    QString subState;     // "running", "exited", "dead", ...
};

// Lists systemd units over D-Bus (org.freedesktop.systemd1.Manager). This is
// the one datasource class that depends on Qt (Qt6::DBus): talking to D-Bus
// without it means hand-rolling the libdbus C API, which is disproportionate
// effort next to QDBusInterface. Falls back to parsing `systemctl
// list-units` output via QProcess if the D-Bus session/system bus call
// fails (e.g. sandboxed environments without D-Bus access).
class SystemdServices {
public:
    std::vector<ServiceUnit> listUnits() const;

private:
    std::vector<ServiceUnit> listUnitsViaDBus() const;
    std::vector<ServiceUnit> listUnitsViaSystemctl() const;
};

} // namespace taskmanager::datasource
