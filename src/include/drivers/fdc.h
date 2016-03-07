#ifndef _fdc_h
#define _fdc_h

#include <system.h>

#include <i386/handlers.h>

enum FDC_IO {
    FDC_DOR = 0x3f2,
    FDC_MSR = 0x3f4,
    FDC_FIFO = 0x3f5,
    FDC_CTRL = 0x3f7
};

enum FDC_DOR_MASK {

	FDC_DOR_MASK_DRIVE0			=	0,	//00000000	= here for completeness sake
	FDC_DOR_MASK_DRIVE1			=	1,	//00000001
	FDC_DOR_MASK_DRIVE2			=	2,	//00000010
	FDC_DOR_MASK_DRIVE3			=	3,	//00000011
	FDC_DOR_MASK_RESET			=	4,	//00000100
	FDC_DOR_MASK_DMA			=	8,	//00001000
	FDC_DOR_MASK_DRIVE0_MOTOR		=	16,	//00010000
	FDC_DOR_MASK_DRIVE1_MOTOR		=	32,	//00100000
	FDC_DOR_MASK_DRIVE2_MOTOR		=	64,	//01000000
	FDC_DOR_MASK_DRIVE3_MOTOR		=	128	//10000000
};

enum FDC_MSR_MASK {

	FDC_MSR_MASK_DRIVE1_POS_MODE	=	1,	//00000001
	FDC_MSR_MASK_DRIVE2_POS_MODE	=	2,	//00000010
	FDC_MSR_MASK_DRIVE3_POS_MODE	=	4,	//00000100
	FDC_MSR_MASK_DRIVE4_POS_MODE	=	8,	//00001000
	FDC_MSR_MASK_BUSY			=	16,	//00010000
	FDC_MSR_MASK_DMA			=	32,	//00100000
	FDC_MSR_MASK_DATAIO			=	64, 	//01000000
	FDC_MSR_MASK_DATAREG		=	128	//10000000
};

enum FDC_CMD {

	FDC_CMD_READ_TRACK	=	2,
	FDC_CMD_SPECIFY		=	3,
	FDC_CMD_CHECK_STAT	=	4,
	FDC_CMD_WRITE_SECT	=	5,
	FDC_CMD_READ_SECT	=	6,
	FDC_CMD_CALIBRATE	=	7,
	FDC_CMD_CHECK_INT	=	8,
	FDC_CMD_WRITE_DEL_S	=	9,
	FDC_CMD_READ_ID_S	=	0xa,
	FDC_CMD_READ_DEL_S	=	0xc,
	FDC_CMD_FORMAT_TRACK	=	0xd,
	FDC_CMD_SEEK		=	0xf
};

enum FDC_CMD_EXT {

	FDC_CMD_EXT_SKIP	=	0x20,	//00100000
	FDC_CMD_EXT_DENSITY	=	0x40,	//01000000
	FDC_CMD_EXT_MULTITRACK	=	0x80	//10000000
};

enum FDC_GAP3_LENGTH {

	FDC_GAP3_LENGTH_STD = 42,
	FDC_GAP3_LENGTH_5_14= 32,
	FDC_GAP3_LENGTH_3_5= 27
};

enum FDC_SECTOR_DTL {

	FDC_SECTOR_DTL_128	=	0,
	FDC_SECTOR_DTL_256	=	1,
	FDC_SECTOR_DTL_512	=	2,
	FDC_SECTOR_DTL_1024	=	4
};

#define FDD_SECTORS_PER_TRACK 80
#define DMA_BUFFER 0x1000

void fdc_init();
void fdc_irq_handler(__attribute__((unused)) registers_t *regs);
void fdc_wait_irq();
void fdc_write_dor(uint8_t val);
int fdc_is_busy();
uint32_t fdc_read_status();
void fdc_send_command(uint8_t cmd);
uint8_t fdc_read_data();
void fdc_write_ccr(uint8_t val);
void fdc_read_sector_imp (uint8_t head, uint8_t track, uint8_t sector);
void fdc_drive_data (uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma );
int fdc_calibrate (uint32_t drive);
void fdc_check_int (uint32_t* st0, uint32_t* cyl);
int fdc_seek ( uint32_t cyl, uint32_t head );
void fdc_disable_controller ();
void fdc_enable_controller ();
void fdc_reset ();
void fdc_lba_to_chs (int lba,int *head,int *track,int *sector);
uint8_t* fdc_read_sector (int sectorLBA);

#endif
