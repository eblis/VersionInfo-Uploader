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

#ifndef M_VIUPLOADER_LIST_H
#define M_VIUPLOADER_LIST_H

#include "commonheaders.h"

struct TVIPlugin{
	char *name;
	char *oldVersion;
	char *newVersion;
	char *URL;
	
	TVIPlugin()
	{
		name = oldVersion = newVersion = URL = NULL;
	}
	
	~TVIPlugin()
	{
		if (name)
		{
			free(name);
		}
		
		if (oldVersion)
		{
			free(oldVersion);
		}
		
		if (newVersion)
		{
			free(newVersion);
		}
		
		if (URL)
		{
			free(URL);
		}
	}
};

typedef TVIPlugin *PVIPlugin;

class CVIPluginsList
{
	protected:
		PVIPlugin *plugins;
		int size;
		int count;
		
		void Enlarge(int amount);
		void EnsureCapacity();
		
	public:
		CVIPluginsList();
		~CVIPluginsList();
		
		void Add(PVIPlugin plugin);
		void Clear();
		
		int Count();
		
		PVIPlugin operator [](int index);
};

extern CVIPluginsList &newerPlugins;

#endif //M_VIUPLOADER_LIST_H