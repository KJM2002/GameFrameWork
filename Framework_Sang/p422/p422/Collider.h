#pragma once
#include "Windows.h"
#include "Utillity.h"

namespace learning
{
	struct Collider {
		Vector2f center;
		virtual Collider& GetCollider() = 0;
		virtual bool IsIntersect(Collider* colTarget) = 0;
		virtual void Draw(HDC hdc) = 0;
		bool isIntersect = false;
	};

	struct ColliderCircle : Collider
	{
		double radius;
		Collider& GetCollider() override;
		bool IsIntersect(Collider* colTarget) override;
		void Draw(HDC hdc)override;
	};

	struct ColliderBox : Collider
	{
		Vector2f halfSize;
		Collider& GetCollider() override;
		bool IsIntersect(Collider* colTarget) override;
		void Draw(HDC hdc)override;
	};

	// Circle
	bool Intersect(ColliderCircle const& lhs, ColliderCircle const& rhs);
	// AABB
	bool Intersect(ColliderBox const& lhs, ColliderBox const& rhs);
}

