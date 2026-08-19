#include "UserSessions.h"

#include <cstring>
#include <utmpx.h>

namespace taskmanager::datasource {

UserSessions::UserSessions(std::optional<std::string> utmpPath) : utmpPath_(std::move(utmpPath)) {}

std::vector<UserSession> UserSessions::listSessions() const {
    std::vector<UserSession> sessions;

    if (utmpPath_) {
        utmpxname(utmpPath_->c_str());
    }

    setutxent();
    while (struct utmpx* entry = getutxent()) {
        if (entry->ut_type != USER_PROCESS) {
            continue;
        }

        UserSession session;
        session.username.assign(entry->ut_user, strnlen(entry->ut_user, sizeof(entry->ut_user)));
        session.terminal.assign(entry->ut_line, strnlen(entry->ut_line, sizeof(entry->ut_line)));
        session.host.assign(entry->ut_host, strnlen(entry->ut_host, sizeof(entry->ut_host)));
        session.loginTimeEpoch = entry->ut_tv.tv_sec;
        sessions.push_back(std::move(session));
    }
    endutxent();

    return sessions;
}

} // namespace taskmanager::datasource
