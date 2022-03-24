#include "project02.h"

#include <unistd.h>

#define INSTR_SIZE 4
#define NUM_STAGES 3
#define FALSE 0
#define TRUE 1

enum Opcode {
    SET = 0x00,
    ADD = 0x11,
    ADDI = 0x12,
    SUB = 0x20,
    SUBI = 0x21,
    MUL = 0x30,
    MULI = 0x31,
    DIV = 0x40,
    DIVI = 0x41
};

typedef struct {
    unsigned char opcode;
    unsigned char destination;
    unsigned char left_op;
    unsigned char right_op;
} Instruction;

typedef struct {
    int stage_num;
    int raw_instr;
    int has_request;
    Instruction decoded_instr;
} Stage;

Stage stages[NUM_STAGES];

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

int all_stages_empty() {
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].has_request == TRUE) {
            return FALSE;
        }
    }
    return TRUE;
}

void initialize_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        stages[i].stage_num = i;
        stages[i].raw_instr = -1;
        stages[i].has_request = FALSE;
    }
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
    return sz / INSTR_SIZE;
}

int read_input(const char * filename, unsigned int *input_array, long array_length) {
    FILE *fp;
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        return -1;
    }

    for (int i = 0; i < array_length; i++) {
        int n = fread(input_array+i, 1, INSTR_SIZE, fp);
        if (n == INSTR_SIZE) {
            printf("%x\n", input_array[i]);
        } else {
            printf("Error reading file.\n");
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

void compute_execution() {

}

void compute_decode() {

}

void compute_fetch(long * instructions_processed, unsigned int * input_array, long instruction_count) {
    *instructions_processed = *instructions_processed + 1;
}

void exec_pipeline(int active_stage, long * instructions_processed, unsigned int * input_array, long instruction_count) {
    if (active_stage == 0) {
        compute_fetch(instructions_processed, input_array, instruction_count);
    } else if (active_stage == 1) {
        compute_decode();
    } else if (active_stage == 2) {
        compute_execution();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Input in the form: ./simple_pipe <trace file>\n");
        return -1;
    }

    const char * filename = argv[1];

    // Calculate the number of instructions
    long num_instructions = calc_input_size(filename);
    if (num_instructions < 0) {
        return num_instructions;
    }

    // Read input
    unsigned int input_array[num_instructions];
    int input_rv = read_input(filename, input_array, num_instructions);
    if (input_rv < 0) {
        return input_rv;
    }

    // Initialize the three stages
    initialize_stages();

    long instructions_processed = 0;

    // execute
    while (instructions_processed < num_instructions || !all_stages_empty()) {
        for (int i = NUM_STAGES-1; i >= 0; i--) {
            exec_pipeline(i, &instructions_processed, input_array, num_instructions);
        }
        execution_time++;
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
