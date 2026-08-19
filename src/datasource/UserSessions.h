#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace taskmanager::datasource {

struct UserSession {
    std::string username;
    std::string terminal;      // e.g. "tty1", "pts/0"
    std::string host;          // remote host, empty for local sessions
    std::int64_t loginTimeEpoch = 0;
};

// Reads currently logged-in users via utmpx (getutxent). The backing file
// defaults to the system's utmp database; tests may override it with
// utmpxname() by constructing with an explicit path.
class UserSessions {
public:
    explicit UserSessions(std::optional<std::string> utmpPath = std::nullopt);

    std::vector<UserSession> listSessions() const;

private:
    std::optional<std::string> utmpPath_;
};

} // namespace taskmanager::datasource
