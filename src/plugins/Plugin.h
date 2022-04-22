#pragma once

#include <QString>
#include <QQuickItem>
#include <Foundation/Foundation.h>

@protocol Plugin<NSObject>

@end

@protocol FolderBar<NSObject>

@property QQuickItem* item;

@end

@protocol FolderBarPlugin<NSObject>

- (bool)applicableForFolder:(const QString&)path;
- (id<FolderBar>)createFolderBar;

@end

