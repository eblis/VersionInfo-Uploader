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

#include "hooked_events.h"
#include "services.h"

HANDLE hModulesLoaded;
HANDLE hOptionsInitialize;

#define HOST "http://eblis.tla.ro/projects"

#define VIUPLOADER_VERSION_URL HOST "/miranda/VIUploader/updater/VIUploader.html"

#ifdef DYNAMIC_BUILD
#define VIUPLOADER_UPDATE_URL HOST "/miranda/VIUploader/updater/VIUploader (dynamic).zip"
#else
#define VIUPLOADER_UPDATE_URL HOST "/miranda/VIUploader/updater/VIUploader.zip"
#endif

#define VIUPLOADER_VERSION_PREFIX "VersionInfo Uploader version "

int HookEvents()
{
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	hOptionsInitialize = HookEvent(ME_OPT_INITIALISE, OnOptionsInitialise);
	
	return 0;
}

int UnhookEvents()
{
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hOptionsInitialize);
	
	return 0;
}

int OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	char buffer[1024];
	Update update = {0};
	update.cbSize = sizeof(Update);
	update.szComponentName = __PLUGIN_DISPLAY_NAME;
	update.pbVersion = (BYTE *) CreateVersionString(VERSION, buffer);
	update.cpbVersion = strlen((char *) update.pbVersion);
	update.szUpdateURL = UPDATER_AUTOREGISTER;
	update.szBetaVersionURL = VIUPLOADER_VERSION_URL;
	update.szBetaUpdateURL = VIUPLOADER_UPDATE_URL;
	update.pbBetaVersionPrefix = (BYTE *) VIUPLOADER_VERSION_PREFIX;
	update.cpbBetaVersionPrefix = strlen(VIUPLOADER_VERSION_PREFIX);
	CallService(MS_UPDATE_REGISTER, 0, (LPARAM) &update);
	
	if (DBGetContactSettingByte(NULL, ModuleName, "UploadOnStartup", 0))
	{
		UploadReportService(0, 0);
	}
	
	return 0;
}

int OnOptionsInitialise(WPARAM wParam, LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = { 0 };
	
	odp.cbSize = sizeof(odp);
	odp.position = 100000000;
	odp.hInstance = hInstance;
	
	odp.pszTemplate = MAKEINTRESOURCE(IDD_VIUPLOADER_OPTIONS);
	odp.pszTitle = Translate("VersionInfo Uploader");
	odp.pszGroup = Translate("Services");
	odp.groupPosition = 910000000;
	odp.flags=ODPF_BOLDGROUPS;
	odp.pfnDlgProc = DlgProcVIUploaderOpts;
	
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	return 0;
}
