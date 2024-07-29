#include "Computer.h"

int main() {

    Computer comp(100);
    comp.load("add_two_numbers.sml");
    comp.dump();

    return 0;
}