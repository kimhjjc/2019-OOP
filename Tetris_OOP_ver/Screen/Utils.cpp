#include "Utils.h"


Vector2 Vector2::zero{ 0, 0 };
Vector2 Vector2::ones{ 1, 1 };
Vector2 Vector2::up{ 0, 1 };
Vector2 Vector2::down{ 0, -1 };
Vector2 Vector2::left{ -1, 0 };
Vector2 Vector2::right{ 1, 0 };



Vector2 operator-(Vector2& a, const Vector2& b)
{
	return (a.operator-(b));
}

double Distance(Vector2& a, const Vector2& b)
{
	return (a.operator-(b)).magnitude();
}

INPUT_RECORD Input::InputRecord[128];
DWORD Input::Events;

bool Input::evaluated = false;
bool Input::gotMouseEvent = false;
bool Input::gotKeyEvent = false;
Vector2 Input::mousePosition{ -1, -1 };
WORD Input::vKeyCode{ 0 };

Screen* Screen::instance = nullptr;