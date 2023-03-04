#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void f(int timeout)
{
    std::cout << "f() starts; my arg is " << timeout << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    std::cout << "f() completes; my arg is " << timeout << std::endl;
}

int main()
{
    std::cout << "main() starts" << std::endl;
    int num = 4;
    std::thread t1 { f, num }; // thread arguments as constructor arguments
    std::thread t2 { [] { f(3); } }; // []{ f(5);} is a lambda (anonymous function) that executes f(5)
    num = 6;
    std::thread t3 { [num] { f(num); } }; // num is captured by value in lambda
    t1.join();
    t2.join();
    t3.join();
    std::cout << "main() completes" << std::endl;
}
