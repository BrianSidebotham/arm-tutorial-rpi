#ifndef __GIC_400_H__
#define __GIC_400_H__

/* CoreLink GIC-400 Generic Interrupt Controller */

/* Register bit definitions */
#define GIC400_CTL_ENABLE ( 1 << 0 )
#define GIC400_CTL_DISABLE ( 0 << 0 )

/** @brief Max interrupts that the GIC supports

    Indicates the maximum number of interrupts that the GIC supports. If ITLinesNumber=N, the
    maximum number of interrupts is 32(N+1). The interrupt ID range is from 0 to
    (number of IDs – 1). For example: 0b00011

    Up to 128 interrupt lines, interrupt IDs 0-127.

    The maximum number of interrupts is 1020 ( 0b11111 ). See the text in this section for more
    information.

    Regardless of the range of interrupt IDs defined by this field, interrupt IDs 1020-1023 are
    reserved for special purposes, see Special interrupt numbers on page 3-43 and Interrupt IDs
    on page 2-24.
*/
#define GIC400_TYPE_ITLINESNUMBER_GET(x) ( x & 0xF )

/*
    If the GIC implements the Security Extensions, the value of this field is the maximum number
    of implemented lockable SPIs, from 0 ( 0b00000 ) to 31 ( 0b11111 ), see Configuration lockdown
    on page 4-82. If this field is 0b00000 then the GIC does not implement configuration lockdown.

    If the GIC does not implement the Security Extensions, this field is reserved.
*/
#define GIC400_TYPE_LSPI_GET(x) ((x >> 11 ) & 0x1F)

/*
    Indicates whether the GIC implements the Security Extensions.
    0 - Security Extensions not implemented.
    1 - Security Extensions implemented.
*/
#define GIC400_TYPE_SECURITY_EXTENSIONS_GET(x) ((x >> 10) & 0x1)

/*
    Indicates the number of implemented CPU interfaces. The number of implemented CPU interfaces
    is one more than the value of this field, for example if this field is 0b011 , there are four
    CPU interfaces. If the GIC implements the Virtualization Extensions, this is also the number
    of virtual CPU interfaces.
*/
#define GIC400_TYPE_CPU_NUMBER_GET(x) ((x >> 5) & 0x7)

#define GIC400_TARGET_CPU0 ( 1 << 0 )
#define GIC400_TARGET_CPU1 ( 1 << 1 )
#define GIC400_TARGET_CPU2 ( 1 << 2 )
#define GIC400_TARGET_CPU3 ( 1 << 3 )
#define GIC400_TARGET_CPU4 ( 1 << 4 )
#define GIC400_TARGET_CPU5 ( 1 << 5 )
#define GIC400_TARGET_CPU6 ( 1 << 6 )
#define GIC400_TARGET_CPU7 ( 1 << 7 )

#define GIC400_ICFG_LEVEL_SENSITIVE ( 0 << 1 )
#define GIC400_ICFG_EDGE_TRIGGERED  ( 1 << 1 )

typedef struct {
    volatile unsigned int ctl;
    volatile const unsigned int type;
    volatile const unsigned int iid;
    volatile unsigned int _res0[((0x80 - 0xC) / (sizeof(unsigned int)))];
    volatile unsigned int igroup[((0x100 - 0x80) / (sizeof(unsigned int)))];
    volatile unsigned int isenable[((0x180 - 0x100) / (sizeof(unsigned int)))];
    volatile unsigned int icenable[((0x200 - 0x180) / (sizeof(unsigned int)))];
    volatile unsigned int ispend[((0x280 - 0x200) / (sizeof(unsigned int)))];
    volatile unsigned int icpend[((0x300 - 0x280) / (sizeof(unsigned int)))];
    volatile unsigned int isactive[((0x380 - 0x300) / (sizeof(unsigned int)))];
    volatile unsigned int icactive[((0x400 - 0x380) / (sizeof(unsigned int)))];
    volatile unsigned char ipriority[((0x800 - 0x400) / (sizeof(unsigned char)))];
    volatile unsigned char istargets[((0xC00 - 0x800) / (sizeof(unsigned char)))];
    volatile unsigned int icfg[((0xD00 - 0xC08) / (sizeof(unsigned int)))];
    volatile const unsigned int ppis;
    volatile unsigned int spis[((0xF00 - 0xD04) / (sizeof(unsigned int)))];
    volatile unsigned int sgi;
    volatile unsigned int _res1[((0xF10 - 0xF04) / (sizeof(unsigned int)))];
    volatile unsigned int cpendsgi[((0xF20 - 0xF10) / (sizeof(unsigned int)))];
    volatile unsigned int spendsgi[((0xF30 - 0xF20) / (sizeof(unsigned int)))];
    volatile unsigned int _res2[((0xFD0 - 0xF30) / (sizeof(unsigned int)))];
    volatile const unsigned int pid[((0xFF0 - 0xFD0) / (sizeof(unsigned int)))];
    volatile const unsigned int cid[((0x1000 - 0xFF0) / (sizeof(unsigned int)))];
} gic400_gicd_t;

typedef struct {
  volatile unsigned int ctl;
  volatile unsigned int pm;
  volatile unsigned int bp;
  volatile const unsigned int ia;
  volatile unsigned int eoi;
  volatile const unsigned int rp;
  volatile const unsigned int hppi;
  volatile unsigned int abp;
  volatile const unsigned int aia;
  volatile unsigned int aeoi;
  volatile const unsigned int ahppi;
  volatile unsigned int _res0[(0xD0 - 0x2C) / sizeof(unsigned int)];
  volatile unsigned int ap;
  volatile unsigned int _res1[(0xE0 - 0xD4) / sizeof(unsigned int)];
  volatile unsigned int nasp;
  volatile unsigned int _res2[(0xFC - 0xE4) / sizeof(unsigned int)];
  volatile const unsigned int iid;
  volatile unsigned int _res3[(0x1000 - 0x100) / sizeof(unsigned int)];
  volatile unsigned int dir;
} gic400_gicc_t;

extern int gic400_init(void* interrupt_controller_base);

#endif
