/*

����̓f�X�N�g�b�v�̃A�C�R�������擾����v���O�����ł��B

*/

#pragma once

namespace gd
{
	class Icon {
	public:
		Icon(HWND hWnd, HANDLE hProc, LPVOID ptr, size_t index)
			: hWnd(hWnd), hProc(hProc), ptr(ptr), index(index) {}
		virtual ~Icon() {}

		const size_t index;  // 0����n�܂�A�C�R���̔ԍ�

		bool update()
		{
			// �A�C�R�������͈͎̔擾
			iconArea_.left = LVIR_ICON;
			if (WriteProcessMemory(hProc, ptr, &iconArea_, sizeof(RECT), NULL) == 0) return 1;
			SendMessage(hWnd, LVM_GETITEMRECT, index, (LPARAM)ptr);
			if (ReadProcessMemory(hProc, ptr, &iconArea_, sizeof(RECT), NULL) == 0) return 1;

			// �A�C�R�������ƃe�L�X�g�����̗����͈͎̔擾
			itemArea_.left = LVIR_BOUNDS;
			if (WriteProcessMemory(hProc, ptr, &itemArea_, sizeof(RECT), NULL) == 0) return 1;
			SendMessage(hWnd, LVM_GETITEMRECT, index, (LPARAM)ptr);
			if (ReadProcessMemory(hProc, ptr, &itemArea_, sizeof(RECT), NULL) == 0) return 1;

			// �I���A�t�H�[�J�X���擾
			UINT ret = SendMessage(hWnd, LVM_GETITEMSTATE, index, LVIS_SELECTED | LVIS_FOCUSED);
			isSelect_ = ((ret & LVIS_SELECTED) == LVIS_SELECTED);
			isFocus_ = ((ret & LVIS_FOCUSED) == LVIS_FOCUSED);

			// �z�b�g�A�C�e�����擾
			isHot_ = (index == SendMessage(hWnd, LVM_GETHOTITEM, 0, 0));

			// �����I��
			return 0;
		}

		bool select(UINT state, UINT stateMast)
		{
			// �A�C�e���̃X�e�[�^�X�ύX
			LVITEM lvi{};  // �������q���ĂԂƃ[���ŏ����������
			lvi.state = state;
			lvi.stateMask = stateMast;
			if (WriteProcessMemory(hProc, ptr, &lvi, sizeof(LVITEM), NULL) == 0) return 1; // ptr��item���
			SendMessage(hWnd, LVM_SETITEMSTATE, index, (LPARAM)ptr);

			// �t�H�[�J�X��L��������ׂɃE�B���h�E���A�N�e�B�u�ɂ���
			SendMessage(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);

			// ���
			isSelect_ = 1;
			isFocus_ = 1;

			// �����I��
			return 0;
		}

		bool hot()
		{
			// �z�b�g�A�C�e���ݒ�
			SendMessage(hWnd, LVM_SETHOTITEM, index, 0);

			// ���
			isHot_ = 1;

			// �����I��
			return 0;
		}

		RECT iconArea() const { return iconArea_; };
		RECT itemArea() const { return itemArea_; };
		bool isSelect() const { return isSelect_; };
		bool isFocus() const { return isFocus_; };
		bool isHot() const { return isHot_; }

	private:
		const HWND   hWnd;   // SysListView�̃E�B���h�E�n���h��
		const HANDLE hProc;  // SysListView�̃v���Z�X�n���h��
		const LPVOID ptr;    // SysListView�ɐ��������������̐擪�|�C���^
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
		 * �S�ẴA�C�R�����擾����
		 * @return ���������0�A���s�����1���Ԃ�
		 */
		bool update()
		{
			// ����������Ă��Ȃ���Ώ�����
			if (nullptr == ptr && pAlloc()) return 1;
			assert(nullptr != hProc);
			assert(nullptr != ptr);

			// �f�X�N�g�b�v�̃A�C�R�����擾
			auto numItems = ListView_GetItemCount(hWnd);

			// �A�C�R�����ɕύX������Δz�񒷍X�V
			if (icons.size() != numItems)
			{
				icons.clear();
				icons.reserve(numItems);
				for (size_t i = 0; i < numItems; i++)
				{
					icons.emplace_back(hWnd, hProc, ptr, i);
				}
			}

			// �S�A�C�R�����X�V
			for (Icon& icon : icons)
			{
				if (icon.update()) return 1;
			}

			return 0;
		}

		/**
		 * �S�ẴA�C�R���𖢑I����Ԃɂ���
		 */
		void unselect()
		{
			// �A�C�e���̃X�e�[�^�X�ύX
			LVITEM lvi{};  // �������q���ĂԂƃ[���ŏ����������
			lvi.state = 0;
			lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
			if (WriteProcessMemory(hProc, ptr, &lvi, sizeof(LVITEM), NULL) == 0) return;
			SendMessage(hWnd, LVM_SETITEMSTATE, -1, (LPARAM)ptr);

			// �t�H�[�J�X��L��������ׂɃE�B���h�E���A�N�e�B�u�ɂ���
			SendMessage(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}

		/**
		 * �S�ẴA�C�R�����u�}�E�X�J�[�\�����G��Ă��Ȃ��v��Ԃɂ���
		 */
		void unhot() { PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(0, 0)); }

	private:
		const HWND hWnd;          // SysListView�̃E�B���h�E�n���h��
		HANDLE hProc;             // SysListView�̃v���Z�X�n���h��
		LPVOID ptr;               // SysListView�ɐ��������������̐擪�|�C���^
		std::vector<Icon> icons;  // SysListView���̑S�ẴA�C�R��

		/**
		 * ListView�̃v���Z�X�Ƀ������m�ۂ���
		 * @return ���������0�A���s�����1���Ԃ�
		 */
		bool pAlloc()
		{
			pFree();

			if (nullptr == hProc)
			{
				// ListView�̃v���Z�XID�̎擾
				DWORD pId;
				GetWindowThreadProcessId(hWnd, &pId);
				if (NULL == pId) return 1;

				// ListView�̃v���Z�X�n���h���擾
				hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, pId);
				if (nullptr == hProc) return 1;
			}

			// ListView�̃v���Z�X�Ƀ������m��
			constexpr size_t allocSize = std::max(sizeof(RECT), sizeof(LVITEM));
			ptr = VirtualAllocEx(hProc, NULL, allocSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (nullptr == ptr) return 1;

			return 0;
		}

		/**
		 * ListView�̃v���Z�X�Ɋm�ۂ������������������
		 */
		void pFree()
		{
			if (nullptr == ptr) return;

			icons.clear();

			// ListView�̃v���Z�X�Ɋm�ۂ������������������
			VirtualFreeEx(hProc, ptr, 0, MEM_RELEASE);
			ptr = nullptr;
		}
	};
}
