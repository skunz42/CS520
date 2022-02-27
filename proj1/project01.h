#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bakery_time;
int no_request;
int bagel_baked;
int baguette_baked;
int baking_count;
float performance;

enum RequestType {
    NO_REQUEST=0,
    BAKE_BAGEL=1,
    BAKE_BAGUETTE=2,
};

typedef struct {
    int minutes_until_free;
    int stage_num;
    int request_type;

    int wait_queue[1000000];
    int in_queue;
} Stage;
