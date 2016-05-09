
#pragma once

#include <windows.h>


#define ENABLE_USBSTOR 0x00000003
#define DISABLE_USBSTOR 0x00000004


BOOL USB_Mass_Storage_ReadState ();
BOOL USB_Mass_Storage_Control (BOOL);

