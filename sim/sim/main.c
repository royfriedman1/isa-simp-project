/**
 * @file main.c
 * @brief Entry point for the SIMP processor simulator.
 *
 * This file initializes the simulation by loading input files, setting up
 * memory structures, and invoking the main simulation loop.
 * Results are written to output files after the simulation completes.
 */


#include "simulator_functions.h"


int main(int argc, char* argv[])
{
    // variable definitions
    char instruction_memory[MEM_SIZE][CMD_BYTES + 1] = { "000000000000" };
    int data_memory[MEM_SIZE] = { 0 };
    int registers[REG_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    int IOR[IOR_NUM] = { 0 };
    int disk[NUMBER_OF_SECTORS][SECTOR_SIZE] = { 0 };
    unsigned char screen[MONITOR_SIZE][MONITOR_SIZE] = { 0 };
    unsigned int interupt2_events[MEM_SIZE] = { 0 };

    // clear instruction_memory
    for (int i = 0; i < MEM_SIZE; i++) {
        strcpy(instruction_memory[i], "000000000000");  
    }

    // Validate the number of arguments
    if (argc != 15) {
        fprintf(stderr, "Usage: %s imemin.txt dmemin.txt diskin.txt irq2in.txt dmemout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv - not good\n", argv[1]);
        return EXIT_FAILURE;
    }

    // File pointers for input files and output files
    FILE* dmemout_file     = fopen(argv[5], "w");
    FILE* regout_file      = fopen(argv[6], "w");
    FILE* trace_file       = fopen(argv[7], "w");
    FILE* hwregtrace_file  = fopen(argv[8], "w");
    FILE* cycles_file      = fopen(argv[9], "w");
    FILE* leds_file        = fopen(argv[10], "w");
    FILE* display7seg_file = fopen(argv[11], "w");
    FILE* diskout_file     = fopen(argv[12], "w");
    FILE* monitor_file     = fopen(argv[13], "w");
    FILE* monitor_yuv_file = fopen(argv[14], "w");

    // Check if any file failed to open
    if (!dmemout_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[5]);
    }
    if (!regout_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[6]);
    }
    if (!trace_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[7]);
    }
    if (!hwregtrace_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[8]);
    }
    if (!cycles_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[9]);
    }
    if (!leds_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[10]);
    }
    if (!display7seg_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[11]);
    }
    if (!diskout_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[12]);
    }
    if (!monitor_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[13]);
    }
    if (!monitor_yuv_file) {
        fprintf(stderr, "Error: Failed to open file: %s\n", argv[14]);
    }

    // Load initial data
    load_instruction_memory(argv[1], instruction_memory);
    load_data_memory(argv[2], data_memory);
    load_disk_contents(argv[3], disk);
    load_irq2_events(argv[4], interupt2_events);


    // Pointer output files array
    FILE* output_files[] = {dmemout_file, regout_file, trace_file, hwregtrace_file, cycles_file, leds_file, display7seg_file, diskout_file, monitor_file, monitor_yuv_file};

    simulate(output_files, IOR, registers, data_memory, screen, disk, instruction_memory, interupt2_events);

    return EXIT_SUCCESS;

}
