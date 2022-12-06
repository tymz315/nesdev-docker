#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "defs.h"
#include "externs.h"
#include "lib6502.h"
#include "protos.h"

unsigned char emulator_memory[0x10000];
FILE*emu_fp=0;
static M6502*mpu;
static int rom_out_length=0x2000;
int use_custom_output=0;
FILE*emu_datafiles[32];
int emu_datafiles_count=0;
static int cur_datafile=0;
static int address_increment=0;
static int cur_address=0;

static void emu_assemble(uint16_t addr) {
  int i,j;
	for (i = 0; i < LAST_CH_POS; i++)
		prlnbuf[i] = ' ';
	for (i = SFIELD; i < LAST_CH_POS; i++) {
		j = emulator_memory[(addr+i-SFIELD)&0xFFFF];
		if(j==0 || j=='\r' || j=='\n') {
			prlnbuf[i] = 0;
			break;
		}
		prlnbuf[i] = j;
	}
	assemble();
}

static int Cb_rom_read(M6502 *mpu, uint16_t address, uint8_t data) {
  if(bank>=128 || section == S_EMU) return emulator_memory[address];
  else return rom[bank][address&0x1FFF];
}

static int Cb_rom_write(M6502 *mpu, uint16_t address, uint8_t data) {
  if(bank>=128 || section == S_EMU) emulator_memory[address]=data;
  else rom[bank][address&0x1FFF]=data;
}

static int Cb_map_read(M6502 *mpu, uint16_t address, uint8_t data) {
  if(bank>=128 || section == S_EMU) return emulator_memory[address];
  else return map[bank][address&0x1FFF];
}

static int Cb_map_write(M6502 *mpu, uint16_t address, uint8_t data) {
  if(bank>=128 || section == S_EMU) emulator_memory[address]=data;
  else map[bank][address&0x1FFF]=data;
}

static int Cb_io_read(M6502 *mpu, uint16_t address, uint8_t data) {
  switch(address&0xFF) {
    case 0x00:
      return bank;
    case 0x01:
      return errcnt;
    case 0x02:
      return loccnt&0xFF;
    case 0x03:
      return loccnt>>8;
    case 0x04:
      return section;
    case 0x06:
      return rom_out_length&0xFF;
    case 0x07:
      return rom_out_length>>8;
    case 0x09:
      return max_bank;
    case 0x0A:
      fflush(stdout);
      return fgetc(stdin);
    case 0x0B:
      return (emu_datafiles_count>cur_datafile)?fgetc(emu_datafiles[cur_datafile]):0;
    case 0x0C:
      return (emu_datafiles_count>cur_datafile || feof(emu_datafiles[cur_datafile]))?255:0;
    case 0x0D:
      return address_increment;
    case 0x0E:
      return cur_address&255;
    case 0x0F:
      return cur_address>>8;
    case 0x10:
      return rom[bank][cur_address];
    case 0x11:
      return map[bank][cur_address];
    case 0x12:
      data=rom[bank][cur_address];
      cur_address+=address_increment;
      while(cur_address>=0x2000) bank++,cur_address-=0x2000;
      if(bank>bank_limit) bank=0;
      return data;
    case 0x13:
      data=map[bank][cur_address];
      cur_address+=address_increment;
      while(cur_address>=0x2000) bank++,cur_address-=0x2000;
      if(bank>bank_limit) bank=0;
      return data;
    default:
      return 0xFF;
  }
}

static int Cb_io_write(M6502 *mpu, uint16_t address, uint8_t data) {
  switch(address&0xFF) {
    case 0x00:
      if(data<=bank_limit) {
        bank_glabl[section][bank]  = glablptr;
        bank_loccnt[section][bank] = loccnt;
        bank_page[section][bank]   = page;
        bank     = data;
        page     = bank_page[section][bank];
        loccnt   = bank_loccnt[section][bank];
        glablptr = bank_glabl[section][bank];
        if (max_bank < bank) max_bank = bank;
      }
      break;
    case 0x02:
      loccnt&=0xFF00;
      loccnt|=data;
      break;
    case 0x03:
      loccnt&=0x00FF;
      loccnt|=data<<8;
      break;
    case 0x04:
      if(data<5) {
        section_bank[section]      = bank;
        bank_glabl[section][bank]  = glablptr;
        bank_loccnt[section][bank] = loccnt;
        bank_page[section][bank]   = page;
        section  = data;
        bank     = section_bank[section];
        page     = bank_page[section][bank];
        loccnt   = bank_loccnt[section][bank];
        glablptr = bank_glabl[section][bank];
        if (max_bank < bank) max_bank = bank;
      }
      break;
    case 0x05:
      if(emu_fp) fputc(data,emu_fp);
      break;
    case 0x06:
      rom_out_length&=0xFF00;
      rom_out_length|=data;
      break;
    case 0x07:
      rom_out_length&=0x00FF;
      rom_out_length|=data<<8;
      break;
    case 0x08:
      if(emu_fp && bank<128) {
        if(rom_out_length>8192) rom_out_length=8192;
        fwrite(rom[bank],rom_out_length,1,emu_fp);
      }
      break;
    case 0x0A:
      fputc(data,stdout);
      break;
    case 0x0B:
      if(emu_datafiles_count>cur_datafile) fclose(emu_datafiles[cur_datafile++]);
      break;
    case 0x0C:
      if(emu_datafiles_count>cur_datafile) rewind(emu_datafiles[cur_datafile]);
      break;
    case 0x0D:
      address_increment=data;
      break;
    case 0x0E:
      cur_address=(cur_address&0xFF00)|data;
      break;
    case 0x0F:
      cur_address=((cur_address&0x00FF)|(data<<8))&0x1FFF;
      break;
    case 0x10:
      rom[bank][cur_address]=data;
      break;
    case 0x11:
      map[bank][cur_address]=data;
      break;
    case 0x12:
      rom[bank][cur_address]=data;
      cur_address+=address_increment;
      while(cur_address>=0x2000) bank++,cur_address-=0x2000;
      if(bank>bank_limit) bank=0;
      return data;
      break;
    case 0x13:
      map[bank][cur_address]=data;
      cur_address+=address_increment;
      while(cur_address>=0x2000) bank++,cur_address-=0x2000;
      if(bank>bank_limit) bank=0;
      return data;
      break;
    case 0x14:
      if(emu_fp) fprintf(emu_fp,"%d",loccnt);
      break;
    case 0x15:
      pass=0;
      emu_assemble(data<<8);
      break;
    case 0x16:
      pass=1;
      emu_assemble(data<<8);
      break;
    default:
      break;
  }
}

void init_emulator(void) {
  int x;
  if(mpu) return;
  mpu=M6502_new(0, emulator_memory, 0);
  for(x=0x2000;x<0x4000;x++) {
    M6502_setCallback(mpu,read,x,Cb_io_read);
    M6502_setCallback(mpu,write,x,Cb_io_write);
  }
  for(x=0x4000;x<0x6000;x++) {
    M6502_setCallback(mpu,read,x,Cb_rom_read);
    M6502_setCallback(mpu,write,x,Cb_rom_write);
  }
  for(x=0x6000;x<0x8000;x++) {
    M6502_setCallback(mpu,read,x,Cb_map_read);
    M6502_setCallback(mpu,write,x,Cb_map_write);
  }
}

void call_emulator(void) {
  init_emulator();
  if(mpu->registers->pc=M6502_getVector(mpu,RST)) M6502_run(mpu);
}

// http://www.piumarta.com/software/lib6502/lib6502.3.html
