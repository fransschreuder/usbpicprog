#ifndef MAIN_H
#define MAIN_H

char exitToBootloader( char set );

/******************************************************************************
 This function resets the usb module to bring it in the same state as after a
 power on reset (because it may have been altered by the bootloader)
 ******************************************************************************/
void usb_reset( void );


/******************************************************************************
 * Function:        void USBTasks(void)
 *
 * PreCondition:    InitializeSystem has been called.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Service loop for USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void USBTasks( void );

#endif //MAIN_H
