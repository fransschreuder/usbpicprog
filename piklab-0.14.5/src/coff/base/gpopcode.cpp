/* GNU PIC opcode definitions
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

#include "gpopcode.h"

/* FIXME: use const struct */

/* PIC 12-bit instruction set */
struct insn  op_12c5xx[] = {
  { "addwf",  0xfc0, 0x1c0,     INSN_CLASS_OPWF5        },
  { "andlw",  0xf00, 0xe00,     INSN_CLASS_LIT8         },
  { "andwf",  0xfc0, 0x140,     INSN_CLASS_OPWF5        },
  { "bcf",    0xf00, 0x400,     INSN_CLASS_B5           },
  { "bsf",    0xf00, 0x500,     INSN_CLASS_B5           },
  { "btfsc",  0xf00, 0x600,     INSN_CLASS_B5           },
  { "btfss",  0xf00, 0x700,     INSN_CLASS_B5           },
  { "call",   0xf00, 0x900,     INSN_CLASS_LIT8C12      },
  { "clrf",   0xfe0, 0x060,     INSN_CLASS_OPF5         },
  { "clrw",   0xfff, 0x040,     INSN_CLASS_IMPLICIT     },
  { "clrwdt", 0xfff, 0x004,     INSN_CLASS_IMPLICIT     },
  { "comf",   0xfc0, 0x240,     INSN_CLASS_OPWF5        },
  { "decf",   0xfc0, 0x0c0,     INSN_CLASS_OPWF5        },
  { "decfsz", 0xfc0, 0x2c0,     INSN_CLASS_OPWF5        },
  { "goto",   0xe00, 0xa00,     INSN_CLASS_LIT9         },
  { "incf",   0xfc0, 0x280,     INSN_CLASS_OPWF5        },
  { "incfsz", 0xfc0, 0x3c0,     INSN_CLASS_OPWF5        },
  { "iorlw",  0xf00, 0xd00,     INSN_CLASS_LIT8         },
  { "iorwf",  0xfc0, 0x100,     INSN_CLASS_OPWF5        },
  { "movf",   0xfc0, 0x200,     INSN_CLASS_OPWF5        },
  { "movlw",  0xf00, 0xc00,     INSN_CLASS_LIT8         },
  { "movwf",  0xfe0, 0x020,     INSN_CLASS_OPF5         },
  { "nop",    0xfff, 0x000,     INSN_CLASS_IMPLICIT     },
  { "option", 0xfff, 0x002,     INSN_CLASS_IMPLICIT     },
  { "retlw",  0xf00, 0x800,     INSN_CLASS_LIT8         },
  { "return", 0xfff, 0x800,     INSN_CLASS_IMPLICIT     }, /* FIXME: special mnemonic */
  { "rlf",    0xfc0, 0x340,     INSN_CLASS_OPWF5        },
  { "rrf",    0xfc0, 0x300,     INSN_CLASS_OPWF5        },
  { "sleep",  0xfff, 0x003,     INSN_CLASS_IMPLICIT     },
  { "subwf",  0xfc0, 0x080,     INSN_CLASS_OPWF5        },
  { "swapf",  0xfc0, 0x380,     INSN_CLASS_OPWF5        },
  { "tris",   0xff8, 0x000,     INSN_CLASS_OPF5         },
  { "xorlw",  0xf00, 0xf00,     INSN_CLASS_LIT8         },
  { "xorwf",  0xfc0, 0x180,     INSN_CLASS_OPWF5        }
};

const int num_op_12c5xx = TABLE_SIZE(op_12c5xx);

/* Scenix SX has a superset of the PIC 12-bit instruction set */
/*
 * It would be nice if there was a more elegant way to do this,
 * either by adding a flags field to struct insn, or by allowing a
 * processor to have more than one associated table.
 */
struct insn  op_sx[] = {
  { "addwf",  0xfc0, 0x1c0,     INSN_CLASS_OPWF5        },
  { "andlw",  0xf00, 0xe00,     INSN_CLASS_LIT8         },
  { "andwf",  0xfc0, 0x140,     INSN_CLASS_OPWF5        },
  { "bank",   0xff8, 0x018,     INSN_CLASS_LIT3_BANK    }, /* SX only */
  { "bcf",    0xf00, 0x400,     INSN_CLASS_B5           },
  { "bsf",    0xf00, 0x500,     INSN_CLASS_B5           },
  { "btfsc",  0xf00, 0x600,     INSN_CLASS_B5           },
  { "btfss",  0xf00, 0x700,     INSN_CLASS_B5           },
  { "call",   0xf00, 0x900,     INSN_CLASS_LIT8C12      },
  { "clrf",   0xfe0, 0x060,     INSN_CLASS_OPF5         },
  { "clrw",   0xfff, 0x040,     INSN_CLASS_IMPLICIT     },
  { "clrwdt", 0xfff, 0x004,     INSN_CLASS_IMPLICIT     },
  { "comf",   0xfc0, 0x240,     INSN_CLASS_OPWF5        },
  { "decf",   0xfc0, 0x0c0,     INSN_CLASS_OPWF5        },
  { "decfsz", 0xfc0, 0x2c0,     INSN_CLASS_OPWF5        },
  { "goto",   0xe00, 0xa00,     INSN_CLASS_LIT9         },
  { "incf",   0xfc0, 0x280,     INSN_CLASS_OPWF5        },
  { "incfsz", 0xfc0, 0x3c0,     INSN_CLASS_OPWF5        },
  { "iorlw",  0xf00, 0xd00,     INSN_CLASS_LIT8         },
  { "iorwf",  0xfc0, 0x100,     INSN_CLASS_OPWF5        },
  { "iread",  0xfff, 0x041,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "mode",   0xff0, 0x050,     INSN_CLASS_LIT4         }, /* SX only */
  { "movf",   0xfc0, 0x200,     INSN_CLASS_OPWF5        },
  { "movlw",  0xf00, 0xc00,     INSN_CLASS_LIT8         },
  { "movmw",  0xfff, 0x042,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "movwf",  0xfe0, 0x020,     INSN_CLASS_OPF5         },
  { "movwm",  0xfff, 0x043,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "nop",    0xfff, 0x000,     INSN_CLASS_IMPLICIT     },
  { "option", 0xfff, 0x002,     INSN_CLASS_IMPLICIT     },
  { "page",   0xff8, 0x010,     INSN_CLASS_LIT3_PAGE    }, /* SX only */
  { "reti",   0xfff, 0x00e,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "retiw",  0xfff, 0x00f,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "retlw",  0xf00, 0x800,     INSN_CLASS_LIT8         },
  { "retp",   0xfff, 0x00d,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "return", 0xfff, 0x00c,     INSN_CLASS_IMPLICIT     }, /* SX only */
  { "rlf",    0xfc0, 0x340,     INSN_CLASS_OPWF5        },
  { "rrf",    0xfc0, 0x300,     INSN_CLASS_OPWF5        },
  { "sleep",  0xfff, 0x003,     INSN_CLASS_IMPLICIT     },
  { "subwf",  0xfc0, 0x080,     INSN_CLASS_OPWF5        },
  { "swapf",  0xfc0, 0x380,     INSN_CLASS_OPWF5        },
  { "tris",   0xff8, 0x000,     INSN_CLASS_OPF5         },
  { "xorlw",  0xf00, 0xf00,     INSN_CLASS_LIT8         },
  { "xorwf",  0xfc0, 0x180,     INSN_CLASS_OPWF5        }
};

const int num_op_sx = TABLE_SIZE(op_sx);

/* PIC 14-bit instruction set */
struct insn  op_16cxx[] = {
  { "addlw",  0x3e00, 0x3e00,   INSN_CLASS_LIT8         },
  { "addwf",  0x3f00, 0x0700,   INSN_CLASS_OPWF7        },
  { "andlw",  0x3f00, 0x3900,   INSN_CLASS_LIT8         },
  { "andwf",  0x3f00, 0x0500,   INSN_CLASS_OPWF7        },
  { "bcf",    0x3c00, 0x1000,   INSN_CLASS_B7           },
  { "bsf",    0x3c00, 0x1400,   INSN_CLASS_B7           },
  { "btfsc",  0x3c00, 0x1800,   INSN_CLASS_B7           },
  { "btfss",  0x3c00, 0x1c00,   INSN_CLASS_B7           },
  { "call",   0x3800, 0x2000,   INSN_CLASS_LIT11        },
  { "clrf",   0x3f80, 0x0180,   INSN_CLASS_OPF7         },
  { "clrw",   0x3fff, 0x0103,   INSN_CLASS_IMPLICIT     },
  { "clrwdt", 0x3fff, 0x0064,   INSN_CLASS_IMPLICIT     },
  { "comf",   0x3f00, 0x0900,   INSN_CLASS_OPWF7        },
  { "decf",   0x3f00, 0x0300,   INSN_CLASS_OPWF7        },
  { "decfsz", 0x3f00, 0x0b00,   INSN_CLASS_OPWF7        },
  { "goto",   0x3800, 0x2800,   INSN_CLASS_LIT11        },
  { "incf",   0x3f00, 0x0a00,   INSN_CLASS_OPWF7        },
  { "incfsz", 0x3f00, 0x0f00,   INSN_CLASS_OPWF7        },
  { "iorlw",  0x3f00, 0x3800,   INSN_CLASS_LIT8         },
  { "iorwf",  0x3f00, 0x0400,   INSN_CLASS_OPWF7        },
  { "movf",   0x3f00, 0x0800,   INSN_CLASS_OPWF7        },
  { "movlw",  0x3c00, 0x3000,   INSN_CLASS_LIT8         },
  { "movwf",  0x3f80, 0x0080,   INSN_CLASS_OPF7         },
  { "nop",    0x3f9f, 0x0000,   INSN_CLASS_IMPLICIT     },
  { "option", 0x3fff, 0x0062,   INSN_CLASS_IMPLICIT     },
  { "retfie", 0x3fff, 0x0009,   INSN_CLASS_IMPLICIT     },
  { "retlw",  0x3c00, 0x3400,   INSN_CLASS_LIT8         },
  { "return", 0x3fff, 0x0008,   INSN_CLASS_IMPLICIT     },
  { "rlf",    0x3f00, 0x0d00,   INSN_CLASS_OPWF7        },
  { "rrf",    0x3f00, 0x0c00,   INSN_CLASS_OPWF7        },
  { "sleep",  0x3fff, 0x0063,   INSN_CLASS_IMPLICIT     },
  { "sublw",  0x3e00, 0x3c00,   INSN_CLASS_LIT8         },
  { "subwf",  0x3f00, 0x0200,   INSN_CLASS_OPWF7        },
  { "swapf",  0x3f00, 0x0e00,   INSN_CLASS_OPWF7        },
  { "tris",   0x3ff8, 0x0060,   INSN_CLASS_OPF7         },
  { "xorlw",  0x3f00, 0x3a00,   INSN_CLASS_LIT8         },
  { "xorwf",  0x3f00, 0x0600,   INSN_CLASS_OPWF7        }
};

const int num_op_16cxx = TABLE_SIZE(op_16cxx);

/* PIC 16-bit instruction set */
struct insn  op_17cxx[] = {
  { "addlw",  0xff00, 0xb100,   INSN_CLASS_LIT8         },
  { "addwf",  0xfe00, 0x0e00,   INSN_CLASS_OPWF8        },
  { "addwfc", 0xfe00, 0x1000,   INSN_CLASS_OPWF8        },
  { "andlw",  0xff00, 0xb500,   INSN_CLASS_LIT8         },
  { "andwf",  0xfe00, 0x0a00,   INSN_CLASS_OPWF8        },
  { "bcf",    0xf800, 0x8800,   INSN_CLASS_B8           },
  { "bsf",    0xf800, 0x8000,   INSN_CLASS_B8           },
  { "btfsc",  0xf800, 0x9800,   INSN_CLASS_B8           },
  { "btfss",  0xf800, 0x9000,   INSN_CLASS_B8           },
  { "btg",    0xf800, 0x3800,   INSN_CLASS_B8           },
  { "call",   0xe000, 0xe000,   INSN_CLASS_LIT13        },
  { "clrf",   0xfe00, 0x2800,   INSN_CLASS_OPWF8        },
  { "clrwdt", 0xffff, 0x0004,   INSN_CLASS_IMPLICIT     },
  { "comf",   0xfe00, 0x1200,   INSN_CLASS_OPWF8        },
  { "cpfseq", 0xff00, 0x3100,   INSN_CLASS_OPF8         },
  { "cpfsgt", 0xff00, 0x3200,   INSN_CLASS_OPF8         },
  { "cpfslt", 0xff00, 0x3000,   INSN_CLASS_OPF8         },
  { "daw",    0xfe00, 0x2e00,   INSN_CLASS_OPWF8        },
  { "decf",   0xfe00, 0x0600,   INSN_CLASS_OPWF8        },
  { "decfsz", 0xfe00, 0x1600,   INSN_CLASS_OPWF8        },
  { "dcfsnz", 0xfe00, 0x2600,   INSN_CLASS_OPWF8        },
  { "goto",   0xe000, 0xc000,   INSN_CLASS_LIT13        },
  { "incf",   0xfe00, 0x1400,   INSN_CLASS_OPWF8        },
  { "incfsz", 0xfe00, 0x1e00,   INSN_CLASS_OPWF8        },
  { "infsnz", 0xfe00, 0x2400,   INSN_CLASS_OPWF8        },
  { "iorlw",  0xff00, 0xb300,   INSN_CLASS_LIT8         },
  { "iorwf",  0xfe00, 0x0800,   INSN_CLASS_OPWF8        },
  { "lcall",  0xff00, 0xb700,   INSN_CLASS_LIT8C16      },
  { "movfp",  0xe000, 0x6000,   INSN_CLASS_FP           },
  { "movpf",  0xe000, 0x4000,   INSN_CLASS_PF           },
  { "movlb",  0xff00, 0xb800,   INSN_CLASS_LIT8         },
  { "movlr",  0xfe00, 0xba00,   INSN_CLASS_LIT4S        },
  { "movlw",  0xff00, 0xb000,   INSN_CLASS_LIT8         },
  { "movwf",  0xff00, 0x0100,   INSN_CLASS_OPF8         },
  { "mullw",  0xff00, 0xbc00,   INSN_CLASS_LIT8         },
  { "mulwf",  0xff00, 0x3400,   INSN_CLASS_OPF8         },
  { "negw",   0xfe00, 0x2c00,   INSN_CLASS_OPWF8        },
  { "nop",    0xffff, 0x0000,   INSN_CLASS_IMPLICIT     },
  { "retfie", 0xffff, 0x0005,   INSN_CLASS_IMPLICIT     },
  { "retlw",  0xff00, 0xb600,   INSN_CLASS_LIT8         },
  { "return", 0xffff, 0x0002,   INSN_CLASS_IMPLICIT     },
  { "rlcf",   0xfe00, 0x1a00,   INSN_CLASS_OPWF8        },
  { "rlncf",  0xfe00, 0x2200,   INSN_CLASS_OPWF8        },
  { "rrcf",   0xfe00, 0x1800,   INSN_CLASS_OPWF8        },
  { "rrncf",  0xfe00, 0x2000,   INSN_CLASS_OPWF8        },
  { "setf",   0xfe00, 0x2a00,   INSN_CLASS_OPWF8        },
  { "sleep",  0xffff, 0x0003,   INSN_CLASS_IMPLICIT     },
  { "sublw",  0xff00, 0xb200,   INSN_CLASS_LIT8         },
  { "subwf",  0xfe00, 0x0400,   INSN_CLASS_OPWF8        },
  { "subwfb", 0xfe00, 0x0200,   INSN_CLASS_OPWF8        },
  { "swapf",  0xfe00, 0x1c00,   INSN_CLASS_OPWF8        },
  { "tablrd", 0xfc00, 0xa800,   INSN_CLASS_TBL3         },
  { "tablwt", 0xfc00, 0xac00,   INSN_CLASS_TBL3         },
  { "tlrd",   0xfc00, 0xa000,   INSN_CLASS_TBL2         },
  { "tlwt",   0xfc00, 0xa400,   INSN_CLASS_TBL2         },
  { "tstfsz", 0xff00, 0x3300,   INSN_CLASS_OPF8         },
  { "xorlw",  0xff00, 0xb400,   INSN_CLASS_LIT8         },
  { "xorwf",  0xfe00, 0x0c00,   INSN_CLASS_OPWF8        }
};

const int num_op_17cxx = TABLE_SIZE(op_17cxx);

struct insn  op_18cxx[] = {
  { "addlw",  0xff00, 0x0f00,   INSN_CLASS_LIT8         },
  { "addwf",  0xfc00, 0x2400,   INSN_CLASS_OPWFA8       },
  { "addwfc", 0xfc00, 0x2000,   INSN_CLASS_OPWFA8       },
  { "andlw",  0xff00, 0x0b00,   INSN_CLASS_LIT8         },
  { "andwf",  0xfc00, 0x1400,   INSN_CLASS_OPWFA8       },
  { "bc",     0xff00, 0xe200,   INSN_CLASS_RBRA8        },
  { "bcf",    0xf000, 0x9000,   INSN_CLASS_BA8          },
  { "bn",     0xff00, 0xe600,   INSN_CLASS_RBRA8        },
  { "bnc",    0xff00, 0xe300,   INSN_CLASS_RBRA8        },
  { "bnn",    0xff00, 0xe700,   INSN_CLASS_RBRA8        },
  { "bnov",   0xff00, 0xe500,   INSN_CLASS_RBRA8        },
  { "bnz",    0xff00, 0xe100,   INSN_CLASS_RBRA8        },
  { "bov",    0xff00, 0xe400,   INSN_CLASS_RBRA8        },
  { "bra",    0xf800, 0xd000,   INSN_CLASS_RBRA11       },
  { "bsf",    0xf000, 0x8000,   INSN_CLASS_BA8          },
  { "btfsc",  0xf000, 0xb000,   INSN_CLASS_BA8          },
  { "btfss",  0xf000, 0xa000,   INSN_CLASS_BA8          },
  { "btg",    0xf000, 0x7000,   INSN_CLASS_BA8          },
  { "bz",     0xff00, 0xe000,   INSN_CLASS_RBRA8        },
  { "call",   0xfe00, 0xec00,   INSN_CLASS_CALL20       },
  { "clrf",   0xfe00, 0x6a00,   INSN_CLASS_OPFA8        },
  { "clrwdt", 0xffff, 0x0004,   INSN_CLASS_IMPLICIT     },
  { "comf",   0xfc00, 0x1c00,   INSN_CLASS_OPWFA8       },
  { "cpfseq", 0xfe00, 0x6200,   INSN_CLASS_OPFA8        },
  { "cpfsgt", 0xfe00, 0x6400,   INSN_CLASS_OPFA8        },
  { "cpfslt", 0xfe00, 0x6000,   INSN_CLASS_OPFA8        },
  { "daw",    0xffff, 0x0007,   INSN_CLASS_IMPLICIT     },
  { "decf",   0xfc00, 0x0400,   INSN_CLASS_OPWFA8       },
  { "decfsz", 0xfc00, 0x2c00,   INSN_CLASS_OPWFA8       },
  { "dcfsnz", 0xfc00, 0x4c00,   INSN_CLASS_OPWFA8       },
  { "goto",   0xff00, 0xef00,   INSN_CLASS_LIT20        },
  { "incf",   0xfc00, 0x2800,   INSN_CLASS_OPWFA8       },
  { "incfsz", 0xfc00, 0x3c00,   INSN_CLASS_OPWFA8       },
  { "infsnz", 0xfc00, 0x4800,   INSN_CLASS_OPWFA8       },
  { "iorlw",  0xff00, 0x0900,   INSN_CLASS_LIT8         },
  { "iorwf",  0xfc00, 0x1000,   INSN_CLASS_OPWFA8       },
  { "lfsr",   0xffc0, 0xee00,   INSN_CLASS_FLIT12       },
  { "movf",   0xfc00, 0x5000,   INSN_CLASS_OPWFA8       },
  { "movff",  0xf000, 0xc000,   INSN_CLASS_FF           },
  { "movlb",  0xff00, 0x0100,   INSN_CLASS_LIT8         },
  { "movlw",  0xff00, 0x0e00,   INSN_CLASS_LIT8         },
  { "movwf",  0xfe00, 0x6e00,   INSN_CLASS_OPFA8        },
  { "mullw",  0xff00, 0x0d00,   INSN_CLASS_LIT8         },
  { "mulwf",  0xfe00, 0x0200,   INSN_CLASS_OPFA8        },
  { "negf",   0xfe00, 0x6c00,   INSN_CLASS_OPFA8        },
  { "nop",    0xffff, 0x0000,   INSN_CLASS_IMPLICIT     },
  { "pop",    0xffff, 0x0006,   INSN_CLASS_IMPLICIT     },
  { "push",   0xffff, 0x0005,   INSN_CLASS_IMPLICIT     },
  { "rcall",  0xf800, 0xd800,   INSN_CLASS_RBRA11       },
  { "reset",  0xffff, 0x00ff,   INSN_CLASS_IMPLICIT     },
  { "retfie", 0xfffe, 0x0010,   INSN_CLASS_LIT1         },
  { "retlw",  0xff00, 0x0c00,   INSN_CLASS_LIT8         },
  { "return", 0xfffe, 0x0012,   INSN_CLASS_LIT1         },
  { "rlcf",   0xfc00, 0x3400,   INSN_CLASS_OPWFA8       },
  { "rlncf",  0xfc00, 0x4400,   INSN_CLASS_OPWFA8       },
  { "rrcf",   0xfc00, 0x3000,   INSN_CLASS_OPWFA8       },
  { "rrncf",  0xfc00, 0x4000,   INSN_CLASS_OPWFA8       },
  { "setf",   0xfe00, 0x6800,   INSN_CLASS_OPFA8        },
  { "sleep",  0xffff, 0x0003,   INSN_CLASS_IMPLICIT     },
  { "subfwb", 0xfc00, 0x5400,   INSN_CLASS_OPWFA8       },
  { "sublw",  0xff00, 0x0800,   INSN_CLASS_LIT8         },
  { "subwf",  0xfc00, 0x5c00,   INSN_CLASS_OPWFA8       },
  { "subwfb", 0xfc00, 0x5800,   INSN_CLASS_OPWFA8       },
  { "swapf",  0xfc00, 0x3800,   INSN_CLASS_OPWFA8       },
  { "tblrd",  0xfffc, 0x0008,   INSN_CLASS_TBL          },
  { "tblwt",  0xfffc, 0x000c,   INSN_CLASS_TBL          },
  { "tstfsz", 0xfe00, 0x6600,   INSN_CLASS_OPFA8        },
  { "xorlw",  0xff00, 0x0a00,   INSN_CLASS_LIT8         },
  { "xorwf",  0xfc00, 0x1800,   INSN_CLASS_OPWFA8       }
};

const int num_op_18cxx = TABLE_SIZE(op_18cxx);

/* PIC 16-bit "Special" instruction set */
struct insn  op_18cxx_sp[] = {
  { "clrc",   0xffff, 0x90d8,   INSN_CLASS_IMPLICIT     },
  { "clrdc",  0xffff, 0x92d8,   INSN_CLASS_IMPLICIT     },
  { "clrn",   0xffff, 0x98d8,   INSN_CLASS_IMPLICIT     },
  { "clrov",  0xffff, 0x96d8,   INSN_CLASS_IMPLICIT     },
  { "clrw",   0xffff, 0x6ae8,   INSN_CLASS_IMPLICIT     },
  { "clrz",   0xffff, 0x94d8,   INSN_CLASS_IMPLICIT     },
  { "setc",   0xffff, 0x80d8,   INSN_CLASS_IMPLICIT     },
  { "setdc",  0xffff, 0x82d8,   INSN_CLASS_IMPLICIT     },
  { "setn",   0xffff, 0x88d8,   INSN_CLASS_IMPLICIT     },
  { "setov",  0xffff, 0x86d8,   INSN_CLASS_IMPLICIT     },
  { "setz",   0xffff, 0x84d8,   INSN_CLASS_IMPLICIT     },
  { "skpc",   0xffff, 0xa0d8,   INSN_CLASS_IMPLICIT     },
  { "skpdc",  0xffff, 0xa2d8,   INSN_CLASS_IMPLICIT     },
  { "skpn",   0xffff, 0xa8d8,   INSN_CLASS_IMPLICIT     },
  { "skpov",  0xffff, 0xa6d8,   INSN_CLASS_IMPLICIT     },
  { "skpz",   0xffff, 0xa4d8,   INSN_CLASS_IMPLICIT     },
  { "skpnc",  0xffff, 0xb0d8,   INSN_CLASS_IMPLICIT     },
  { "skpndc", 0xffff, 0xb2d8,   INSN_CLASS_IMPLICIT     },
  { "skpnn",  0xffff, 0xb8d8,   INSN_CLASS_IMPLICIT     },
  { "skpnov", 0xffff, 0xb6d8,   INSN_CLASS_IMPLICIT     },
  { "skpnz",  0xffff, 0xb4d8,   INSN_CLASS_IMPLICIT     },
  { "tgc",    0xffff, 0x70d8,   INSN_CLASS_IMPLICIT     },
  { "tgdc",   0xffff, 0x72d8,   INSN_CLASS_IMPLICIT     },
  { "tgn",    0xffff, 0x78d8,   INSN_CLASS_IMPLICIT     },
  { "tgov",   0xffff, 0x76d8,   INSN_CLASS_IMPLICIT     },
  { "tgz",    0xffff, 0x74d8,   INSN_CLASS_IMPLICIT     }

};

const int num_op_18cxx_sp = TABLE_SIZE(op_18cxx_sp);

/* PIC 16-bit Extended instruction set */
struct insn  op_18cxx_ext[] = {
  { "addfsr",  0xff00, 0xe800,  INSN_CLASS_LITFSR       },
  { "addulnk", 0xffc0, 0xe8c0,  INSN_CLASS_LIT6         },
  { "callw",   0xffff, 0x0014,  INSN_CLASS_IMPLICIT     },
  { "movsf",   0xff80, 0xeb00,  INSN_CLASS_SF           },
  { "movss",   0xff80, 0xeb80,  INSN_CLASS_SS           },
  { "pushl",   0xff00, 0xea00,  INSN_CLASS_LIT8         },
  { "subfsr",  0xff00, 0xe900,  INSN_CLASS_LITFSR       },
  { "subulnk", 0xffc0, 0xe9c0,  INSN_CLASS_LIT6         }

};

const int num_op_18cxx_ext = TABLE_SIZE(op_18cxx_ext);

