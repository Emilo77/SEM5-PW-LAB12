#include <codecvt>
#include <fstream>
#include <iostream>
#include <list>
#include <locale>
#include <string>

int grep(std::string filename, std::wstring word)
{
    std::wifstream file(filename);
    std::locale loc("pl_PL.UTF-8");
    file.imbue(loc);
    // Check for failbit now (e.g. if file doesn't exist).
    file.exceptions(std::wfstream::failbit);
    // Check only for badbit from now on.
    file.exceptions(std::wfstream::badbit);

    std::wstring line;
    int count = 0;
    while (getline(file, line)) {
        for (auto pos = line.find(word, 0);
             pos != std::string::npos;
             pos = line.find(word, pos + 1)) {
            count++;
        }
    }
    return count;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::locale loc("pl_PL.UTF-8");
    std::wcout.imbue(loc);
    std::wcin.imbue(loc);
    std::wcout.exceptions(std::wfstream::badbit);
    std::wcin.exceptions(std::wfstream::badbit);

    std::wstring word;
    std::getline(std::wcin, word);

    std::wstring s_file_count;
    std::getline(std::wcin, s_file_count);
    int file_count = std::stoi(s_file_count);

    std::list<std::string> filenames {};

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    for (int file_num = 0; file_num < file_count; file_num++) {
        std::wstring w_filename;
        std::getline(std::wcin, w_filename);
        std::string s_filename = converter.to_bytes(w_filename);
        filenames.push_back(s_filename);
    }

    int count = 0;
    for (auto filename : filenames) {
        count += grep(filename, word);
    }

    std::wcout << count << std::endl;
}
