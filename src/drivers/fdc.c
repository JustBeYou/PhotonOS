#include <system.h>

#include <drivers/fdc.h>
#include <kernel/io.h>
#include <i386/handlers.h>

static volatile uint8_t fdc_irq_done = 0;
int fdd = 0;

static void fdc_init_dma()
{
    outb(0x0a, 0x06);
    outb(0xd8, 0xff);
    outb(0x04, 0x0);
    outb(0x04, 0x10);
    outb(0xd8, 0xff);
    outb(0x05, 0xff);
    outb(0x80, 0x0);
    outb(0x0a, 0x02);
}

static void fdc_init_dma_read()
{
    outb(0x0a, 0x06);
    outb(0x0b, 0x56);
    outb(0x0a, 0x02);
}

static void fdc_init_dma_write()
{
    outb(0x0a, 0x06);
    outb(0x0b, 0x5a);
    outb(0x0a, 0x02);
}

static void fdc_start_motor0()
{
    outb(FDC_DOR, FDC_DOR_MASK_DRIVE0_MOTOR | FDC_DOR_MASK_RESET);
}

static void fdc_stop_motor0()
{
    outb(FDC_DOR, FDC_DOR_MASK_RESET);
}

void fdc_init()
{
    register_interrupt_handler(IRQ6, &fdc_irq_handler);

    fdc_init_dma ();

    //! reset the fdc
    fdc_reset ();

    //! set drive information
    fdc_drive_data (13, 1, 0xf, true);
}

void fdc_irq_handler(__attribute__((unused)) registers_t *regs)
{
    fdc_irq_done = 1;
}

void fdc_wait_irq()
{
    while (!fdc_irq_done) {
        ;
    }
    fdc_irq_done = 0;
}

void fdc_write_dor(uint8_t val)
{
    outb(FDC_DOR, val);
}

int fdc_is_busy()
{
    if (fdc_read_status() & FDC_MSR_MASK_BUSY) {
        return 1;
    }
    return 0;
}

size_t fdc_read_status()
{
    return inb(FDC_MSR);
}

void fdc_send_command(uint8_t cmd)
{
    system_wait(20);
    if (fdc_read_status() & FDC_MSR_MASK_DATAREG) {
        outb(FDC_FIFO, cmd);
    }
}

uint8_t fdc_read_data()
{
    system_wait(20);
    if (fdc_read_status() & FDC_MSR_MASK_DATAREG) {
        return inb(FDC_FIFO);
    }
}

void fdc_write_ccr(uint8_t val)
{
    outb(FDC_CTRL, val);
}

void fdc_read_sector_imp (uint8_t head, uint8_t track, uint8_t sector) {

    size_t st0, cyl;

    //! set the DMA for read transfer
    fdc_init_dma_read ();

    //! read in a sector
    fdc_send_command (
        FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK |
        FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
    fdc_send_command ( head << 2 | fdd );
    fdc_send_command ( track);
    fdc_send_command ( head);
    fdc_send_command ( sector);
    fdc_send_command ( FDC_SECTOR_DTL_512 );
    fdc_send_command (
        ( ( sector + 1 ) >= FDD_SECTORS_PER_TRACK )
            ? FDD_SECTORS_PER_TRACK : sector + 1 );
    fdc_send_command ( FDC_GAP3_LENGTH_3_5 );
    fdc_send_command ( 0xff );

    //! wait for irq
    fdc_wait_irq ();

    //! read status info
    for (int j=0; j<7; j++)
        fdc_read_data ();

    //! let FDC know we handled interrupt
    fdc_check_int (&st0,&cyl);
}

void fdc_drive_data (size_t stepr, size_t loadt, size_t unloadt, bool dma ) {

    size_t data = 0;

    fdc_send_command (FDC_CMD_SPECIFY);

    data = ( (stepr & 0xf) << 4) | (unloadt & 0xf);
    fdc_send_command (data);

    data = (loadt) << 1 | (dma==true) ? 1 : 0;
    fdc_send_command (data);
}

int fdc_calibrate (size_t drive) {

    size_t st0, cyl;

    if (drive >= 4)
        return -2;

    //! turn on the motor
    fdc_start_motor0();

    for (int i = 0; i < 10; i++) {

        //! send command
        fdc_send_command ( FDC_CMD_CALIBRATE );
        fdc_send_command ( drive );
        fdc_wait_irq ();
        fdc_check_int ( &st0, &cyl);

        //! did we fine cylinder 0? if so, we are done
        if (!cyl) {

            fdc_stop_motor0();
            return 0;
        }
    }

    fdc_stop_motor0();
    return -1;
}

void fdc_check_int (size_t* st0, size_t* cyl) {

    fdc_send_command (FDC_CMD_CHECK_INT);

    *st0 = fdc_read_data ();
    *cyl = fdc_read_data ();
}

int fdc_seek ( size_t cyl, size_t head ) {

    size_t st0, cyl0;

    if (fdd >= 4)
        return -1;

    for (int i = 0; i < 10; i++ ) {

        //! send the command
        fdc_send_command (FDC_CMD_SEEK);
        fdc_send_command ( (head) << 2 | fdd);
        fdc_send_command (cyl);

        //! wait for the results phase IRQ
        fdc_wait_irq ();
        fdc_check_int (&st0,&cyl0);

        //! found the cylinder?
        if ( cyl0 == cyl)
            return 0;
    }

    return -1;
}

void fdc_disable_controller () {

    fdc_write_dor (0);
}

void fdc_enable_controller () {

    fdc_write_dor ( FDC_DOR_MASK_RESET | FDC_DOR_MASK_DMA);
}

void fdc_reset () {

    size_t st0, cyl;

    //! reset the controller
    fdc_disable_controller ();
    fdc_enable_controller ();
    fdc_wait_irq ();

    //! send CHECK_INT/SENSE INTERRUPT command to all drives
    for (int i=0; i<4; i++)
        fdc_check_int (&st0,&cyl);

    //! transfer speed 500kb/s
    fdc_write_ccr (0);

    //! pass mechanical drive info. steprate=3ms, unload time=240ms, load time=16ms
    fdc_drive_data (3,16,240,true);

    //! calibrate the disk
    fdc_calibrate ( fdd );
}

void fdc_lba_to_chs (int lba,int *head,int *track,int *sector) {

   *head = ( lba % ( FDD_SECTORS_PER_TRACK * 2 ) ) / ( FDD_SECTORS_PER_TRACK );
   *track = lba / ( FDD_SECTORS_PER_TRACK * 2 );
   *sector = lba % FDD_SECTORS_PER_TRACK + 1;
}

uint8_t* fdc_read_sector (int sectorLBA) {

    if (fdd >= 4)
        return 0;

    //! convert LBA sector to CHS
    int head=0, track=0, sector=1;
    fdc_lba_to_chs (sectorLBA, &head, &track, &sector);

    //! turn motor on and seek to track
    fdc_start_motor0();
    if (fdc_seek (track, head) != 0)
        return 0;

    //! read sector and turn motor off
    fdc_read_sector_imp (head, track, sector);
    fdc_stop_motor0();

    //! warning: this is a bit hackish
    return (uint8_t*) DMA_BUFFER;
}
