#include <stdint.h>

void serialport_output(uint16_t ascii_code) {
  __asm__ volatile(".intel_syntax noprefix\n"
                   "mov dx, 0x3f8\n"
                   /* "mov al, 1\n" */
                   "out dx, al\n"
				   :
				   :"a"(ascii_code));
}

void hlt() {
  __asm__("hlt");
}

void kernel_main() {
  uint16_t i;
  uint16_t output_data[14] = {75, 69, 82, 78, 69, 76, 95, 83, 85, 67, 67, 69, 83, 83};

  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }

  while (1) 	hlt();
}

