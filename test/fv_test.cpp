#include <lshbox/query/fv.h>
#include <string>
#include <iostream>
int main() {
    std::cout << "hello world" << std::endl;
    FV fv(4);
    std::string str = fv.toString();
    std::cout << str << std::endl;
}
