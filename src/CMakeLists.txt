cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set (XHOURGLASS_VERSION 0.0.1.0)
set (XH_TRANSLATIONS_PATH resources/translations)

project(xhourglass LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH C:/Qt/Qt5.14.2/5.14.2/msvc2017_64)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
	Widgets
	Multimedia # QSound
	Winextras # QWinTaskbarProgress
	LinguistTools # qt5_add_translations
)

set( Srcs
	main.cpp
	ui/mainwindow.cpp
	core/xhtimer.cpp
	core/xhduration.cpp
	core/xh_utils.cpp
	core/parse.cpp
)

set( QRCs
	resources/xhourglass.qrc
)

set(TS_FILES
	${XH_TRANSLATIONS_PATH}/xhourglass_lang.ts # default = english
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_en_US.ts
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_fr_FR.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "../src/resources/translations")
qt5_add_translation(tranlationFiles ${TS_FILES})

qt5_add_resources(QRC_Srcs ${QRCs} )

set( WIN_RES
	resources/win_res.rc
)

add_executable(xhourglass
	${Srcs}
	${QRC_Srcs}
	${WIN_RES}
)

target_link_libraries(xhourglass PRIVATE Qt5::Widgets Qt5::Multimedia Qt5::WinExtras)

set_target_properties(xhourglass PROPERTIES
	WIN32_EXECUTABLE ON
)