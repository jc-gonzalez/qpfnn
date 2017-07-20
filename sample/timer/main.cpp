// -*- C++ -*-

#include "timer.h"

#include <iostream>
#include <unistd.h>

void func1();
void func2(int a);

class TimerTest {
public:
    TimerTest() {}
public:
    void run() {
        Timer * delayed_method = new Timer(3000, true, &TimerTest::method, this, 42);
    }
protected:
    void method(int meaning) {
        std::cerr << "The answer to the question of Life,\n"
	          << " the Universe and Everything is " << meaning << '\n';
        Timer * delayed_method = new Timer(3000, true, &TimerTest::method, this, 42);
    }
};

void func1()
{
    std::cerr << "This is func1\n";
    Timer delayed_func2(1000, true, &func2, 10);
}

void func2(int a)
{
    std::cerr << "This is func2 with argument: " << a << "\n";
    Timer delayed_func2(1000, true, &func2, a + 1);
}

int main(int arg, char * argv[])
{
    Timer delayed_func1(2000, true, &func1);
    Timer delayed_func2(1000, false, &func2, 1);
   
    TimerTest a;
    a.run();

    sleep(10);

    return 0;
}
