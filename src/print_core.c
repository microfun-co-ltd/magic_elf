/*

  magic_elf - The ELF file format analyzer.

  Copyright 2009-2019 - Michael Kohn (mike@mikekohn.net)
  http://www.mikekohn.net/

  This program falls under the BSD license.

*/

#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "file_io.h"
#include "magic_elf.h"
//#include "print_program_headers.h"

static int find_program_header(elf_info_t *elf_info, uint64_t address)
{
  int count;
  int program_header = -1;
  long marker = elf_info->file_ptr;

  //uint32_t p_type;
  //uint32_t p_flags;
  //uint64_t p_offset;
  uint64_t p_vaddr;
  //uint64_t p_paddr;
  //uint64_t p_filesz;
  uint64_t p_memsz;

  for(count = 0; count < elf_info->e_phnum; count++)
  {
    elf_info->file_ptr = elf_info->e_phoff + (elf_info->e_phentsize * count);

    if (elf_info->bitwidth == 32)
    {
      elf_info->read_word(elf_info); // p_type
      elf_info->read_offset(elf_info); // p_offset
      p_vaddr = elf_info->read_addr(elf_info);
      elf_info->read_addr(elf_info); // p_addr
      elf_info->read_word(elf_info); // p_filesz
      p_memsz = elf_info->read_word(elf_info);
    }
    else
    {
      elf_info->read_word(elf_info); // p_type
      elf_info->read_word(elf_info); // p_flags
      elf_info->read_offset(elf_info); // p_offset
      p_vaddr = elf_info->read_addr(elf_info);
      elf_info->read_addr(elf_info); // p_addr
      elf_info->read_xword(elf_info); // p_filesz
      p_memsz = elf_info->read_xword(elf_info);
    }

    //printf("%d] %lx %lx %lx\n", count, address, p_vaddr, p_memsz);
    if (address >= p_vaddr && address < p_vaddr + p_memsz)
    {
      program_header = count;
      break;
    }
  }

  elf_info->file_ptr = marker;

  return program_header;
}

void print_core_siginfo(elf_info_t *elf_info)
{
  long marker = elf_info->file_ptr;
  printf("        signal_number: %d\n", elf_info->read_int32(elf_info));
  printf("           extra_code: %d\n", elf_info->read_int32(elf_info));
  printf("                errno: %d\n", elf_info->read_int32(elf_info));
  elf_info->file_ptr = marker;
}

#if 0
void print_core_regs(elf_info_t *elf_info)
{
  long marker = elf_info->file_ptr;
    printf("      R15: %016lx  R14: %016lx   R13: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("      R12: %016lx  RBP: %016lx   RBX: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("      R11: %016lx  R10: %016lx    R9: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("       R8: %016lx  RAX: %016lx   RCX: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("      RDX: %016lx  RSI: %016lx   RDI: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf(" ORIG_RAX: %016lx  RIP: %016lx    CS: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("   EFLAGS: %016lx  RSP: %016lx    SS: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("  FS_BASE: %016lx GS_BASE: %016lx    DS: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
    printf("       ES: %016lx   FS: %016lx    GS: %016lx\n",
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info),
      (uint64_t)elf_info->read_int64(elf_info));
  elf_info->file_ptr = marker;
}
#endif

void print_core_prstatus(elf_info_t *elf_info)
{
  long marker = elf_info->file_ptr;
  uint64_t tv_sec;
  uint64_t tv_usec;

  printf("        signal_number: %d\n", elf_info->read_int32(elf_info));
  printf("           extra_code: %d\n", elf_info->read_int32(elf_info));
  printf("                errno: %d\n", elf_info->read_int32(elf_info));
  printf("               cursig: %d\n", elf_info->read_int16(elf_info));
  // FIXME - only 64 bit
  elf_info->file_ptr += 2;
  printf("              sigpend: %ld\n", elf_info->read_offset(elf_info));
  printf("              sighold: %ld\n", elf_info->read_offset(elf_info));

  int pid = elf_info->read_int32(elf_info);

  printf("                  pid: %d\n", pid);
  printf("                 ppid: %d\n", elf_info->read_int32(elf_info));
  printf("                 pgrp: %d\n", elf_info->read_int32(elf_info));
  printf("                 psid: %d\n", elf_info->read_int32(elf_info));

  tv_sec = elf_info->read_offset(elf_info);
  tv_usec = elf_info->read_offset(elf_info);
  printf("            user time: %" PRId64 " %" PRId64 "\n", tv_sec, tv_usec);
  tv_sec = elf_info->read_offset(elf_info);
  tv_usec = elf_info->read_offset(elf_info);
  printf("          system time: %" PRId64 " %" PRId64 "\n", tv_sec, tv_usec);
  tv_sec = elf_info->read_offset(elf_info);
  tv_usec = elf_info->read_offset(elf_info);
  printf(" cumulative user time: %" PRId64 " %" PRId64 "\n", tv_sec, tv_usec);
  tv_sec = elf_info->read_offset(elf_info);
  tv_usec = elf_info->read_offset(elf_info);
  printf("  cumulative sys time: %" PRId64 " %" PRId64 "\n", tv_sec, tv_usec);

  //elf_info->file_ptr = marker;

  if (elf_info->e_machine == 0x3) // x86_32
  {
    if (pid == elf_info->core_search.pid)
    {
      elf_info->core_search.file_offset = elf_info->file_ptr;
    }

    uint32_t ebx = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t ecx = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t edx = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t esi = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t edi = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t ebp = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t eax = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xds = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xes = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xfs = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xgs = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t orig_eax = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t eip = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xcs = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t eflags = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t esp = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t xss = (uint32_t)elf_info->read_int32(elf_info);

    printf("      EBX: %08x  ECX: %08x    EDX: %08x  ESI: %08x\n",
      ebx, ecx, edx, esi);
    printf("      EDI: %08x  EBP: %08x    EAX: %08x  XDS: %08x\n",
      edi, ebp, eax, xds);
    printf("      XES: %08x  XFS: %08x    XGS: %08x  ORIG_EAX: %08x\n",
      xes, xfs, xgs, orig_eax);
    printf("      EIP: %08x  XCS: %08x EFLAGS: %08x  ESP: %08x\n",
      eip, xcs, eflags, esp);
    printf("      XSS: %08x\n", xss);

    int program_header = find_program_header(elf_info, eip);

    if (program_header != -1)
    {
      printf("     <program header: %d>\n", program_header);
    }
  }
  else if (elf_info->e_machine == 0x3e) // x86_64
  {
    if (pid == elf_info->core_search.pid)
    {
      elf_info->core_search.file_offset = elf_info->file_ptr;
    }

    uint64_t r15 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r14 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r13 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r12 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rbp = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rbx = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r11 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r10 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r9 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t r8 = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rax = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rcx = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rdx = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rsi = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rdi = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t orig_rax = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rip = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t cs = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t eflags = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t rsp = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t ss = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t fs_base = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t gs_base = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t ds = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t es = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t fs = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t gs = (uint64_t)elf_info->read_int64(elf_info);

    printf("      R15: %016" PRIx64 "     R14: %016" PRIx64 "   R13: %016" PRIx64 "\n",
      r15, r14, r13);
    printf("      R12: %016" PRIx64 "     RBP: %016" PRIx64 "   RBX: %016" PRIx64 "\n",
      r12, rbp, rbx);
    printf("      R11: %016" PRIx64 "     R10: %016" PRIx64 "    R9: %016" PRIx64 "\n",
      r11, r10, r9);
    printf("       R8: %016" PRIx64 "     RAX: %016" PRIx64 "   RCX: %016" PRIx64 "\n",
      r8, rax, rcx);
    printf("      RDX: %016" PRIx64 "     RSI: %016" PRIx64 "   RDI: %016" PRIx64 "\n",
      rdx, rsi, rdi);
    printf(" ORIG_RAX: %016" PRIx64 "     RIP: %016" PRIx64 "    CS: %016" PRIx64 "\n",
      orig_rax, rip, cs);
    printf("   EFLAGS: %016" PRIx64 "     RSP: %016" PRIx64 "    SS: %016" PRIx64 "\n",
      eflags, rsp, ss);
    printf("  FS_BASE: %016" PRIx64 " GS_BASE: %016" PRIx64 "    DS: %016" PRIx64 "\n",
      fs_base, gs_base, ds);
    printf("       ES: %016" PRIx64 "      FS: %016" PRIx64 "    GS: %016" PRIx64 "\n",
      es, fs, gs);

    int program_header = find_program_header(elf_info, rip);

    program_header = find_program_header(elf_info, rip);
    if (program_header != -1)
    {
      printf("     <RIP program header: %d>\n", program_header);
    }
    program_header = find_program_header(elf_info, rsp);
    if (program_header != -1)
    {
      printf("     <RSP program header: %d>\n", program_header);
    }
  }
  else if (elf_info->e_machine == 0x28) // arm32
  {
    if (pid == elf_info->core_search.pid)
    {
      elf_info->core_search.file_offset = elf_info->file_ptr;
    }

    uint32_t r0     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r1     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r2     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r3     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r4     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r5     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r6     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r7     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r8     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r9     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r10    = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r11    = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t r12    = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t sp     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t lr     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t pc     = (uint32_t)elf_info->read_int32(elf_info);
    uint32_t pstate = (uint32_t)elf_info->read_int32(elf_info);

    printf("      r0: %08x   r1: %08x    r2: %08x   r3: %08x\n", r0, r1, r2, r3);
    printf("      r4: %08x   r5: %08x    r6: %08x   r7: %08x\n", r4, r5, r6, r7);
    printf("      r8: %08x   r9: %08x   r10: %08x  r11: %08x\n", r8, r9, r10, r11);
    printf("     r12: %08x   sp: %08x    lr: %08x   pc: %08x\n", r12, sp, lr, pc);
    printf("  pstate: %08x\n", pstate);

    int program_header = find_program_header(elf_info, pc);
    if (program_header != -1)
    {
      printf("     <program header: %d>\n", program_header);
    }
  }
  else if (elf_info->e_machine == 0xb7) // aarch64
  {
    if (pid == elf_info->core_search.pid)
    {
      elf_info->core_search.file_offset = elf_info->file_ptr;
    }

    uint64_t x0     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x1     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x2     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x3     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x4     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x5     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x6     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x7     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x8     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x9     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x10    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x11    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x12    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x13    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x14    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x15    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x16    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x17    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x18    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x19    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x20    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x21    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x22    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x23    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x24    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x25    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x26    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x27    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x28    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x29    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t x30    = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t sp     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t pc     = (uint64_t)elf_info->read_int64(elf_info);
    uint64_t pstate = (uint64_t)elf_info->read_int64(elf_info);
           
    printf("      x0 : %016" PRIx64 "     x1 : %016" PRIx64 "   x2 : %016" PRIx64 "\n",  x0 , x1 , x2 );
    printf("      x3 : %016" PRIx64 "     x4 : %016" PRIx64 "   x5 : %016" PRIx64 "\n",  x3 , x4 , x5 );
    printf("      x6 : %016" PRIx64 "     x7 : %016" PRIx64 "   x8 : %016" PRIx64 "\n",  x6 , x7 , x8 );
    printf("      x9 : %016" PRIx64 "     x10: %016" PRIx64 "   x11: %016" PRIx64 "\n",  x9 , x10, x11);
    printf("      x12: %016" PRIx64 "     x13: %016" PRIx64 "   x14: %016" PRIx64 "\n",  x12, x13, x14);
    printf("      x15: %016" PRIx64 "     x16: %016" PRIx64 "   x17: %016" PRIx64 "\n",  x15, x16, x17);
    printf("      x18: %016" PRIx64 "     x19: %016" PRIx64 "   x20: %016" PRIx64 "\n",  x18, x19, x20);
    printf("      x21: %016" PRIx64 "     x22: %016" PRIx64 "   x23: %016" PRIx64 "\n",  x21, x22, x23);
    printf("      x24: %016" PRIx64 "     x25: %016" PRIx64 "   x26: %016" PRIx64 "\n",  x24, x25, x26);
    printf("      x27: %016" PRIx64 "     x28: %016" PRIx64 "   x29: %016" PRIx64 "\n",  x27, x28, x29);
    printf("      x30: %016" PRIx64 "     sp : %016" PRIx64 "   pc : %016" PRIx64 "\n",  x30, sp , pc );
    printf("   pstate: %016" PRIx64 "\n",  pstate );


    int program_header = find_program_header(elf_info, pc);
    if (program_header != -1)
    {
      printf("     <pc program header: %d>\n", program_header);
    }
    program_header = find_program_header(elf_info, sp);
    if (program_header != -1)
    {
      printf("     <sp program header: %d>\n", program_header);
    }
  }
  else
  {
    printf("    Unknown machine code: %d\n", elf_info->e_machine);
    elf_info->file_ptr = marker;
    return;
  }

  elf_info->file_ptr = marker;
}

void print_core_prpsinfo(elf_info_t *elf_info)
{
  long marker = elf_info->file_ptr;
  char filename[16];
  char args[80];
  int n;
  printf("            state: %d\n", elf_info->read_int8(elf_info));
  printf("            sname: %d\n", elf_info->read_int8(elf_info));
  printf("           zombie: %d\n", elf_info->read_int8(elf_info));
  printf("             nice: %d\n", elf_info->read_int8(elf_info));
  // FIXME - only 64 bit
  elf_info->file_ptr += 4;
  printf("             flag: %ld\n", elf_info->read_offset(elf_info));
  //printf("             flag: %d\n", elf_info->read_int32(elf_info));
  printf("              uid: %d\n", elf_info->read_int32(elf_info));
  printf("              gid: %d\n", elf_info->read_int32(elf_info));
  printf("              pid: %d\n", elf_info->read_int32(elf_info));
  printf("             ppid: %d\n", elf_info->read_int32(elf_info));
  printf("             pgrp: %d\n", elf_info->read_int32(elf_info));
  printf("              sid: 0x%x\n", elf_info->read_int32(elf_info));
  for (n = 0; n < 16; n++) { filename[n] = elf_info->read_int8(elf_info); }
  printf("         filename: '%.16s'\n", filename);
  for (n = 0; n < 80; n++) { args[n] = elf_info->read_int8(elf_info); }
  printf("             args: '%.80s'\n", args);
  elf_info->file_ptr = marker;
}

void print_core_mapped_files(elf_info_t *elf_info, int len)
{
  long marker = elf_info->file_ptr;
  long count = elf_info->read_offset(elf_info);
  char *filename = (char *)elf_info->buffer + marker;
  int n;

  printf("            count: %ld\n", count);
  printf("        page size: %ld\n", elf_info->read_offset(elf_info));

  if (elf_info->bitwidth == 32)
  {
    printf("            Page Offset   Start    End\n");
    filename += 4 * 2 + (count * 4 *3);

    for (n = 0; n < count; n++)
    {
       uint32_t page_offset = elf_info->read_int32(elf_info);
       uint32_t start = elf_info->read_int32(elf_info);
       uint32_t end = elf_info->read_int32(elf_info);
       printf("            %08x %08x %08x\n", start, end, page_offset);
       printf("            %s\n\n", filename);
       filename += strlen(filename) + 1;
    }
  }
  else
  {
    printf("            Page Offset      Start            End\n");
    filename += 8 * 2 + (count * 8 *3);

    for (n = 0; n < count; n++)
    {
       uint64_t start = elf_info->read_int64(elf_info);
       uint64_t end = elf_info->read_int64(elf_info);
       uint64_t page_offset = elf_info->read_int64(elf_info);
       printf("            %016" PRIx64 " %016" PRIx64" %016" PRIx64 "\n", page_offset, start, end);
       printf("            %s\n\n", filename);
       filename += strlen(filename) + 1;
    }
  }

  elf_info->file_ptr = marker;
}

