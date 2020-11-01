#include <stdint.h>
#include <stdarg.h>
#include "keyboard_map.h"
#define ENTER_KEY_CODE 0x1C
#define DELETE_KEY_CODE 0x08
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES
#define LINES 25
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */
#pragma once
//extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
unsigned int current_loc = 0;
extern char read_port(unsigned short port);
char *vidptr = (char*)0xb8000;
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("out %0, %1" : : "a"(value), "Nd"(port) :);
}
void EmeraldDevices_keyboard_Keyboard_init(void);
void EmeraldPIC_sendEOI(unsigned char irq);
void EmeraldDevices_keyboard_Keyboard_handler_main(void *nothing);

