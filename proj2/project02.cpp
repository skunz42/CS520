#include "project02.h"

typedef struct {
    unsigned char opcode;
    unsigned char destination;
    unsigned char left_op;
    unsigned char right_op;
} Instruction;

void Simple_Pipe::print_regs(){
    printf("\nRegisters: \n");
    std::cout << "----------------------------------------" << std::endl;
    for(int i = 0; i < REG_COUNT; i+=2){
        std::string regl("R");
        regl.append(std::to_string(i));
        regl.append(": ");

        std::string regr("R");
        regr.append(std::to_string(i+1));
        regr.append(": ");
        if(i < 15){
            std::cout << "  " << std::setiosflags(std::ios::left) 
            << std::setw(5) << regl  << std::setw(10) << registers[i] << " |   " 
            << std::setw(5) << regr << std::setw(10) << registers[i+1] << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        } else{
            std::cout << "  " << std::setiosflags(std::ios::left) 
            << std::setw(5) << regl << std::setw(10) << registers[i] << " |   " << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
    }  
    printf("\n");
}

long calc_input_size(const char * filename) {
    long num_instructions;
    FILE *fp;
    char c;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return -1;
    }
    
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);
    return sz / 4;
}

int read_input(const char * filename, Instruction *input_array, long array_length) {
    FILE *fp;
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return -1;
    }

    // While loop
    // Bit Shift to get values
    // Assign enum values?

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){

    if (argc != 2) {
        std::cout << "Input in the form: ./simple_pipe <trace file>" << std::endl;
        return -1;
    }

    const char * filename = argv[1];

    long num_instructions = calc_input_size(filename);
    if (num_instructions < 0) {
        return num_instructions;
    }

    Instruction input_array[num_instructions];
    int input_rv = read_input(filename, input_array, num_instructions);
    if (input_rv < 0) {
        return input_rv;
    }

    /*Simple_Pipe simple_pipe;
    for(int i = 0; i < REG_COUNT; i++){
        simple_pipe.registers[i] = 0;
    }

    simple_pipe.print_regs();
    std::cout << "Total execution cycles: " << execution_time << std::endl;
    std::cout << "\nIPC: " << (request_done/(double)execution_time) << std::endl << std::endl;*/

    return 0;
}
