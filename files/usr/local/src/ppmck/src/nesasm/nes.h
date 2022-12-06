
/* NES.C */
void nes_write_header(FILE *f, int banks);
int  nes_pack_8x8_tile(unsigned char *buffer, void *data, int line_offset, int format);
void nes_defchr(int *ip);
void nes_inesprg(int *ip);
void nes_ineschr(int *ip);
void nes_inesmap(int *ip);
void nes_inesmir(int *ip);
void nes_nes2sub(int *ip);
void nes_nes2tv(int *ip);
void nes_nes2vs(int *ip);
void nes_nes2chrram(int *ip);
void nes_nes2prgram(int *ip);

/* NES specific instructions (unofficial opcodes) */
struct t_opcode nes_inst[16] = {
	{NULL, "ANC", class4, IMM, 0x0B, 6},
	{NULL, "ALR", class4, IMM, 0x4B, 6},
	{NULL, "ARR", class4, IMM, 0x6B, 6},
	{NULL, "AXS", class4, IMM, 0xCB, 6},
	{NULL, "DCP", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0xC0, 6},
	{NULL, "HLT", class1, 0, 0x02, 0},
	{NULL, "ISC", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0xE0, 6},
	{NULL, "JAM", class1, 0, 0x22, 0},
	{NULL, "KIL", class1, 0, 0x42, 0},
	{NULL, "LAX", class4, ZP|ZP_Y|ZP_IND_X|ZP_IND_Y|ABS|ABS_Y, 0xA0, 6},
	{NULL, "RLA", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0x20, 6},
	{NULL, "RRA", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0x60, 6},
	{NULL, "SAX", class4, ZP|ZP_Y|ZP_IND_X|ABS, 0x80, 6},
	{NULL, "SLO", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0x00, 6},
	{NULL, "SRE", class4, ZP|ZP_X|ZP_IND_X|ZP_IND_Y|ABS|ABS_X|ABS_Y, 0x40, 6},

	{NULL, NULL, NULL, 0, 0, 0}
};

/* NES specific pseudos */
struct t_opcode nes_pseudo[21] = {
	{NULL,  "DEFCHR",  nes_defchr,  PSEUDO, P_DEFCHR,  0},
	{NULL,  "INESPRG", nes_inesprg, PSEUDO, P_INESPRG, 0},
	{NULL,  "INESCHR", nes_ineschr, PSEUDO, P_INESCHR, 0},
	{NULL,  "INESMAP", nes_inesmap, PSEUDO, P_INESMAP, 0},
	{NULL,  "INESMIR", nes_inesmir, PSEUDO, P_INESMIR, 0},
	{NULL,  "NES2CHRRAM", nes_nes2chrram, PSEUDO, P_NES2,    0},
	{NULL,  "NES2PRGRAM", nes_nes2prgram, PSEUDO, P_NES2,    0},
	{NULL,  "NES2SUB", nes_nes2sub, PSEUDO, P_NES2,    0},
	{NULL,  "NES2TV",  nes_nes2tv,  PSEUDO, P_NES2,    0},
	{NULL,  "NES2VS",  nes_nes2vs,  PSEUDO, P_NES2,    0},

	{NULL, ".DEFCHR",  nes_defchr,  PSEUDO, P_DEFCHR,  0},
	{NULL, ".INESPRG", nes_inesprg, PSEUDO, P_INESPRG, 0},
	{NULL, ".INESCHR", nes_ineschr, PSEUDO, P_INESCHR, 0},
	{NULL, ".INESMAP", nes_inesmap, PSEUDO, P_INESMAP, 0},
	{NULL, ".INESMIR", nes_inesmir, PSEUDO, P_INESMIR, 0},
	{NULL, ".NES2CHRRAM", nes_nes2chrram, PSEUDO, P_NES2,    0},
	{NULL, ".NES2PRGRAM", nes_nes2prgram, PSEUDO, P_NES2,    0},
	{NULL, ".NES2SUB", nes_nes2sub, PSEUDO, P_NES2,    0},
	{NULL, ".NES2TV",  nes_nes2tv,  PSEUDO, P_NES2,    0},
	{NULL, ".NES2VS",  nes_nes2vs,  PSEUDO, P_NES2,    0},
	{NULL, NULL, NULL, 0, 0, 0}
};

/* NES machine description */
struct t_machine nes = {
	MACHINE_NES,   /* type */
	"NESASM", /* asm_name */
	"NES Assembler (v2.51)", /* asm_title */
	".nes",  /* rom_ext */
	"NES_INCLUDE", /* include_env */
	0x100,  /* zp_limit */
	0x800,  /* ram_limit */
	0,      /* ram_base */
	0,      /* ram_page */
	RESERVED_BANK, /* ram_bank */
	nes_inst,       /* inst */
	nes_pseudo, /* pseudo_inst */
	nes_pack_8x8_tile, /* pack_8x8_tile */
	NULL,              /* pack_16x16_tile */
	NULL,              /* pack_16x16_sprite */
	nes_write_header   /* write_header */
};

