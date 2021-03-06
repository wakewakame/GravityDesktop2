![demo](image/demo.gif)  

# Gravity Desktop 2
**このプログラムはまだ完成していません。**  
**以下は完成後に使用される予定の文章です。**  

大学4年のときに作成した、デスクトップにあるアイコンが落下運動をするジョークソフトです。

「Gravity Desktop 2」の「2」はその名の通り2代目のプログラムという意味です。
初代 [Gravity Desktop](https://github.com/wakewakame/GravityDesktop) は C++ を勉強し始めたころに作成したもので、このプログラムにはあまり納得できていませんでした。
今は少し C++ が上達したので、改めてこれを作り直すことにしました。

# 仕組み
デスクトップの画面を再現したウィンドウを全画面に表示することでアイコンが落下しているように見せています。
具体的な処理の流れは以下の通りです。  

1. Win32APIを用いてデスクトップ上の全てのアイコンの座標を取得する
2. 1の座標を用いてBox2Dの仮想空間の中に長方形を生成する
3. Windowsの隠しAPIであるDwmGetDxSharedSurfaceを用いてデスクトップのアイコンをαチャンネル付きでキャプチャする
4. 2で生成した落下する長方形に、3で取得したアイコンのキャプチャを貼り付ける
5. ウィンドウに対して送られてくるマウスイベントなどを座標変換したのちにデスクトップに転送する
6. 3に戻る

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


# 実装日記

- 2020/11/05 : Win32APIで単一のウィンドウを生成できるプログラムを実装
- 2020/11/06 : Win32APIで複数のウィンドウを生成できるプログラムを実装
- 2020/11/08 : 簡素なUIコンポーネントクラスの実装
- 2020/11/08 : 簡素なグラフィッククラスの実装 (多角形の図形を描画できる機能を持つ)
- 2020/11/08 : 簡素なマウスイベントの処理を実装 (マウスの座標に図形を描くなどができる)
