#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

Stage stages[NUM_STAGES];

void print_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        printf("%d: %d\t", i, stages[i].request_type);
    }
    printf("\n");
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

// COMPUTE
void compute_stage1(int * instructions_processed, int * input_array, int instruction_count) {
    // get new value from input
    if (*instructions_processed < instruction_count) {
        stages[0].wait_queue[stages[0].in_queue] = input_array[*instructions_processed];
        stages[0].in_queue++;
        *instructions_processed = *instructions_processed + 1;
    }
}

void exec_pipeline(int active_stage, int * instructions_processed, int * input_array, int instruction_count) {
    if (active_stage == 0) {
        compute_stage1(instructions_processed, input_array, instruction_count);
    }
    //TODO
}

int main(int argc, char *argv[])  {
    if (argc != 2) {
        printf("Input in the form: ./baking_sim <trace_filename>\n");
        return -1;
    }

    const char * filename = argv[1];

    int instruction_count = count_lines(filename);
    if (baking_count < 0) {
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

    // Instructions left or still instructions in the pipeline
    // TODO finish pipe
    while (instructions_processed < instruction_count) {
        // for each stage
        // if i == 0, call initial function. if something in wait queue, pass to next level. get a new input and place in queue
        // intermediate, take from queue and toss up
        // baking, take from queue or wait
        // TODO: make queue system work at basic level
        if (instructions_processed == 20) break;
        for (int i = 0; i < NUM_STAGES; i++) {
            exec_pipeline(i, &instructions_processed, input_array, instruction_count);
        }
        for (int i = 0; i < stages[0].in_queue; i++) {
            printf("%d\t", stages[0].wait_queue[i]);
        }
        printf("\n");
        bakery_time++;
    }

    //output formats
    printf("\nBaking count: %d\n", baking_count);
    printf(" - Bagel baked: %d\n", bagel_baked);
    printf(" - Baguette baked: %d\n", baguette_baked);
    printf("No request: %d\n", no_request);

    printf("\nHow many minutes to bake: %d\n", bakery_time);

    printf("\nPerformance (bakes/minutes): %.2f\n", performance);

    return 0;
}
