#ifndef COMPUTER_H
#define COMPUTER_H

#include <cstdlib>
#include <string>

#define EXIT -99999
#define MIN_INSTR -9999
#define MAX_INSTR 9999

class Computer {
    public:
        Computer(size_t memory_size);
        //copy constructor
        Computer(const Computer& other);
        ~Computer();

        void dump() const;

        void load(std::string file_name);

    private:
        int* memory;
        size_t memory_size;
        //registers
        int accum; //accumulator register
        int instReg; //current instruction
        int instCount; //the number of memory location (0 - 99)
        int operCode; //operation code
        int operand; //right most two digits of the operation

        void init_mem();
        void load_command(int instr, int memory_location);
};

#endif