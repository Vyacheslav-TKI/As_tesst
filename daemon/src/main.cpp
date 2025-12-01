#include "daemon/celcon_daemon.h"

int main() {
    try {
        Celcon_daemon daemon;
        daemon.exec();
    } catch (const std::exception& e) {
        // syslog уже внутри, но на всякий
        return 1;
    }
    return 0;
}
