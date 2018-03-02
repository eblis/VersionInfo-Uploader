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

#include "services.h"

HANDLE hsUploadReport = NULL;
HANDLE hsRegisterUser = NULL;
HANDLE hsCreateDialog = NULL;

CRITICAL_SECTION csUpload;

int InitServices()
{
	//CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	CURLcode res = curl_global_init_mem(CURL_GLOBAL_ALL, malloc, free, realloc, _strdup, calloc);
	//char buffer[128];
	//sprintf(buffer, "%d", res);
	//MessageBox(0, buffer, buffer, MB_OK);
	
	hsUploadReport = CreateServiceFunction(MS_VIUPLOADER_UPLOAD_REPORT, UploadReportService);
	hsRegisterUser = CreateServiceFunction(MS_VIUPLOADER_REGISTER_USER, RegisterUserService);
	
	hsCreateDialog = CreateServiceFunction(MS_VIUPLOADER_CREATE_DIALOG, CreateNewerPluginsDialogService);

	InitializeCriticalSection(&csUpload);
	
	return 0;
}

int DestroyServices()
{
	DestroyServiceFunction(hsUploadReport);
	DestroyServiceFunction(hsRegisterUser);
	
	DestroyServiceFunction(hsCreateDialog);

	DeleteCriticalSection(&csUpload);

	return 0;
}

static char *globalReceiveBuffer = (char *) malloc(1024 * 512);

static size_t GetHTTPData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = size * nmemb;
	strncat(globalReceiveBuffer, (char *) ptr, written);

	return written;
}

int MarkUploadComplete(char *uploadHash)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	
	if (!curl) { return 0; }

	char site[1024];
	mir_snprintf(site, sizeof(site), "%s/uploader.php?key=%s", HTTP_SITE, uploadHash);
	
	curl_easy_setopt(curl, CURLOPT_URL, site);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	
	*globalReceiveBuffer = 0;
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetHTTPData);
	
	res = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);
	
	if (res == CURLE_OK)
	{
		return (strstr(globalReceiveBuffer, "successful") != NULL);
	}
	
	return 0;
}

char *GetUploadPassword(char *password, int size)
{
	char *data = "34hDM4Nkj4Hdn57uJDN2hu5jDG9c9gSZ6VT6dhHGg20kVJu8298USF";
	int count = 10;
	memcpy(password, data + 25, count);
	password[count] = 0;
	
	return password;
}

char *GetUploadUserAndPassword(char *uploadAccount, int size)
{
	char password[128];
	GetUploadPassword(password, sizeof(password));
	mir_snprintf(uploadAccount, size, "%s:%s", "data_miranda-vi_org", password);
	
	return uploadAccount;
}

int CreateAndSendReport()
{
	char *report = NULL;
	int result = 0;
	
	//get the VI report
	switch (globalReceiveBuffer[0])
	{
		case '2':
		{
			ShowMessage(__PLUGIN_DISPLAY_NAME, "Invalid password");
		
			break;
		}
	
		case '3':
		{
			ShowMessage(__PLUGIN_DISPLAY_NAME, "Upload was denied"); 
		
			break;
		}
		
		case '0':
		{
			ShowMessage(__PLUGIN_DISPLAY_NAME, "Username does not exist");
			
			break;
		}
		
		case '5':
		{
			ShowMessage(__PLUGIN_DISPLAY_NAME, "Daily upload limit exceeded");
		
			break;
		}
		
		case '1':
		{
			CallService(MS_VERSIONINFO_GETINFO, FALSE, (LPARAM) &report);
			
			if ((report) && (globalReceiveBuffer[0] == '1'))//if there's something to upload
			{
				CURLcode res;
				CURL *curl = curl_easy_init();
				
				if (!curl) { return 0; }
			
				//get the file name and the upload hash we received from the server
				char fileName[128] = {0};
				char uploadHash[128] = {0};
				char *p = strstr(globalReceiveBuffer, ";");
				char *q = strstr(++p, ";");
				*q++ = 0;
				
				strncpy(fileName, p, sizeof(fileName));
				p = strstr(q, ";");
				
				*p = 0;
				strncpy(uploadHash, q, sizeof(uploadHash));
				
				//clear the receive buffer
				*globalReceiveBuffer = 0;
				
				char site[512] = {0};
				mir_snprintf(site, sizeof(site), "%s/%s", FTP_SITE, fileName);
				
				curl_easy_setopt(curl, CURLOPT_URL, site);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
				
				char uploadAccount[256] = {0};
				curl_easy_setopt(curl, CURLOPT_USERPWD, GetUploadUserAndPassword(uploadAccount, sizeof(uploadAccount)));
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
				
				//create a temp file with the report
				char dir[MAX_PATH];
				GetCurrentDirectory(sizeof(dir), dir);
				char *tmpFileName = _tempnam(dir, "VIUpload - ");
				FILE *ftmp = fopen(tmpFileName, "wt+");
				fprintf(ftmp, "%s", report); 
				fflush(ftmp);
				fseek(ftmp, 0, SEEK_SET);
			
				curl_easy_setopt(curl, CURLOPT_READDATA, ftmp);

				res = curl_easy_perform(curl);
				
				fclose(ftmp);
				DeleteFile(tmpFileName);
			
				if (res == CURLE_OK)
				{
					result = MarkUploadComplete(uploadHash);
				}
				
				curl_easy_cleanup(curl);
				
				MirandaFree(report);
			}
			
			break;
		}
	}
	
	return result;
}

int GlobalCompare()
{
	CURL *curl = curl_easy_init();
	CURLcode res;
	
	if (!curl) { return 0; }
	
	char user[256] = {0};
	GetStringFromDatabase("User", "", user, sizeof(user));
	if (strlen(user) > 0)
	{
		char site[512] = {0};
		mir_snprintf(site, sizeof(site), "http://%s.%s/global/", user, SITE);
		
		curl_easy_setopt(curl, CURLOPT_URL, site);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		*globalReceiveBuffer = 0;		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetHTTPData);
		
		curl_easy_perform(curl);
		
		curl_easy_cleanup(curl);
		
		//hack
		//RegExp maybe in the future ?
		char *data = strstr(globalReceiveBuffer, ">Global compare<");
		*globalReceiveBuffer = 0;

		const char *search = "\"red left w50perc\"><span class=\"v white\">";
		
		newerPlugins.Clear();
		
		while (data)
		{
			char *p = data = strstr(data, search);
			char *q;
			char *tr;

			if (p)
			{
				PVIPlugin plugin = new TVIPlugin();
				p += strlen(search);
				q = strstr(p, "</span>");
				
				tr = strstr(p, "</tr>");
				
				if ((q) && (q < tr))
				{
					*q = 0;
					plugin->name = _strdup(p);
					p = q + 1;
					
					char *x = strstr(p, "Older version ("); //user version
					
					if ((x) && (x < tr))
					{
						p = x + 15;
						
						q = strstr(p, ")");
						if ((q) && (q < tr))
						{
							*q = 0;
							plugin->oldVersion = _strdup(p);
							
							p = q + 1;
						}
					}
					
					x = strstr(p, "Newer version ("); //newer version without link
					if ((x) && (x < tr))
					{
					
						p = x + 15;
						
						q = strstr(p, ")");
						if ((q) && (q < tr))
						{
							*q = 0;
							plugin->newVersion = _strdup(p);
							
							p = q + 1;
						}
					}
					else{
						x = strstr(p, "Newer version <a href=\""); //newer version with link
						if ((x) && (x < tr))
						{
							p = x + 24;
							q = strstr(p, "\"");
							if ((q) && (q < tr))
							{
								*q = 0;
								
								char buffer[512];
								mir_snprintf(buffer, sizeof(buffer), "%s/%s", HTTP_SITE, p);
								
								plugin->URL = _strdup(buffer);
								
								p = q + 1;
							}
							
							x = strstr(p, ">("); //get the version number
							if ((x) && (x < tr))
							{
								p = x + 2;
								
								q = strstr(p, ")");
								if ((q) && (q < tr))
								{
									*q = 0;
									
									plugin->newVersion = _strdup(p);
									
									p = q + 1;
								}
							}
						}
						else{
							x = strstr(p, "Unverified version (");
							if ((x) && (x < tr))
							{
								p = x + 20;
								q = strstr(p, ")");
								
								if ((q) && (q < tr))
								{
									*q = 0;
									plugin->newVersion = _strdup(p);
									plugin->URL = _strdup("Unverified");
									
									p = q + 1;
								}
							}
						}
					}
				}
				
				newerPlugins.Add(plugin);
				
				data = tr + 1; //new item
			}
		}
		
		POPUPDATA pd = {0};
		mir_snprintf(pd.lpzContactName, MAX_CONTACTNAME, Translate("VersionInfo Uploader - newer plugins available"));
		mir_snprintf(pd.lptzText, MAX_SECONDLINE, Translate("There are %d newer plugins available"), newerPlugins.Count());
		pd.PluginWindowProc = (WNDPROC) DlgProcPopup;
		pd.lchIcon = hiVIUploaderIcon;
		PUAddPopUp(&pd);

	}
	
	return 0;
}

DWORD WINAPI UploadReportThread(void *param)
{
	EnterCriticalSection(&csUpload);
	
	char user[256] = {0};
	char password[256] = {0};
	
	if (!ServiceExists(MS_VERSIONINFO_GETINFO))
	{
		ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("This plugin requires Version Info plugin to be present.\nIt cannot work without it."));
		return 1;
	}
	
	
	ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("Uploading Version Info report to server"));
	
	globalReceiveBuffer[0] = 0;
	
	GetStringFromDatabase("User", "", user, sizeof(user));
	GetStringFromDatabase("Password", "", password, sizeof(password));

	if (strlen(user) > 0)
	{
		CURL *curl = curl_easy_init();
		
		unsigned char passwordHash[32] = {0};
		
		md5((unsigned char *) password, strlen(password), passwordHash);
		
		char *hash = BinToHex(16, passwordHash);
		
		char site[1024] = {0};
		mir_snprintf(site, sizeof(site), "%s/program.php?name=%s&key=%s&v=2%%2E3", HTTP_SITE, user, hash);

		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, site);
			
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
			
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetHTTPData);
			
			curl_easy_perform(curl);
			
			if (CreateAndSendReport()) //success
			{
				ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("Successfully uploaded version info report"));
				
				if (DBGetContactSettingByte(NULL, ModuleName, "GlobalCompare", FALSE))
				{
					GlobalCompare();
				}
			}
			else{
				ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("Failed to upload version info report"));
			}
			
			curl_easy_cleanup(curl);
		}
		
		if (hash)
		{
			free(hash);
			hash = NULL;
		}
	}
	else{
		ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("Please provide a user and a password before"));
	}
	
	LeaveCriticalSection(&csUpload);

	return 0;
}

int UploadReportService(WPARAM wParam, LPARAM lParam)
{
	DWORD threadID;
	HANDLE hTread = CreateThread(NULL, NULL, UploadReportThread, NULL, NULL, &threadID);
	CloseHandle(hTread);
	
	return 0;
}

int RegisterUserService(WPARAM wParam, LPARAM lParam)
{
	char *user = (char *) wParam;
	char *password = (char *) lParam;
	
	DBWriteContactSettingString(NULL, ModuleName, "User", user);
	DBWriteContactSettingString(NULL, ModuleName, "Password", password);
	
	ShowMessage(__PLUGIN_DISPLAY_NAME, Translate("Successfully registered user"));
	
	return 0;
}

int CreateNewerPluginsDialogService(WPARAM wParam, LPARAM lParam)
{
	HWND hNewerPlugins = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PLUGINS), NULL, DlgProcNewerPlugins);
	ShowWindow(hNewerPlugins, SW_SHOW);
	
	return 0;
}