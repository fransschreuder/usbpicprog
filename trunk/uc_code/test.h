/*********************************************************************************************
 *
 * Harness that prints out each programming step.
 * Must be included after IO_cfg.h (redefines macros)
 *
 ********************************************************************************************/

#ifdef TEST
#include <stdio.h>

#define I2C_delay() 	printf(  "I2C_delay()\n"  );
#define pic_read_byte2( cmd_size,command ) 	(printf(  "pic_read_byte2( cmd_size=%d,command=%02X )\n", cmd_size,command ),4)
#define pic_read_14_bits( cmd_size,command ) 	(printf(  "pic_read_14_bits( cmd_size=%d,command=%02X )\n", cmd_size,command ),14)
#define pic_send_n_bits( cmd_size,command ) 	printf(  "pic_send_n_bits( cmd_size=%d,command=%02X )\n", cmd_size,command )
#define pic_send_word_14_bits( payload ) 	printf(  "pic_send_word_14_bits( payload=%04X )\n", payload )
#define pic_send_word( payload ) 		printf(  "pic_send_word( payload=%04X )\n", payload )
#define pic_send_14_bits( cmd_size,command,payload ) printf(  "pic_send_14_bits( cmd_size=%d,command=%02X,payload=%04X )\n", cmd_size,command,payload )
#define pic_send( cmd_size,command,payload ) 	printf(  "pic_send( cmd_size=%d,command=%02X,payload=%04X )\n", cmd_size,command,payload )
#define dspic_read_16_bits() 			(printf(  "dspic_read_16_bits()\n"  ),16)
#define dspic_send_24_bits( payload ) 		printf(  "dspic_send_24_bits( payload=%06X )\n", payload )
#define I2C_start() 	printf( "I2C_start()\n"  )
#define I2C_stop() 	printf( "I2C_stop()\n"  )
#define I2C_write( d )	printf(  "I2C_write( d=%d )\n", d )
#define I2C_read( ack )	(printf(  "I2C_read( ack=%d )\n", ack ),1)

#define DelayMs( cnt )	printf( "DelayMs( %d ms )\n", cnt )
#undef  enablePGC_LOW
#define enablePGC_LOW()	printf( "TRISPGC_LOW=0\n" )
#undef  trisPGC_LOW
#define trisPGC_LOW()	printf( "TRISPGC_LOW=1\n" )
#undef  PGC_LOWoff
#define PGC_LOWoff()	printf( "PGC_LOW=1\n" )
#undef  PGC_LOWon
#define PGC_LOWon()	printf( "PGC_LOW=0\n" )
#undef  enablePGD_LOW
#define enablePGD_LOW()	printf( "TRISPGD_LOW=0\n" )
#undef  trisPGD_LOW
#define trisPGD_LOW()	printf( "TRISPGD_LOW=1\n" )
#undef  PGD_LOWoff
#define PGD_LOWoff()	printf( "PGD_LOW=1\n" )
#undef  PGD_LOWon
#define PGD_LOWon()	printf( "PGD_LOW=0\n" )
#undef  enableVPP_RUN
#define enableVPP_RUN()	printf( "TRISVPP_RUN=0\n" )
#undef  trisVPP_RUN
#define trisVPP_RUN()	printf( "TRISVPP_RUN=1\n" )
#undef  VPP_RUNoff
#define VPP_RUNoff()	printf( "VPP_RUN=0\n" )
#undef  VPP_RUNon
#define VPP_RUNon()	printf( "VPP_RUN=1\n" )
#undef  enableVDD
#define enableVDD()	printf( "TRISVDD=0\n" )
#undef  trisVDD
#define trisVDD()	printf( "TRISVDD=1\n" )
#undef  VDDoff
#define VDDoff()	printf( "VDD=1\n" )
#undef  VDDon
#define VDDon()		printf( "VDD=0\n" )
#undef  enablePGC
#define enablePGC()	printf( "TRISPGC=0\n" )
#undef  trisPGC
#define trisPGC()	printf( "TRISPGC=1\n" )
#undef  PGClow
#define PGClow()	printf( "PGC=0\n" )
#undef  PGChigh
#define PGChigh()	printf( "PGC=1\n" )
#undef  PGD_READoff
#define PGD_READoff()	printf( "PGD_READ=0\n" )
#undef  PGD_READon
#define PGD_READon()	printf( "PGD_READ=1\n" )
#undef  enablePGD
#define enablePGD()	printf( "TRISPGD=0\n" )
#undef  trisPGD
#define trisPGD()	printf( "TRISPGD=1\n" )
#undef  PGDlow
#define PGDlow()	printf( "PGD=0\n" )
#undef  PGDhigh
#define PGDhigh()	printf( "PGD=1\n" )
#undef  enableVPP_RST
#define enableVPP_RST()	printf( "TRISVPP_RST=0\n" )
#undef  trisVPP_RST
#define trisVPP_RST()	printf( "TRISVPP_RST=1\n" )
#undef  VPP_RSToff
#define VPP_RSToff()	printf( "VPP_RST=0\n" )
#undef  VPP_RSTon
#define VPP_RSTon()	printf( "VPP_RST=1\n" )
#undef  enableVPP
#define enableVPP()	printf( "TRISVPP=0\n" )
#undef  trisVPP
#define trisVPP()	printf( "TRISVPP=1\n" )
#undef  VPPoff
#define VPPoff()	printf( "VPP=1\n" )
#undef  VPPon
#define VPPon()		printf( "VPP=0\n" )

#endif
