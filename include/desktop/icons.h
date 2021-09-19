/*

これはデスクトップのアイコン情報を取得するプログラムです。

*/

#pragma once

namespace gd
{
	class Icon {
	public:
		Icon(HWND hWnd, HANDLE hProc, LPVOID ptr, size_t index)
			: hWnd(hWnd), hProc(hProc), ptr(ptr), index(index) {}
		virtual ~Icon() {}

		const size_t index;  // 0から始まるアイコンの番号

		bool update()
		{
			// アイコン部分の範囲取得
			iconArea_.left = LVIR_ICON;
			if (WriteProcessMemory(hProc, ptr, &iconArea_, sizeof(RECT), NULL) == 0) return 1;
			SendMessage(hWnd, LVM_GETITEMRECT, index, (LPARAM)ptr);
			if (ReadProcessMemory(hProc, ptr, &iconArea_, sizeof(RECT), NULL) == 0) return 1;

			// アイコン部分とテキスト部分の両方の範囲取得
			itemArea_.left = LVIR_BOUNDS;
			if (WriteProcessMemory(hProc, ptr, &itemArea_, sizeof(RECT), NULL) == 0) return 1;
			SendMessage(hWnd, LVM_GETITEMRECT, index, (LPARAM)ptr);
			if (ReadProcessMemory(hProc, ptr, &itemArea_, sizeof(RECT), NULL) == 0) return 1;

			// 選択、フォーカス情報取得
			UINT ret = SendMessage(hWnd, LVM_GETITEMSTATE, index, LVIS_SELECTED | LVIS_FOCUSED);
			isSelect_ = ((ret & LVIS_SELECTED) == LVIS_SELECTED);
			isFocus_ = ((ret & LVIS_FOCUSED) == LVIS_FOCUSED);

			// ホットアイテム情報取得
			isHot_ = (index == SendMessage(hWnd, LVM_GETHOTITEM, 0, 0));

			// 処理終了
			return 0;
		}

		bool select(UINT state, UINT stateMast)
		{
			// アイテムのステータス変更
			LVITEM lvi{};  // 初期化子を呼ぶとゼロで初期化される
			lvi.state = state;
			lvi.stateMask = stateMast;
			if (WriteProcessMemory(hProc, ptr, &lvi, sizeof(LVITEM), NULL) == 0) return 1; // ptrにitem代入
			SendMessage(hWnd, LVM_SETITEMSTATE, index, (LPARAM)ptr);

			// フォーカスを有効化する為にウィンドウをアクティブにする
			SendMessage(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);

			// 代入
			isSelect_ = 1;
			isFocus_ = 1;

			// 処理終了
			return 0;
		}

		bool hot()
		{
			// ホットアイテム設定
			SendMessage(hWnd, LVM_SETHOTITEM, index, 0);

			// 代入
			isHot_ = 1;

			// 処理終了
			return 0;
		}

		RECT iconArea() const { return iconArea_; };
		RECT itemArea() const { return itemArea_; };
		bool isSelect() const { return isSelect_; };
		bool isFocus() const { return isFocus_; };
		bool isHot() const { return isHot_; }

	private:
		const HWND   hWnd;   // SysListViewのウィンドウハンドル
		const HANDLE hProc;  // SysListViewのプロセスハンドル
		const LPVOID ptr;    // SysListViewに生成したメモリの先頭ポインタ
		RECT iconArea_{};
		RECT itemArea_{};
		bool isSelect_ = false;
		bool isFocus_ = false;
		bool isHot_ = false;
	};

	class Icons
	{
	public:
		Icons(HWND sysListViewHWnd)
			: hWnd(sysListViewHWnd), hProc(nullptr), ptr(nullptr) {}
		virtual ~Icons() { pFree(); }

		Icon& operator[](const size_t n) { return icons[n]; }
		size_t size() const { return icons.size(); }
		std::vector<gd::Icon>::iterator begin() { return icons.begin(); }
		std::vector<gd::Icon>::iterator end() { return icons.end(); }

		/**
		 * 全てのアイコンを取得する
		 * @return 成功すると0、失敗すると1が返る
		 */
		bool update()
		{
			// 初期化されていなければ初期化
			if (nullptr == ptr && pAlloc()) return 1;
			assert(nullptr != hProc);
			assert(nullptr != ptr);

			// デスクトップのアイコン数取得
			auto numItems = ListView_GetItemCount(hWnd);

			// アイコン数に変更があれば配列長更新
			if (icons.size() != numItems)
			{
				icons.clear();
				icons.reserve(numItems);
				for (size_t i = 0; i < numItems; i++)
				{
					icons.emplace_back(hWnd, hProc, ptr, i);
				}
			}

			// 全アイコン情報更新
			for (Icon& icon : icons)
			{
				if (icon.update()) return 1;
			}

			return 0;
		}

		/**
		 * 全てのアイコンを未選択状態にする
		 */
		void unselect()
		{
			// アイテムのステータス変更
			LVITEM lvi{};  // 初期化子を呼ぶとゼロで初期化される
			lvi.state = 0;
			lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
			if (WriteProcessMemory(hProc, ptr, &lvi, sizeof(LVITEM), NULL) == 0) return;
			SendMessage(hWnd, LVM_SETITEMSTATE, -1, (LPARAM)ptr);

			// フォーカスを有効化する為にウィンドウをアクティブにする
			SendMessage(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}

		/**
		 * 全てのアイコンを「マウスカーソルが触れていない」状態にする
		 */
		void unhot() { PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(0, 0)); }

	private:
		const HWND hWnd;          // SysListViewのウィンドウハンドル
		HANDLE hProc;             // SysListViewのプロセスハンドル
		LPVOID ptr;               // SysListViewに生成したメモリの先頭ポインタ
		std::vector<Icon> icons;  // SysListView内の全てのアイコン

		/**
		 * ListViewのプロセスにメモリ確保する
		 * @return 成功すると0、失敗すると1が返る
		 */
		bool pAlloc()
		{
			pFree();

			if (nullptr == hProc)
			{
				// ListViewのプロセスIDの取得
				DWORD pId;
				GetWindowThreadProcessId(hWnd, &pId);
				if (NULL == pId) return 1;

				// ListViewのプロセスハンドル取得
				hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, pId);
				if (nullptr == hProc) return 1;
			}

			// ListViewのプロセスにメモリ確保
			constexpr size_t allocSize = std::max(sizeof(RECT), sizeof(LVITEM));
			ptr = VirtualAllocEx(hProc, NULL, allocSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (nullptr == ptr) return 1;

			return 0;
		}

		/**
		 * ListViewのプロセスに確保したメモリを解放する
		 */
		void pFree()
		{
			if (nullptr == ptr) return;

			icons.clear();

			// ListViewのプロセスに確保したメモリを解放する
			VirtualFreeEx(hProc, ptr, 0, MEM_RELEASE);
			ptr = nullptr;
		}
	};
}
