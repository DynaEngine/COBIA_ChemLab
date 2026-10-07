cmake_minimum_required(VERSION 3.19)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
	Core
	Gui
	Widgets
	OpenGL
	OpenGLWidgets
	Svg
)

find_package(qwt REQUIRED QUIET
	HINTS "${CMAKE_SOURCE_DIR}/third_party/qwt7/lib/cmake/qwt"
)

set(SOURCES
	main.cpp
	mainwindow.cpp
	mainwindow.h
	ribbonbar.cpp
	ribbonbar.h
	simulationmanager.cpp
	simulationmanager.h
	panels/logpanel.cpp
	panels/logpanel.h
	panels/unitoperationpanel.cpp
	panels/unitoperationpanel.h
	dialogs/propertypackagedialog.cpp
	dialogs/propertypackagedialog.h
	dialogs/propertyresearchdialog.cpp
	dialogs/propertyresearchdialog.h
	dialogs/unitinfodialog.cpp
	dialogs/unitinfodialog.h
	dialogs/configdialog.cpp
	dialogs/configdialog.h
	canvas/FlowsheetScene.cpp
	canvas/FlowsheetScene.h
	canvas/FlowsheetGraphicsView.cpp
	canvas/FlowsheetGraphicsView.h
	canvas/UnitOperationItem.cpp
	canvas/UnitOperationItem.h
	canvas/PortItem.cpp
	canvas/PortItem.h
	canvas/StreamItem.cpp
	canvas/StreamItem.h
	canvas/StreamBlockItem.cpp
	canvas/StreamBlockItem.h
	resources/chemlab.qrc
)

add_executable(ChemLab WIN32 ${SOURCES})

target_link_libraries(ChemLab PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	qwt::plot
	QWindowKit
	ChemEngine
)

if(WIN32)
	target_link_libraries(ChemLab PRIVATE ${WIN32_EXTRA_LIBS})
endif()

if(MINGW)
	target_link_options(ChemLab PRIVATE ${MINGW_EXTRA_LINK_OPTIONS})
endif()

target_include_directories(ChemLab PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${ChemEngine_SOURCE_DIR}/include
	${COBIA_INCLUDE_DIR}
)

target_compile_features(ChemLab PRIVATE cxx_std_17)

install(TARGETS ChemLab
	RUNTIME DESTINATION .
	BUNDLE DESTINATION .
)

install(FILES
	"$<TARGET_FILE:qwt::core>"
	"$<TARGET_FILE:qwt::plot>"
	DESTINATION .
)

set(WINDEPLOYQT_EXE "D:/program/QT6/6.9.1/mingw_64/bin/windeployqt.exe")

install(CODE "
	message(STATUS \"Running windeployqt to deploy Qt libraries...\")
	set(QT_PLUGIN_DIR \"D:/program/QT6/6.9.1/mingw_64/plugins\")
	set(QT_QML_DIR \"D:/program/QT6/6.9.1/mingw_64/qml\")
	execute_process(
		COMMAND \"${WINDEPLOYQT_EXE}\"
			--no-translations
			--no-compiler-runtime
			--plugindir \"\${CMAKE_INSTALL_PREFIX}/plugins\"
			\"\${CMAKE_INSTALL_PREFIX}/ChemLab.exe\"
		WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}\"
		RESULT_VARIABLE _r
	)
	if(NOT _r EQUAL 0)
		message(WARNING \"windeployqt returned \${_r}\")
	else()
		message(STATUS \"Qt deployment done.\")
	endif()
")