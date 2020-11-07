#pragma once

#include "pch.h"


namespace gd
{
	struct Mouse
	{
		POINTS point  = {};       // 現在のフレームのマウス座標
		POINTS point_ = {};       // 1フレーム前のマウス座標委

		POINTS lDragStart;        // 現在のフレームのaLDragStart

		POINTS rDragStart;        // 現在のフレームのaRDragStart

		POINTS mDragStart;        // 現在のフレームのaMDragStart

		bool lPressed  = false;  // 現在のフレームの左クリック
		bool lPressed_ = false;  // 1フレーム前の左クリック

		bool rPressed  = false;  // 現在のフレームの右クリック
		bool rPressed_ = false;  // 1フレーム前の右クリック

		bool mPressed  = false;  // 現在のフレームの中央クリック
		bool mPressed_ = false;  // 1フレーム前の中央クリック

		bool lDouble = false;    // 現在のフレームの左ダブルクリック

		bool rDouble = false;    // 現在のフレームの右ダブルクリック

		bool mDouble = false;    // 現在のフレームの中央ダブルクリック
	};

	class MouseProcess
	{
	public:
		MouseProcess();
		virtual ~MouseProcess();
		void OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void nextFrame();
		Mouse getMouseStatus();

	private:
		POINTS aMouse = {};       // 最新のマウス座標
		POINTS bMouse = {};       // 現在のフレームのマウス座標
		POINTS cMouse = {};       // 1フレーム前のマウス座標委

		POINTS aLDragStart;       // 左ドラッグを開始した時点でのマウス座標
		POINTS bLDragStart;       // 現在のフレームのaLDragStart

		POINTS aRDragStart;       // 右ドラッグを開始した時点でのマウス座標
		POINTS bRDragStart;       // 現在のフレームのaRDragStart

		POINTS aMDragStart;       // 中央ドラッグを開始した時点でのマウス座標
		POINTS bMDragStart;       // 現在のフレームのaMDragStart

		bool aLPressed = false;  // 最新の左クリック
		bool bLPressed = false;  // 現在のフレームの左クリック
		bool cLPressed = false;  // 1フレーム前の左クリック

		bool aRPressed = false;  // 最新の右クリック
		bool bRPressed = false;  // 現在のフレームの右クリック
		bool cRPressed = false;  // 1フレーム前の右クリック

		bool aMPressed = false;  // 最新の中央クリック
		bool bMPressed = false;  // 現在のフレームの中央クリック
		bool cMPressed = false;  // 1フレーム前の中央クリック
		
		bool aLDouble = false;   // 最新の左ダブルクリック
		bool bLDouble = false;   // 現在のフレームの左ダブルクリック

		bool aRDouble = false;   // 最新の右ダブルクリック
		bool bRDouble = false;   // 現在のフレームの右ダブルクリック

		bool aMDouble = false;   // 最新の中央ダブルクリック
		bool bMDouble = false;   // 現在のフレームの中央ダブルクリック

		bool isCapture = false;  // SetCaptureが有効な場合はtrue
	};
}