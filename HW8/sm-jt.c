#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int           reg[8];
unsigned char mem[10000*4];
int           pc = -1;
unsigned char insOpCode;
unsigned char insOp0;
unsigned char insOp1;
unsigned char insOp2;
char          insOpImm;
int           insOpExt;

void fetch() {
  insOpCode = mem [pc] >> 4;
  insOp0    = mem [pc] & 0xf;
  insOp1    = mem [pc+1] >> 4;
  insOp2    = mem [pc+1] & 0xf;
  insOpImm  = mem [pc+1];
  pc += 2;
  if(insOpCode == 0xb || insOpCode == 0x0) {
    insOpExt = mem [pc] << 24 | mem [pc+1] << 16 | mem [pc+2] << 8 | mem [pc+3];
    pc += 4;
  }
}

int exec() {
  int cont = 1;
  int addr, val;

  static const void* jt_insOpCode[16] = {&&C0,&&C1,&&C2,&&C3,&&C4,&&DEFAULT,&&C6,&&C7,
                                        &&C8,&&C9,&&CA,&&CB,&&CC,&&DEFAULT,&&DEFAULT,&&CF};

  static const void* jt_insOp0[16] = {&&C60, &&C61, &&C62, &&C63, &&C64, &&C65, &&C66, &&C67,
                                        &&C6DEFAULT, &&C6DEFAULT, &&C6DEFAULT, &&C6DEFAULT,
                                        &&C6DEFAULT, &&C6DEFAULT, &&C6DEFAULT, &&C6F};
  if(insOpCode > 0xf || insOpCode < 0x0) goto DEFAULT;
  goto *jt_insOpCode[insOpCode];
  C0: reg [insOp0] = insOpExt;
      goto CONT;
  C1: addr = (insOp0 << 2) + reg [insOp1];
      reg [insOp2] = mem [addr] << 24 | mem [addr+1] << 16 | mem [addr+2] << 8 | mem [addr+3];
      goto CONT;
  C2: addr = reg [insOp0] + (reg [insOp1] << 2);
      reg [insOp2] = mem [addr] << 24 | mem [addr+1] << 16 | mem [addr+2] << 8 | mem [addr+3];
      goto CONT;
  C3: addr = (insOp1 << 2) + reg [insOp2];
      val  = reg [insOp0];
      mem [addr]   = val >> 24 & 0xff;
      mem [addr+1] = val >> 16 & 0xff;
      mem [addr+2] = val >>  8 & 0xff;
      mem [addr+3] = val       & 0xff;
      goto CONT;
  C4: addr = reg [insOp1] + (reg [insOp2] << 2);
      val  = reg [insOp0];
      mem [addr]   = val >> 24 & 0xff;
      mem [addr+1] = val >> 16 & 0xff;
      mem [addr+2] = val >>  8 & 0xff;
      mem [addr+3] = val       & 0xff;
      goto CONT;
  C6: if(insOp0 > 0xf || insOp0 < 0x0) goto C6DEFAULT;
      goto *jt_insOp0[insOp0];
      C60: reg [insOp2] = reg [insOp1];
            goto CONT;
      C61: reg [insOp2] = reg [insOp1] + reg [insOp2];
            goto CONT;
      C62: reg [insOp2] = reg [insOp1] & reg [insOp2];
            goto CONT;
      C63: reg [insOp2] = reg [insOp2] + 1;
            goto CONT;
      C64: reg [insOp2] = reg [insOp2] + 4;
           goto CONT;
      C65: reg [insOp2] = reg [insOp2] - 1;
            goto CONT;
      C66: reg [insOp2] = reg [insOp2] -4;
            goto CONT;
      C67: reg [insOp2] = ~ reg [insOp2];
            goto CONT;
      C6F: reg [insOp2] = pc + (insOp1 << 1);
           goto CONT;
      C6DEFAULT: printf ("Illegal ALU instruction: pc=0x%x fun=0x%x\n", pc, insOp0);
                goto CONT;
  C7: if (insOpImm > 0)
          reg [insOp0] = reg [insOp0] << insOpImm;
      else
          reg [insOp0] = reg [insOp0] >> -insOpImm;
      goto CONT;

  C8: pc += insOpImm << 1;
      goto CONT;
  C9: if (reg [insOp0] == 0)
          pc += insOpImm << 1;
      goto CONT;
  CA: if (reg [insOp0] > 0)
          pc += insOpImm << 1;
      goto CONT;
  CB:  pc = insOpExt;
      goto CONT;
  CC: pc = (((unsigned short) insOpImm) << 1) + reg [insOp0];
      goto CONT;
  CF: if (insOp0 == 0)
          cont = 0;
        else if (insOp0 == 1) {
          // sys i                           f1ii
          if (insOpImm == 0) {
            // sys read
            reg[0] = read(reg[0], &mem[reg[1]], reg[2]);
          } else if (insOpImm == 1) {
            // sys write
            reg[0] = write(reg[0], &mem[reg[1]], reg[2]);
          } else if (insOpImm == 2) {
            // sys exec
            char *unsafe = getenv("SIMPLE_MACHINE_ALLOW_EXEC");
            if(unsafe && strcmp(unsafe, "1") == 0) {
              int size = reg[1];
              char *str = malloc(size + 1);
              memcpy(str, &mem[reg[0]], size);
              str[size] = 0;
              reg[0] = system(str);
              free(str);
            } else {
              printf("<<<WOULD EXECUTE %.*s>>>\n", reg[1], &mem[reg[0]]);
            }
          }
        }
      goto CONT;

  DEFAULT: 
    printf ("Illegal  instruction: pc=0x%x opCode=0x%x\n", pc, insOpCode);
    goto CONT;
  CONT:
    return cont;
}

int loadFile (char* filename) {
  char*  line = NULL;
  size_t len  = 0;
  FILE* f = fopen (filename, "r");
  if (f == NULL)
    return 0;
  while (getline (&line, &len, f) > 0) {
    char* sep = line;
    while (*sep != ':')
      sep++;
    *sep = 0;
    int addr = strtol (line, 0, 16);
    if (pc < 0)
      pc = addr;
    line = sep + 1;
    char v[3] = {0,0,0};
    while (1) {
      while (*line && *line == ' ')
        line ++;
      if (*line == 0 || *(line+1) == 0)
        break;
      v[0] = line[0];
      v[1] = line[1];
      mem [addr] = strtol (v, 0, 16);
      line += 2;
      addr += 1;
    }
    line = NULL;
  }
  fclose (f);
  return 1;
}

void showMem (int start, int count) {
  int i;
  for (i=0; i<count*4; i+=4)
    printf ("0x%08x: %02x %02x %02x %02x\n", start+i, mem [start+i], mem [start+i+1], mem [start+i+2], mem [start+i+3]);
}

void compute() {
  int cont = 0;
  do {
    fetch();
    cont = exec();
  } while (cont);
}

struct showMemList {
  int  len;
  int* addr;
  int* count;
} showMemList;

int showReg;

int parseArgs (int argc, char** argv) {
  if (argc < 2)
    return 0;
  for (int i=1; i<argc; i++)
    if (argv[i][0] == '-') {
      char* op = argv[i] + 1;
      if (strcmp (op, "p") == 0) {
        i += 1;
        pc = strtol (argv[i], 0, 16);
      } else if (strcmp (op, "m") == 0) {
        i += 1;
        char* sep = argv[i];
        while (*sep !=':')
          sep++;
        *sep = 0;
        showMemList.len++;
        showMemList.addr = realloc (showMemList.addr, showMemList.len * sizeof (*showMemList.addr));
        showMemList.addr [showMemList.len-1] = strtol (argv[i], 0, 16);
        showMemList.count = realloc (showMemList.count, showMemList.len * sizeof (*showMemList.count));
        showMemList.count [showMemList.len-1] = strtol (sep+1, 0, 16);
      } else if (strcmp (op, "r") == 0)
        showReg = 1;
      else
        return 0;
    } else if (i != argc-1)
      return 0;
  return 1;
}

int main (int argc, char** argv) {
  if (! parseArgs (argc, argv)) {
    fprintf (stderr, "usage: sm [-p startPC] [-m addr:count]* [-r] filename\n");
    return EXIT_FAILURE;
  }
  int ok = loadFile (argv[argc-1]);
  if (! ok) {
    fprintf (stderr, "error reading input file\n");
    return EXIT_FAILURE;
  }
  compute ();
  for (int i=0; i<showMemList.len; i++)
    showMem (showMemList.addr[i], showMemList.count[i]);
  if (showReg)
    for (int i=0; i<8; i++)
      printf ("r%d: 0x%08x (%d)\n", i, reg[i], reg[i]);
}
