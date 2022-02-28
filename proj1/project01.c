#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

Stage stages[NUM_STAGES];
int start_wait;
int baking_stage_count;
int times_stalled;

int temp_bagel;

void print_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        printf("%d: %d\t", i, stages[i].request);
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
        }
        stages[11].request = -1;
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
//    if (baking_stage_count == 9) {
//        baking_stage_count = 0;
//        stages[9].stalled = TRUE;
//        return;
//    }
    if (stages[9].has_request == TRUE) {
        if (stages[9].request == BAKE_BAGUETTE) {
            stages[9].stalled = TRUE;
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
    if (stages[active_stage].has_request == TRUE && stages[active_stage+1].has_request == FALSE && stages[active_stage+1].stalled == FALSE) {
        stages[active_stage+1].request = stages[active_stage].request;
//        if (stages[active_stage].request != NO_REQUEST) {
            stages[active_stage+1].has_request = TRUE;
//        } else {
//            stages[active_stage+1].has_request = FALSE;
//        }
        stages[active_stage].has_request = FALSE;
//    } else if (stages[active_stage].in_queue == 0 && stages[active_stage+1].in_queue == 0) {
//        stages[active_stage+1].wait_queue[0] = 0;
    } else if (stages[active_stage+1].stalled == TRUE) {
        stages[active_stage].stalled = TRUE;
    }
}

void compute_stage1(int * instructions_processed, int * input_array, int instruction_count) {
    // throw to next stage
    if (stages[0].has_request == TRUE && stages[1].has_request == FALSE && stages[1].stalled == FALSE) {
        stages[1].request = stages[0].request;
//        if (stages[0].request != NO_REQUEST) {
            stages[1].has_request = TRUE;
//        } else {
//            stages[1].has_request = FALSE;
//        }
        stages[0].has_request = FALSE;
//    } else if (stages[0].in_queue == 0 && stages[1].in_queue == 0) {
//        stages[1].wait_queue[0] = 0;
    } else if (stages[1].stalled == TRUE) {
        stages[0].stalled = TRUE;
    }

//    if (bakery_time%1000 == 999) {
//        start_wait = 10;
//    }
    // TODO fix this
//    if (start_wait == 1) stages[0].in_queue = 0;
//    if (start_wait > 0) {
//        start_wait--;
//        printf("C");
//        stages[0].wait_queue[0] = 0;
//        stages[0].in_queue++;
//    } else if (*instructions_processed < instruction_count && stages[0].in_queue == 0) {
    if (*instructions_processed < instruction_count && stages[0].has_request == FALSE && stages[0].stalled == FALSE) {
//        if (input_array[*instructions_processed] != 0) {
            stages[0].request = input_array[*instructions_processed];
//            if (stages[0].request != NO_REQUEST) {
                stages[0].has_request = TRUE;
//            } else {
//                stages[0].has_request = FALSE;
//            }
//        }
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

    while (instructions_processed < instruction_count || !all_queues_empty()) {
        //if (instructions_processed == 1100) break;
        for (int i = NUM_STAGES-1; i >= 0; i--) {
            exec_pipeline(i, &instructions_processed, input_array, instruction_count);
        }
        for (int i = 0; i < NUM_STAGES; i++) {
            stages[i].stalled = FALSE;
        }
        //print_stages();
        bakery_time++;
    }

    //TODO Fix stuff left in pipe after while loop ends

    /*int remainder = 0;
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].request == 0) {
            remainder += 1;
        }
    }

    printf("%d\n", temp_bagel+remainder);*/
    printf("%d\n", temp_bagel);

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
