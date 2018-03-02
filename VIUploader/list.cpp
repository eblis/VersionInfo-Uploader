#include "list.h"

CVIPluginsList &newerPlugins = CVIPluginsList();

void CVIPluginsList::Enlarge(int amount)
{
	size += amount;
	plugins = (PVIPlugin *) realloc(plugins, size * sizeof(PVIPlugin));
}

void CVIPluginsList::EnsureCapacity()
{
	if (count >= size - 1)
	{
		Enlarge(size / 2);
	}
}

CVIPluginsList::CVIPluginsList()
{
	plugins = NULL;
	size = 0;
	count = 0;
	Enlarge(10);
}

CVIPluginsList::~CVIPluginsList()
{
	Clear();
	
	free(plugins);
	plugins = NULL;
}

void CVIPluginsList::Clear()
{
	if (plugins)
	{
		for (int i = 0; i < count; i++)
		{
			delete plugins[i];
		}
		
		count = 0;
	}
}

void CVIPluginsList::Add(PVIPlugin plugin)
{
	EnsureCapacity();
	plugins[count++] = plugin;
}

PVIPlugin CVIPluginsList::operator [](int index)
{
	if ((index < 0) || (index >= count))
	{
		return NULL;
	}
	
	return plugins[index];
}

int CVIPluginsList::Count()
{
	return count;
}