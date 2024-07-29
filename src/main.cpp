#include "Computer.h"

int main() {

    Computer comp(100);
    comp.load("..//data/add_two_numbers.sml");
    comp.dump();
    comp.execute();

    return 0;
}