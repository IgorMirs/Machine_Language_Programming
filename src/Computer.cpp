#include "Computer.h"
#include <cstdio>
#include <fstream>
#include <unistd.h>

Computer::Computer(size_t mem_size) 
    : memory_size(mem_size), accum(0), instReg(0),
      instCount(0), operCode(0),
      operand(0) 
{
    memory = new int[memory_size];
}

Computer::Computer(const Computer& other) 
    : accum(other.accum), instReg(other.instReg),
      instCount(other.instCount), operCode(other.operCode),
      operand(other.operand) 
{
    memory_size = other.memory_size;
    memory = new int[memory_size];
    for (int i = 0; i < memory_size; i++) {
        memory[i] = other.memory[i];
    }
}

Computer::~Computer() {
    delete [] memory;
}

/**
 * @brief Initialize memory with zeros
 * 
 */

void Computer::init_mem() {
    int i = 0;
    while (i < memory_size) {
        memory[i++] = 0;
    }
}

/**
 * @brief Dumps memory content
 * 
 */

void Computer::dump() const {
    puts("REGISTERS:");
    printf("%-20s %+05d\n", "accumulator", accum);
    printf("%-23s %02d\n", "instructionCounter", instCount);
    printf("%-20s %+05d\n", "instructionRegister", instReg);
    printf("%-23s %02d\n", "operationCode", operCode);
    printf("%-23s %02d\n\n", "operand", operand);
    puts("MEMORY:");
    printf("%9d", 0);
    for(int i = 1; i < 10; i++) {
        printf("%7d", i);
    }
    putchar('\n');
    int row_index = 0;
    printf("%2d", row_index);
    for(int i = 0; i < memory_size; i++) {
        if (i > 0 && i % 10 == 0) {
            putchar('\n');
            printf("%2d", row_index += 10);
        }
        printf("%+7.4d", memory[i]);
    }
    putchar('\n');
}   


/**
 * @brief Loads the data from the given file into the memory
 * 
 * @param file_name file name
 */

void Computer::load(std::string file_name) {
    std::ifstream input;
    char cwd[PATH_MAX];
    //get cur dir path
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        strcat(cwd, "/src/");
        strcat(cwd, file_name.c_str());
        input.open(cwd);
    } else {
        perror("getcwd() error");
    }
    std::string msg;
    int mem_loc = 0;
    while (std::getline(input, msg)) {
        load_command(atoi(msg.c_str()), mem_loc++);
    }
    puts("*** Program loading from file completed ***");
    puts("*** Program execution begins            ***");
    input.close();
}

/**
 * @brief  Load an instruction to the memory
 * 
 * @param instr Instruction value
 * @param memory_location Memory cell location
 */
void Computer::load_command(int instr, int memory_location) {
    if (instr == EXIT || (instr <= MIN_INSTR && instr >= MAX_INSTR)) {
        return;
    }
    memory[memory_location] = instr;
}