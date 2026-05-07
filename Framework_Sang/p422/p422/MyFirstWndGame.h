#pragma once
#include "NzWndBase.h"
#include "Utillity.h"

// [CHECK] #7  전방 선언을 사용하여 헤더파일의 의존성을 줄임.
class GameTimer;
class GameObjectBase;
class GameObject;
namespace renderHelp {
    class BitmapInfo;
}
namespace learning {
    class Vector2f;
    class Collider;
}

class MyFirstWndGame : public NzWndBase
{
public:
    MyFirstWndGame() = default;
    ~MyFirstWndGame() override = default;

    bool Initialize();
    void Run();
    void Finalize();

    static float FClamp(float value, float min, float max);

private:
    void Update();
    void Render();

    void OnResize(int width, int height) override;     //On은 이벤트가 이뤄졌을 때를 의미
    void OnClose() override;

    void OnMouseMove(int x, int y);
    void OnLButtonDown(int x, int y);
    void OnRButtonDown(int x, int y);
    void OnMButtonDown(int x, int y);

    void FixedUpdate();
    void LogicUpdate();

    void CreatePlayer();
    void CreateCircleEnemy();
    void CreateBoxEnemy();
    void UpdatePlayerInfo();
    void UpdateEnemyInfo();         // 적이 플레이어 추적 가능하도록 함수 추가
    void UpdateWholeIntersect();
    void PushEnemiesEachOther();
    void PushEnemiesOutOfPlayer();


    void SettingBoxPos(learning::Collider* thisBox, learning::Collider* targetBox, GameObject* pThis, learning::Vector2f firstDir);
    learning::Vector2f GetBoxDir(learning::Collider* thisBox, learning::Collider* targetBox);
    void SettingCirPos(learning::Collider* thisCir, learning::Collider* targetCir, GameObject* pThis);
   
    GameObject* GetPlayer() const { return (GameObject*)m_GameObjectPtrTable[0]; }

private:
    HDC m_hFrontDC = nullptr;
    HDC m_hBackDC = nullptr;
    HBITMAP m_hBackBitmap = nullptr;
    HBITMAP m_hDefaultBitmap = nullptr;

    // [CHECK] #8. 게임 타이머를 사용하여 프레임을 관리하는 예시.F
    GameTimer* m_pGameTimer = nullptr;
    float m_fDeltaTime = 0.0f;
    float m_fFrameCount = 0.0f;

    // [CHECK] #8. 게임 오브젝트를 관리하는 컨테이너.
    GameObjectBase** m_GameObjectPtrTable = nullptr;

    struct MOUSE_POS
    {
        int x = 0;
        int y = 0;

        bool operator!=(const MOUSE_POS& other) const //연산자 오버로딩 
        {
            return (x != other.x || y != other.y);
        }
    };

    MOUSE_POS m_MousePos = { 0, 0 };
    MOUSE_POS m_MousePosPrev = { 0, 0 };

    MOUSE_POS m_PlayerTargetPos = { 0, 0 };
    MOUSE_POS m_CirEnemySpawnPos = { 0, 0 };
    MOUSE_POS m_BoxEnemySpawnPos = { 0, 0 };

    
    renderHelp::BitmapInfo* m_pPlayerBitmapInfo = nullptr;

    renderHelp::BitmapInfo* m_pEnemyBitmapInfo = nullptr;

};