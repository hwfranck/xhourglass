cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set (XHOURGLASS_VERSION 0.0.1.0)
set (XH_TRANSLATIONS_PATH resources/translations)
set(PROJECT_NAME xhourglass)

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ADDITIONAL_SRCS_TARGET)
set(TARGET_LIBS)
set(ADDITIONAL_QT_PKGS)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_PREFIX_PATH C:/Qt/Qt5.14.2/5.14.2/msvc2017_64)

	string(APPEND ADDITIONAL_QT_PKGS "Winextras") # QWinTaskbarProgress
	list(APPEND TARGET_LIBS Qt5::WinExtras)

	add_library(WinToast STATIC third-party/wintoast/wintoastlib.cpp)
	list(APPEND TARGET_LIBS WinToast)
endif()

find_package(Qt5 REQUIRED COMPONENTS
	Widgets
	Multimedia # QSound
	${ADDITIONAL_QT_PKGS}
	LinguistTools # qt5_add_translations
)

add_compile_definitions(
	$<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
	$<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>
	$<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/W4)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wall -Wextra)
endif()

set( Srcs
	main.cpp
	ui/mainwindow.cpp
	core/xhtimer.cpp
	core/xhduration.cpp
	core/xh_utils.cpp
	core/parse.cpp
)

set( QRCs
	resources/${PROJECT_NAME}.qrc
)

set(TS_FILES
	${XH_TRANSLATIONS_PATH}/xhourglass_lang.ts # default = english
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_de_DE.ts
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_en_US.ts
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_es_ES.ts
	${XH_TRANSLATIONS_PATH}/xhourglass_lang_fr_FR.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "../src/resources/translations")
qt5_add_translation(tranlationFiles ${TS_FILES})

qt5_add_resources(QRC_Srcs ${QRCs} )

set( WIN_RES
	resources/win_res.rc
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	string(APPEND ADDITIONAL_SRCS_TARGET "${WIN_RES}")
endif()

add_executable(${PROJECT_NAME}
	${Srcs}
	${QRC_Srcs}
	${ADDITIONAL_SRCS_TARGET}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/graphics/xh_icon_48x.png ${CMAKE_CURRENT_BINARY_DIR}/xhourglass.png
)

list(APPEND TARGET_LIBS
	Qt5::Widgets
	Qt5::Multimedia
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${TARGET_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE ON
)
