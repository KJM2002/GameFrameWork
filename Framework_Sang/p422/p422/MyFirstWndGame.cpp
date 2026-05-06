#include "INC_Windows.h"
#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include "renderHelp.h"
#include <iostream>
#include <assert.h>

using namespace learning;

constexpr int MAX_GAME_OBJECT_COUNT = 1000;




bool MyFirstWndGame::Initialize()
{
	m_pGameTimer = new GameTimer(); // 타이머 생성
	m_pGameTimer->Reset();

	const wchar_t* className = L"MyFirstWndGame"; //L은 유니코드로 하겠다는 거
	const wchar_t* windowName = L"MyFirstWndGame";

	if (false == __super::Create(className, windowName, 1024, 720))
	{
		return false;
	}


	RECT rcClient = {};
	GetClientRect(m_hWnd, &rcClient);
	m_width = rcClient.right - rcClient.left;
	m_height = rcClient.bottom - rcClient.top;

	m_hFrontDC = GetDC(m_hWnd);
	m_hBackDC = CreateCompatibleDC(m_hFrontDC);
	m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

	m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

	m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		m_GameObjectPtrTable[i] = nullptr;
	}


	//필요 리소스 로드
#pragma region resource
	//주의 ! IDE 에서 인지하는 현재 경로와 실제 실행 파일을 ㅏ로 실행했을 때의 경로 기준이 달라짐.
	m_pPlayerBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/redbird.png");
	m_pEnemyBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/graybird.png");
#pragma endregion

	// [CHECK]. 첫 번째 게임 오브젝트는 플레이어 캐릭터로 고정!
	CreatePlayer();

	return true;

}

void MyFirstWndGame::Run()
{
	MSG msg = { 0 };     //디스패치하지 않고(프로시져를 거치지 않고) 이렇게 처리할 수도 있다.
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MyFirstWndGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_RBUTTONDOWN)
			{
				MyFirstWndGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_MBUTTONDOWN)
			{
				MyFirstWndGame::OnMButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_MOUSEMOVE)
			{
				MyFirstWndGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			Update();
			Render();
		}
	}
}

void MyFirstWndGame::Finalize()
{
	delete m_pGameTimer;
	m_pGameTimer = nullptr;

	if (m_GameObjectPtrTable)
	{
		for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
		{
			if (m_GameObjectPtrTable[i])
			{
				delete m_GameObjectPtrTable[i];
				m_GameObjectPtrTable[i] = nullptr;
			}
		}
		delete m_GameObjectPtrTable;
	}

	__super::Destroy();
}

// 만약 적이 생성된다면, 플레이어를 추적하기

void MyFirstWndGame::FixedUpdate()
{
	UpdateWholeIntersect();
	if (m_CirEnemySpawnPos.x != 0 && m_CirEnemySpawnPos.y != 0)
	{
		CreateCircleEnemy();
	}
	else if (m_BoxEnemySpawnPos.x != 0 && m_BoxEnemySpawnPos.y != 0) {

		CreateBoxEnemy();
	}
}

void MyFirstWndGame::LogicUpdate()
{

	UpdatePlayerInfo();
	UpdateEnemyInfo();

	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i]->Update(m_fDeltaTime);
		}
	}
}

void MyFirstWndGame::CreatePlayer()
{
	assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

	GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

	pNewObject->SetName("Player");
	pNewObject->SetPosition(0.0f, 0.0f); // 일단, 임의로 설정 
	pNewObject->SetSpeed(1.0f); // 일단, 임의로 설정   

	pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
	pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo);
	pNewObject->SetFrameInfo(102, 94, 14, 5);
	pNewObject->SetRenderSize(50, 50);

	m_GameObjectPtrTable[0] = pNewObject;
}

void MyFirstWndGame::CreateCircleEnemy()
{
	GameObject* pNewObject = new GameObject(ObjectType::ENEMY);

	pNewObject->SetName("Enemy");

	float x = m_CirEnemySpawnPos.x;
	float y = m_CirEnemySpawnPos.y;

	m_CirEnemySpawnPos = { 0, 0 };

	pNewObject->SetPosition(x, y);
	pNewObject->SetSpeed(0.3f); // 일단, 임의로 설정   

	pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
	pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);
	pNewObject->SetFrameInfo(183, 168, 14, 5);
	pNewObject->SetRenderSize(90, 83);

	bool isInter = false;

	learning::Collider* thisCollider = nullptr;

	Vector2f firstDir;

	//충돌 처리
	int j = 0;
	int t = 0;

	while (j < MAX_GAME_OBJECT_COUNT) {
		thisCollider = pNewObject->GetCollider();
		GameObjectBase* target = m_GameObjectPtrTable[j];
		if (target == nullptr) break;

		learning::Collider* targetCollider = target->GetCollider();

		if (thisCollider->IsIntersect(targetCollider)) {
			// 그리는 위치 업데이트
			MyFirstWndGame::SettingCirPos(thisCollider, targetCollider, pNewObject);
			j = 0;
			++t;
			//판정상 무한루프를 빠질 때가 있어서 그것 처리
			if (t == MAX_GAME_OBJECT_COUNT) { 
				isInter = true;  
				break; 
			}
			continue;
		}
		++j;
	}


	int i = 0;
	while (++i < MAX_GAME_OBJECT_COUNT && !isInter) //0번째는 언제나 플레이어!
	{
		if (nullptr == m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i] = pNewObject;
			break;
		}
	}

	if (i == MAX_GAME_OBJECT_COUNT || isInter)
	{
		// 게임 오브젝트 테이블이 가득 찼습니다.
		delete pNewObject;
		pNewObject = nullptr;
		isInter = false;
	}
}

void MyFirstWndGame::CreateBoxEnemy()
{
	GameObject* pNewObject = new GameObject(ObjectType::ENEMY);

	pNewObject->SetName("Enemy");

	float x = m_BoxEnemySpawnPos.x;
	float y = m_BoxEnemySpawnPos.y;

	m_BoxEnemySpawnPos = { 0, 0 };

	pNewObject->SetPosition(x, y);
	pNewObject->SetSpeed(0.3f); // 일단, 임의로 설정   

	pNewObject->SetColliderBox(50.0f, 50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
	pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);
	pNewObject->SetFrameInfo(183, 168, 14, 5);
	pNewObject->SetRenderSize(90, 83);


	bool isInter = false;

	learning::Collider* thisCollider = nullptr;

	Vector2f firstDir;

	//충돌 처리
	int j = 0;
	int t = 0;
	while (j < MAX_GAME_OBJECT_COUNT) {
		thisCollider = pNewObject->GetCollider();
		GameObjectBase* target = m_GameObjectPtrTable[j];
		if (target == nullptr) break;

		learning::Collider* targetCollider = target->GetCollider();

		if (thisCollider->IsIntersect(targetCollider)) {
			//firstDir이 초기값일 때 
			if (firstDir.x == 0 && firstDir.y == 0) {
				firstDir = MyFirstWndGame::GetBoxDir(thisCollider, targetCollider);
			}
			// 그리는 위치 업데이트
			MyFirstWndGame::SettingBoxPos(thisCollider, targetCollider, pNewObject,firstDir);
			j = 0;
			++t;
			//판정상 무한루프를 빠질 때가 있어서 그것 처리
			if (t == MAX_GAME_OBJECT_COUNT) { 
				isInter = true;  
			break; }
			continue;
		}
		++j;
	}
	


	int i = 0;
	while (++i < MAX_GAME_OBJECT_COUNT && !isInter) //0번째는 언제나 플레이어!
	{
		if (nullptr == m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i] = pNewObject;
			break;
		}
	}

	if (i == MAX_GAME_OBJECT_COUNT || isInter)
	{
		// 게임 오브젝트 테이블이 가득 찼습니다.
		delete pNewObject;
		pNewObject = nullptr;
		isInter = false;
	}
}



void MyFirstWndGame::SettingCirPos(learning::Collider* thisCir, learning::Collider* targetCir, GameObject* pThis) {
	auto a = dynamic_cast<ColliderCircle*>(thisCir);
	float radius = a->radius;
	Vector2f thisPos = thisCir->center;
	Vector2f targetPos = targetCir->center;
	Vector2f dir = thisPos - targetPos;
	float x = targetCir->center.x;
	float y = targetCir->center.y;
	//0.5f는 float 계산으로 생기는 오차 때문에 온전하게 밀어주는 길이가 radius * 2 가 되질 않아 더해준다.
	thisCir->center += dir.Normalized() * ((radius*2 + 0.5f) * (1 - (dir.Length()/ (radius*2))));
	pThis->SetPosition(thisCir->center.x, thisCir->center.y);
}



Vector2f MyFirstWndGame::GetBoxDir(learning::Collider* thisBox, learning::Collider* targetBox) {
	auto a = dynamic_cast<ColliderBox*>(thisBox);
	float boxSize = a->halfSize.x * 2;
	Vector2f thisPos = thisBox->center;
	Vector2f targetPos = targetBox->center;
	Vector2f dir = thisPos - targetPos;
	return dir.Normalized();
}


void MyFirstWndGame::SettingBoxPos(learning::Collider* thisBox, learning::Collider* targetBox, GameObject* pThis,Vector2f firstDir) {
	auto a = dynamic_cast<ColliderBox*>(thisBox);
	float boxSize = a->halfSize.x * 2;
	Vector2f thisPos = thisBox->center;
	Vector2f targetPos = targetBox->center;
	Vector2f dir = thisPos - targetPos;
	float mScala = dir.Length();
	float x = targetBox->center.x;
	float y = targetBox->center.y;
	thisBox->center += firstDir * ((boxSize * 1.41f) * (1 - (mScala / (boxSize * 1.41f))));
	thisBox->center.x = FClamp(thisBox->center.x, x - boxSize, x + boxSize);
	thisBox->center.y = FClamp(thisBox->center.y, y - boxSize, y + boxSize);
	pThis->SetPosition(thisBox->center.x, thisBox->center.y);
}

float MyFirstWndGame::FClamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
};

void MyFirstWndGame::UpdateWholeIntersect() {
	static GameObject* pPlayer = GetPlayer();

	auto playerCollider = pPlayer->GetCollider();

	//전체 초기화
	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; i++) {
		GameObjectBase* target = m_GameObjectPtrTable[i];
		if (target == nullptr) break;
		target->GetCollider()->isIntersect = false;
	}

	int j = 1;
	while (j < MAX_GAME_OBJECT_COUNT) {
		GameObjectBase* target = m_GameObjectPtrTable[j];
		if (target == nullptr) break;
		learning::Collider* targetCollider = target->GetCollider();
		if (playerCollider->IsIntersect(targetCollider)) {
			targetCollider->isIntersect = true;
			playerCollider->isIntersect = true;
		}			
		++j;
	}


}
void MyFirstWndGame::UpdatePlayerInfo()
{
	static GameObject* pPlayer = GetPlayer();

	assert(pPlayer != nullptr);

	Vector2f mousePos(m_PlayerTargetPos.x, m_PlayerTargetPos.y);
	Vector2f playerPos = pPlayer->GetPosition();

	Vector2f playerDir = mousePos - playerPos;
	float distance = playerDir.Length(); // 거리 계산

	if (distance > 50.f) //임의로 설정한 거리
	{
		playerDir.Normalize(); // 정규화
		pPlayer->SetDirection(playerDir); // 플레이어 방향 설정
	}
	else
	{
		pPlayer->SetDirection(Vector2f(0, 0)); // 플레이어 정지
	}
}

void MyFirstWndGame::UpdateEnemyInfo()
{
	GameObjectBase* pPlayer = m_GameObjectPtrTable[0];

	if (pPlayer == nullptr)
	{
		return;
	}

	Vector2f playerPos = pPlayer->GetPosition();

	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		GameObjectBase* pEnemy = m_GameObjectPtrTable[i];

		if (pEnemy == nullptr)
		{
			break;
		}

		Vector2f enemyPos = pEnemy->GetPosition();
		Vector2f enemyDir = playerPos - enemyPos;

		float distance = enemyDir.Length();

		if (distance > 1.0f)
		{
			enemyDir.Normalize();
			pEnemy->SetDirection(enemyDir);
		}
		else
		{
			pEnemy->SetDirection(Vector2f(0.0f, 0.0f));
		}
	}
}

void MyFirstWndGame::Update()
{
	m_pGameTimer->Tick();

	LogicUpdate();

	m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
	m_fFrameCount += m_fDeltaTime;

	while (m_fFrameCount >= 200.0f)
	{
		FixedUpdate();
		m_fFrameCount -= 200.0f;
	}
}

void MyFirstWndGame::Render()
{
	//Clear the back buffer
	//일단 하얀색으로 채움 초기화임
	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

	//메모리 DC에 그리기
	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i]->Render(m_hBackDC);
		}
	}

	//메모리 DC에 그려진 결과를 실제 DC(m_hFrontDC)로 복사
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyFirstWndGame::OnResize(int width, int height)
{
	std::cout << __FUNCTION__ << std::endl;

	learning::SetScreenSize(width, height);

	__super::OnResize(width, height);

	m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

	HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

	DeleteObject(hPrevBitmap);
}

void MyFirstWndGame::OnClose()
{
	std::cout << __FUNCTION__ << std::endl;

	SelectObject(m_hBackDC, m_hDefaultBitmap);

	DeleteObject(m_hBackBitmap);
	DeleteDC(m_hBackDC);

	ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyFirstWndGame::OnMouseMove(int x, int y)
{
	/*   std::cout << __FUNCTION__ << std::endl;
	   std::cout << "x: " << x << ", y: " << y << std::endl;*/
	m_MousePosPrev = m_MousePos;
	m_MousePos = { x, y };
}

void MyFirstWndGame::OnLButtonDown(int x, int y)
{
	/*  std::cout << __FUNCTION__ << std::endl;
 std::cout << "x: " << x << ", y: " << y << std::endl;*/

	m_PlayerTargetPos.x = x;
	m_PlayerTargetPos.y = y;

}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
	/*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

	m_CirEnemySpawnPos.x = x;
	m_CirEnemySpawnPos.y = y;
}

void MyFirstWndGame::OnMButtonDown(int x, int y)
{
	m_BoxEnemySpawnPos.x = x;
	m_BoxEnemySpawnPos.y = y;
}