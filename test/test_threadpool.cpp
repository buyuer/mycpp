#include <mycpp/base/ThreadPool.hpp>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
    mycpp::ThreadPool pool;

    for (size_t index = 0; index < 100; index++)
    {
        pool.run(
            [](void *) -> void {
                std::this_thread::sleep_for(1s);
                std::cout << "hahaha" << std::endl;
            },
            nullptr);
    }

    std::this_thread::sleep_for(100s);

    return 0;
}