#include <stdint.h>

#define INTEL_ASM_BEGIN ".intel_syntax noprefix\n\t"
#define INTEL_ASM_END   ".att_syntax prefix\n\t"

void serialport_output(uint8_t ascii_code) {
  __asm__ volatile(INTEL_ASM_BEGIN
                   "mov dx, 0x3f8\n\t"
                   /* "mov al, 1\n" */
                   "out dx, al\n\t"
				   INTEL_ASM_END
				   :
				   :"a"(ascii_code)/* EAXレジスタに変数の値を代入 */
				   :"%dx");		   /* clover_listでレジスタを破壊 */
}

void hlt() {
  __asm__("hlt");
}

void kernel_main() {
  int i;
  uint8_t output_data[14] = {75, 69, 82, 78, 69, 76, 95, 83, 85, 67, 67, 69, 83, 83};

  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }

  while (1) 	hlt();
}

