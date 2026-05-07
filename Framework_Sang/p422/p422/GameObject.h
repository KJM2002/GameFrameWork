#pragma once

#include "Utillity.h"
#include <algorithm>
// [CHECK]. namespace 포함해서 전방 선언해야 함
// 헤더에서는 절대 using namespace 사용 금지

namespace renderHelp {
    struct BitmapInfo;
}

namespace learning
{
    struct Collider;
    struct ColliderCircle;
    struct ColliderBox;
}

enum class ObjectType
{
    PLAYER,
    ENEMY,
    BULLET,
    ITEM,
    BACKGROUND,
};

constexpr int OBJECT_NAME_LEN_MAX = 15;

class GameObjectBase
{
    using Vector2f = learning::Vector2f;
public:
    GameObjectBase() = delete;
    GameObjectBase(const GameObjectBase&) = delete;

    GameObjectBase(ObjectType type) : m_type(type) {}

    virtual ~GameObjectBase() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hdc) = 0;
    virtual learning::Collider* GetCollider() = 0;
    
    void SetPosition(float x, float y) { m_pos = { x, y }; }
    void SetDirection(Vector2f dir) { m_dir = dir; }
    void SetSpeed(float speed) { m_speed = speed; }
    void SetName(const char* name);

    ObjectType Type() const { return m_type; }

    const char* GetName() const { return m_name; }

    Vector2f GetPosition() const { return m_pos; }
    Vector2f GetDirection() const { return m_dir; }

    float GetSpeed() const { return m_speed; }


protected:

    void Move(float deltaTime)
    {
        m_pos.x += m_dir.x * m_speed * deltaTime;
        m_pos.y += m_dir.y * m_speed * deltaTime;
    }

protected:
    ObjectType m_type;

    Vector2f m_pos = { 0.0f, 0.0f };
    Vector2f m_dir = { 0.0f, 0.0f }; // 방향 (단위 벡터)

    float m_speed = 0.0f; // 속력

    char m_name[OBJECT_NAME_LEN_MAX] = "";
};

class GameObject : public GameObjectBase
{

    using BitmapInfo = renderHelp::BitmapInfo;


public:
    GameObject(const GameObject&) = delete;
    GameObject(ObjectType type) : GameObjectBase(type) {}
    ~GameObject() override;

    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

    learning::Collider* GetCollider()override;

    void SetColliderCircle(float radius);
    void SetColliderBox(float halfWidth, float halfHeight);

    void SetBitmapInfo(BitmapInfo* bitmapInfo);  //
    void SetFrameInfo(int frameWidth, int frameHeight, int frameCount, int columnCount);


    void SetRenderSize(int width, int height);

protected:
    void DrawCollider(HDC hdc);

    void Move(float deltaTime);

    void DrawBitmap(HDC hdc);
    void UpdateFrame(float deltaTime);

    // Bitmap 정보
    BitmapInfo* m_pBitmapInfo = nullptr;

    int m_width = 50;
    int m_height = 50;

    // 점진적으로 예쁘게 고쳐 보아요.
    struct FrameFPos
    {
        int x;
        int y;
    };
    // 프레임 정보: 왜 14개냐고 물으시면 셌다고 밖에...:)
    // 
    FrameFPos m_frameXY[32] = {};

    int m_frameWidth = 102;
    int m_frameHeight = 94;
    int m_frameIndex = 0;
    int m_frameCount = 14;
    int m_frameColumnCount = 5;

    float m_frameTime = 0.0f;
    float m_frameDuration = 60.0f; // 임의 설정




    // Collider
    learning::Collider* myCollider = nullptr;
};