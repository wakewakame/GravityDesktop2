/*

����̓f�X�N�g�b�v�ɂȂ肷�܂��E�B���h�E�𐶐�����R���|�[�l���g�ł��B

*/

#pragma once

#include "utils/types.h"
#include "components/root_component.h"
#include "desktop/capture.h"
#include "hook.h"

namespace gd
{
	class FakeDesktopComponent : public RootComponent
	{
	public:
		struct DesktopHwnds
		{
			HWND listview = NULL;        // �f�X�N�g�b�v�̃A�C�R����\������E�B���h�E�n���h��
			HWND wallpaper = NULL;       // �f�X�N�g�b�v�̕ǎ���\������E�B���h�E�n���h��
			std::vector<HWND> taskbars;  // �e���j�^�[�̃^�X�N�o�[�̃E�B���h�E�n���h��
		};

		FakeDesktopComponent() {}
		virtual ~FakeDesktopComponent() {
			/*
			����
			�ȉ��̏����͏I�����ɐ�΂ɌĂ΂�Ăق����̂ŁAComponent::exit�֐��Ɉړ������Ă͂����Ȃ��B
			*/

			// �f�X�N�g�b�v�̑c��E�B���h�E��s�����ɂ���
			if (desktopHwnds.isErr || desktopHwnds.isNone) { return; }
			show(GetAncestor(desktopHwnds.value.listview, GA_ROOTOWNER));

			// �t�b�N���O��
			removeHook();
		}

		DWORD getWindowStyle() const override final { return WS_POPUP | WS_CHILD; }

		void init(gd::Graph& graph)
		{
			RootComponent::init(graph);
			HWND hWnd = getHwnd();

			// �f�X�N�g�b�v�̃E�B���h�E�n���h�����擾����
			desktopHwnds = getDesktopHwnd();
			if (desktopHwnds.isErr || desktopHwnds.isNone)
			{
				Windows::error(desktopHwnds.description);
				closeWindow();
				return;
			}

			// ���g�̐e�E�B���h�E��ǎ��̃E�B���h�E�ɐݒ�
			SetParent(hWnd, desktopHwnds.value.wallpaper);

			// ���g�̃E�B���h�E�̈ʒu�ƃT�C�Y�𒲐�����
			RECT rc;
			GetWindowRect(desktopHwnds.value.wallpaper, &rc);
			SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0);
			setSize(rc.right - rc.left, rc.bottom - rc.top);

			// �f�X�N�g�b�v�A�C�R���𓧖��ɂ���
			HWND ancestor = GetAncestor(desktopHwnds.value.listview, GA_ROOTOWNER);
			hide(ancestor);

			// �ǎ��E�B���h�E�ɃL�[�C�x���g��}�E�X�C�x���g���͂��悤�ɂ���
			EnableWindow(desktopHwnds.value.wallpaper, TRUE);

			// �ǎ��E�B���h�E�ւ̃}�E�X�C�x���g�ƃL�[�C�x���g�̃t�b�N������
			if (insertHook(desktopHwnds.value.wallpaper, hWnd))
			{
				Windows::error(L"�f�X�N�g�b�v�̃t�b�N�Ɏ��s���܂����B");
				closeWindow();
				return;
			}
		}
		void render(gd::Graph& graph, gd::Mouse& mouse) override
		{
			RootComponent::render(graph, mouse);
		}
		void exit(gd::Graph& graph) override
		{
			RootComponent::exit(graph);
		}
		void resize(int width, int height) override
		{
			RootComponent::resize(width, height);
		}

		// �f�X�N�g�b�v�̃E�B���h�E�n���h�����擾����
		static Result<DesktopHwnds> getDesktopHwnd()
		{
			DesktopHwnds result;

			/*
			����
			Windows�̃f�X�N�g�b�v�́u�ǎ��v�Ɓu�A�C�R���v�����ꂼ��ʂ̃E�B���h�E
			�ɕ\�������Ƃ�(���A�Ƃ���)�ƁA1�̃E�B���h�E�ɂ܂Ƃ߂ĕ\�������
			�Ƃ�(���B�Ƃ���)��2�p�^�[������B����̏d�̓f�X�N�g�b�v�ł́A�A�C�R��
			�̓A�C�R���ŃL���v�`�����A�ǎ��͕ǎ��ŃL���v�`���������̂ŁA���A�̕K
			�v������B�ȉ��͏��B�����A�ɕύX���邽�߂̏����ł���B

			�Q�l��: https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows-plus
			*/

			HWND Progman = FindWindow(L"Progman", NULL);
			if (NULL == Progman) return Result<DesktopHwnds>::Err(L"Progman�̃E�B���h�E�n���h���擾�Ɏ��s���܂����B");
			HWND SHELLDLL_DefView = FindWindowEx(Progman, NULL, L"SHELLDLL_DefView", L"");
			// �f�X�N�g�b�v�̕ǎ��ƃA�C�R����1�̃E�B���h�E�ɂ܂Ƃ܂��Ă���ꍇ
			if (NULL != SHELLDLL_DefView)
			{
				// �ǎ��ƃA�C�R�������ꂼ��ʂ̃E�B���h�E�ɕ���������
				if (0 == SendMessageTimeout(Progman, 0x052C, 0, 0, 0x0, 10000, NULL))
				{
					return Result<DesktopHwnds>::Err(L"�ǎ��ƃA�C�R���̕����Ɏ��s���܂����B");
				}
			}

			// WorkerW��SysListView32�n���h���擾
			HWND WorkerW = NULL;
			SHELLDLL_DefView = NULL;
			while (true)
			{
				WorkerW = FindWindowEx(NULL, WorkerW, L"WorkerW", L"");
				if (NULL == WorkerW)
				{
					SHELLDLL_DefView = NULL;
					break;
				}
				SHELLDLL_DefView = FindWindowEx(WorkerW, NULL, L"SHELLDLL_DefView", L"");
				if (NULL != SHELLDLL_DefView)
				{
					break;
				}
			}
			if (NULL == SHELLDLL_DefView)
			{
				return Result<DesktopHwnds>::Err(L"�f�X�N�g�b�v�̃E�B���h�E�n���h���̎擾�Ɏ��s���܂����B");
			}

			// �f�X�N�g�b�v�̃A�C�R����\������ListView�̃n���h���擾
			result.listview = FindWindowEx(SHELLDLL_DefView, NULL, L"SysListView32", NULL);
			if (NULL == result.listview)
			{
				return Result<DesktopHwnds>::Err(L"�f�X�N�g�b�v��ListView�̃n���h���̎擾�Ɏ��s���܂����B");
			}

			// �f�X�N�g�b�v�̕ǎ���\������E�B���h�E�n���h���̎擾
			result.wallpaper = NULL;
			while (true)
			{
				result.wallpaper = FindWindowEx(NULL, result.wallpaper, L"WorkerW", L"");
				if (result.wallpaper == NULL) {
					break;
				}
				if (GetParent(result.wallpaper) == Progman) break;
			}
			if (NULL == result.wallpaper)
			{
				return Result<DesktopHwnds>::Err(L"�ǎ��̃E�B���h�E�n���h���̎擾�Ɏ��s���܂����B");
			}

			// �S�Ẵ^�X�N�o�[�̃E�B���h�E�n���h�����擾
			result.taskbars;
			HWND buf = NULL;
			// ���C�����j�^�[�̃^�X�N�o�[���擾
			while (true)
			{
				buf = FindWindowEx(NULL, buf, L"Shell_TrayWnd", NULL);
				if (NULL == buf) break;
				result.taskbars.push_back(buf);
			}
			// �T�u���j�^�[�̃^�X�N�o�[���擾
			while (true)
			{
				buf = FindWindowEx(NULL, buf, L"Shell_SecondaryTrayWnd", NULL);
				if (NULL == buf) break;
				result.taskbars.push_back(buf);
			}

			return Result<DesktopHwnds>::Ok(result);
		}

	protected:
		Result<DesktopHwnds> desktopHwnds;

		/**
		 * �E�B���h�E�𓧖��ɂ���
		 * @param �����ɂ���E�B���h�E�̃n���h��
		 * @return ���������true�A���s�����false���Ԃ�
		 */
		bool hide(HWND hwnd) {
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
			if (0 == style) return false;
			SetLastError(0);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
			if (0 != GetLastError()) return false;
			if (0 == SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) return false;
			return true;
		}

		/**
		 * �E�B���h�E��s�����ɂ���
		 * @param �s�����ɂ���E�B���h�E�̃n���h��
		 * @return ���������true�A���s�����false���Ԃ�
		 */
		bool show(HWND hwnd) {
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
			if (0 == style) return false;
			SetLastError(0);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
			if (0 != GetLastError()) return false;
			if (0 == SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) return false;
			return true;
		}
	};
}
