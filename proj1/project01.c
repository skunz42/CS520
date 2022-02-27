#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

Stage stages[NUM_STAGES];
int start_wait;
int baking_stage_count;

void print_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        printf("%d: %d\t", i, stages[i].in_queue);
    }
    printf("\n");
}

int all_queues_empty() {
    for (int i = 0; i < NUM_STAGES; i++) {
        if (stages[i].in_queue > 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// INITITALIZATION FUNCTIONS

void initialize_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        stages[i].minutes_until_free = 0;
        stages[i].stage_num = i;
        stages[i].request_type = NO_REQUEST;
        stages[i].in_queue = 0;
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
    if (stages[11].in_queue > 0) {
        for (int i = 0; i < stages[11].in_queue - 1; i++) {
            stages[11].wait_queue[i] = stages[11].wait_queue[i+1];
        }
        stages[11].in_queue--;
    }
}

// baking
void compute_stage9() {
    if (baking_stage_count == 9) {
        baking_stage_count = 0;
    } else if (stages[9].in_queue > 0) {
        if (stages[9].wait_queue[0] == BAKE_BAGUETTE) {
            stages[9].wait_queue[0]--;
        } else {
            stages[10].wait_queue[stages[10].in_queue] = stages[9].wait_queue[0];
            stages[10].in_queue++;
            for (int i = 0; i < stages[9].in_queue - 1; i++) {
                stages[9].wait_queue[i] = stages[9].wait_queue[i+1];
            }
            stages[9].in_queue--;
            baking_stage_count++;
        }
    }
}

void compute_intermediate_stages(int active_stage) {
    if (stages[active_stage].in_queue > 0) {
        stages[active_stage+1].wait_queue[stages[active_stage+1].in_queue] = stages[active_stage].wait_queue[0];
        stages[active_stage+1].in_queue++;
        for (int i = 0; i < stages[active_stage].in_queue - 1; i++) {
            stages[active_stage].wait_queue[i] = stages[active_stage].wait_queue[i+1];
        }
        stages[active_stage].in_queue--;
    }
}

void compute_stage1(int * instructions_processed, int * input_array, int instruction_count) {
    // throw to next stage
    if (stages[0].in_queue > 0) {
        stages[1].wait_queue[stages[1].in_queue] = stages[0].wait_queue[0];
        stages[1].in_queue++;
        for (int i = 0; i < stages[0].in_queue-1; i++) {
            stages[0].wait_queue[i] = stages[0].wait_queue[i+1];
        }
        stages[0].in_queue--;
    }

    //TODO wait 10 min every 1k
    // get new value from input
    if (bakery_time%1000 == 999) {
        start_wait = 10;
        //printf("%d\n", bakery_time);
    }
    if (start_wait > 0) {
        start_wait--;
    } else if (*instructions_processed < instruction_count) {
        stages[0].wait_queue[stages[0].in_queue] = input_array[*instructions_processed];
        stages[0].in_queue++;
        *instructions_processed = *instructions_processed + 1;
    }
}

void exec_pipeline(int active_stage, int * instructions_processed, int * input_array, int instruction_count) {
    if (active_stage == 0) {
        compute_stage1(instructions_processed, input_array, instruction_count);
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

    // Instructions left or still instructions in the pipeline
    // TODO finish pipe
    while (instructions_processed < instruction_count || !all_queues_empty()) {
        // for each stage
        // if i == 0, call initial function. if something in wait queue, pass to next level. get a new input and place in queue
        // intermediate, take from queue and toss up
        // baking, take from queue or wait
        // TODO: make queue system work at basic level
        //if (instructions_processed == 400000) break;
        for (int i = 0; i < NUM_STAGES; i++) {
            exec_pipeline(i, &instructions_processed, input_array, instruction_count);
        }
        /*for (int i = 0; i < stages[0].in_queue; i++) {
            printf("%d\t", stages[0].wait_queue[i]);
        }
        printf("\n");*/
        //print_stages();
        bakery_time++;
    }

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
