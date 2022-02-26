#include "project01.h"

#define FALSE 0
#define TRUE 1
#define NUM_STAGES 12

Stage stages[NUM_STAGES];

int all_stages_clear() {
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
        stages[i].minutes_until_free = 0;
        stages[i].stage_num = i;
        stages[i].has_request = FALSE;
        stages[i].request_type = INVALID_REQUEST;
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

//TODO condense these

// Stage 11
void compute_stocking() {
    stages[11].minutes_until_free--;
    if (stages[11].has_request && stages[11].minutes_until_free <= 0) {
        stages[11].has_request = FALSE;
        stages[11].request_type = INVALID_REQUEST;
    }
}

// Stage 10
void compute_cooling() {
    stages[10].minutes_until_free--;
    if (!stages[11].has_request && stages[10].minutes_until_free <= 0) {
        stages[11].has_request = TRUE;
        stages[11].minutes_until_free = 1;
        stages[10].has_request = FALSE;
    }
}

// Stage 9
void compute_baking() {
    stages[9].minutes_until_free--;
    if (!stages[10].has_request && stages[9].minutes_until_free <= 0) {
        stages[10].has_request = TRUE;
        stages[10].minutes_until_free = 1;
        stages[9].has_request = FALSE;
    }
}

// Stage 8
void compute_proofing() {
    stages[8].minutes_until_free--;
    if (!stages[9].has_request && stages[8].minutes_until_free <= 0) {
        stages[9].has_request = TRUE;
        if (stages[9].request_type == BAKE_BAGUETTE) {
            stages[9].minutes_until_free = BAKE_BAGUETTE;
        } else if (stages[9].request_type == BAKE_BAGEL) {
            stages[9].minutes_until_free = BAKE_BAGEL;
        }
        stages[8].has_request = FALSE;
    }
}

// Stage 7
void compute_shaping() {
    stages[7].minutes_until_free--;
    if (!stages[8].has_request && stages[7].minutes_until_free <= 0) {
        stages[8].has_request = TRUE;
        stages[8].minutes_until_free = 1;
        stages[7].has_request = FALSE;
    }
}


void compute_stage(int stage_num) {
    switch (stage_num) {
        case 11:
            compute_stocking();
            break;
        case 10:
            compute_cooling();
            break;
        case 9:
            compute_baking();
            break;
        case 8:
            compute_proofing();
            break;
        default:
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

    int program_counter = 0;
    // Instructions left or still instructions in the pipeline
    while (program_counter < instruction_count || !all_stages_clear()) {
        if (program_counter >= instruction_count && !all_stages_clear()) {
            printf("Still some in pipe\n");
            break;
        }
        // TODO get instruction
        // TODO stage 11
        // TODO stage 10
        // TODO ...
        for (int i = NUM_STAGES; i >= 0; i--) {
            compute_stage(i);
        }
        program_counter++;
    }

    //output formats
    printf("\nBaking count: %d\n", baking_count);
    printf(" - Bagel baked: %d\n", bagel_baked);
    printf(" - Baguette baked: %d\n", baguette_baked);
    printf("No request: %d\n", no_request);

    /*printf("\nHow many minutes to bake: %d\n", bakery_time);

    printf("\nPerformance (bakes/minutes): %.2f\n", performance);*/

    return 0;
}
