

#include "usb-msc.h"


BOOL USB_Mass_Storage_ReadState () {
	HKEY hKey;
	DWORD usb_mode;
	DWORD dword_size = sizeof(DWORD);
	LONG result;

	result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\services\\USBSTOR\\", 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS) return FALSE;

	result = RegQueryValueExA(hKey, "Start", 0, NULL, &usb_mode, &dword_size);
	if (result != ERROR_SUCCESS) return FALSE;

	RegCloseKey(hKey);

	if (usb_mode == ENABLE_USBSTOR)
		return TRUE;
	else if (usb_mode == DISABLE_USBSTOR)
		return FALSE;
	else 
		return FALSE;
}

BOOL USB_Mass_Storage_Control (BOOL state) {
	DWORD usb_mode;
	HKEY hKey;
	LONG result;

	if (state == FALSE)
		usb_mode = DISABLE_USBSTOR;
	else if (state == TRUE)
 		usb_mode = ENABLE_USBSTOR;


	result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\services\\USBSTOR\\", 0, KEY_ALL_ACCESS, &hKey);
	if (result != ERROR_SUCCESS) return FALSE;
	
	result = RegSetValueExA(hKey, "Start", 0, REG_DWORD, (const BYTE*) &usb_mode, sizeof(DWORD));
	if (result != ERROR_SUCCESS) return FALSE;
	
	RegCloseKey(hKey);

	return TRUE;
}
