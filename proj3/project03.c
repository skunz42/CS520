#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define INSTR_SIZE 4
#define NUM_STAGES 9
#define FALSE 0
#define TRUE 1
#define REG_COUNT 16

enum Opcode {
    SET = 0x00,
    ADD = 0x11,
    ADDI = 0x12,
    SUB = 0x20,
    SUBI = 0x21,
    MUL = 0x30,
    MULI = 0x31,
    DIV = 0x40,
    DIVI = 0x41,
    LDI = 0x50,
    LD = 0X51,
    STI = 0x60,
    ST = 0x61,
    BEZ = 0x70,
    BGEZ = 0x71,
    BLEZ = 0x72,
    BGTZ = 0x73,
    BLTZ = 0x74,
    RET = 0xFF
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
//            printf("%x\n", input_array[i]>>24);
        } else {
            printf("%d\n", n);
            printf("Error reading file.\n");
            return -1;
        }
    }

    fclose(fp);

    return 0;
}

void initialize_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        stages[i].stage_num = i;
        stages[i].raw_instr = -1;
        stages[i].has_request = FALSE;
    }
}

int all_stages_empty() {
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].has_request == TRUE) {
            return FALSE;
        }
    }
    return TRUE;
}

void compute_writeback() {
    if (stages[8].has_request == TRUE) {
        stages[8].has_request = FALSE;
    }
}

void compute_mem2() {
    if (stages[7].has_request == TRUE && stages[8].has_request == FALSE) {
        stages[8].raw_instr = stages[7].raw_instr;
        stages[8].has_request = TRUE;
        stages[8].decoded_instr = stages[7].decoded_instr;
        stages[7].has_request = FALSE;
    }
}

void compute_mem1() {
    if (stages[6].has_request == TRUE && stages[7].has_request == FALSE) {
        stages[7].raw_instr = stages[6].raw_instr;
        stages[7].has_request = TRUE;
        stages[7].decoded_instr = stages[6].decoded_instr;
        stages[6].has_request = FALSE;
    }
}

void compute_ex2() {
    if (stages[5].has_request == TRUE && stages[6].has_request == FALSE) {
        stages[6].raw_instr = stages[5].raw_instr;
        stages[6].has_request = TRUE;
        stages[6].decoded_instr = stages[5].decoded_instr;
        stages[5].has_request = FALSE;
    }
}

void compute_branch() {
    if (stages[4].has_request == TRUE && stages[5].has_request == FALSE) {
        stages[5].raw_instr = stages[4].raw_instr;
        stages[5].has_request = TRUE;
        stages[5].decoded_instr = stages[4].decoded_instr;
        stages[4].has_request = FALSE;
    }
}

void compute_ex1() {
    if (stages[3].has_request == TRUE && stages[4].has_request == FALSE) {
        stages[4].raw_instr = stages[3].raw_instr;
        stages[4].has_request = TRUE;
        stages[4].decoded_instr = stages[3].decoded_instr;
        stages[3].has_request = FALSE;
    }
}

void compute_analyze() {
    if (stages[2].has_request == TRUE && stages[3].has_request == FALSE) {
        stages[3].raw_instr = stages[2].raw_instr;
        stages[3].has_request = TRUE;
        stages[3].decoded_instr = stages[2].decoded_instr;
        stages[2].has_request = FALSE;
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
        compute_analyze();
    } else if (active_stage == 3) {
        compute_ex1();
    } else if (active_stage == 4) {
        compute_branch();
    } else if (active_stage == 5) {
        compute_ex2();
    } else if (active_stage == 6) {
        compute_mem1();
    } else if (active_stage == 7) {
        compute_mem2();
    } else if (active_stage == 8) {
        compute_writeback();
    }
}

int main(int argc, char const *argv[]){
    if (argc != 2) {
        printf("Input in the form: ./simple_pipe <trace file>\n");
        return -1;
    }

    const char * filename = argv[1];

    int data_hazard_count = 0;
    int control_hazard_count = 0;
    int execution_cycles = 0;
    int completed_instructions = 0;
    int regs[16];
    for(int i = 0; i < 16; i++){
        regs[i] = 0;
    }

    // Calculate the number of instructions
    long num_instructions = calc_input_size(filename);
    if (num_instructions < 0) {
        return num_instructions;
    }
    
    // Read input
    unsigned int input_array[num_instructions];
    memset(input_array, 0, num_instructions*sizeof(unsigned int));
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
//        print_debug_statement(input_array);
        execution_cycles++;
    }

    printf("================================\n");
    printf("--------------------------------\n");
    for (int reg=0; reg<16; reg++) {
        printf("REG[%2d]   |   Value=%d  \n",reg, regs[reg]);
        printf("--------------------------------\n");
    }
    printf("================================\n\n");


    printf("Stalled cycles due to data hazard: %d \n", data_hazard_count);
    printf("Stalled cycles due to control hazard: %d \n", control_hazard_count);
    printf("\n");
    printf("Total stalls: %d \n", data_hazard_count+control_hazard_count);
    printf("Total execution cycles: %d\n", execution_cycles);
    printf("Total instruction simulated: %d\n", completed_instructions);
    printf("IPC: %f\n", ((double)completed_instructions/execution_cycles));

    return 0;
}
