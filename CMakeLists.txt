cmake_minimum_required(VERSION 3.16)

project(Melon VERSION 0.1)

set(KF_MIN_VERSION "6.4")
set(QT_MIN_VERSION "6.5")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KDE_COMPILERSETTINGS_LEVEL 6.0)

include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS XmlGui I18n ItemModels Config KIO WindowSystem Baloo Bookmarks)
find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Core Gui Widgets Quick QuickControls2 Qml Sql Concurrent)
find_package(Nongurigaeru)

set_package_properties(KF6 PROPERTIES TYPE REQUIRED PURPOSE "KF6")
set_package_properties(Qt6 PROPERTIES TYPE REQUIRED PURPOSE "Qt6")
set_package_properties(Nongurigaeru PROPERTIES TYPE REQUIRED PURPOSE "Nongurigaeru")

add_subdirectory(src)
add_subdirectory("application data")

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
