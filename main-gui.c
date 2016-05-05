/*
	USB Mass Storage Control
	http://github.com/dns/USB-Mass-Storage-Control

	Copyright (c) 2007 by Daniel Sirait

*/


#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


//#pragma comment(lib, "Comctl32.lib")
//#pragma comment(lib, "Msimg32.lib")

/* force MSVC to use ANSI/WideChar function, must be before #include <windows.h> */
//#define UNICODE
#undef UNICODE

/* turn off deprecation & warnings */
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE_GLOBALS
#define _CRT_OBSOLETE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS


#include <windows.h>
#include <commctrl.h>
//#include <exdisp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#define ENABLE_USBSTOR 0x00000003
#define DISABLE_USBSTOR 0x00000004


HWND radiobtn_enabled;
HWND radiobtn_disabled;

INT WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, INT);
LRESULT CALLBACK Dialog1_Proc (HWND, UINT, WPARAM, LPARAM);

BOOL USB_Mass_Storage_ReadState ();
BOOL USB_Mass_Storage_Control (BOOL);



INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow) {
	
	DialogBoxA(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, Dialog1_Proc);

	
	return 0;
}


INT_PTR CALLBACK Dialog1_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	BOOL usb_state;

	switch (msg) {
		case WM_INITDIALOG:
			usb_state = USB_Mass_Storage_ReadState();

			radiobtn_enabled = GetDlgItem(hwnd, IDC_RADIO_ENABLED);
			radiobtn_disabled = GetDlgItem(hwnd, IDC_RADIO_DISABLED);
			
			if (usb_state == TRUE)
				SendMessageA(radiobtn_enabled, BM_SETCHECK, BST_CHECKED, TRUE);
			else 
				SendMessageA(radiobtn_disabled, BM_SETCHECK, BST_CHECKED, TRUE);

			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_RADIO_ENABLED:
					if (HIWORD(wParam) == BN_CLICKED) {
						//MessageBoxA(NULL, "enabled", "DEBUG", MB_OK);
						USB_Mass_Storage_Control(TRUE);
					}
					break;
				case IDC_RADIO_DISABLED:
					if (HIWORD(wParam) == BN_CLICKED) {
						//MessageBoxA(NULL, "disabled", "DEBUG", MB_OK);
						USB_Mass_Storage_Control(FALSE);
					}
					break;
			}
			break;
		case WM_NOTIFY:
			switch ( ((LPNMHDR)lParam)->code ) {
				case NM_CLICK:		// msg from SysLink control, Fall through to the next case.
				case NM_RETURN:		// handle mouse click & tabstop [ENTER]
					{
						PNMLINK pNMLink = (PNMLINK) lParam;
						LITEM item = pNMLink->item;
						if ((((LPNMHDR)lParam)->idFrom == IDC_SYSLINK1)) {
							ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
						}
					}
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
		default:
			return FALSE;	// MUST BE SET TO FALSE, dialog box procedure never calls the DefWindowProc function
	}

	
	//return DefDlgProcW(hwnd, msg, wParam, lParam);
	return FALSE;	// MUST BE SET TO FALSE, dialog box procedure never calls the DefWindowProc function
}



BOOL USB_Mass_Storage_ReadState () {
	HKEY hKey;
	LONG ret;
	DWORD usb_mode;
	char buf[10];
	DWORD dword_size = sizeof(DWORD);

	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\services\\USBSTOR\\", 0, KEY_READ, &hKey);
	RegQueryValueExA(hKey, "Start", 0, NULL, &usb_mode, &dword_size);
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

	if (state == FALSE) {
		usb_mode = DISABLE_USBSTOR;
	} else if (state == TRUE) {
 		usb_mode = ENABLE_USBSTOR;
	}

	result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\services\\USBSTOR\\", 0, KEY_ALL_ACCESS, &hKey);
	
	if (result != ERROR_SUCCESS)
		return FALSE;
	
	result = RegSetValueExA(hKey, "Start", 0, REG_DWORD, (const BYTE*) &usb_mode, sizeof(DWORD));

	if (result != ERROR_SUCCESS)
		return FALSE;
	

	RegCloseKey(hKey);

	return TRUE;
}
