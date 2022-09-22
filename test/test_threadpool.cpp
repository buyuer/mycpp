#include <mycpp/base/ThreadPool.hpp>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
    auto pool = new mycpp::ThreadPool(std::thread::hardware_concurrency());

    for (size_t index = 0; index < 100; index++)
    {
        pool->run(
            [](void *) -> void {
                std::this_thread::sleep_for(1s);
                std::cout << "hahaha" << std::endl;
            },
            nullptr);
    }

    std::this_thread::sleep_for(2s);

    delete pool;

    std::this_thread::sleep_for(2s);

    return 0;
}