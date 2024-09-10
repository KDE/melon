add_executable(org.kde.Melon
	app.cpp
	columnsmodel.cpp
	dbus.cpp
	dirmodel.cpp
	document.cpp
	dragthing.cpp
	folderclassifier.cpp
	main.cpp
	menubar.cpp
	placesdelegate.cpp
	previewer.cpp
	reverser.cpp
	toolbardelegate.cpp
	window.cpp
	windowrestorer.cpp
	model_wrapper.cpp
	foreigns.h
	closesignalwindow.cpp
	../data/app.qrc
)
qt_add_qml_module(org.kde.Melon
  URI org.kde.melon
  VERSION 1.0
)
target_link_libraries(org.kde.Melon
	PRIVATE
		KF6::XmlGui KF6::ConfigCore KF6::I18n KF6::KIOCore KF6::KIOFileWidgets KF6::ItemModels KF6::KIOWidgets KF6::WindowSystem KF6::Baloo KF6::Bookmarks KF6::BookmarksWidgets
		Qt::Core Qt::Gui Qt::GuiPrivate Qt::Quick Qt::QuickWidgets Qt::QuickPrivate Qt::Qml Qt::Sql Qt::Concurrent
		KF6::NGLib KF6::NGLibQuick
)
install(TARGETS org.kde.Melon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
