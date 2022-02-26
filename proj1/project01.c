#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

Stage stages[NUM_STAGES];
//int stream_ended;
//int stream_ended_index;
int reached_baking_stage;

void print_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        printf("%d: %d\t", i, stages[i].has_request);
    }
    printf("\n");
}

// INITITALIZATION FUNCTIONS

void initialize_stages() {
    for (int i = 0; i < NUM_STAGES; i++) {
        stages[i].minutes_until_free = 0;
        stages[i].stage_num = i;
        stages[i].has_request = FALSE;
        stages[i].request_type = INVALID_REQUEST;
        stages[i].temp_request_type = INVALID_REQUEST;
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
        } else {
            input_array[line_num] = INVALID_REQUEST;
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

void compute_stocking() {
    if (stages[11].has_request == TRUE) {
        stages[11].has_request = FALSE;
        stages[11].minutes_until_free = 1;
        stages[11].request_type = stages[11].temp_request_type;
        /*if (stages[11].request_type > 0) {
            stages[11].has_request = TRUE;
        } else {
            stages[11].has_request = FALSE;
        }*/
    }
    /*if (!stages[11].has_request) {
        stages[11].request_type = stages[11].temp_request_type;
    }*/
    stages[11].minutes_until_free--;
}

void compute_baking() {
    if (stages[9].has_request == TRUE && stages[9].minutes_until_free <= 1 && stages[10].has_request == FALSE) {
        stages[10].has_request = TRUE;
        stages[10].minutes_until_free = 1;
        stages[10].temp_request_type = stages[9].request_type;
        //stages[9].has_request = FALSE;
        stages[9].request_type = stages[9].temp_request_type;
        if (stages[9].request_type > 0) {
            stages[9].has_request = TRUE;
        } else {
            stages[9].has_request = FALSE;
        }
        stages[9].minutes_until_free = stages[9].temp_request_type;
        reached_baking_stage++;
    }
    /*if (stages[9].has_request == FALSE) {
        stages[9].request_type = stages[9].temp_request_type;
        stages[9].has_request = TRUE;
    }*/
    if (reached_baking_stage == 10) {
        stages[9].minutes_until_free--;
        reached_baking_stage = 1;
    }
}
        
void compute_intermediate_step(int stage_num) {
    // pass old
    printf("IF STATEMNT: %d\t%d\t%d\n", stages[stage_num].has_request == TRUE, stages[stage_num].minutes_until_free <= 1, stages[stage_num+1].has_request == FALSE);
    if (stages[stage_num].has_request == TRUE && stages[stage_num].minutes_until_free <= 1 && stages[stage_num+1].has_request == FALSE) {
        stages[stage_num+1].has_request = TRUE;
        stages[stage_num+1].minutes_until_free = 1;
        stages[stage_num+1].temp_request_type = stages[stage_num].request_type;
        //stages[stage_num].has_request = FALSE;
        stages[stage_num].request_type = stages[stage_num].temp_request_type;
        if (stages[stage_num].request_type > 0) {
            stages[stage_num].has_request = TRUE;
        } else {
            stages[stage_num].has_request = FALSE;
        }
        stages[stage_num].minutes_until_free = 1;
    }
    // get new
    /*if (stages[stage_num].has_request == FALSE) {
        stages[stage_num].request_type = stages[stage_num].temp_request_type;
        stages[stage_num].has_request = TRUE;
    }*/
    stages[stage_num].minutes_until_free--;
}

void compute_scaling(int *program_counter, int instruction_count, int *input_array) {
    // pass old
    if (stages[0].has_request == TRUE && stages[0].minutes_until_free <= 1 && stages[1].has_request == FALSE) {
        stages[1].has_request = TRUE;
        stages[1].minutes_until_free = 1;
        stages[1].temp_request_type = stages[0].request_type;
        stages[0].has_request = FALSE;
        stages[0].minutes_until_free = 1;
        stages[0].request_type = INVALID_REQUEST;
    }
    // get new
    if (*program_counter < instruction_count && stages[0].has_request == FALSE && stages[0].minutes_until_free <= 1) {
        stages[0].has_request = TRUE;
        stages[0].request_type = input_array[*program_counter];
        *program_counter = *program_counter + 1;
        if ((*program_counter % 999) == 0 && *program_counter != 0) {
            stages[0].minutes_until_free = 11;
        } else {
            stages[0].minutes_until_free = 1;
        }
    }
    stages[0].minutes_until_free--;
}

void compute_stage(int stage_num, int * program_counter, int instruction_count, int *input_array) {
    switch (stage_num) {
        case 11:
            compute_stocking();
            break;
        case 9:
            compute_baking();
            break;
        case 0:
            compute_scaling(program_counter, instruction_count, input_array);
            break;
        default:
            compute_intermediate_step(stage_num);
            break;
    }
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

    int instructions_processed = 0; // which instruction im at
    bakery_time = 0;
    reached_baking_stage = 1;

    // Instructions left or still instructions in the pipeline
    // TODO finish pipe
    while (instructions_processed < instruction_count) {
        if (bakery_time == 20) break;
        /*if (instructions_processed >= instruction_count) {
            //printf("%d\n", stream_ended_index);
            break;
        }*/
        for (int i = NUM_STAGES-1; i >= 0; i--) {
            compute_stage(i, &instructions_processed, instruction_count, input_array);
        }
        bakery_time++;
        //printf("%d\n", instructions_processed);
        printf("%d\t%d\t\t", instructions_processed, bakery_time);
        print_stages();
    }

    instructions_processed++;

    //output formats
    printf("\nBaking count: %d\n", baking_count);
    printf(" - Bagel baked: %d\n", bagel_baked);
    printf(" - Baguette baked: %d\n", baguette_baked);
    printf("No request: %d\n", no_request);

    printf("\nHow many minutes to bake: %d\n", bakery_time);

    printf("\nPerformance (bakes/minutes): %.2f\n", performance);

    return 0;
}
