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
int registers[REG_COUNT];

int CYCLE_STALL;
int FRESH_INSTR;

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
    
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        return -1;
    }

    for (int i = 0; i < array_length; i++) {
        int n = fread(input_array+i, 1, INSTR_SIZE, fp);
        if (n == INSTR_SIZE) {
//            printf("%x\n", input_array[i]);
        } else {
            printf("%d\n", n);
            printf("Error reading file.\n");
            return -1;
        }
    }

    fclose(fp);

    return 0;
}

void execute_helper(Instruction instr) {
    unsigned char opcode = instr.opcode;
    switch(opcode) {
        case 0x00:
            registers[instr.destination] = instr.left_op;
            CYCLE_STALL = 1;
            break;
        case 0x10:
            registers[instr.destination] = registers[instr.left_op] + registers[instr.right_op];
            CYCLE_STALL = 1;
            break;
        case 0x11:
            registers[instr.destination] = registers[instr.left_op] + instr.right_op;
            CYCLE_STALL = 1;
            break;
        case 0x20:
            registers[instr.destination] = registers[instr.left_op] - registers[instr.right_op];
            CYCLE_STALL = 1;
            break;
        case 0x21:
            registers[instr.destination] = registers[instr.left_op] - instr.right_op;
            CYCLE_STALL = 1;
            break;
        case 0x30:
            if (FRESH_INSTR == TRUE) {
                CYCLE_STALL = 2;
                FRESH_INSTR = FALSE;
            }
            if (CYCLE_STALL == 1) {
                registers[instr.destination] = registers[instr.left_op] * registers[instr.right_op];
                FRESH_INSTR = TRUE;
            }
            break;
        case 0x31:
            if (FRESH_INSTR == TRUE) {
                CYCLE_STALL = 2;
                FRESH_INSTR = FALSE;
            }
            if (CYCLE_STALL == 1) {
                registers[instr.destination] = registers[instr.left_op] * instr.right_op;
                FRESH_INSTR = TRUE;
            }
            break;
        case 0x40:
            if (FRESH_INSTR == TRUE) {
                CYCLE_STALL = 4;
                FRESH_INSTR = FALSE;
            }
            if (CYCLE_STALL == 1) {
                registers[instr.destination] = registers[instr.left_op] / registers[instr.right_op];
                FRESH_INSTR = TRUE;
            }
            break;
        case 0x41:
            if (FRESH_INSTR == TRUE) {
                CYCLE_STALL = 4;
                FRESH_INSTR = FALSE;
            }
            if (CYCLE_STALL == 1) {
                registers[instr.destination] = registers[instr.left_op] / instr.right_op;
                FRESH_INSTR = TRUE;
            }
            break;
        default:
            printf("Should not have gotten here!!!!!\n");
            break;
    }

    CYCLE_STALL--;
}

void compute_execution() {
    if (stages[2].has_request == TRUE) {
        execute_helper(stages[2].decoded_instr);
        if (CYCLE_STALL <= 0) {
            stages[2].has_request = FALSE;
        }
    } 
}

Instruction decode_helper(unsigned int raw_instr) {
    unsigned char ops[INSTR_SIZE];

//    printf("RAW: %x ", raw_instr);

    for (int i = 0; i < INSTR_SIZE; i++) {
        ops[i] = raw_instr & 0xff;
        raw_instr >>= 8;
    }
//    for (int i = INSTR_SIZE-1; i >= 0; i--) {
//        printf("%x ", ops[i]);
//    }
//    printf("\n");
    Instruction instr = {ops[3], ops[2], ops[1], ops[0]};
    return instr;
}

void compute_decode() {
    if (stages[1].has_request == TRUE && stages[2].has_request == FALSE) {
        stages[1].decoded_instr = decode_helper(stages[1].raw_instr);

        stages[2].raw_instr = stages[1].raw_instr;
        stages[2].has_request = TRUE;
        stages[2].decoded_instr = stages[1].decoded_instr;
        stages[1].has_request = FALSE;
    }
}

void compute_fetch(long * instructions_processed, unsigned int * input_array, long instruction_count) {
    if (stages[0].has_request == TRUE && stages[1].has_request == FALSE) {
        stages[1].raw_instr = stages[0].raw_instr;
        stages[1].has_request = TRUE;
        stages[0].has_request = FALSE;
    }

    if (*instructions_processed < instruction_count && stages[0].has_request == FALSE)  {
        stages[0].raw_instr = input_array[*instructions_processed];
        stages[0].has_request = TRUE;
        *instructions_processed = *instructions_processed + 1;
    } else if (*instructions_processed >= instruction_count && stages[0].has_request == FALSE) {
        stages[0].raw_instr = 0;
    }
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

void print_debug_statement(unsigned int * input_array) {
    if (stages[2].has_request || stages[1].has_request || stages[0].has_request) {
        printf("\nexecution time: %d\n", execution_time);
    }
    if (stages[2].has_request == TRUE) {
        printf("[3] executing: %x current cycle: 1 cycle needed: 1\n", stages[2].raw_instr);
    }
    if (stages[1].has_request == TRUE) {
        printf("[2] decoding: %x\n", stages[1].raw_instr);
    }
    if (stages[0].has_request == TRUE) {
        printf("[1] fetching: %x\n", stages[0].raw_instr);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Input in the form: ./simple_pipe <trace file>\n");
        return -1;
    }

    const char * filename = argv[1];

    //Initialize registers
    Simple_Pipe simple_pipe;
    for(int i = 0; i < REG_COUNT; i++){
        simple_pipe.registers[i] = 0;
        registers[i] = 0;
    }

    // Calculate the number of instructions
    long num_instructions = calc_input_size(filename);
    if (num_instructions < 0) {
        return num_instructions;
    }

    // Read input
    unsigned int input_array[num_instructions] = {0};
    int input_rv = read_input(filename, input_array, num_instructions);
    if (input_rv < 0) {
        return input_rv;
    }
    
    // Initialize the three stages
    initialize_stages();

    long instructions_processed = 0;
    CYCLE_STALL = 0;
    FRESH_INSTR = TRUE;

    // execute
    while (instructions_processed < num_instructions || !all_stages_empty()) {
        for (int i = NUM_STAGES-1; i >= 0; i--) {
            exec_pipeline(i, &instructions_processed, input_array, num_instructions);
        }
//        print_debug_statement(input_array);
        execution_time++;
    }

    for (int i = 0; i < REG_COUNT; i++) {
        simple_pipe.registers[i] = registers[i];
    }

    execution_time--;
    request_done = num_instructions;

    simple_pipe.print_regs();
    std::cout << "Total execution cycles: " << execution_time << std::endl;
    std::cout << "\nIPC: " << (request_done/(double)execution_time) << std::endl << std::endl;

    return 0;
}
