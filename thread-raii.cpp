// code adopted from Scott Meyers, Effective Modern C++
#include <chrono>
#include <iostream>
#include <thread>

class ThreadRAII {
public:
    enum class DtorAction {
        join,
        detach
    };

    ThreadRAII(std::thread&& t, DtorAction a)
        : action(a)
        , t(std::move(t))
    {
    }

    ~ThreadRAII()
    {
        if (t.joinable()) {
            if (action == DtorAction::join) {
                t.join();
            } else {
                t.detach();
            }
        }
    }

    std::thread& get() { return t; }

private:
    DtorAction action;
    std::thread t;
};

void f()
{
    std::cout << "f() starts" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "f() completes" << std::endl;
}

int main()
{
    std::cout << "main() starts" << std::endl;
    {
        ThreadRAII(std::thread { f }, ThreadRAII::DtorAction::join);
    }
    std::cout << "main() sleeps" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "main() completes" << std::endl;
}
