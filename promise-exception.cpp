#include <exception>
#include <future>
#include <iostream>
#include <thread>

void f(const std::string& s, std::promise<int>& len_promise)
{
    std::cout << "f() starts for " << s << std::endl;
    try {
        if (s == "nie deptac trawnikow")
            throw std::runtime_error("deptac!");
        len_promise.set_value(s.length());
    } catch (...) {
        try {
            len_promise.set_exception(std::current_exception());
        } catch (...) { // set_exception throws when len_promise already set
            throw; // safer to rethrow than to deadlock
        }
    }
}

int main()
{
    unsigned int len1 { 0 }, len2 { 0 };
    std::promise<int> len_promise1, len_promise2; // promise for the result
    const std::string s1 { "nie wychylac sie" };
    const std::string s2 { "nie deptac trawnikow" };
    std::cout << "main() starts" << std::endl;
    std::thread t1 { [&s1, &len_promise1] { f(s1, len_promise1); } };
    std::thread t2 { [&s2, &len_promise2] { f(s2, len_promise2); } };
    try {
        std::future<int> len_future1 = len_promise1.get_future();
        len1 = len_future1.get();
        std::cout << "len1: " << len1 << " correct? " << (len1 == s1.length()) << std::endl;
        std::future<int> len_future2 = len_promise2.get_future();
        len2 = len_future2.get(); // if an exception is set, it will be thrown here
        std::cout << "len2: " << len2 << " correct? " << (len2 == s2.length()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "exception from a thread: " << e.what() << std::endl;
    }
    t1.join();
    t2.join();
}
