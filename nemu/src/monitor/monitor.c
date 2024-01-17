/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
//pa2.2
//#include <elf.h>// already in isa.h
#include <stdio.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

/*
//get symtab
static size_t get_sym(Elf64_Sym** elf_sym_ents,Elf64_Shdr elf_sym,FILE *elf){
  *elf_sym_ents=(Elf64_Sym*) malloc (elf_sym.sh_size);
  rewind(elf);
  fseek(elf,elf_sym.sh_offset,SEEK_SET);
  int entry_num=elf_sym.sh_size/elf_sym.sh_entsize;
  if (elf_sym.sh_size%elf_sym.sh_entsize!=0){printf("the number of entries is not int!!\n "); exit(0);}
  size_t ret=fread(*elf_sym_ents,elf_sym.sh_entsize,entry_num,elf);
  if(ret!=entry_num){printf("wrong when read Elf_symtab\n");exit(0);}
  return ret;
  
}

//get and load func in strtab
static void load_str_func(size_t n,Elf64_Sym* elf_sym_ents,Elf64_Shdr elf_str,FILE *elf){
  rewind(elf);
  fseek(elf,elf_str.sh_offset,SEEK_SET);
  uint32_t elf_func_idx=0;
  Elf64_Off sh_offset=elf_str.sh_offset;
  for(int i=0;i<n;i++){
    if(ELF64_ST_TYPE(elf_sym_ents[i].st_info)==STT_FUNC){
     elf_func[elf_func_idx].func_start=elf_sym_ents[i].st_value;
     elf_func[elf_func_idx].size=elf_sym_ents[i].st_size;
     elf_func[elf_func_idx].func_end=elf_func[elf_func_idx].func_start+elf_func[elf_func_idx].size;
     Elf64_Off offset=elf_sym_ents[i].st_name;
     rewind(elf);
     fseek(elf,sh_offset+offset,SEEK_SET);
     size_t ret=fscanf(elf, "%63s", elf_func[elf_func_idx].func_name);
     //printf("ret:%lu\n",ret);
     if(ret!=1){printf("wrong when load elf funcs\n");exit(0);}
    // printf("elf_func[%u].func_name:%s\n",elf_func_idx,elf_func[elf_func_idx].func_name);
     elf_func_idx++;
     }
  }
  return;
}


//get target sec (.symtab and strtab)
static void get_target_sec(FILE *elf){
  // get elf_head 
  rewind(elf);
  //Elf32_Ehdr elf_header;
  //size_t ret=fread(&elf_header,1,sizeof(Elf32_Ehdr),elf);
  //if(ret!=sizeof(Elf32_Ehdr)){exit(0);}
  Elf64_Ehdr elf_header;
  int ret=fread(&elf_header,1,sizeof(Elf64_Ehdr),elf);
  if(ret!=sizeof(Elf64_Ehdr)){exit(0);}
  
  printf("elf_header.e_shnum:%u\n",elf_header.e_shnum);//num of entry
  printf("elf_header.e_shentsize:%u\n",elf_header.e_shentsize);//size of each entey
  
  //get symtab and strtab
  // get symtab 
  Elf64_Sym* elf_sym_ents;
  size_t n;
  rewind(elf);
  fseek(elf,elf_header.e_shoff,SEEK_SET);//point to the first tab
  // need to be improved, consider the length of tabs
  while(1){
    Elf64_Shdr elf_entry;
    int ret=fread(&elf_entry,1,sizeof(Elf64_Shdr),elf);
    if(ret!=sizeof(Elf64_Shdr)){printf("wrong when read elf--symtab\n");exit(0);}
    if(elf_entry.sh_type==SHT_SYMTAB){
      printf("find .symtab\n");
      printf("symtab.size:%lu\n",elf_entry.sh_size);
      n=get_sym(&elf_sym_ents,elf_entry,elf);// get entries in symtab from sec into elf_sym_ents
      break;
    }
  }
  // get strtab 
  rewind(elf);
  fseek(elf,elf_header.e_shoff,SEEK_SET);
  Elf64_Off shs_offset=elf_header.e_shoff+elf_header.e_shentsize*elf_header.e_shstrndx;
  for(int i=0;i<elf_header.e_shnum;i++)
  {
     Elf64_Shdr elf_entry;
     size_t ret=fread(&elf_entry,1,sizeof(Elf64_Shdr),elf);
     if(ret!=sizeof(Elf64_Shdr)){printf("Wrong when get strtab\n");exit(0);}
     if(elf_entry.sh_type==SHT_STRTAB && elf_entry.sh_offset!=shs_offset){
        printf("find .strtab\n");
        printf("strtab.size:%lu\n",elf_entry.sh_size);
        load_str_func(n,elf_sym_ents,elf_entry,elf);
        break;
     } 
  }
}
*/

//Elf32 success! 12.17
#ifdef CONFIG_FTRACE
//get symtab
static size_t get_sym(Elf32_Sym** elf_sym_ents,Elf32_Shdr elf_sym,FILE *elf){
  *elf_sym_ents=(Elf32_Sym*) malloc (elf_sym.sh_size);
  rewind(elf);
  fseek(elf,elf_sym.sh_offset,SEEK_SET);
  int entry_num=elf_sym.sh_size/elf_sym.sh_entsize;
  if (elf_sym.sh_size%elf_sym.sh_entsize!=0){printf("the number of entries is not int!!\n "); exit(0);}
  size_t ret=fread(*elf_sym_ents,elf_sym.sh_entsize,entry_num,elf);
  if(ret!=entry_num){printf("wrong when read Elf_symtab\n");exit(0);}
  return ret;
  
}

//get and load func in strtab
static void load_str_func(size_t n,Elf32_Sym* elf_sym_ents,Elf32_Shdr elf_str,FILE *elf){
  rewind(elf);
  fseek(elf,elf_str.sh_offset,SEEK_SET);
  uint32_t elf_func_idx=0;
  Elf32_Off sh_offset=elf_str.sh_offset;
  for(int i=0;i<n;i++){
    if(ELF32_ST_TYPE(elf_sym_ents[i].st_info)==STT_FUNC){
     elf_func[elf_func_idx].func_start=elf_sym_ents[i].st_value;
     elf_func[elf_func_idx].size=elf_sym_ents[i].st_size;
     elf_func[elf_func_idx].func_end=elf_func[elf_func_idx].func_start+elf_func[elf_func_idx].size-4;
     Elf32_Off offset=elf_sym_ents[i].st_name;
     rewind(elf);
     fseek(elf,sh_offset+offset,SEEK_SET);
     size_t ret=fscanf(elf, "%63s", elf_func[elf_func_idx].func_name);
     //printf("ret:%lu\n",ret);
     if(ret!=1){printf("wrong when load elf funcs\n");exit(0);}
     //printf("elf_func[%u].func_name:%s\n",elf_func_idx,elf_func[elf_func_idx].func_name);
     //printf("elf_func[%u].func_start:%x, .size:%lx, .end:%x\n",elf_func_idx,elf_func[elf_func_idx].func_start,elf_func[elf_func_idx].size,elf_func[elf_func_idx].func_end);
     elf_func_idx++;
     }
  }
  return;
}


//get target sec (.symtab and strtab)
static void get_target_sec(FILE *elf){
  // get elf_head 
  rewind(elf);
  Elf32_Ehdr elf_header;
  size_t ret=fread(&elf_header,1,sizeof(Elf32_Ehdr),elf);
  if(ret!=sizeof(Elf32_Ehdr)){exit(0);}
  //Elf64_Ehdr elf_header;
  //int ret=fread(&elf_header,1,sizeof(Elf64_Ehdr),elf);
  //if(ret!=sizeof(Elf64_Ehdr)){exit(0);}
  
  //printf("elf_header.e_shnum:%u\n",elf_header.e_shnum);//num of entry
  //printf("elf_header.e_shentsize:%u\n",elf_header.e_shentsize);//size of each entey
  
  //get symtab and strtab
  // get symtab 
  Elf32_Sym* elf_sym_ents;
  size_t n;
  rewind(elf);
  fseek(elf,elf_header.e_shoff,SEEK_SET);//point to the first tab
  // need to be improved, consider the length of tabs
  while(1){
    Elf32_Shdr elf_entry;
    int ret=fread(&elf_entry,1,sizeof(Elf32_Shdr),elf);
    if(ret!=sizeof(Elf32_Shdr)){printf("wrong when read elf--symtab\n");exit(0);}
    if(elf_entry.sh_type==SHT_SYMTAB){
     // printf("find .symtab\n");
     //printf("symtab.size:%u\n",elf_entry.sh_size);
      n=get_sym(&elf_sym_ents,elf_entry,elf);// get entries in symtab from sec into elf_sym_ents
      break;
    }
  }
  // get strtab 
  rewind(elf);
  fseek(elf,elf_header.e_shoff,SEEK_SET);
  Elf32_Off shs_offset=elf_header.e_shoff+elf_header.e_shentsize*elf_header.e_shstrndx;
  for(int i=0;i<elf_header.e_shnum;i++)
  {
     Elf32_Shdr elf_entry;
     size_t ret=fread(&elf_entry,1,sizeof(Elf32_Shdr),elf);
     if(ret!=sizeof(Elf32_Shdr)){printf("Wrong when get strtab\n");exit(0);}
     if(elf_entry.sh_type==SHT_STRTAB && elf_entry.sh_offset!=shs_offset){
        //printf("find .strtab\n");
        //printf("strtab.size:%u\n",elf_entry.sh_size);
        load_str_func(n,elf_sym_ents,elf_entry,elf);
        break;
     } 
  }
}
#endif
static int parse_args(int argc, char *argv[]) {
/*
#ifdef CONFIG_FTRACE
//读取elf文件
  //printf("argv[0]:%s\n",argv[0]);
  //printf("argv[1]:%s\n",argv[1]);
  //printf("argv[2]:%s\n",argv[2]);
  //printf("argv[3]:%s\n",argv[3]);
  //FILE *elf=fopen(argv[0],"rb");
  //this way --> terrible!!!!!!!!!
  //FILE *elf=fopen("/home/pagexia/ics2023/am-kernels/tests/cpu-tests/build/mersenne-riscv32-nemu.elf","rb");
  
  if (NULL == elf)
  {
    printf("fail to open the file\n");
    exit(0);
  }
  get_target_sec(elf);
  fclose(elf);
#endif
*/
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {"elf"      , required_argument, NULL, 'e'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  //char *optarg;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
#ifdef CONFIG_FTRACE
      case 'e': 
        FILE *elf=fopen(optarg);
        if (NULL == elf)
       {
         printf("fail to open the file\n");
         exit(0);
       }
       get_target_sec(elf);
       fclose(elf);
       break;
#endif
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}



void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */
  
  /* init ftrace. */

#ifdef CONFIG_FTRACE
  init_ftrace();
  //printf("monitor init_ftrace()\n");
  //printf("elf_func[0].start:%u\n",elf_func[0].func_start);
#endif
  
  
  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

#ifndef CONFIG_ISA_loongarch32r
  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv,
      MUXDEF(CONFIG_RV64,      "riscv64",
                               "riscv32"),
                               "bad"))) "-pc-linux-gnu"
  ));
#endif

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
