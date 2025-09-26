/**
 * @file output.c
 * @brief Handles writing simulation results to output files.
 *
 * This file includes functions to write the final states of memory, registers,
 * and hardware devices (e.g., LEDs, 7-segment display, monitor).
 *
 * Functions:
 * - write_data_memory: Writes the contents of the data memory to a file.
 * - write_registers: Writes the values of registers R3-R15 to a file.
 * - write_cycle_count: Writes the total cycle count to a file.
 * - write_disk_contents: Writes the disk contents to a file.
 * - write_monitor_pixels: Writes the monitor's pixel data to a text file.
 * - write_monitor_yuv: Writes the monitor's pixel data to a binary YUV file.
 * - log_trace: Logs a trace of executed instructions.
 * - log_hw_register: Logs hardware register interactions.
 * - log_led_change: Logs changes to the LED state.
 * - log_display_change: Logs changes to the 7-segment display state.
 */


#include "simulator_functions.h"

 // Writes the contents of the data memory to dmemout.txt
void write_data_memory(FILE* file, int data_memory[MEM_SIZE])
{
    // Write each memory value to the file in hexadecimal format
    for (int i = 0; i < MEM_SIZE; i++)
    {
        fprintf(file, "%08X\n", data_memory[i]); // Each value is written as an 8-character hex number

        
    }

}

// Writes the values of registers R3-R15 to regout.txt
void write_registers(FILE* file, int registers[REG_NUM])
{
    // Write registers R3 to R15 to the file
    for (int i = 3; i < 16; i++)
    {
        fprintf(file, "%08X\n", registers[i]); // Format: 8-character hexadecimal per line
    }

}

// Writes the total clock cycles to cycles.txt
void write_cycle_count(FILE* file, unsigned int cycle_count)
{

    // Write the cycle count to the file
    fprintf(file, "%u\n", cycle_count);

}

// Logs LED state changes to leds.txt
void log_led_change(FILE* file, unsigned int cycle, int led_status)
{
    // Ensure the file pointer is valid
    if (!file)
    {
        perror("Invalid file pointer for leds.txt");
        return;
    }

    // Write the log entry to the file
    fprintf(file, "%u %08X\n", cycle, led_status);
}

// Logs 7-segment display changes to display7seg.txt
void log_display_change(FILE* file, unsigned int cycle, int display_status)
{
    // Ensure the file pointer is valid
    if (!file)
    {
        perror("Invalid file pointer for display7seg.txt");
        return;
    }

    // Write the log entry to the file
    fprintf(file, "%u %08X\n", cycle, display_status);
}

// Writes the disk contents to diskout.txt
void write_disk_contents(FILE* file, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]) {
    if (!file) {
        perror("Invalid file pointer");
        return;
    }
    int last_x = -1;
    int last_y = -1;
    for (int sector = 0; sector < NUMBER_OF_SECTORS; sector++) {
        for (int line = 0; line < SECTOR_SIZE; line++) {
            if (disk[sector][line] != 0) {
                last_x = sector;
                last_y = line;

            }
        }
    }
    // Iterate through the disk contents and write to the file
    for (int sector = 0; sector < NUMBER_OF_SECTORS; sector++) {
        for (int line = 0; line < SECTOR_SIZE; line++) {
            if (last_x == -1 && last_y == -1) {
                break;
            }
            fprintf(file, "%08X\n", disk[sector][line]);
            if (sector == last_x && line == last_y) {
                break;
            }
            
        }
        if (last_x == -1) {
            break;
        }
        if (sector == last_x) {
            break;
        }
        
    }
}

// Writes the pixel data to monitor.txt
void write_monitor_pixels(FILE* file, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE])
{
    int last_x = -1;
    int last_y = -1;
    for (int i = 0; i < MONITOR_SIZE; i++)
    {
        for (int j = 0; j < MONITOR_SIZE; j++)
        {
            if (screen[i][j] != 0) {
                last_x = i;
                last_y = j;
                
            }
        }
    }

    for (int i = 0; i < MONITOR_SIZE; i++)
    {
        for (int j = 0; j < MONITOR_SIZE; j++)
        {
            if (last_x == -1 && last_y == -1) {
                break;
            }
            fprintf(file, "%02X\n", screen[i][j]); // Write each pixel as a 2-character hex value
            if (i == last_x && j == last_y) {
                break;
            }
        }
        if (last_x == -1) {
            break;
        }
        if (i == last_x) {
            break;
        }
       
    }
}

// Writes the pixel data in binary format to monitor.yuv
void write_monitor_yuv(FILE* file, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE])
{
    for (int i = 0; i < MONITOR_SIZE; i++)
    {
        fwrite(screen[i], sizeof(unsigned char), MONITOR_SIZE, file); // Write each row as binary data
    }
}

// Logs execution trace to trace.txt - need to open the file for writing when use!!
void log_trace(FILE* file, unsigned int pc, const char* instruction, int registers[REG_NUM])
{
    // Ensure the file pointer and instruction are valid
    if (!file || !instruction)
    {
        perror("Invalid file pointer or instruction");
        return;
    }

    // Log the PC and instruction
    fprintf(file, "%03X %s ", pc, instruction); // PC in 3-digit hex, instruction as is
    
    // Log all register values (R0 to R15)
    for (int i = 0; i <16; i++)
    {
        fprintf(file, "%08X ", registers[i]); // Each register in 8-digit hex
    }
    

    // End the line
    fprintf(file, "\n");
}

// Logs hardware register interactions to hwregtrace.txt - need to open the file for writing when use!!
void log_hw_register(FILE* file, unsigned int cycle, const char* action, const char* reg_name, char* data)
{
    // Ensure the file pointer and strings are valid
    if (!file || !action || !reg_name)
    {
        perror("Invalid file pointer or strings");
        return;
    }

    // Write the log entry to the file
    fprintf(file, "%u %s %s %s\n", cycle, action, reg_name, data);
}


