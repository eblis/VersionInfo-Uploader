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

#ifndef M_VIUPLOADER_SERVICES_H
#define M_VIUPLOADER_SERVICES_H

#include "commonheaders.h"
#include "list.h"

#define MS_VIUPLOADER_UPLOAD_REPORT "VIUploader/Report/Upload"
#define MS_VIUPLOADER_REGISTER_USER "VIUploader/User/Register"

#define MS_VIUPLOADER_CREATE_DIALOG "VIUploader/Dialog/Show"

#define SITE "miranda-vi.org"
#define HTTP_SITE "http://" SITE
//#define FTP_SITE "ftp://data_miranda-vi_org:G9c9gSZ6VT@" SITE
#define FTP_SITE "ftp://" SITE

int InitServices();
int DestroyServices();

int UploadReportService(WPARAM wParam, LPARAM lParam);
int RegisterUserService(WPARAM wParam, LPARAM lParam);

int CreateNewerPluginsDialogService(WPARAM wParam, LPARAM lParam);

#endif //M_VIUPLOADER_SERVICES_H
