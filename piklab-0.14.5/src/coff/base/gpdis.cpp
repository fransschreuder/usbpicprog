/* Disassemble memory
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <assert.h>
#include "devices/pic/base/pic.h"
#include "coff.h"
#include "gpopcode.h"

#define DECODE_ARG0 snprintf(buffer, sizeof_buffer, "%s", instruction->name)

#define DECODE_ARG1(ARG1) snprintf(buffer, sizeof_buffer, "%s\t%#lx", \
                                  instruction->name,\
                                  ARG1)

#define DECODE_ARG1WF(ARG1, ARG2) snprintf(buffer, sizeof_buffer, "%s\t%#lx, %s", \
                                          instruction->name,\
                                          ARG1, \
                                          (ARG2 ? "f" : "w"))

#define DECODE_ARG2(ARG1, ARG2) snprintf(buffer, sizeof_buffer, "%s\t%#lx, %#lx", \
                                        instruction->name,\
                                        ARG1, \
                                        ARG2)

#define DECODE_ARG3(ARG1, ARG2, ARG3) snprintf(buffer, sizeof_buffer, "%s\t%#lx, %#lx, %#lx", \
                                             instruction->name,\
                                             ARG1, \
                                             ARG2, \
                                             ARG3)

bool gp_decode_mnemonics = false;
bool gp_decode_extended = false;

int Coff::disassemble(long int opcode, long int opcode2,
               int org,
               Pic::Architecture architecture,
               char *buffer,
               size_t sizeof_buffer)
{
  int i;
  int value;
  struct insn *instruction = NULL;
  int num_words = 1;

  switch (architecture) {
  case Pic::Arch_30X:
    snprintf(buffer, sizeof_buffer, "unsupported processor class");
    return 0;
  case Pic::Arch_10X:
    for(i = 0; i < num_op_12c5xx; i++) {
      if((op_12c5xx[i].mask & opcode) == op_12c5xx[i].opcode) {
        instruction = &op_12c5xx[i];
        break;
      }
    }
    break;
/*  case PROC_CLASS_SX:
    for(i = 0; i < num_op_sx; i++) {
      if((op_sx[i].mask & opcode) == op_sx[i].opcode) {
        instruction = &op_sx[i];
        break;
      }
    }
    break;
*/
  case Pic::Arch_16X:
    for(i = 0; i < num_op_16cxx; i++) {
      if((op_16cxx[i].mask & opcode) == op_16cxx[i].opcode) {
        instruction = &op_16cxx[i];
        break;
      }
    }
    break;
  case Pic::Arch_17X:
    for(i = 0; i < num_op_17cxx; i++) {
      if((op_17cxx[i].mask & opcode) == op_17cxx[i].opcode) {
        instruction = &op_17cxx[i];
        break;
      }
    }
    break;
  case Pic::Arch_18C:
  case Pic::Arch_18F:
  case Pic::Arch_18J:
    if (gp_decode_mnemonics) {
      for(i = 0; i < num_op_18cxx_sp; i++) {
        if((op_18cxx_sp[i].mask & opcode) == op_18cxx_sp[i].opcode) {
          instruction = &op_18cxx_sp[i];
          break;
        }
      }
    }
    if (instruction == NULL) {
      for(i = 0; i < num_op_18cxx; i++) {
        if((op_18cxx[i].mask & opcode) == op_18cxx[i].opcode) {
          instruction = &op_18cxx[i];
          break;
        }
      }
    }
    if ((instruction == NULL) && (gp_decode_extended)) {
      /* might be from the extended instruction set */
      for(i = 0; i < num_op_18cxx_ext; i++) {
        if((op_18cxx_ext[i].mask & opcode) == op_18cxx_ext[i].opcode) {
          instruction = &op_18cxx_ext[i];
          break;
        }
      }
    }
    break;
  default:
    assert(0);
  }

  if (instruction == NULL)  {
    snprintf(buffer, sizeof_buffer, "dw\t%#lx  ;unknown opcode", opcode);
    return num_words;
  }

  switch (instruction->classType)
    {
    case INSN_CLASS_LIT3_BANK:
      DECODE_ARG1((opcode & 0x7) << 5);
      break;
    case INSN_CLASS_LIT3_PAGE:
      DECODE_ARG1((opcode & 0x7) << 9);
      break;
    case INSN_CLASS_LIT1:
      DECODE_ARG1(opcode & 1);
      break;
    case INSN_CLASS_LIT4:
      DECODE_ARG1(opcode & 0xf);
      break;
    case INSN_CLASS_LIT4S:
      DECODE_ARG1((opcode & 0xf0) >> 4);
      break;
    case INSN_CLASS_LIT6:
      DECODE_ARG1(opcode & 0x3f);
      break;
    case INSN_CLASS_LIT8:
    case INSN_CLASS_LIT8C12:
    case INSN_CLASS_LIT8C16:
      DECODE_ARG1(opcode & 0xff);
      break;
    case INSN_CLASS_LIT9:
      DECODE_ARG1(opcode & 0x1ff);
      break;
    case INSN_CLASS_LIT11:
      DECODE_ARG1(opcode & 0x7ff);
      break;
    case INSN_CLASS_LIT13:
      DECODE_ARG1(opcode & 0x1fff);
      break;
    case INSN_CLASS_LITFSR:
      DECODE_ARG2(((opcode >> 6) & 0x3), (opcode & 0x3f));
      break;
    case INSN_CLASS_RBRA8:
      value = opcode & 0xff;
      /* twos complement number */
      if (value & 0x80) {
        value = -((value ^ 0xff) + 1);
      }
      DECODE_ARG1((unsigned long)(org + value + 1) * 2);
      break;
    case INSN_CLASS_RBRA11:
      value = opcode  & 0x7ff;
      /* twos complement number */
      if (value & 0x400) {
        value = -((value ^ 0x7ff) + 1);
      }
      DECODE_ARG1((unsigned long)(org + value + 1) * 2);
      break;
    case INSN_CLASS_LIT20:
      {
        long int dest;

        num_words = 2;
        dest = (opcode2 & 0xfff) << 8;
        dest |= opcode & 0xff;
        DECODE_ARG1(dest * 2);
      }
      break;
    case INSN_CLASS_CALL20:
      {
        long int dest;

        num_words = 2;
        dest = (opcode2 & 0xfff) << 8;
        dest |= opcode & 0xff;
        snprintf(buffer, sizeof_buffer, "%s\t%#lx, %#lx",
                instruction->name,
                dest * 2,
                (opcode >> 8) & 1);
      }
      break;
    case INSN_CLASS_FLIT12:
      {
        long int k;
        long int file;

        num_words = 2;
        k = opcode2 & 0xff;
        k |= ((opcode & 0xf) << 8);
        file = (opcode >> 4) & 0x3;
        DECODE_ARG2(file, k);
      }
      break;
    case INSN_CLASS_FF:
      {
        long int file1;
        long int file2;

        num_words = 2;
        file1 = opcode & 0xfff;
        file2 = opcode2 & 0xfff;
        DECODE_ARG2(file1, file2);
      }
      break;
    case INSN_CLASS_FP:
      DECODE_ARG2((opcode & 0xff), ((opcode >> 8) & 0x1f));
      break;
    case INSN_CLASS_PF:
      DECODE_ARG2(((opcode >> 8) & 0x1f), (opcode & 0xff));
      break;
    case INSN_CLASS_SF:
      {
        long int offset;
        long int file;

        num_words = 2;
        offset = opcode & 0x7f;
        file = opcode2 & 0xfff;
        DECODE_ARG2(offset, file);
      }
      break;
    case INSN_CLASS_SS:
      {
        long int offset1;
        long int offset2;

        num_words = 2;
        offset1 = opcode & 0x7f;
        offset2 = opcode2 & 0x7f;
        DECODE_ARG2(offset1, offset2);
      }
      break;
    case INSN_CLASS_OPF5:
      DECODE_ARG1(opcode & 0x1f);
      break;
    case INSN_CLASS_OPWF5:
      DECODE_ARG1WF((opcode & 0x1f), ((opcode >> 5) & 1));
      break;
    case INSN_CLASS_B5:
      DECODE_ARG2((opcode & 0x1f), ((opcode >> 5) & 7));
      break;
    case INSN_CLASS_B8:
      DECODE_ARG2((opcode & 0xff), ((opcode >> 8) & 7));
      break;
    case INSN_CLASS_OPF7:
      DECODE_ARG1(opcode & 0x7f);
      break;
    case INSN_CLASS_OPF8:
      DECODE_ARG1(opcode & 0xff);
      break;
    case INSN_CLASS_OPWF7:
      DECODE_ARG1WF((opcode & 0x7f), ((opcode >> 7) & 1));
      break;
    case INSN_CLASS_OPWF8:
      DECODE_ARG1WF((opcode & 0xff), ((opcode >> 8) & 1));
      break;
    case INSN_CLASS_B7:
      DECODE_ARG2((opcode & 0x7f), ((opcode >> 7) & 7));
      break;
    case INSN_CLASS_OPFA8:
      DECODE_ARG2((opcode & 0xff), ((opcode >> 8) & 1));
      break;
    case INSN_CLASS_BA8:
      DECODE_ARG3((opcode & 0xff), ((opcode >> 9) & 7), ((opcode >> 8) & 1));
      break;
    case INSN_CLASS_OPWFA8:
      DECODE_ARG3((opcode & 0xff), ((opcode >> 9) & 1), ((opcode >> 8) & 1));
      break;
    case INSN_CLASS_IMPLICIT:
      DECODE_ARG0;
      break;
    case INSN_CLASS_TBL:
      {
        char op[5];

        switch(opcode & 0x3)
          {
          case 0:
            strncpy(op, "*", sizeof(op));
            break;
          case 1:
            strncpy(op, "*+", sizeof(op));
            break;
          case 2:
            strncpy(op, "*-", sizeof(op));
            break;
          case 3:
            strncpy(op, "+*", sizeof(op));
            break;
          default:
            assert(0);
          }

        snprintf(buffer,
                 sizeof_buffer,
                 "%s\t%s",
                 instruction->name,
                 op);
      }
      break;
    case INSN_CLASS_TBL2:
      DECODE_ARG2(((opcode >> 9) & 1), (opcode & 0xff));
      break;
    case INSN_CLASS_TBL3:
      DECODE_ARG3(((opcode >> 9) & 1),
                  ((opcode >> 8) & 1),
                   (opcode & 0xff));
      break;
    default:
      assert(0);
    }

  return num_words;
}
