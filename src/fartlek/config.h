#pragma once

extern int fartlek_step_time;
extern int fartlek_max_time;
extern int fartlek_rest_time;
extern int fartlek_rounds;
extern bool fartlek_extended_rest;
extern int fartlek_extended_rest_time;
extern int fartlek_extended_rest_rounds;

void fartlek_read_persistent();
void fartlek_write_persistent();