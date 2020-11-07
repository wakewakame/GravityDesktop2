# CMakeの最小バージョンの指定
cmake_minimum_required(VERSION 3.16)

# プロジェクトの作成
project(GravityDesktop2 LANGUAGES CXX)

# 依存ライブラリを読み込む
set(LIBRARIES_PATH ${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${LIBRARIES_PATH}/hook)
add_subdirectory(${LIBRARIES_PATH}/DirectXTK)
add_subdirectory(${LIBRARIES_PATH}/box2d)

# C++14を使用する
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 全てのcppファイルを再帰的に検索する
file(GLOB_RECURSE ALL_CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# メインプログラムを作成する
add_executable(GravityDesktop2 WIN32

	${ALL_CPP_FILES}
	${PROJECT_SOURCE_DIR}/main.cpp

)

# インクルードディレクトリを指定する
target_include_directories(GravityDesktop2 PRIVATE ${PROJECT_SOURCE_DIR}/include)

# プリコンパイル済みヘッダを指定する
target_precompile_headers(GravityDesktop2 PRIVATE ${PROJECT_SOURCE_DIR}/include/pch.h)

# GravityDesktop2 に必要なlibファイルを追加する
target_link_libraries(GravityDesktop2 PRIVATE

	DirectXTK
	box2d
	hook

    d3d11.lib dxgi.lib dxguid.lib uuid.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib comctl32.lib

)

# デフォルトのスタートアッププロジェクトを GravityDesktop2 に変更する
set_target_properties(GravityDesktop2 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:GravityDesktop2>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GravityDesktop2)

# コンパイルオプションを指定する
target_compile_definitions(GravityDesktop2 PRIVATE

	# unicode文字を使用する (デフォルトはワイド文字)
	_UNICODE
	UNICODE

	# ターゲットを Windows 7 以降に指定 (これがないとDirectXTKがエラーを出す)
	WINVER=_WIN32_WINNT_WIN7
	_WIN32_WINNT=_WIN32_WINNT_WIN7

)

target_compile_options(GravityDesktop2 PRIVATE

	/permissive- /Zc:__cplusplus

	# 一部の警告を無視する
	"/wd4061" "/wd4365" "/wd4514" "/wd4571" "/wd4668" "/wd4710" "/wd4820" "/wd5039" "/wd5045" "/wd26812"

)