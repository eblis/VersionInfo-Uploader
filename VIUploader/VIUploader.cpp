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

#include "commonheaders.h"

char ModuleName[] = "VIUpload";
HINSTANCE hInstance;

PLUGINLINK *pluginLink;

HICON hiVIUploaderIcon = NULL;

PLUGININFOEX pluginInfo = {
	sizeof(PLUGININFOEX),
	__PLUGIN_DISPLAY_NAME,
	VERSION,
	__DESC,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	1, //unicode aware
	0,
	{0x2ffaa200, 0x8f89, 0x49c2, {0xb6, 0x89, 0xf6, 0x1a, 0xcd, 0xa8, 0x67, 0xa1}} //{2ffaa200-8f89-49c2-b689-f61acda867a1}
	}; //not used
	
OLD_MIRANDAPLUGININFO_SUPPORT;

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion) 
{
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_VIUPLOADER, MIID_LAST};

extern "C" __declspec(dllexport) const MUUID *MirandaPluginInterfaces()
{
	return interfaces;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	
	InitServices();
	
	HookEvents();
	
	InitializeMirandaMemFunctions();
	
	hiVIUploaderIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VIUPLOADER));
	
	CLISTMENUITEM mi = { 0 };
	
	mi.cbSize = sizeof(mi);
	mi.position = mi.popupPosition = 2000089998;
	mi.flags = 0;
	mi.hIcon = hiVIUploaderIcon;
	mi.pszName = Translate("Upload Version Info report");
	mi.pszService = MS_VIUPLOADER_UPLOAD_REPORT;
	CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&mi);
	
	return 0;
}

extern "C" int __declspec(dllexport) Unload()
{
	UnhookEvents();
	
	DestroyServices();
	
	return 0;
}

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
		{
			DisableThreadLibraryCalls(hinstDLL);
		}
		
	return TRUE;
}
