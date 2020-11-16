# アイコンのキャプチャについて
私の知る限りでは、Windowsでキャプチャを行う方法は以下の3通りある

- BitBlt
- Windows.Graphics.Capture
- DwmGetDxSharedSurface

`BitBlt` はキャプチャした画像にαチャンネルが含まれないので、今回は使用しない。

`Windows.Graphics.Capture` はトップレベルウィンドウ?以外のウィンドウハンドルからキャプチャしようとすると
`IGraphicsCaptureItemInterop::CreateForWindow` 関数で例外が飛ばされるっぽいので、デスクトップのアイコンのキャプチャには使用できなさそうだった。
このときに使用したプログラムはこれ。
[ScreenCaptureforHWND](https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/tree/master/cpp/ScreenCaptureforHWND)

`DwmGetDxSharedSurface` は任意のウィンドウハンドルをαチャンネル付きでキャプチャできる関数である。
ただし、これは非公開APIなのであまり使用したくはない。
しかし、他に方法がないので今回はこれを使用することにした。
