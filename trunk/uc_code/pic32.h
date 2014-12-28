#ifndef PIC32_H
#define PIC32_H

void bulk_erase_P32( unsigned char doRestore );
void write_code_P32(unsigned long address, unsigned char* data, char blocksize, char lastblock);
void read_code_P32( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void write_config_bits_P32( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void get_device_id_P32(unsigned char* data); 
unsigned long P32CheckDeviceStatus(void);

#endif //PIC32_H

