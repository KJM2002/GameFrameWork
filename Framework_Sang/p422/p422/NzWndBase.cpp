#include "NzWndBase.h"
#include <iostream>

LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	//[CHECK] #3-Ask NzWndBase의 protected 멤버변수에 접근 가능한 이유는?
	switch (msg)
	{
	case WM_SIZE:
	{
		NzWndBase* pNzWnd = (NzWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA); // 저장되어있던 MyFirstWndGame 포인터 사용
		// 다형성을 충족함, 부모로 업캐스팅 했지만 OnResize 자체가 MyFirstWndGame에 오버라이딩 된 OnResize 호출
		if (pNzWnd)pNzWnd->OnResize(LOWORD(lparam), HIWORD(lparam));
		break;
	}

	case WM_CLOSE:
	{
		NzWndBase* pNzWnd = (NzWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (pNzWnd) pNzWnd->OnClose();
		PostQuitMessage(0);
		break;
	}


	default:
		return::DefWindowProc(hwnd, msg, wparam, lparam);
	}//switch

	return NULL;
}


bool NzWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = className;
	wc.lpfnWndProc = NzWndProc; // //[CHECK] #5. 윈도우 프로시저(함수)의 포인터 등록


	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), className, &wc))
	{
		classId = RegisterClassEx(&wc);

		if (0 == classId) return false;
	}

	m_width = width;
	m_height = height;

	RECT rc = { 0, 0, width, height }; 
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	//[CHECK] AdjustWindowRect()의 의미는?
	m_hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

	if (NULL == m_hWnd) return false;

	::SetWindowText((HWND)m_hWnd, windowName);

	//[CHECK] #5. SetWindowLongPtr()의 의미는?
	//전역 함수 proc에서 지금 인스턴스화된 NzWndBase를 사용하고 싶은데
	//이걸 바로 넘기는 방법임
	SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (LONG_PTR)this); // 내 포인터 등록?

	ShowWindow((HWND)m_hWnd, SW_SHOW);
	UpdateWindow((HWND)m_hWnd);

	return true;
}

void NzWndBase::Destroy()
{
	if (NULL != m_hWnd)
	{
		DestroyWindow((HWND)m_hWnd);
		m_hWnd = NULL;
	}
}

void NzWndBase::OnResize(int width, int height)
{
	m_width = width;
	m_height = height;
}