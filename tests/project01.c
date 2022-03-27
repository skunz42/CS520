#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

enum RequestType {
    NO_REQUEST=0,
    BAKE_BAGEL=1,
    BAKE_BAGUETTE=2,
};

typedef struct {
    int stage_num;
    int request;
    int has_request;
} Stage;

Stage stages[NUM_STAGES];
int start_wait;
int baking_stage_count;
int one_k_instructions;
int times_stalled;

int temp_bagel;
int temp_baguette;
int real_instructions_processed;

int BAKING_STALLED;

const char *stage_names[12] = {"scaling", "mixing", "fermentation", "folding", "dividing", "rounding",
    "resting", "shaping", "proofing", "baking", "cooling", "stocking"};

const char *request_names[4] = {"No_Request", "Bake_Bagel", "Bake_Baguette", "Bake_Baguette"};

void print_stages() {
    printf("current bakery time: %d\n", bakery_time);
    for (int i = NUM_STAGES-1; i >= 0; i--) {
        if (stages[i].request != NO_REQUEST || (stages[i].request == NO_REQUEST && i == 0)) {
            printf("[%d] %s %s\n", i+1, stage_names[i], request_names[stages[i].request]);
        }

        if (i == 9 && BAKING_STALLED) {
            printf("[10] baking stall\n");
        }
    }
    printf("\n");
}

int all_queues_empty() {
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].has_request == TRUE) {
            return FALSE;
        }
    }
    return TRUE;
}

// INITITALIZATION FUNCTIONS

void initialize_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        stages[i].stage_num = i;
        stages[i].request = NO_REQUEST;
        stages[i].has_request = FALSE;
    }
}

int read_input(const char * filename, int *input_array, int array_length) {
    FILE *fp;
    int line_num = 0;
    int line_length = 255;
    char line[line_length];

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return -1;
    }

    while (fgets(line, line_length, fp)) {
        if (strcmp(line, "No-Request\n") == 0) {
            input_array[line_num] = NO_REQUEST;
            no_request++;
        } else if (strcmp(line, "Bake-Bagel\n") == 0) {
            input_array[line_num] = BAKE_BAGEL;
            bagel_baked++;
            baking_count++;
        } else if (strcmp(line, "Bake-Baguette\n") == 0) {
            input_array[line_num] = BAKE_BAGUETTE;
            baguette_baked++;
            baking_count++;
        }
        line_num++;
    }

    fclose(fp);
    return 0;
}

int count_lines(const char * filename) {
    int num_lines;
    FILE *fp;
    char c;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return -1;
    }

    for (c = getc(fp); c != EOF; c = getc(fp)) {
        if (c == '\n') {
            num_lines++;
        }
    }

    fclose(fp);
    return num_lines;
}

void compute_stage11() {
    if (stages[11].has_request = TRUE) {
        stages[11].has_request = FALSE;
        if (stages[11].request == 1) {
            temp_bagel++;
        } else if (stages[11].request == 2 || stages[11].request == 3) {
            temp_baguette++;
        }
        stages[11].request = 0;
    }
}

void compute_stage10() {
    if (stages[10].has_request == TRUE) {
        stages[11].request = stages[10].request;
        stages[11].has_request = TRUE;
        stages[10].has_request = FALSE;
    }
}

// baking
void compute_stage9() {
    //TODO something wrong here
    if (baking_stage_count == 10) {
        baking_stage_count = 0;
        if (stages[9].request == NO_REQUEST) {
            stages[10].request = stages[9].request;
            stages[10].has_request = TRUE;
            stages[9].request = FALSE;
        }
        BAKING_STALLED = TRUE;
        return;
    }
    if (stages[9].has_request == TRUE) {
        if (stages[9].request == BAKE_BAGUETTE) {
            stages[9].request = 3;
//            printf("B");
        } else {
            stages[10].request = stages[9].request;
            stages[10].has_request = TRUE;
            stages[9].has_request = FALSE;
            if (stages[9].request != NO_REQUEST) {
                baking_stage_count++;
            }
        }
    }
}

void compute_intermediate_stages(int active_stage) {
    if ((stages[active_stage].has_request == TRUE && stages[active_stage+1].has_request == FALSE) ||
            (stages[active_stage].has_request == TRUE && stages[active_stage+1].has_request == TRUE && stages[active_stage+1].request == NO_REQUEST)) {
        
        stages[active_stage+1].request = stages[active_stage].request;
        stages[active_stage+1].has_request = TRUE;
        stages[active_stage].has_request = FALSE;
        stages[active_stage].request = NO_REQUEST;
    }
}

void compute_stage1(int * instructions_processed, int * input_array, int instruction_count) {
    // throw to next stage
    if ((stages[0].has_request == TRUE && stages[1].has_request == FALSE) ||
            (stages[0].has_request == TRUE && stages[1].has_request == TRUE && stages[1].request == NO_REQUEST)) {
        
        stages[1].request = stages[0].request;
        stages[1].has_request = TRUE;
        stages[0].has_request = FALSE;
        stages[0].request = NO_REQUEST;
    }
    
    if (one_k_instructions == 1000 || start_wait > 0) {
        if (one_k_instructions == 1000) start_wait = 10;
        one_k_instructions = 0;
        start_wait--;
        if (input_array[*instructions_processed] == NO_REQUEST) {
            stages[0].has_request = TRUE;
            *instructions_processed = *instructions_processed + 1;
        }
        return;
    }

    if ((*instructions_processed < instruction_count && stages[0].has_request == FALSE) ||
            (*instructions_processed < instruction_count && stages[0].has_request == TRUE && stages[0].request == NO_REQUEST)) {
        stages[0].request = input_array[*instructions_processed];
        stages[0].has_request = TRUE;
        if (stages[0].request != NO_REQUEST) {
            one_k_instructions++;
        }
        *instructions_processed = *instructions_processed + 1;
    }
    
}

void exec_pipeline(int active_stage, int * instructions_processed, int * input_array, int instruction_count) {
    if (active_stage == 0) {
        compute_stage1(instructions_processed, input_array, instruction_count);
    } else if (active_stage == 10) {
        compute_stage10();
    } else if (active_stage == 9) {
        compute_stage9();
    } else if (active_stage == 11) {
        compute_stage11();
    } else {
        compute_intermediate_stages(active_stage);
    }
}

int main(int argc, char *argv[])  {
    if (argc != 2) {
        printf("Input in the form: ./baking_sim <trace_filename>\n");
        return -1;
    }

    const char * filename = argv[1];

    int instruction_count = count_lines(filename);
    if (instruction_count < 0) {
        return -1;
    }

    no_request = 0;
    bagel_baked = 0;
    baguette_baked = 0;
    
    int input_array[instruction_count];
    read_input(filename, input_array, instruction_count);

    initialize_stages();

    bakery_time = 0;
    int instructions_processed = 0;
    start_wait = 0;
    baking_count = 0;
    times_stalled = 0;

    temp_bagel = 0;
    temp_baguette = 0;
    real_instructions_processed = 0;
    one_k_instructions = 0;

    while (instructions_processed < instruction_count || !all_queues_empty()) {
        //if (instructions_processed == 1100) break;
        for (int i = NUM_STAGES-1; i >= 0; i--) {
            exec_pipeline(i, &instructions_processed, input_array, instruction_count);
        }
        //print_stages();
        bakery_time++;
        BAKING_STALLED = FALSE;
    }

    //TODO Fix stuff left in pipe after while loop ends

    /*int remainder = 0;
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].request == 0) {
            remainder += 1;
        }
    }

    printf("%d\n", temp_bagel+remainder);*/
    //printf("%d\t%d\n", temp_bagel, temp_baguette);
    //bakery_time -= 2;

    baking_count = bagel_baked + baguette_baked;
    //output formats
    printf("\nBaking count: %d\n", baking_count);
    printf(" - Bagel baked: %d\n", bagel_baked);
    printf(" - Baguette baked: %d\n", baguette_baked);
    printf("No request: %d\n", no_request);

    printf("\nHow many minutes to bake: %d\n", bakery_time);

    performance = (baking_count * 1.0) / (bakery_time * 1.0);
    printf("\nPerformance (bakes/minutes): %.2f\n", performance);

    return 0;
}

