#pragma once
#include "INC_Windows.h"

// 함수 선언
LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// [CHECK] #1. 윈도우 클래스 생성 및 등록을 클래스로 구현한 예시.
// * 상속을 받아 확장할 수 있도록 구현.

class NzWndBase
{
public:
    NzWndBase() = default;
    virtual ~NzWndBase() = default;

    bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
    void Destroy();

    void* GetHandle() const { return m_hWnd; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

protected:

    // [CHECK] #3 friend 로 선언하여 해당 함수에서 접근할 수 있도록 함. (왜일까요?)
    //NzWndBase는 NzWndProc을 친구로 인정, NzWndProc가 외부에서 정의 시 
    //NzWndBase의 멤버 변수나 함수를 접근제한자에 상관없이 사용 가능(양방향 아님 인정한 쪽만 사용 가능)
   
    friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // [CHECK] #4 메시지 프로시저를 가상함수로 구현하여 상속받은 클래스에서 재정의할 수 있도록 함.
    virtual void OnResize(int width, int height);
    virtual void OnClose() {}

    HWND m_hWnd = HWND(); // 초기화 방법이 이럼, HWND() 가 기본값으로 초기화된 핸들 사실상 NULL
    int m_width = 0;
    int m_height = 0;


    // [CHECK] #2. 무엇을 위한 코드일까요? 
    // 프로그래머의 의도를 코드로 강제 하는 것
    // 지금 이 클래스 내에는 os에게 받은 핸들이 들어가게 딜 것이고
    // 이 객체가 윈도우의 생명 주기를 관리하게 될 것이다.
    // 만약 복사가 된다면 같은 핸들을 여러 객체가 공유하게 되며 이때 누가 Destroy를 호출해야 하는지 애매하게 된다.
    // 즉 복사후 얕은 참조로 m_hWnd 에 주소가 복사가 될 것이고 한쪽에서 Destroy시 다른 객체의 핸들은 댕글링 포인터가 되버린다.
    // 또한 이동까지 막은 이유는 핸들이 os메시지 루프,콜백,프로시저와 연결되는데 내부적으로 this 포인터 매핑 같은 걸 쓰는 경우가 많아서
    // 객체의 주소가 바뀌었을 때 문제가 생길 가능성이 있기 때문이다.
    // 그래서 복사,이동 생성자로 복사를 시도할 때 그냥 안되게 할 때 쓰는 문법임
    NzWndBase(const NzWndBase&) = delete;
    NzWndBase& operator=(const NzWndBase&) = delete;
    NzWndBase(NzWndBase&&) = delete;
    NzWndBase& operator=(NzWndBase&&) = delete;
};
