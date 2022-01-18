#include "keyboard.h"
#include <stdio.h>

#ifdef _WIN32

#pragma comment(lib, "User32.lib")
using namespace keyboard;

static OnKeyPressCallback onKeyPressCallback = nullptr;
static DWORD keyboardThreadId = 0;

namespace keyboard
{
	bool PressKeyDetectMethod(KeyCode &keyCode, KeyStatus &keyStatus)
	{
		const char *printFlag = nullptr;
		static char buf[64];
		if (keyCode >= 0x30 && keyCode <= 0x5A)
			printf("%s '%c' 键\r\n", keyStatus == WM_KEYUP ? "释放" : "按下", keyCode);
		else
			printf("%s '<%ld>' 键\r\n", keyStatus == WM_KEYUP ? "释放" : "按下", keyCode);
		return true;
	}
}

OnKeyPressCallback keyboard::PressKeyDetect = PressKeyDetectMethod;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT *)lParam; // 包含低级键盘输入事件信息
	DWORD code = ks->vkCode;
	onKeyPressCallback(ks->vkCode, wParam);
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void keyboard::OnKeyPress(OnKeyPressCallback callback)
{
	onKeyPressCallback = callback;
	HINSTANCE hInst = GetModuleHandle(NULL);
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
	keyboardThreadId = GetCurrentThreadId();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hook);
}

void keyboard::CancelOnKeyPress() { PostThreadMessage(keyboardThreadId, WM_QUIT, 0x00, 0x00); }

// void Forward()
// {
// 	keybd_event('W', 0x00, 0x00, 0);
// }

// void StopForward()
// {
// 	keybd_event('W', 0x1F, KEYEVENTF_KEYUP, 0);
// }

// std::string get_time()
// {
// 	SYSTEMTIME sys;
// 	GetLocalTime(&sys);
// 	char time[1024] = {0};
// 	sprintf(time, "[%4d/%02d/%02d %02d:%02d:%02d]", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

// 	return std::string(time);
// }

// int KeyboardListen()
// {
// 	HINSTANCE hInst = GetModuleHandle(NULL);
// 	HHOOK g_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
// 	MSG msg;
// 	while (GetMessage(&msg, NULL, 0, 0))
// 	{
// 		TranslateMessage(&msg);
// 		DispatchMessage(&msg);
// 	}
// 	UnhookWindowsHookEx(g_Hook);
// 	return 0;
// }

// using namespace std;

// int KeyboardEvent()
// {
// 	cout << "请把光标移动到输入框" << endl;
// 	Sleep(2000);
// 	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
// 	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); //模拟单击鼠标左键

// 	for (;;)
// 	{
// 		//字母 A 按下
// 		keybd_event(65, 0x1E, 0, 0);
// 		keybd_event(65, 0x1E, KEYEVENTF_KEYUP, 0);
// 		Sleep(100);
// 		//字母 S 按下
// 		keybd_event(83, 0x1F, 0, 0);
// 		keybd_event(83, 0x1F, KEYEVENTF_KEYUP, 0);
// 		Sleep(100);
// 		//system("cls");//清屏
// 	}
// 	return 0;
// }

// void ShowDesktop()
// {
// 	OutputString(L"Sending 'Win-D'\r\n");
// 	INPUT inputs[4] = {};
// 	ZeroMemory(inputs, sizeof(inputs));

// 	inputs[0].type = INPUT_KEYBOARD;
// 	inputs[0].ki.wVk = VK_LWIN;

// 	inputs[1].type = INPUT_KEYBOARD;
// 	inputs[1].ki.wVk = VK_D;

// 	inputs[2].type = INPUT_KEYBOARD;
// 	inputs[2].ki.wVk = VK_D;
// 	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

// 	inputs[3].type = INPUT_KEYBOARD;
// 	inputs[3].ki.wVk = VK_LWIN;
// 	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

// 	UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
// 	if (uSent != ARRAYSIZE(inputs))
// 	{
// 		OutputString(L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
// 	}
// }

// using namespace std;

// int main()
// {
// 	// return KeyboardListen();
// }

#endif