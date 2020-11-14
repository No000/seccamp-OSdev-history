#include <stdio.h>
#include <stdlib.h>

#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long

#define Elf64_Addr uint64_t
#define Elf64_Off  uint64_t

#define EI_NIDENT 16

typedef struct elfheader {
    unsigned char e_ident [EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct pheader {
    uint32_t p_type;
    uint32_t p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;


void print_pheader(Elf64_Phdr *ELF_programheader) {
    int p_type;
    static int index = 1;
    p_type = ELF_programheader->p_type;
    
    if (p_type != 1) {
        return;
    }
    printf("\n");
    printf("VirAddr:\t%16x");
    index = index + 1;
    print_pheader(ELF_programheader+1);

}

int main(int argc, char argv[]) {
    FILE *elf_file;
    unsigned char *buff;
    Elf64_Ehdr *ELF_header;
    Elf64_Phdr *ELF_programheader;

    // バッファーの確保
    buff = malloc(sizeof(char)*10240);
    elf_file = fopen(argv[1], "rb");

    printf("%s", argv[1]);
    if (elf_file == NULL) {
        printf("err\n");
        return 0;
    }

    // buffにelf_fileのファイル内容を展開する
    fread(buff, 1, 8192, elf_file);
    ELF_header = buff;

    // ELFヘッダ
    printf("エントリ:\t%16x\n", ELF_header->e_entry);
    ELF_programheader = buff + ELF_header->e_phoff;
    
    print_pheader(ELF_programheader);
    // ファイルとじー
    fclose(elf_file);

    return 1;
}
