/*
	USB Mass Storage Control
	http://github.com/dns/USB-Mass-Storage-Control

	Copyright (c) 2007 by Daniel Sirait

*/


#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



/* force MSVC to use ANSI/WideChar function, must be before #include <windows.h> */
//#define UNICODE
#undef UNICODE



#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>

#include "resource.h"


#include "CommandLineToArgvA.h"
#include "usb-msc.h"



HWND radiobtn_enabled;
HWND radiobtn_disabled;

INT WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, INT);
INT_PTR CALLBACK Dialog1_Proc (HWND, UINT, WPARAM, LPARAM);





INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow) {
	PCHAR *szArglist;
	int nArgs = 0;
	int i;
	DWORD slen;
	DWORD lpNumberOfCharsWritten;
	CHAR sDebug[500];

	szArglist = CommandLineToArgvA(GetCommandLineA(), &nArgs);

	if (nArgs > 1) {
		
		lstrcpyA(sDebug, 
"\
GUI : mass-stor-winapi-gui.exe\n\
CLI (to enable)  : mass-stor-winapi-gui.exe -enable\n\
CLI (to disable) : mass-stor-winapi-gui.exe -disable\n\
");

		slen = lstrlenA(sDebug);

		if (lstrcmpiA(szArglist[1], "-enable") == 0) {
			USB_Mass_Storage_Control(TRUE);
		} else if (lstrcmpiA(szArglist[1], "-disable") == 0) {
			USB_Mass_Storage_Control(FALSE);
		} else {
			MessageBoxA(NULL, sDebug, "Usage", MB_OK);
		}
		LocalFree(szArglist);	// Free memory allocated for CommandLineToArgvW arguments.
	} else {
		DialogBoxA(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, Dialog1_Proc);
	}



	


	

	
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
						USB_Mass_Storage_Control(TRUE);
					}
					break;
				case IDC_RADIO_DISABLED:
					if (HIWORD(wParam) == BN_CLICKED) {
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
	}

	
	//return DefDlgProcW(hwnd, msg, wParam, lParam);
	return FALSE;	// MUST BE SET TO FALSE, dialog box procedure never calls the DefWindowProc function
}


