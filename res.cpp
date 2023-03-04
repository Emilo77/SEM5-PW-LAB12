#include <iostream>
#include <thread>

void f(const std::string& s, unsigned int& res)
{ // res is a placeholder for the result
    res = s.length();
}

int main()
{
    unsigned int len1 { 0 }, len2 { 0 }, len3 { 0 };
    const std::string s1 { "na szyi zyrafy" };
    const std::string s2 { "pchla zaczyna wierzyc" };
    const std::string s3 { "w niesmiertelnosc" };
    std::cout << "main() starts" << std::endl;
    std::thread t1 { [s1, &len1] { f(s1, len1); } }; // lambda captures len1 by reference and s by value, hence [&len1]
    std::thread t2 { [&s2, &len2] { f(s2, len2); } }; // here both variables captured by reference
    std::thread t3 { f, s3, std::ref(len3) }; // using a std::ref wrapper, as std::thread constructor takes a template parameter by value
    t1.join();
    t2.join();
    t3.join();
    std::cout << "len1: " << len1 << " correct? " << (len1 == s1.length()) << std::endl;
    std::cout << "len2: " << len2 << " correct? " << (len2 == s2.length()) << std::endl;
    std::cout << "len3: " << len3 << " correct? " << (len3 == s3.length()) << std::endl;
    std::cout << "main() completes" << std::endl;
}
