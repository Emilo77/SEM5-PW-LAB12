#include <future>
#include <iostream>
#include <thread>

void f(const std::string& s, std::promise<unsigned int>& len_promise)
{
    // len_promise is a placeholder for the result
    // warning: std::cout is thread-safe, but
    // using std::cout in multiple threads may result in mixed output
    std::cout << "f() starts for " << s << std::endl;
    // we simulate some processing here
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "f() result is ready for " << s << std::endl;
    len_promise.set_value(s.length());
    // we simulate cleanup here
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "f() completes for " << s << std::endl;
}

int main()
{
    unsigned int len1 { 0 }, len2 { 0 };
    std::promise<unsigned int> len_promise1, len_promise2; // promise for the result
    std::future<unsigned int> len_future1 = len_promise1.get_future(); // represents the promised result
    std::future<unsigned int> len_future2 = len_promise2.get_future();
    const std::string s1 { "najkrwawsza to tragedia" };
    const std::string s2 { "gdy krew zalewa widzow" };
    std::cout << "main() starts" << std::endl;
    std::thread t1 { [&s1, &len_promise1] { f(s1, len_promise1); } };
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t2 { [&s2, &len_promise2] { f(s2, len_promise2); } };
    len1 = len_future1.get(); // wait until len_promise1.set_value()
    std::cout << "len1: " << len1 << " correct? " << (len1 == s1.length()) << std::endl;
    len2 = len_future2.get();
    std::cout << "len2: " << len2 << " correct? " << (len2 == s2.length()) << std::endl;
    t1.join();
    t2.join();
    std::cout << "main() completes" << std::endl;
}
