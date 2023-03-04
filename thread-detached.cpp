#include <chrono>
#include <iostream>
#include <thread>

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
        std::thread t1 { f };
        // try to comment out the following line
        t1.detach();
        // if t1 is joinable, destructor of t1 executes std::terminate!
    }
    std::cout << "main() sleeps" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "main() completes" << std::endl;
}
