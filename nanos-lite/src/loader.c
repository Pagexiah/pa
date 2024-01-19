#include <proc.h>
#include <elf.h>
//pa3
#include<fs.h>
//
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  int fd=fs_open(filename,0,0);
  if(fd<3) panic("wrong fd loader\n");
  //size_t ramdisk_offfset
  Elf_Ehdr head;
  //ramdisk_read(&head,0,sizeof(Elf_Ehdr));
  assert(fs_read(fd,&head,sizeof(Elf_Ehdr))==sizeof(Elf_Ehdr));
  if(*(uint32_t *)head.e_ident!=0x464c457f) {printf("Not Elf\n");assert(0);}
  Elf_Phdr phdr;
  //ramdisk_read(phdr,head.e_phoff,head.e_phnum*sizeof(Elf_Phdr));
  for(int i=0;i<head.e_phnum;i++){
    fs_lseek(fd,head.e_phoff+i*head.e_phentsize,0);
    assert(fs_read(fd,&phdr,sizeof(Elf_Phdr))==sizeof(Elf_Phdr));
    if(phdr.p_type==PT_LOAD){
      fs_lseek(fd,phdr.p_offset,0);
      assert(fs_read(fd,(void *)phdr.p_vaddr,phdr.p_memsz)==phdr.p_memsz);
      memset((void*)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    }
  }
  assert(fs_close(fd)==0);
  return head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

