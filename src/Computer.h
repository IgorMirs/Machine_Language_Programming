#ifndef COMPUTER_H
#define COMPUTER_H

#include <cstdlib>
#include <string>

#define EXIT -99999
#define MIN_INSTR -9999
#define MAX_INSTR 9999

/****Input/output operations: ****/
#define READ 10 // Read a word from the keyboard into a specific location in memory.
#define WRITE 11 //Write a word from a specific location in memory to the screen.

/****Load/store operations: ****/
#define LOAD 20 //Load a word from a specific location in memory into the accumulator.
#define STORE 21 //Store a word from the accumulator into a specific location in memory.
#define DIRECT_LOAD 22 //Load a given word to memory //TODO

/****Arithmetic operations:****/
#define ADD 30 //Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
#define SUBTRACT 31 //Subtract a word from a specific location in memory from the word in the accumulator (leave the result in the accumulator).
#define DIVIDE 32 //Divide the word in the accumulator by a word from a specific location in memory (leave the result in the accumulator).
#define MULTIPLY 33 //Multiply a word from a specific location in memory by the word in the accumulator (leave the result in the accumulator).
#define REMAINDER 34 //Take a remainder after the division of the word in the accumulator by a word from a specific location in memory (leave the result in the accumulator).

/****Transfer-of-control operations:****/
#define BRANCH 40 //Branch to a specific location in memory.
#define BRANCHNEG 41 //Branch to a specific location in memory if the accumulator is negative.
#define BRANCHZERO 42 //Branch to a specific location in memory if the accumulator is zero.
#define HALT 43 //Halt—i.e., the program has completed its task.


enum ComputerStatus {
    OK,
    ERROR,
    INVALID_FILE_NAME,
    INVALID_INSTRUCTION,
    INVALID_OPERATION,
    AC_OVERFLOW,
    DIVIDE_ZERO,
    END
};

class Computer {
    public:
        Computer(size_t memory_size);
        //copy constructor
        Computer(const Computer& other);
        ~Computer();

        void dump() const;

        void load(std::string file_name);
        void execute();

    private:
        int* memory;
        size_t memory_size;
        enum ComputerStatus status;
        //registers
        int accum; //accumulator register
        int instReg; //current instruction
        int instCount; //the number of memory location (0 - 99)
        int operCode; //operation code
        int operand; //right most two digits of the operation

        void init_mem();
        void load_command(int instr, int memory_location);
        bool check_overflow() const;
        void print_registers() const;
        void print_memory() const;
        void get_cur_dir_path(char* cwd, size_t size) const;
        bool check_instruction() const;
        void run_operation();
        void print_status() const;
        void read_instruction();
        void check_status() const;
        


};

#endif