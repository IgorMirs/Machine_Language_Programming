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
    1. Fetch (read) the instruction in (instCount) memory location and save it in instReg.
    2. Split the instruction into operCode and operand
    3. Define the operation
*/
    //fetch the contents of the (instCount) memory location into the instReg
    bool exit_ = false;
    while (!exit_) {
        //read instruction
        instReg = memory[instCount];
        if (check_instruction() == true) {
            status = INVALID_INSTRUCTION;
            print_status();
            puts("*** Simpletron execution abnormally terminated ***\n");
            exit(EXIT_FAILURE);
        }
        operCode = instReg / 100;
        operand = instReg % 100;
        get_operation();
        if (status != OK) {
            print_status();
            exit(EXIT_FAILURE);
        }

        
        if (check_overflow()) {
            print_overflow();
            exit_ = true;
        }
        if (operCode == BRANCH || (operCode == BRANCHNEG && accum < 0) || (operCode == BRANCHZERO && accum == 0)) {
            instCount = operand;
        }
        else {
            instCount++;
        }
    } //end while
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
    return (instReg < MIN_INSTR || instReg > MAX_INSTR);
}

enum ComputerStatus Computer::get_operation() {
    switch(operCode) {
    case READ:
        putchar('?');
        putchar(' ');
        scanf("%d", &memory[operand]);
        return OK;
    case WRITE:
        printf("%d\n", memory[operand]);
        return OK;   
    case LOAD:
        accum = memory[operand];
        return OK;
    case DIRECT_LOAD:
        //get memory location
        instCount++; //go to the next word
        memory[operand] = memory[instCount];
        return OK;   
    case STORE:
        memory[operand] = accum;
        return OK;   
    case ADD:
        accum += memory[operand];
        return OK;   
    case SUBTRACT:
        accum -= memory[operand];
        return OK;   
    case DIVIDE:
        if (memory[operand] == 0) {
            puts("*** Simpletron execution abnormally terminated ***");
            return DIVIDE_ZERO;
        } 
        else {
            accum /= memory[operand];
        }
        return OK; 
    case MULTIPLY:
        accum *= memory[operand];
        return OK;   
    case HALT:
        return END; 
    case BRANCH:
    case BRANCHNEG:
    case BRANCHZERO:
        return OK;
    default:
        status = INVALID_OPERATION;
        print_status();
        return ERROR;
    }
}


void Computer::print_status() const {
    switch (status) {
    case INVALID_FILE_NAME:
        puts("\n*** Invalid file name                        ***");
        break;
    case END:
        puts("*** Simpletron execution terminated ***");
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
    default:
        puts("*** Unknown status                             ***");

    }

    if (status == INVALID_FILE_NAME || status == INVALID_INSTRUCTION || status == DIVIDE_ZERO || status == INVALID_OPERATION) {
        puts("*** Simpletron execution abnormally terminated ***\n");
    }
}