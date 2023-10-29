#include <iostream>
#include <chrono>
#include <thread>
#include <libcron/Cron.h>

int main() {
    using namespace std::chrono_literals;

    libcron::Cron cron;

    auto task = [](auto& i) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::ctime(&time) << std::endl;
        };

    cron.add_schedule("Print time task", "* * * * * ?", task);

    while (true) {
        cron.tick();
        std::this_thread::sleep_for(500ms);
    }
}