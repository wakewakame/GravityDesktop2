![demo](image/demo.gif)  

# Gravity Desktop 2

大学4年のときに作成した、デスクトップにあるアイコンが落下運動をするジョークソフトです。

「Gravity Desktop 2」の「2」はその名の通り2代目のプログラムという意味です。
初代 [Gravity Desktop](https://github.com/wakewakame/GravityDesktop) は C++ を勉強し始めたころに作成したもので、このプログラムにはあまり納得できていませんでした。
今は少し C++ が上達したので、改めてこれを作り直すことにしました。


# ダウンロード
[GravityDesktop2 v1.0.0](https://github.com/wakewakame/GravityDesktop2/releases/download/v1.0.0/GravityDesktop2.zip)  

ダウンロード後、 `GravityDesktop2.zip` を展開し、 `GravityDesktop.exe` を実行するとデスクトップのアイコンが落下します。


# 操作方法
- 実行するとデスクトップのアイコンが落下します。
- 右クリックでアイコンをつまむことができます。
- 左クリックで選択中の全てのアイコンを寄せ集めることができます。
- 1キーを押しながらドラッグで重力を操作できます。
- 2キーを押すと無重力状態になります。
- 3キーを押すと全てのアイコンがマウスに寄ります。
- 4キーを押すと初期位置に移動します。
- Escキーで終了します。


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
cmake --build . --target GravityDesktop2 --target hook --config Release
```

コマンドを実行すると `build/Release` フォルダの中に `GravityDesktop2.exe` が生成されます。


# 配布用zipファイルの作成

```cmd
cd build\Release
cmake -E copy ..\..\README.md .\
cmake -E copy ..\..\CREDIT .\
cmake -E copy ..\..\LICENSE .\
cmake -E tar "cfv" GravityDesktop2.zip --format=zip GravityDesktop2.exe hook.dll README.md CREDIT LICENSE
```

コマンドを実行すると `build/Release` フォルダの中に `GravityDesktop2.zip` が生成されます。


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


# 仕組み
デスクトップの画面を再現したウィンドウを全画面に表示することでアイコンが落下しているように見せています。
具体的な処理の流れは以下の通りです。  

1. Win32APIを用いてデスクトップ上の全てのアイコンの座標を取得する
2. 1の座標を用いてBox2Dの仮想空間の中に長方形を生成する
3. Windowsの隠しAPIであるDwmGetDxSharedSurfaceを用いてデスクトップのアイコンをαチャンネル付きでキャプチャする
4. 2で生成した落下する長方形に、3で取得したアイコンのキャプチャを貼り付ける
5. ウィンドウに対して送られてくるマウスイベントなどを座標変換したのちにデスクトップに転送する
6. 3に戻る
