# CMakeの最小バージョンの指定
cmake_minimum_required(VERSION 3.11)

# プロジェクトの作成
project(hook LANGUAGES CXX)

# C++14 を使用する
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ライブラリの作成
add_library(hook SHARED

	${PROJECT_SOURCE_DIR}/hook.cpp
	${PROJECT_SOURCE_DIR}/hook.def
	
)

# インクルードディレクトリの指定
target_include_directories(hook PRIVATE ${PROJECT_SOURCE_DIR})