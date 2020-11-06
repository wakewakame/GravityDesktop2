![demo](image/demo.gif)

# Gravity Desktop 2
大学4年のときに作成した、デスクトップにあるアイコンが落下運動をするジョークソフトです。

「Gravity Desktop 2」の「2」はその名の通り2代目のプログラムという意味です。
初代 [Gravity Desktop](https://github.com/wakewakame/GravityDesktop) は C++ を勉強し始めたころに作成したもので、このプログラムにはあまり納得できていませんでした。
今は少し C++ が上達したので、改めてこれを作り直すことにしました。


# ダウンロード
作成中...


# 操作方法
作成中...


# ビルド方法
Git、CMake、Visual Studioが必要になります。
これらがインストールされていない場合は、事前にインストールしておいてください。

コマンドプロンプトを立ち上げ、任意の場所で以下のコマンドを実行します。
1行ずつ実行することをお勧めします。

```cmd
git clone https://github.com/wakewakame/GravityDesktop2.git
cd GravityDesktop2
git submodule update --init --recursive
mkdir build
cd build
cmake ..
```

コマンドを実行すると `build` フォルダの中に `GravityDesktop2.sln` が生成されるので、これを開いてプログラムをビルドします。


# ファイル構成

| ファイル名                    | 説明 |
| :---                       | :--- |
| main.cpp                   | このプログラムにmain関数が書かれています。 |
| include/                   | C++のヘッダファイルがまとまっています。 |
| src/                       | C++のソースファイルがまとまっています。 |
| lib/                       | 依存ライブラリがここに格納されます。 |
| lib/hook/                  | 他のウィンドウのマウスやキーボードのイベントをフックするライブラリです。 |
| lib/DirectXTK/             | 描画ライブラリです。 |
| lib/directx-vs-templates/  | DirectXの開発環境の雛形がまとまっています。 |
| lib/box2d/                 | 二次元の物理演算ライブラリです。 |
| examples/                  | サンプルプログラムがまとまっています。 |
| CMakeLists.txt             | CMakeLists.txtです。CMakeで環境構築するので、それに使います。 |