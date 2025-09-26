/**
 * @file device_operations.c
 * @brief Manages device-specific operations for the monitor, disk, and timer.
 *
 * This file includes functions to update the monitor's display, handle disk
 * read/write operations, and manage timer-based interrupts.
 *
 * Functions Implemented:
 * - update_monitor: Updates the monitor pixel data.
 * - handle_disk: Manages disk read/write operations.
 * - manage_disk_status: Updates the disk's status during operations.
 * - handle_timer: Handles timer-based interrupts and events.
 * - dma_read_sector: Reads a sector from the disk to memory.
 * - dma_write_sector: Writes a sector from memory to the disk.
 */

#include "simulator_functions.h"

 // Updates the monitor frame buffer with pixel data
void update_monitor(unsigned int monitor_addr, unsigned int monitor_data, unsigned char screen[MONITOR_SIZE][MONITOR_SIZE])
{
    /*
        INPUT:
        - monitor_addr: The address in the monitor memory to update.
        - monitor_data: The data to write (e.g., pixel information).
        - screen: The 2D array representing the monitor frame buffer.

        FUNCTIONALITY:
        - Updates the pixel data on the monitor based on the given address and data.
        - Simulates a grayscale monitor with 8-bit pixel intensity values.
    */

    // Extract X and Y coordinates from the monitor address
    unsigned char x = (monitor_addr >> 8) & 0xFF; // Higher 8 bits for X-coordinate
    unsigned char y = monitor_addr & 0xFF;        // Lower 8 bits for Y-coordinate

    // Validate coordinates
    if (x >= MONITOR_SIZE || y >= MONITOR_SIZE)
    {
        fprintf(stderr, "Error: Monitor address out of bounds. X: %u, Y: %u\n", x, y);
        return;
    }

    // Update the pixel at (x, y) with the lower 8 bits of monitor_data
    screen[y][x] = monitor_data & 0xFF;
    
}

// Function to manage disk operations and their timing
void handle_disk(int IOR[IOR_NUM], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int data_memory[MEM_SIZE], int* disk_timer)
{
    // Check for a new command to execute
    if (IOR[14] == 1)
    { // diskcmd == 1 (read command)
        // Perform a DMA read operation from the specified sector to the memory buffer
        dma_read_sector(IOR[15], IOR[16], data_memory, disk); // IOR[15] = sector, IOR[16] = buffer address
        IOR[17] = 1;                                    // Mark the disk as busy
        *disk_timer = 1024;                             // Set the disk service time to 1024 cycles
    }
    else if (IOR[14] == 2)
    { // diskcmd == 2 (write command)
        // Perform a DMA write operation from the memory buffer to the specified sector
        dma_write_sector(IOR[15], IOR[16], data_memory, disk); // IOR[15] = sector, IOR[16] = buffer address
        IOR[17] = 1;                                     // Mark the disk as busy
        *disk_timer = 1024;                              // Set the disk service time to 1024 cycles
    }
}

void manage_disk_status(int IOR[IOR_NUM], int* disk_timer)
{
    // Check if the disk is busy (diskstatus = 1)
    if (IOR[17] == 1)
    {
        // Decrement the disk timer
        (*disk_timer)--;

        // If the operation is complete
        if (*disk_timer == 0)
        {
            IOR[17] = 0; // Mark the disk as free (diskstatus = 0)
            IOR[14] = 0; // Reset diskcmd to 0 (no command)
            IOR[4]  = 1;  // Set irq1status to 1 to trigger the status interrupt - letting the proccessor know finished performing a read or write command.
        }
    }
}

// Handles timer-based interrupts and events.
void handle_timer_status(int IOR[IOR_NUM])
{
    // Check if the timer is enabled (timerenable = 1) -IOR that says if timer is enabled or not
    if (IOR[11] == 1)
    {
        // Increment the current timer counter (timercurrent)
        IOR[12]++;

        // Check if the current timer value has reached the maximum value (timermax)
        if (IOR[12] == IOR[13])
        {
            IOR[3] = 1;  // Set irq0status to trigger an interrupt
            IOR[12] = 0; // Reset timercurrent to zero
        }
    }
}

// Function to read a sector from the disk to the memory buffer
void dma_read_sector(unsigned int sector, unsigned int buffer_address, int data_memory[MEM_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE])
{
    // Copy all 128 words from the specified sector to the memory starting at buffer_address
    for (int i = 0; i < SECTOR_SIZE; i++)
    {
        data_memory[buffer_address + i] = disk[sector][i];
    }
}

// Function to write a sector from the memory buffer to the disk
void dma_write_sector(unsigned int sector, unsigned int buffer_address, int data_memory[MEM_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE])
{
    // Copy all 128 words from the memory starting at buffer_address to the specified sector
    for (int i = 0; i < SECTOR_SIZE; i++)
    {
        disk[sector][i] = data_memory[buffer_address + i];
    }
}
