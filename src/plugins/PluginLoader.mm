#include <Foundation/Foundation.h>
#include <Foundation/NSArray.h>

#include "Plugin.h"

NSArray<id<Plugin>>* loadPlugins()
{
	NSBundle* main = [NSBundle mainBundle];
	NSArray* all = [main pathsForResourcesOfType:@"bundle" inDirectory:@"../PlugIns"];
	NSMutableArray<id<Plugin>>* loadedPlugins = [NSMutableArray array];

	for (NSString* path in all) {
		NSBundle* pluginBundle = [NSBundle bundleWithPath:path];
		[pluginBundle load];

		Class principalClass = [pluginBundle principalClass];
		if (![principalClass conformsToProtocol:@protocol(Plugin)]) {
			continue;
		}

		id<Plugin> plugin = [[principalClass alloc] init];
		[loadedPlugins addObject:plugin];
		[plugin release], plugin = nil;
		pluginBundle = nil;
	}

	return loadedPlugins;
}