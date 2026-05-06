#include "Collider.h"



learning::Collider& learning::ColliderCircle::GetCollider() {
	return *this;
}

bool learning::ColliderCircle::IsIntersect(Collider* colTarget){
	if (auto circle = dynamic_cast<ColliderCircle*>(colTarget)) {
		return learning::Intersect(*this, *circle);
	}
	return false;
}

learning::Collider& learning::ColliderBox::GetCollider() {
	return *this;
}

bool learning::ColliderBox::IsIntersect(Collider* colTarget){
	if (auto box = dynamic_cast<ColliderBox*>(colTarget)) {
		return Intersect(*this, *box);
	}
	return false;
}


void learning::ColliderCircle::Draw(HDC hdc) {
	Ellipse(hdc, this->center.x - this->radius,
		this->center.y - this->radius,
		this->center.x + this->radius,
		this->center.y + this->radius);
}

void learning::ColliderBox::Draw(HDC hdc) {
	Rectangle(hdc, this->center.x - this->halfSize.x,
		this->center.y - this->halfSize.y,
		this->center.x + this->halfSize.x,
		this->center.y + this->halfSize.y);
}



bool learning::Intersect(ColliderCircle const& lhs, ColliderCircle const& rhs)
{
	return (rhs.center - lhs.center).LengthSquared() <= pow(lhs.radius + rhs.radius, 2);
}

bool learning::Intersect(ColliderBox const& lhs, ColliderBox const& rhs)
{
	//x-direction
	if (lhs.center.x - lhs.halfSize.x >= rhs.center.x + rhs.halfSize.x) return false;
	if (lhs.center.x + lhs.halfSize.x <= rhs.center.x - rhs.halfSize.x) return false;

	//y-direction
	if (lhs.center.y - lhs.halfSize.y >= rhs.center.y + rhs.halfSize.y) return false;
	if (lhs.center.y + lhs.halfSize.y <= rhs.center.y - rhs.halfSize.y) return false;

	return true;

}