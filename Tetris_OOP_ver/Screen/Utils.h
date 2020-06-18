#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <Windows.h>

using namespace std;

struct Vector2 {
	float x;
	float y;
	const int X() const { return (int)x; }
	const int Y() const { return (int)y; }
	Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
	Vector2(int x = 0, int y = 0) : Vector2((float)x, (float)y) {}
	Vector2(const Vector2& other) : Vector2(other.x, other.y) {}

	static Vector2 zero;
	static Vector2 ones;
	static Vector2 up;
	static Vector2 down;
	static Vector2 left;
	static Vector2 right;

	double magnitude() {
		return sqrt(this->sqrMagnitude() );
	}

	double sqrMagnitude() {
		return (double)x * x + y * y;
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2{ this->x - other.x, this->y - other.y };
	}

	static friend Vector2 operator-(Vector2& a, const Vector2& b);

	static friend double Distance(Vector2& a, const Vector2& b);
	
	
	Vector2 operator+(const Vector2& other) {
		return Vector2{ this->x + other.x, this->y + other.y };
	}
	bool operator==(const Vector2& other) {
		return (x == other.x && y == other.y);
	}
	bool operator!=(const Vector2& other) {
		return (x != other.x || y != other.y);
	}
	Vector2& operator+=(const Vector2& other) {
		x += other.x, y += other.y;
		return *this;
	}
};

enum class KeyCode {
	Space = 0,
	Left,
	Right,
	Up,
	Down,

	Esc,
	Enter,

	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

static vector<WORD> keyCodeTable {
	VK_SPACE, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
	VK_ESCAPE, VK_RETURN, 
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A
};


class Input {
	static INPUT_RECORD InputRecord[128];
	static DWORD Events;
	static bool evaluated;
	static bool gotMouseEvent;
	static bool gotKeyEvent;
	static Vector2 mousePosition;
	static WORD vKeyCode;

	static void GetEvent()
	{
		evaluated = true;
		DWORD numEvents = 0;

		GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &numEvents);
		if (!numEvents) return;

		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), InputRecord, numEvents, &Events);
		for (int i = 0; i < numEvents; i++) {
			if (InputRecord[i].EventType == MOUSE_EVENT) {
				if (InputRecord[i].Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
					COORD coord;
					coord.X = InputRecord[i].Event.MouseEvent.dwMousePosition.X;
					coord.Y = InputRecord[i].Event.MouseEvent.dwMousePosition.Y;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
					mousePosition.x = InputRecord[i].Event.MouseEvent.dwMousePosition.X;
					mousePosition.y = InputRecord[i].Event.MouseEvent.dwMousePosition.Y;
					gotMouseEvent = true;
				}
			}
			else if (InputRecord[i].EventType == KEY_EVENT) {
				if (InputRecord[i].Event.KeyEvent.bKeyDown) {
					vKeyCode = InputRecord[i].Event.KeyEvent.wVirtualKeyCode;
					gotKeyEvent = true;
				}
			}
		}
	}

public:

	static void EndOfFrame()
	{
		evaluated = false;
		gotMouseEvent = false;
		gotKeyEvent = false;
	}

	static void Initialize()
	{

		CONSOLE_CURSOR_INFO cci;
		cci.dwSize = 25;
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
		SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT );

		EndOfFrame();
	}
	static bool GetMouseEvent(Vector2& pos) {
		if (evaluated == false) GetEvent();

		if (gotMouseEvent == true) {
			pos = mousePosition;
			return true;
		}
		return false;}

	static bool GetKeyEvent(WORD& keyCode) {
		if (evaluated == false) GetEvent();

		if (gotKeyEvent == true) {
			keyCode = vKeyCode;
			return true;
		}
		return false;
	}

	static bool GetKeyDown(KeyCode key) {
		if (evaluated == false) GetEvent();

		if (gotKeyEvent == true) return keyCodeTable[(int)key] == vKeyCode;		
		return false;
	}
};


class Borland {

public:
	

	static int wherex()
	{
		CONSOLE_SCREEN_BUFFER_INFO  csbiInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo);
		return csbiInfo.dwCursorPosition.X;
	}
	static int wherey()
	{
		CONSOLE_SCREEN_BUFFER_INFO  csbiInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo);
		return csbiInfo.dwCursorPosition.Y;
	}
	static void gotoxy(int x, int y)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), _COORD{ (SHORT)x, (SHORT)y });
	}
	static void gotoxy(const Vector2* pos)
	{
		if (!pos) return;
		gotoxy( (*pos).X(), (*pos).Y());
	}
	static void gotoxy(const Vector2& pos)
	{
		gotoxy( pos.X(), pos.Y());
	}
};

class Screen {
	int width;
	int height;
	char* canvas;

	static Screen* instance;
	Screen(int width = 90, int height = 50)
		: width(width), height(height),
		canvas(new char[(width + 1)*height])

	{
		Input::Initialize();
	}
public:
	static Screen& getInstance() {
		if (instance == nullptr) {
			instance = new Screen();
		}
		return *instance;
	}

	~Screen() {
		if (instance) {
			delete[] canvas;
			instance = nullptr;
		}
	}

	int getWidth() const { return width; }

	int getHeight() const { return height;  }

	void drawRect(const Vector2& pos, int w, int h)
	{
		canvas[pos.X() + pos.Y()*(width + 1)] = '\xDA';
		canvas[pos.X() + pos.Y()*(width + 1)+ w-1] = '\xBF';
		memset(&canvas[pos.X() + pos.Y()*(width + 1) + 1], '\xC4', w - 2);
		canvas[pos.X() + (pos.Y() + (h - 1))*(width + 1)] = '\xC0';
		canvas[pos.X() + (pos.Y() + (h - 1))*(width + 1) + w-1] = '\xD9';
		memset(&canvas[pos.X() + 1 + (pos.Y() + (h - 1))*(width + 1)], '\xC4', w - 2);
		for (int i = 1; i < h-1; i++) {
			canvas[pos.X() + (pos.Y() + i)*(width + 1)] = '\xB3';
			canvas[pos.X() + w-1 + (pos.Y() + i)*(width + 1)] = '\xB3';
		}
	}

	void backLineDraw(int w, int h, const Vector2& pos)
	{
		for (int i = 0; i < h; i++)
		{
			memset(&canvas[pos.X() + (pos.Y() + i)*(width + 1)], '\xF9', w);
		}
	}

	void draw(const char* shape, int w, int h, const Vector2& pos)
	{
		if (!shape) return;
		for (int i = 0; i < h; i++)
		{
			strncpy(&canvas[pos.X() + (pos.Y() + i)*(width + 1)], &shape[i*w], w);
		}
	}

	void blockDraw(const char* shape, int w, int h, const Vector2& pos)
	{
		if (!shape) return;
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				if (shape[j + i * w] != '\xB2') continue;
				canvas[pos.X() + j + (pos.Y() + i)*(width + 1)] = shape[j + i*w];
			}
		}
	}

	void render()
	{
		for (int i = 0; i < height; i++)
			canvas[width + i * (width + 1)] = '\n';
		canvas[width + (height - 1) * (width + 1)] = '\0';
		Borland::gotoxy(0, 0);
		cout << canvas;
	}

	void clear()
	{
		memset(canvas, ' ', (width + 1)*height);
		canvas[width + (height - 1)*(width + 1)] = '\0';
	}
};

