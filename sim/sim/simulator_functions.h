#ifndef SIMULATOR_FUNCTIONS_H
#define SIMULATOR_FUNCTIONS_H

// Disable security warnings for standard C functions
#define _CRT_SECURE_NO_WARNINGS

// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// General configuration
#define REG_NUM 16
#define CMD_BYTES 12
#define MEM_SIZE 4096
#define IOR_NUM 23
#define MONITOR_SIZE 256
#define MAX_IRQ2_EVENTS 4096
#define SECTOR_SIZE 128
#define NUMBER_OF_SECTORS 128
#define MAX_DISK_ENTRIES (NUMBER_OF_SECTORS * SECTOR_SIZE)

// Opcodes
#define ADD 0
#define SUB 1
#define MAC 2
#define AND 3
#define OR 4
#define XOR 5
#define SLL 6
#define SRA 7
#define SRL 8
#define BEQ 9
#define BNE 10
#define BLT 11
#define BGT 12
#define BLE 13
#define BGE 14
#define JAL 15
#define LW 16
#define SW 17
#define RETI 18
#define IN 19
#define OUT 20
#define HALT 21

// Masks
#define MASK_12_BIT 0xFFF

// Structs
typedef struct
{
    long opcode; // Operation code (opcode)
    int rd;      // Destination register
    int rs;      // Source register 1
    int rt;      // Source register 2
    int rm;      // Source register 3
    int imm1;    // Immediate value 1
    int imm2;    // Immediate value 2
} instruction_decode;

/*
// Global variables
extern char instruction_memory[MEM_SIZE][CMD_BYTES + 1];
extern int data_memory[MEM_SIZE];
extern int registers[REG_NUM];
extern unsigned char screen[MONITOR_SIZE][MONITOR_SIZE];
extern int disk[NUMBER_OF_SECTORS][SECTOR_SIZE];
extern unsigned int irq2_events[MAX_IRQ2_EVENTS];
extern int IOR[IOR_NUM];
extern int irq2_event_count;

*/



// Function declarations
 

//////////////////////////////
//  Input File Functions   //
/////////////////////////////

void load_instruction_memory(char* filename, char instruction_memory[MEM_SIZE][13]);
// Loads instruction memory from a file.
void load_data_memory(char* filename, int data_memory[MEM_SIZE]);
// Loads data memory from a file.
void load_disk_contents(char* filename, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]);
// Loads disk contents from a file.
int load_irq2_events(char* filename, unsigned int irq2_events[]);
// Loads IRQ2 events from a file.


////////////////////////////////
///  Output File Functions  ///
///////////////////////////////

void write_data_memory(FILE* file, int data_memory[MEM_SIZE]);
// Writes the data memory to a file.
void write_registers(FILE* file, int registers[REG_NUM]);
// Writes the values of registers R3-R15 to a file.
void write_cycle_count(FILE* file, unsigned int cycle_count);
// Writes the total number of executed cycles to a file.
void write_disk_contents(FILE* file, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]);
// Writes the disk contents to a file.
void write_monitor_pixels(FILE* file, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE]);
// Writes the monitor pixel data to a text file.
void write_monitor_yuv(FILE* file, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE]);
// Writes the monitor pixel data to a binary YUV file.


/////////////////////////////////
///    Logging Functions   /////
////////////////////////////////

void log_trace(FILE* file, unsigned int pc, const char* instruction, int registers[16]);
// Logs the executed instructions trace.
void log_hw_register(FILE* file, unsigned int cycle, const char* action, const char* reg_name, char* data);// Logs hardware register interactions.
void log_led_change(FILE* file, unsigned int cycle, int led_status);
// Logs changes to the LED state.
void log_display_change(FILE* file, unsigned int cycle, int display_status);
// Logs changes to the 7-segment display state.


//////////////////////////////////
///   Simulation Functions  /////
////////////////////////////////

void simulate(FILE* output_files[], int IOR[23], int registers[REG_NUM], int data_memory[MEM_SIZE], unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], char instruction_memory[MEM_SIZE][CMD_BYTES + 1], unsigned int interupt2_events[MEM_SIZE]);// Runs the simulation of the fetch-decode-execute loop.
const char* fetch_instruction(const char instruction_memory[MEM_SIZE][CMD_BYTES + 1], int* PC);
// Fetches the next instruction from memory.
void decode_instruction(const char* instruction, instruction_decode* decoded_instruction, int registers[]);
// Decodes an instruction into its components.
int str_hex_2_bin(const char* array_of_char, int start_index, int slice_width);
// Converts a portion of a hex string to an integer.


//////////////////////////////////
////  Execution Functions  //////
////////////////////////////////

void execute_instruction(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC, int data_memory[MEM_SIZE],
    int IOR[23], unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], FILE* hwregtrace_file, FILE* leds_file, FILE* display7seg_file, FILE* monitor_file, int* disk_timer, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]);
// Executes a single decoded instruction.
void arithmetic_operation(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC);
// Performs arithmetic operations (e.g., ADD, SUB).
void comparison_operation(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC);
// Handles comparison and branching operations.
void load_store_operation(instruction_decode* decoded_instruction, int data_memory[MEM_SIZE], int register_array[REG_NUM], int* PC);
// Executes memory load and store operations.
void IO_operation(instruction_decode* instruction, int data_memory[MEM_SIZE], FILE* hwregtrace, FILE* leds, FILE* display7seg, FILE* monitor, int* PC, int register_array[REG_NUM], int IOR[],
    unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int* disk_timer);
// Executes I/O instructions (e.g., IN, OUT).


///////////////////////////////////////////
////  Device Management Functions  //////
/////////////////////////////////////////

void update_monitor(unsigned int monitor_addr, unsigned int monitor_data, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE]);
// Updates a specific pixel in the monitor's frame buffer.
void handle_disk(int IOR[IOR_NUM], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int data_memory[MEM_SIZE], int* disk_timer);
// Manages disk read/write operations.
void manage_disk_status(int IOR[IOR_NUM], int* disk_timer);
// Updates the status of the disk.
void handle_timer_status(int IOR[IOR_NUM]);
// Handles timer-based interrupts and events.
void dma_read_sector(unsigned int sector, unsigned int buffer_address, int data_memory[MEM_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]);
// Reads a sector from the disk into memory using DMA.
void dma_write_sector(unsigned int sector, unsigned int buffer_address, int data_memory[MEM_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE]);
// Writes a sector from memory to the disk using DMA.
void handle_interrupts(int* PC, int IOR[IOR_NUM], FILE* hwregtrace_file);

#endif // SIMULATOR_FUNCTIONS_H

