# Laboratorium - wątki w c++

### Dołączone pliki
Do laboratorium zostały dołączone pliki:
<details><summary>:pushpin: Rozwiń </summary>
<p>

- [data-txt](data-txt)
  - [burza.txt](burza.txt)
  - [komedia-omylek.txt](komedia-omylek.txt)
  - [makbet.txt](makbet.txt)
  - [poskromienie-zlosnicy.txt](poskromienie-zlosnicy.txt)
  - [romeo-i-julia.txt](romeo-i-julia.txt)
  - [sen-nocy-letniej.txt](sen-nocy-letniej.txt)
- [args.cpp](args.cpp)
- [CMakeLists.txt](CMakeLists.txt)
- [grep-input.txt](grep-input.txt)
- [grep-seq.cpp](grep-seq.cpp)
- [promise-exception.cpp](promise-exception.cpp)
- [promise.cpp](promise.cpp)
- [res.cpp](res.cpp)
- [thread-detached.cpp](thread-detached.cpp)
- [thread-join.cpp](thread-join.cpp)
- [thread-raii.cpp](thread-raii.cpp)
- [thread.cpp](thread.cpp)

</p>
</details>



## Instrukcje techniczne

Tak jak wcześniej, do kompilacji będziemy wykorzystywać program `cmake`. Przykładowe programy wymagają aktualnego kompilatora `C++`, np. `GCC` w wersji `7` (ale już nie `Clang 9`). W `OS X` musisz zacząć od instalacji i skonfigurowania `GCC`, np. za pomocą `homebrew`:
```bash
brew install gcc
brew link gcc
ln -s /usr/local/bin/gcc-7 /usr/local/bin/gcc
ln -s /usr/local/bin/g++-7 /usr/local/bin/g++
export CXX=g++  # zmienne te muszą zostać ustawione przed wywołaniem cmake w konsoli OS X
export CC=gcc
```

Instrukcje budowy projektu są w pliku [CMakeLists.txt](CMakeLists.txt). Jeśli kompilator nie wspiera standardu `C++20`, należy zmienić opcję `CMAKE_CXX_STANDARD na 14`.

Najlepiej wykonywać `cmake` po prostu otwierając środowisko graficzne wewnątrz folderu, który zawiera ten plik, po czym użyć wbudowanych funkcji środowiska (np. przyciski w pasku na dole w `VS Code`).  Jeśli chcemy cmake wykonać ręcznie, to przypominamy krótką instrukcję (`cmake` generuje plik `Makefile`, który następnie wykorzystywany jest przez standardowy `make`). Krótka instrukcja:

```bash
cd cpp-lab1
mkdir build  # "out-of-source build", czyli separujemy wyniki budowania do osobnego folderu
cd build
cmake ..
make
```

`cmake ..` musisz wtedy uruchomić tylko za pierwszym razem, po stworzeniu katalogu `build/`.  `make` uruchamiasz przy zmianie plików źródłowych lub zmianie instrukcji budowania.

## Tworzenie wątków

Obiekt klasy [std::thread](https://en.cppreference.com/w/cpp/thread/thread) reprezentuje wątek. Wątek zaczyna wykonywać się po zakończeniu konstruktora. [Konstruktor](https://en.cppreference.com/w/cpp/thread/thread/thread) zadeklarowany jest jako `template< class Function, class... Args > explicit thread( Function&& f, Args&&... args )`.

Przykład [thread.cpp](thread.cpp):

```cpp
#include <thread>
#include <iostream>
#include <chrono>

void f() {
    std::cout << "f(): ensemble on est trop" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "f() completes" << std::endl;
}

void g() {
    std::cout << "g(): ensemble on est trop" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "g() completes" << std::endl;
}

int main() {
    std::cout << "tout seul on n'est rien" << std::endl;
    std::thread t1{f}; // creates a thread executing function f
    std::thread t2{g}; // creates another thread executing function g
    std::cout << "main() completes" << std::endl;
}
```

Powyższy kod co prawda tworzy dwa wątki, ale proces główny kończy się, zanim oba wątki skończą pracę. Wraz z zakończeniem głównego wątku, program wkracza w zachowanie nieokreślone przez standard cpp. W tej sytuacji system operacyjny zwykle kończy pozostałe wątki i zwalnia ich zasoby - ale nie należy na tym polegać.

Dlatego trzeba zlecić wątkowi głównemu poczekanie na zakończenie stworzonych wątków `t1` i `t2`.

Przykład [thread-join.cpp](thread-join.cpp):

```c
int main() {
    std::cout << "tout seul on n'est rien" << std::endl;
    std::thread t1{f}; // creates a thread executing function f
    std::thread t2{g}; // creates another thread executing function g
    t1.join(); // wait for t1 to complete
    t2.join(); // wait for t2 to complete
    std::cout << "main() completes" << std::endl;

}
```

#### Zadanie
Dodaj trzeci wątek do powyższego przykładu.

**Uwaga**: wątków nie można kopiować. Jeśli chcemy stworzony wątek dodać na przykład do vectora, najlepiej go tam przenieść:

```cpp
std::vector<std::thread> threads;
threads.push_back(std::move(t));
```

Po przeniesieniu zmienna `t` nie zawiera już naszego wątku (destruktor `t` nie ma efektu).

## join/detach

Wątek w `C++` jest w jednym z dwóch stanów: podłączalny (*joinable*) albo nie podłączalny (*unjoinable*). Jeśli wątek `t` jest podłączalny, by zwolnić zasoby używane przez t po zakończeniu jego pracy, inny wątek musi podłączyć się do `t` wywołując operację `t.join()`. Wywołanie `t.join()` kilkukrotnie lub wywołanie `t.join()` gdy `t` nie jest podłączalny jest błędem. Stan wątku zwraca metoda `t.joinable()`.

Odłączając wątek od jego obiektu (`t.detach()`), tworzymy *wątek-demona*. Po zakończeniu takiego wątku, system operacyjny sam automatycznie zwalnia jego zasoby.

Konstruktor obiektu wątku zaczyna go w stanie podłączalnym. [Destruktor](https://en.cppreference.com/w/cpp/thread/thread/%7Ethread) obiektu wątku `t`, jeśli wątek `t` jest dalej podłączalny (nikt nie wywołał `t.join()` ani `t.detach()` przed destruktorem) kończy cały proces tak jak po niezłapanym wyjątku, czyli wołając `std::terminate`.

Przykład [thread-detached.cpp](thread-detached.cpp):

```cpp
#include <thread>
#include <iostream>
#include <chrono>

void f() {
    std::cout << "f() starts" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "f() completes" << std::endl;
}

int main() {
    std::cout << "main() starts" << std::endl;
    { 
        std::thread t1{f};
        // try to comment out the following line
        t1.detach();
        // if t1 is joinable, destructor of t1 executes std::terminate!
    }

    std::cout << "main() sleeps" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "main() completes" << std::endl;
}
```

#### Zadanie
- Zakomentuj `t1.detach()` i sprawdź zachowanie programu
- Zwiększ czas działania `t1`. Czy odłączony wątek skończy się wraz z końcem procesu?

Gdy struktura programu jest złożona, zagwarantowanie wykonania `join()` przy wszystkich przebiegach może być trudne. Prostym sposobem rozwiązania tego problemu jest obudowanie wątku klasą-kontenerem, który, podczas destrukcji, wywoła `join` albo `detach` (kod za *Scott Meyers, Effective Modern C++*, a pomysł realizuję [RAII](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization), *resource acquisition is initialization*; w standardzie `C++20` można już użyć podobnej klasy `jthread`).

Przykład [thread-raii.cpp](thread-raii.cpp):

```cpp
// code adopted from Scott Meyers, Effective Modern C++

#include <thread>
#include <iostream>
#include <chrono>

class ThreadRAII {
public:
    enum class DtorAction { join, detach };
    ThreadRAII(std::thread&& t, DtorAction a)  // in dtor, take
        : action(a), t(std::move(t)) {}
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

void f() {

    std::cout << "f() starts" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "f() completes" << std::endl;
}

int main() {
    std::cout << "main() starts" << std::endl;
    {
        ThreadRAII(std::thread{f}, ThreadRAII::DtorAction::join);
    }

    std::cout << "main() sleeps" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "main() completes" << std::endl;
}
```

#### Zadanie
Zmień akcję przy destrukcji na `detach`.

## Przekazywanie parametrów

Parametry możemy przekazać do wątku na dwa sposoby: (1) poprzez argumenty konstruktora; (2) przez argumenty funkcji **lambda** (i wtedy konstruktor ma tylko jeden argument: tę funkcję lambda).

Przykład [args.cpp](args.cpp):

```cpp
#include <thread>
#include <iostream>
#include <chrono>
#include <string>

void f(int timeout) {
    std::cout << "f() starts; my arg is " << timeout << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    std::cout << "f() completes; my arg is " << timeout << std::endl;
}

int main() {
    std::cout << "main() starts" << std::endl;
    int num = 4;
    std::thread t1{f, num}; // thread arguments as constructor arguments
    std::thread t2{[]{ f(3); }}; // []{ f(5);} is a lambda (anonymous function) that executes f(5)
    num = 6;
    std::thread t3{[num] { f(num); }}; // num is captured by value in lambda
    t1.join();
    t2.join(); 
    t3.join(); 
    std::cout << "main() completes" << std::endl;
}
```

#### Zadanie
Do funkcji dodaj drugi argument: napis (o typie `const std::string`). W funkcji wydrukuj ten napis.

## Pobieranie wyników

### przez referencję

W poniższym przykładzie argumentem funkcji `f` wątku jest referencja do zmiennej, w której zapisze on wynik. Konstruktor `std::thread` normalnie przekazuje parametry przez wartość, ale można to zmienić na dwa sposoby:

- opakowując zmienną w `std::ref`, np. `std::thread t3{f, s3, std::ref(len3)}`;
- przekazująć funkcję lambda, której argument łączymy przez referencję z zewnętrzną zmienną, np. `std::thread t1{[s1, &len1]{ f(s1, len1); }};` (`len` jest przekazane przez referencję);

Przykład [res.cpp](res.cpp):

```c
#include <thread>
#include <iostream>

void f(const std::string& s, unsigned int& res) { // res is a placeholder for the result
    res = s.length();
}

int main() {
    unsigned int len1{0}, len2{0}, len3{0};
    const std::string s1{"na szyi zyrafy"};
    const std::string s2{"pchla zaczyna wierzyc"};
    const std::string s3{"w niesmiertelnosc"};
    std::cout << "main() starts" << std::endl;
    std::thread t1{[s1, &len1]{ f(s1, len1); }};  // lambda captures len1 by reference and s by value, hence [&len1]
    std::thread t2{[&s2, &len2]{ f(s2, len2); }}; // here both variables captured by reference
    std::thread t3{f, s3, std::ref(len3)}; // using a std::ref wrapper, as std::thread constructor takes a template parameter by value
    t1.join();
    t2.join();
    t3.join();

    std::cout << "len1: "<<len1<<" correct? "<< (len1==s1.length()) << std::endl;
    std::cout << "len2: "<<len2<<" correct? "<< (len2==s2.length()) << std::endl;
    std::cout << "len3: "<<len3<<" correct? "<< (len3==s3.length()) << std::endl;
    std::cout << "main() completes" << std::endl;
}
```

#### Zadanie
Do funkcji dodaj drugi rezultat: pierszy znak w napisie.

### przez obietnice (promise/future)

Mechanizm `promise/future` jest zalecanym sposobem na przekazywanie rezultatów z wątku. Umożliwia czekanie na konkretny rezultat, a nie tylko na zakończenie wątku. Mechanizm ten realizowany jest za pomocą obiektu klasy `std::promise (template< class R > class std::promise<R&>)` z którego następnie pobierany jest rezultat reprezentowany przez obiekt klasy `std::future (template< class T > class std::future<T&>)`. Obiekt `std::promise` przekazujemy wątkowi, który "spełnia obietnicę" (ustawia wynik), zaś obiekt `std::future` jest potrzebny tam, gdzie czekamy na rezultat (zazwyczaj w wątku głównym).

Przykład [promise.cpp](promise.cpp):

```cpp
#include <thread>
#include <future>
#include <iostream>

void f(const std::string& s, std::promise<unsigned int>& len_promise) { // len_promise is a placeholder for the result
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

int main() {

    unsigned int len1{0}, len2{0};
    std::promise<unsigned int> len_promise1, len_promise2; // promise for the result
    std::future<unsigned int> len_future1 = len_promise1.get_future(); // represents the promised result
    std::future<unsigned int> len_future2 = len_promise2.get_future();
    const std::string s1{"najkrwawsza to tragedia"};
    const std::string s2{"gdy krew zalewa widzow"};
    std::cout << "main() starts" << std::endl;
    std::thread t1{[&s1, &len_promise1]{ f(s1, len_promise1); }};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t2{[&s2, &len_promise2]{ f(s2, len_promise2); }};
    len1 = len_future1.get(); // wait until len_promise1.set_value()
    std::cout << "len1: "<<len1<<" correct? "<< (len1==s1.length()) << std::endl;
    len2 = len_future2.get();
    std::cout << "len2: "<<len2<<" correct? "<< (len2==s2.length()) << std::endl;
    t1.join();
    t2.join();
    std::cout << "main() completes" << std::endl;
}
```
#### Zadanie
- Do funkcji dodaj drugi rezultat: pierszy znak w napisie.
- Spróbuj drugi raz ustawić wartość `len_promise`.
- Spróbuj nie ustawiać wartości `len_promise`.
- Spróbuj drugi raz odczytać wartość `len_future`.

Mechanizm `promise/future` w elegancki sposób rozwiązuje problem wyjątków wygenerowanych w wątku. Wątek przekazuje wyjątek ustawiając `promise.set_exception()`; wyjątek ten jest następnie generowany w wątku, który usiłuje odczytać stan `future`.

Przykład [promise-exception.cpp](promise-exception.cpp):

```cpp
#include <thread>
#include <future>
#include <iostream>
#include <exception>

void f(const std::string& s, std::promise<int>& len_promise) {

    std::cout << "f() starts for " << s << std::endl;
    try {
        if (s == "nie deptac trawnikow")
            throw std::runtime_error("deptac!");
        len_promise.set_value(s.length());
    }
    catch (...) {
        try {
            len_promise.set_exception(std::current_exception());
        }
        catch (...) {} // ignore exceptions thrown in set_exception
    }
}

int main() {
    unsigned int len1{0}, len2{0};
    std::promise<int> len_promise1, len_promise2; // promise for the result
    const std::string s1{"nie wychylac sie"};
    const std::string s2{"nie deptac trawnikow"};
    std::cout << "main() starts" << std::endl;
    std::thread t1{[&s1, &len_promise1]{ f(s1, len_promise1); }};
    std::thread t2{[&s2, &len_promise2]{ f(s2, len_promise2); }};

    try {
        std::future<int> len_future1 = len_promise1.get_future();
        len1 = len_future1.get(); 
        std::cout << "len1: "<<len1<<" correct? "<< (len1==s1.length()) << std::endl;
        std::future<int> len_future2 = len_promise2.get_future();
        len2 = len_future2.get(); // if an exception is set, it will be thrown here
        std::cout << "len2: "<<len2<<" correct? "<< (len2==s2.length()) << std::endl;
    }

    catch(const std::exception& e) {
        std::cout << "exception from a thread: "<< e.what() << std::endl;
    }
    t1.join();
    t2.join();

}
```

## Zadanie punktowane: zlicz słowa

Napisz wielowątkowy program, który zliczy ile w sumie razy wystąpiło dane słowo w wymienionych plikach tekstowych. Wejście programu (na `stdin`) wygląda następująco:

```console
szukane_słowo
liczba_plików
nazwa_pliku1
nazwa_pliku2
...
```

Twój program powinien tworzyć stałą liczbę wątków. Wątki powinny statycznie podzielić się pracą (każdy powinien otrzymać możliwie podobną liczbę nazw plików). Wątki powinny przekazać rezultaty do wątku głównego używając obietnic.

Kilka plików tekstowych - sztuk Szekspira udostępnionych przez Projekt Gutenberg - znajdziesz w [data-txt](data-txt).

Sekwencyjną implementację znajdziesz w [grep-seq.cpp](grep-seq.cpp). (Możesz ją przetestować wykonują `./grep-seq < ../grep-input.txt`)

## Bibliografia

- Scott Meyers, Effective Modern C++, O'Reilly 2015, Items 37, 39
- https://isocpp.org/wiki/faq/cpp11-library-concurrency
- http://stackoverflow.com/questions/22803600/when-should-i-use-stdthreaddetach
- http://stackoverflow.com/questions/19744250/what-happens-to-a-detached-thread-when-main-exits

Data: 2017/11/13

Autor: Krzysztof Rządca

Created: 2017-11-13 Mon 18:06

[Emacs](http://www.gnu.org/software/emacs/) 25.1.1 ([Org](http://orgmode.org/) mode 8.2.10)

[Validate](http://validator.w3.org/check?uri=referer)
