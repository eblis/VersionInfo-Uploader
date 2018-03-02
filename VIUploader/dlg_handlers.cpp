/*
VersionInfo Uploader plugin for Miranda IM

Copyright © 2007 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "dlg_handlers.h"

#define MIN_PLUGINS_WIDTH 200
#define MIN_PLUGINS_HEIGHT 200

const char *szPluginsColumns[] = {"Plugin", "Old version", "New version", "URL"};
const int sizePluginsColumns[] = {150, 85, 85, 420};
const int cPluginsColumns  = sizeof(szPluginsColumns) / sizeof(szPluginsColumns[0]);

static WNDPROC OldPluginsListProc = NULL;

int CALLBACK DlgProcVIUploaderOpts(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int bOptionsInitializing = 0;
	
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			bOptionsInitializing = 1;
			TranslateDialogDefault(hWnd);
			
			CheckDlgButton(hWnd, IDC_UPLOADONSTARTUP, (BOOL) DBGetContactSettingByte(NULL, ModuleName, "UploadOnStartup", FALSE) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_GLOBALCOMPARE, (BOOL) DBGetContactSettingByte(NULL, ModuleName, "GlobalCompare", FALSE) ? BST_CHECKED : BST_UNCHECKED);
			
			char buffer[256];
			GetStringFromDatabase("User", "", buffer, sizeof(buffer));
			SetWindowText(GetDlgItem(hWnd, IDC_USERNAME), buffer);
			
			GetStringFromDatabase("Password", "", buffer, sizeof(buffer));
			SetWindowText(GetDlgItem(hWnd, IDC_PASSWORD), buffer);
			
			bOptionsInitializing = 0;
		
			return TRUE;
			break;
		}
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_USERNAME:
				case IDC_PASSWORD:
				{
					if ((HIWORD(wParam) == EN_CHANGE))// || (HIWORD(wParam) == CBN_SELENDOK)) //CBN_EDITCHANGE
						{
							if (!bOptionsInitializing)
              {
                SendMessage(GetParent(hWnd), PSM_CHANGED, 0, 0);
              }
						}
					
					break;
				}
				
				case IDC_UPLOADONSTARTUP:
				case IDC_GLOBALCOMPARE:
				{
					SendMessage(GetParent(hWnd), PSM_CHANGED, 0, 0);
				
					break;
				}
			}
			
			break;
		}
			
		case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->idFrom)
			{
				case 0:
				{
					switch (((LPNMHDR)lParam)->code)
					{
						case PSN_APPLY:
						{
							char buffer[256];
							GetWindowText(GetDlgItem(hWnd, IDC_USERNAME), buffer, sizeof(buffer));
							DBWriteContactSettingString(NULL, ModuleName, "User", buffer);
							
							GetWindowText(GetDlgItem(hWnd, IDC_PASSWORD), buffer, sizeof(buffer));
							DBWriteContactSettingString(NULL, ModuleName, "Password", buffer);
							
							DBWriteContactSettingByte(NULL, ModuleName, "UploadOnStartup", IsDlgButtonChecked(hWnd, IDC_UPLOADONSTARTUP) ? TRUE : FALSE);
							DBWriteContactSettingByte(NULL, ModuleName, "GlobalCompare", IsDlgButtonChecked(hWnd, IDC_GLOBALCOMPARE) ? TRUE : FALSE);
							
							break;
						}
					}
					
					break;
				}
			}
			
			break;
		}
	}
	
	return 0;
}

#include "commctrl.h"

void LoadPlugins(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_PLUGINS);
	
	ListView_DeleteAllItems(hList);
	
	LVITEM item = {0};
	
	item.mask = LVIF_TEXT;
	
	for (int i = 0; i < newerPlugins.Count(); i++)
	{
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = newerPlugins[i]->name;
		
		ListView_InsertItem(hList, &item);
		
		ListView_SetItemText(hList, i, 1, newerPlugins[i]->oldVersion);
		
		ListView_SetItemText(hList, i, 2, newerPlugins[i]->newVersion);
		
		if (newerPlugins[i]->URL)
		{
			ListView_SetItemText(hList, i, 3, newerPlugins[i]->URL);
		}
		
	}
}

int CALLBACK PluginsListSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYUP:
		{
			if (wParam == VK_ESCAPE)
			{
				SendMessage(GetParent(hWnd), WM_CLOSE, 0, 0);
			}
			
			break;
		}
		
		case WM_SYSKEYDOWN:
		{
			if (wParam == 'X')
			{
				SendMessage(GetParent(hWnd), WM_CLOSE, 0, 0);
			}
			
			break;
		}
		
		case WM_LBUTTONDBLCLK:
		{
			char buffer[1024];
			int count = ListView_GetItemCount(hWnd);
			
			for (int i = 0; i < count; i++)
			{
				if (ListView_GetItemState(hWnd, i, LVIS_SELECTED))
				{
					ListView_GetItemText(hWnd, i, 3, buffer, sizeof(buffer));
					
					if (strlen(buffer) > 0)
					{
						ShellExecute(hWnd, "open", buffer, NULL, NULL, SW_SHOW);
					}
				}
			}
			
		
			break;
		}
	}
	
	return CallWindowProc(OldPluginsListProc, hWnd, msg, wParam, lParam);
}

void AddAnchorWindowToDeferList(HDWP &hdWnds, HWND window, RECT *rParent, WINDOWPOS *wndPos, int anchors)
{
	RECT rChild = AnchorCalcPos(window, rParent, wndPos, anchors);
	hdWnds = DeferWindowPos(hdWnds, window, HWND_NOTOPMOST, rChild.left, rChild.top, rChild.right - rChild.left, rChild.bottom - rChild.top, SWP_NOZORDER);
}

int CALLBACK DlgProcNewerPlugins(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hWnd);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hiVIUploaderIcon);
			
			HWND hList = GetDlgItem(hWnd, IDC_PLUGINS);
			
			ListView_SetExtendedListViewStyleEx(hList, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
			
			OldPluginsListProc = (WNDPROC) SetWindowLong(hList, GWL_WNDPROC, (LONG) PluginsListSubclassProc);
			
			LVCOLUMN col = {0};
			
			col.mask = LVCF_TEXT | LVCF_WIDTH;
			for (int i = 0; i < cPluginsColumns; i++)
			{
				col.pszText = TranslateTS(szPluginsColumns[i]);
				col.cx = sizePluginsColumns[i];
				
				ListView_InsertColumn(hList, i, &col);
			}
			
			LoadPlugins(hWnd);
			
			char message[512];
			mir_snprintf(message, sizeof(message), Translate("%d newer plugins"), newerPlugins.Count());
			SetWindowText(hWnd, message);
			
			return TRUE;
			break;
		}
		
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
		
			break;
		}
		
		case WM_WINDOWPOSCHANGING:
		{
			HDWP hdWnds = BeginDeferWindowPos(2);
			RECT rParent;
			WINDOWPOS *wndPos = (WINDOWPOS *) lParam;
			GetWindowRect(hWnd, &rParent);

			if (wndPos->cx < MIN_PLUGINS_WIDTH)
			{
				wndPos->cx = MIN_PLUGINS_WIDTH;
			}
			if (wndPos->cy < MIN_PLUGINS_HEIGHT)
			{
				wndPos->cy = MIN_PLUGINS_HEIGHT;
			}
			
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_CLOSE), &rParent, wndPos, ANCHOR_RIGHT | ANCHOR_BOTTOM);
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_PLUGINS), &rParent, wndPos, ANCHOR_ALL);
			
			EndDeferWindowPos(hdWnds);			
		
			break;
		}
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_CLOSE:
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				
					break;
				}
			}
		
			break;
		}
	}
	
	
	return 0;
}


int CALLBACK DlgProcPopup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
		{
			case WM_COMMAND:
				{
					switch (HIWORD(wParam))
						{
							case STN_CLICKED:
								{
									CallServiceSync(MS_VIUPLOADER_CREATE_DIALOG, 0, 0);
									
									PUDeletePopUp(hWnd);
									
									break;
								}
						}
					break;
				}
				
			case WM_CONTEXTMENU:
				{
					PUDeletePopUp(hWnd);
					
					break;
				}
		}
		
	return DefWindowProc(hWnd, msg, wParam, lParam);
}