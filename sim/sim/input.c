/**
 * @file input.c
 * @brief Handles loading data from input files into memory.
 *
 * This file includes functions to read initial states for the instruction memory,
 * data memory, disk contents, and IRQ2 events.
 *
 * Functions:
 * - load_instruction_memory: Loads the instruction memory from a file.
 * - load_data_memory: Loads the data memory from a file.
 * - load_disk_contents: Loads the disk contents from a file.
 * - load_irq2_events: Loads IRQ2 event timings from a file.
 */

#include "simulator_functions.h"

 // Load instruction memory from imemin.txt
void load_instruction_memory(char* filename, char instruction_memory[MEM_SIZE][13])
{
    int LINE_SIZE = 13;
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    int line = 0;
    
    while (fgets(instruction_memory[line], LINE_SIZE, file) != NULL) {
        // remove \n in the line
        size_t len = strlen(instruction_memory[line]);
        if (len > 0 && instruction_memory[line][len - 1] == '\n') {
            instruction_memory[line][len - 1] = '\0';
        }
        // if line empty -> skip
        if (strlen(instruction_memory[line]) == 0) {
            continue;
        }
        line++;
    }
}
    

// Load data memory from dmemin.txt
void load_data_memory(char* filename, int data_memory[MEM_SIZE])
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    for (int i = 0; i < MEM_SIZE; i++)
    {
        if (fscanf(file, "%8x", &data_memory[i]) != 1)
        {
            data_memory[i] = 0; // Default to zero
        }
    }
    fclose(file);
}

// Load disk contents from diskin.txt into a two-dimensional array
void load_disk_contents(char* filename, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE])
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    for (int sector = 0; sector < NUMBER_OF_SECTORS; sector++)
    {
        for (int word = 0; word < SECTOR_SIZE; word++)
        {
            if (fscanf(file, "%8x", &disk[sector][word]) != 1)
            {
                disk[sector][word] = 0; // Default to zero
            }
        }
    }

    fclose(file);
}

// Load IRQ2 events from irq2in.txt
int load_irq2_events(char* filename, unsigned int irq2_events[])
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }
    int count = 0;
    while (count < MAX_IRQ2_EVENTS && fscanf(file, "%u", &irq2_events[count]) == 1)
    {
        count++;
    }
    fclose(file);
    return count;
}