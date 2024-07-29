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

bool Computer::check_overflow() const {
    return (accum > 9999 || accum < -9999);
}

void Computer::print_overflow() const {
    puts("*** Accumulator overflow                       ***");
    puts("*** Simpletron execution abnormally terminated ***");
}

void Computer::execute() {
    /*
    1. Fetch (read) the instrucion in (instCount) memory location and save it in instReg.
    2. Split the instruction into operCode and operand
    3. Define the operation
*/
    //fetch the contents of the (ic) memory location into the ir
    bool exit = false;
    while (!exit) {
        
        instReg = memory[instCount];
        operCode = instReg / 100;
        operand = instReg % 100;

        switch(operCode) {
        case READ:
            putchar('?');
            putchar(' ');
            scanf("%d", &memory[operand]);
            break;
        case WRITE:
            printf("%d\n", memory[operand]);
            break;   
        case LOAD:
            accum = memory[operand];
            break;   
        case DIRECT_LOAD:
            //get memory location
            instCount++; //go to the next word
            memory[operand] = memory[instCount];
            break;   
        case STORE:
            memory[operand] = accum;
            break;   
        case ADD:
            accum += memory[operand];
            break;   
        case SUBTRACT:
            accum -= memory[operand];
            break;   
        case DIVIDE:
            if (memory[operand] == 0) {
                puts("*** Attemp to divide by zero                   ***");
                puts("*** Simpletron execution abnormally terminated ***");
                exit = true;
            } 
            else {
                accum /= memory[operand];
            }
            break;   
        case MULTIPLY:
            accum *= memory[operand];
            break;   
        case HALT:
            puts("*** Simpletron execution terminated ***");
            exit = true;
            break;   
        case BRANCH:
        case BRANCHNEG:
        case BRANCHZERO:
            break;
        default:
            puts("\n*** Invalid operation code                     ***");
            puts("*** Simpletron execution abnormally terminated ***\n");
            exit = true;
        }
        if (check_overflow()) {
            print_overflow();
            exit = true;
        }
        if (operCode == BRANCH || (operCode == BRANCHNEG && accum < 0) || (operCode == BRANCHZERO && accum == 0)) {
            instCount = operand;
        }
        else {
            instCount++;
        }
    }
}


