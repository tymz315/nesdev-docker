#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"
#include "nes.h"

/* locals */
static int ines_prg;		/* number of prg banks */
static int ines_chr;		/* number of character banks */
static int ines_mapper[2];	/* rom mapper type */
static int nes2_extras[8];	/* NES 2.0 extra header data */
static struct INES {		/* INES rom header */
	unsigned char id[4];
	unsigned char prg;
	unsigned char chr;
	unsigned char mapper[2];
	unsigned char unused[8];
} header;


/* ----
 * write_header()
 * ----
 * generate and write rom header
 */

void
nes_write_header(FILE *f, int banks)
{
	/* setup INES header */
	memset(&header, 0, sizeof(header));
	header.id[0] = 'N';
	header.id[1] = 'E';
	header.id[2] = 'S';
	header.id[3] = 26;
	header.prg = ines_prg;
	header.chr = ines_chr;
	header.mapper[0] = ines_mapper[0];
	header.mapper[1] = ines_mapper[1];
	header.unused[0] = nes2_extras[0];
	header.unused[1] = nes2_extras[1];
	header.unused[2] = nes2_extras[2];
	header.unused[3] = nes2_extras[3];
	header.unused[4] = nes2_extras[4];
	header.unused[5] = nes2_extras[5];

	/* write */
	fwrite(&header, sizeof(header), 1, f);
}


/* ----
 * pack_8x8_tile()
 * ----
 * encode a 8x8 tile for the NES
 */

int
nes_pack_8x8_tile(unsigned char *buffer, void *data, int line_offset, int format)
{
	int i, j;
	int cnt, err;
	unsigned int   pixel;
	unsigned char *ptr;
	unsigned int  *packed;

	/* pack the tile only in the last pass */
	if (pass != LAST_PASS)
		return (16);

	/* clear buffer */
	memset(buffer, 0, 16);

	/* encode the tile */
	switch (format) {
	case CHUNKY_TILE:
		/* 8-bit chunky format */
		cnt = 0;
		ptr = data;

		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				pixel = ptr[j ^ 0x07];
				buffer[cnt]   |= (pixel & 0x01) ? (1 << j) : 0;
				buffer[cnt+8] |= (pixel & 0x02) ? (1 << j) : 0;
			}				
			ptr += line_offset;
			cnt += 1;
		}
		break;

	case PACKED_TILE:
		/* 4-bit packed format */
		cnt = 0;
		err = 0;
		packed = data;
	
		for (i = 0; i < 8; i++) {
			pixel = packed[i];
	
			for (j = 0; j < 8; j++) {
				/* check for errors */
				if (pixel & 0x0C)
					err++;

				/* convert the tile */
				buffer[cnt]   |= (pixel & 0x01) ? (1 << j) : 0;
				buffer[cnt+8] |= (pixel & 0x02) ? (1 << j) : 0;
				pixel >>= 4;
			}
			cnt += 1;
		}

		/* error message */
		if (err)
			error("Incorrect pixel color index!");
		break;

	default:
		/* other formats not supported */
		error("Internal error: unsupported format passed to 'pack_8x8_tile'!");
		break;
	}

	/* ok */
	return (16);
}


/* ----
 * do_defchr()
 * ----
 * .defchr pseudo
 */

void
nes_defchr(int *ip)
{
	unsigned char buffer[16];
	unsigned int data[8];
	int size;
	int i;

	/* define label */
	labldef(loccnt, 1);

	/* output infos */
	data_loccnt = loccnt;
	data_size   = 3;
	data_level  = 3;

	/* get tile data */
	for (i = 0; i < 8; i++) {
		/* get value */
		if (!evaluate(ip, (i < 7) ? ',' : ';'))
			return;

		/* store value */
		data[i] = value;
	}

	/* encode tile */
	size = nes_pack_8x8_tile(buffer, data, 0, PACKED_TILE);

	/* store tile */
	putbuffer(buffer, size);

	/* output line */
	if (pass == LAST_PASS)
		println();
}


/* ----
 * do_inesprg()
 * ----
 * .inesprg pseudo
 */

void
nes_inesprg(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 0xFFF)) /* was: (value > 64) */
	{
		error("PRG bank value out of range!");
	
		return;
	}
	
	ines_prg = value & 0xFF;
	if(value & 0xF00) ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */

	nes2_extras[1] &= 0xF0;
	nes2_extras[1] |= (value & 0xF00) >> 8;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * do_ineschr()
 * ----
 * .ineschr pseudo
 */

void
nes_ineschr(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 0xFFF)) /* was: (value > 64) */
	{
		error("CHR bank value out of range!");
	
		return;
	}
	
	ines_chr = value & 0xFF;
	if(value & 0xF00) ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */

	nes2_extras[1] &= 0x0F;
	nes2_extras[1] |= (value & 0xF00) >> 4;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * do_inesmap()
 * ----
 * .inesmap pseudo
 */

void
nes_inesmap(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 0xFFF)) 
	{
		error("Mapper value out of range!");
	
		return;
	}
	
	ines_mapper[0] &= 0x0F;
	ines_mapper[0] |= (value & 0x0F) << 4;
	ines_mapper[1] &= 0x0F;
	ines_mapper[1] |= (value & 0xF0);

	nes2_extras[0] &= 0xF0;
	if(value & 0xF00) {
		ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */
		nes2_extras[0] |= value >> 8;
	}

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * do_inesmir()
 * ----
 * .inesmir pseudo
 */

void
nes_inesmir(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 15)) 
	{
		error("Mirror value out of range!");
	
		return;
	}
	
	ines_mapper[0] &= 0xF0;
	ines_mapper[0] |= (value  & 0x0F);

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * nes_nes2sub()
 * ----
 * .nes2sub pseudo
 */

void
nes_nes2sub(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 15)) 
	{
		error("Submapper value out of range!");
	
		return;
	}
	
	ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */
	nes2_extras[0] &= 0x0F;
	nes2_extras[0] |= value << 4;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * nes_nes2tv()
 * ----
 * .nes2tv pseudo
 */

void
nes_nes2tv(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 3)) 
	{
		error("TV mode value out of range!");
	
		return;
	}
	
	ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */
	nes2_extras[4] &= 0xFC;
	nes2_extras[4] |= value;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * nes_nes2vs()
 * ----
 * .nes2vs pseudo
 */

void
nes_nes2vs(int *ip)
{
	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 255)) 
	{
		error("Vs. system value out of range!");
	
		return;
	}
	
	ines_mapper[1] |= 0x09; /* turn on NES 2.0 mode and Vs. system mode */
	nes2_extras[5]  = value;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * nes_nes2chrram()
 * ----
 * .nes2chrram pseudo
 */

void
nes_nes2chrram(int *ip)
{
	int x;

	if (!evaluate(ip, ','))
		return;

	if ((value < 0) || (value > 0 && value < 128) || (value > 1048576) || (value & (value - 1))) 
	{
		error("CHR RAM value out of range!");
	
		return;
	}
	
	ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */

	if(value) { x=-6; while(value>>=1) x++; } else x=0;
	nes2_extras[3] = x;

	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 0 && value < 128) || (value > 1048576) || (value & (value - 1))) 
	{
		error("Battery CHR RAM value out of range!");
	
		return;
	}

	if(value) { x=-6; while(value>>=1) x++; } else x=0;
	nes2_extras[3] |= x << 4;

	if (pass == LAST_PASS) 
	{
		println();
	}
}


/* ----
 * nes_nes2prgram()
 * ----
 * .nes2prgram pseudo
 */

void
nes_nes2prgram(int *ip)
{
	int x;

	if (!evaluate(ip, ','))
		return;

	if ((value < 0) || (value > 0 && value < 128) || (value > 1048576) || (value & (value - 1))) 
	{
		error("PRG RAM value out of range!");
	
		return;
	}
	
	ines_mapper[1] |= 0x08; /* turn on NES 2.0 mode */

	if(value) { x=-6; while(value>>=1) x++; } else x=0;
	nes2_extras[2] = x;

	if (!evaluate(ip, ';'))
		return;

	if ((value < 0) || (value > 0 && value < 128) || (value > 1048576) || (value & (value - 1))) 
	{
		error("Battery PRG RAM value out of range!");
	
		return;
	}

	if(value) { x=-6; while(value>>=1) x++; } else x=0;
	nes2_extras[2] |= x << 4;

	if (pass == LAST_PASS) 
	{
		println();
	}
}
