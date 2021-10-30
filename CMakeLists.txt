# CMakeの最小バージョンの指定
cmake_minimum_required(VERSION 3.16)

# プロジェクトの作成
project(GravityDesktop2 LANGUAGES CXX)

# C++17を使用する
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 依存ライブラリを読み込む
set(LIBRARIES_PATH ${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${LIBRARIES_PATH}/DirectXTK)
add_subdirectory(${LIBRARIES_PATH}/box2d)

include(FetchContent)
FetchContent_Declare(
	gRPC
	GIT_REPOSITORY https://github.com/grpc/grpc
	GIT_TAG        v1.41.1
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)
set(_REFLECTION grpc++_reflection)
set(_GRPC_GRPCPP grpc++)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
get_filename_component(hw_proto "./protos/imu.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/imu.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/imu.pb.h")
set(hw_grpc_srcs  "${CMAKE_CURRENT_BINARY_DIR}/imu.grpc.pb.cc")
set(hw_grpc_hdrs  "${CMAKE_CURRENT_BINARY_DIR}/imu.grpc.pb.h")
add_custom_command(
	OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
	COMMAND ${_PROTOBUF_PROTOC}
	ARGS
		--grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
		--cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
		-I "${hw_proto_path}"
		--plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
		"${hw_proto}"
	DEPENDS "${hw_proto}"
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(hw_grpc_proto
	${hw_grpc_srcs}
	${hw_grpc_hdrs}
	${hw_proto_srcs}
	${hw_proto_hdrs}
)
target_link_libraries(hw_grpc_proto
	${_REFLECTION}
	${_GRPC_GRPCPP}
	${_PROTOBUF_LIBPROTOBUF}
)

# hookライブラリの作成
add_library(hook SHARED

	${LIBRARIES_PATH}/hook/hook.cpp
	${LIBRARIES_PATH}/hook/hook.def

)
target_include_directories(hook PRIVATE ${PROJECT_SOURCE_DIR})

# 全てのcppファイルを再帰的に検索する
file(GLOB_RECURSE ALL_CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 実行ファイルを作成するためのマクロ
macro(gd_make_app)

	# 引数の解析
	set(oneValueArgs APP_NAME SOURCE)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

	# メインプログラムを作成する
	add_executable("${ARG_APP_NAME}" WIN32 "${ARG_SOURCE}" "${ALL_CPP_FILES}")

	# インクルードディレクトリを指定する
	target_include_directories("${ARG_APP_NAME}" PRIVATE

		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/lib/hook

	)

	# プリコンパイル済みヘッダを指定する
	target_precompile_headers("${ARG_APP_NAME}" PRIVATE ${PROJECT_SOURCE_DIR}/include/utils/pch.h)

	# 必要なlibファイルを追加する
	target_link_libraries("${ARG_APP_NAME}" PRIVATE

		DirectXTK
		box2d
		hook

		d3d11.lib dxgi.lib dxguid.lib uuid.lib
		kernel32.lib user32.lib
		comdlg32.lib advapi32.lib shell32.lib
		ole32.lib oleaut32.lib comctl32.lib
		runtimeobject.lib

		hw_grpc_proto
		${_REFLECTION}
		${_GRPC_GRPCPP}
		${_PROTOBUF_LIBPROTOBUF}

	)

	# コンパイルオプションを指定する
	target_compile_definitions("${ARG_APP_NAME}" PRIVATE

		# unicode文字を使用する (デフォルトはワイド文字)
		_UNICODE
		UNICODE

		# ターゲットを Windows 10 以降に指定 (これがないとDirectXTKがエラーを出す)
		WINVER=_WIN32_WINNT_WIN10
		_WIN32_WINNT=_WIN32_WINNT_WIN10

	)

	target_compile_options("${ARG_APP_NAME}" PRIVATE

		/permissive- /Zc:__cplusplus

		# 一部の警告を無視する
		"/wd4061" "/wd4365" "/wd4514" "/wd4571" "/wd4668" "/wd4710" "/wd4820" "/wd5039" "/wd5045" "/wd26812"

	)

endmacro()

# メインプログラムの作成
gd_make_app(APP_NAME GravityDesktop2 SOURCE "${PROJECT_SOURCE_DIR}/main.cpp")

# デフォルトのスタートアッププロジェクトを GravityDesktop2 に変更する
set_target_properties(GravityDesktop2 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:GravityDesktop2>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GravityDesktop2)

# 全サンプルプログラムの作成
file(GLOB_RECURSE ALL_EXAMPLE_FILES "${PROJECT_SOURCE_DIR}/examples/*.cpp")
foreach(SOURCE IN LISTS ALL_EXAMPLE_FILES)

	get_filename_component(EXAMPLE_NAME "${SOURCE}" NAME_WLE)
	gd_make_app(APP_NAME "example_${EXAMPLE_NAME}" SOURCE "${SOURCE}")

endforeach()
