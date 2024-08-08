#include "Computer.h"
#include <cstdio>
#include <fstream>
#include <unistd.h>

Computer::Computer(size_t mem_size) 
    : memory_size(mem_size), accum(0), instReg(0),
      instCount(0), operCode(0), operand(0), status(OK) 
{
    memory = new int[memory_size];
    init_mem();
}

Computer::Computer(const Computer& other) 
    : accum(other.accum), instReg(other.instReg),
      instCount(other.instCount), operCode(other.operCode),
      operand(other.operand), status(other.status) 
{
    memory_size = other.memory_size;
    memory = new int[memory_size];
    init_mem();
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
    print_registers();
    puts("MEMORY:");
    print_memory();
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
    get_cur_dir_path(cwd, PATH_MAX);
    strcat(cwd, file_name.c_str());

    input.open(cwd);
    if (input.fail()) {
        status = INVALID_FILE_NAME;
        print_status();
        input.close();
        exit(EXIT_FAILURE);
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
 * @brief  Load an instruction into the memory
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

void Computer::execute() {
    //read instuctions from memory until an error happens or the end operation is met
    while (status != END) {
        read_instruction();
        check_status();
        run_operation();
        check_status();
    }

    puts("*** Simpletron execution terminated ***");
}


void Computer::print_registers() const {
    printf("%-20s %+05d\n", "accumulator", accum);
    printf("%-23s %02d\n", "instructionCounter", instCount);
    printf("%-20s %+05d\n", "instructionRegister", instReg);
    printf("%-23s %02d\n", "operationCode", operCode);
    printf("%-23s %02d\n\n", "operand", operand);
}

void Computer::print_memory() const {
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


void Computer::get_cur_dir_path(char * cwd, size_t size) const {
    if (getcwd(cwd, size) != NULL) {
        strcat(cwd, "/src/");
    } 
    else {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
}

bool Computer::check_instruction() const {
    return (instReg >= MIN_INSTR && instReg <= MAX_INSTR);
}

void Computer::run_operation() {
    bool has_branch = false;
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
            status = DIVIDE_ZERO;
        } 
        else {
            accum /= memory[operand];
        }
        break; 
    case REMAINDER:
        if (memory[operand] == 0) {
            status = DIVIDE_ZERO;
        } 
        else {
            accum %= memory[operand];
        }
        break; 
    case MULTIPLY:
        accum *= memory[operand];
        break;   
    case HALT:
        status = END;
        break;
    case BRANCHNEG:
        if (check_overflow() || accum >= 0) {
            break;
        }
    case BRANCHZERO:
        if (accum != 0) {
            break;
        }
    case BRANCH:
        has_branch = true;
        instCount = operand;
        break;
    default:
        status = INVALID_OPERATION;
    }

    //check accumulator overflow after the operations
    if (check_overflow()) {
        status = AC_OVERFLOW;
    }
    if (!has_branch) {
        instCount++;
    }
}


void Computer::print_status() const {
    switch (status) {
    case INVALID_FILE_NAME:
        puts("\n*** Invalid file name                        ***");
        break;
    case DIVIDE_ZERO:
        puts("*** Attemp to divide by zero                   ***");
        break;
    case INVALID_INSTRUCTION:
        puts("\n*** Invalid instruction                      ***");
        break;
    case INVALID_OPERATION:
        puts("\n*** Invalid operation code                   ***");
        break;
    case AC_OVERFLOW:
        puts("*** Accumulator overflow                       ***");
        break;
    default:
        puts("*** Unknown status                             ***");

    }

    if (status != END) {
        puts("*** Simpletron execution abnormally terminated ***\n");
    }
}

void Computer::read_instruction() {
    instReg = memory[instCount];
    if (check_instruction() == false) {
        status = INVALID_INSTRUCTION;
    }
    else {
        operCode = instReg / 100;
        operand = instReg % 100;
    }
}


void Computer::check_status() const {
    if (status != OK && status != END) { 
        print_status();
        exit(EXIT_FAILURE);
    }
}