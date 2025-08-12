#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

wstring tetromino[7];

int nFieldWidth = 12;
int nFieldHeight = 18;

int nScreenWidth = 80;
int nScreenHeight = 30;

const wchar_t *glyphs = L" ABCDEFG#"; // index 8 == '#'

unsigned char *pField = nullptr;

const wchar_t *gameOverArt =
    L"   ██████   █████  ███    ███ ███████     ███████ ██    ██ ███████ ██████  \n"
    L"  ██       ██   ██ ████  ████ ██          ██   ██ ██    ██ ██      ██   ██ \n"
    L"  ██   ███ ███████ ██ ████ ██ █████       ██   ██ ██    ██ █████   ██████  \n"
    L"  ██    ██ ██   ██ ██  ██  ██ ██          ██   ██ ██    ██ ██      ██   ██ \n"
    L"   ██████  ██   ██ ██      ██ ███████     ███████  ██████  ███████ ██   ██ \n";

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0:
		return py * 4 + px; // 0 deg
	case 1:
		return 12 + py - (px * 4); // 90 deg
	case 2:
		return 15 - (py * 4) - px; // 180 deg
	case 3:
		return 3 - py + (px * 4); // 270 deg
	}
	return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			int pi = Rotate(px, py, nRotation);
			if (tetromino[nTetromino][pi] != L'X')
				continue;

			int fx = nPosX + px;
			int fy = nPosY + py;

			// Out of bounds -> collision
			if (fx < 0 || fx >= nFieldWidth || fy < 0 || fy >= nFieldHeight)
				return false;

			// In bounds, check field
			if (pField[fy * nFieldWidth + fx] != 0)
				return false;
		}
	}
	return true;
}

int main()
{
	// Pieces (4x4)
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X."); // I
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"...."); // J
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"...."); // L
	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"...."); // O
	tetromino[4].append(L".X..");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"...."); // S
	tetromino[5].append(L"..X.");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L".X..");
	tetromino[5].append(L"...."); // Z
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"...."); // T

	// Field (0 = empty, 8 = border, 1..7 = locked pieces)
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int y = 0; y < nFieldHeight; y++)
	{
		for (int x = 0; x < nFieldWidth; x++)
		{
			bool border = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1);
			pField[y * nFieldWidth + x] = border ? 8 : 0;
		}
	}

	// Screen
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
		screen[i] = L' ';

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	COORD size = {(SHORT)nScreenWidth, (SHORT)nScreenHeight};
	SetConsoleScreenBufferSize(hConsole, size);

	SMALL_RECT win = {0, 0, (SHORT)(nScreenWidth - 1), (SHORT)(nScreenHeight - 1)};
	SetConsoleWindowInfo(hConsole, TRUE, &win);

	// Game state
	srand((unsigned)time(nullptr));
	bool bGameOver = false;
	bool bKey[4], bPrevKey[4] = {false, false, false, false};

	int nCurrentPiece = rand() % 7;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2 - 2;
	int nCurrentY = 0;

	int tickCount = 0;
	const int tickInterval = 10; // gravity

	int score = 0;
	while (!bGameOver)
	{
		// Timing
		this_thread::sleep_for(chrono::milliseconds(50));
		tickCount++;

		// Input (Left, Down, Right, R)
		for (int i = 0; i < 4; i++)
			bKey[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x28\x27R"[i]))) != 0;

		// Movement
		if (bKey[0] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			nCurrentX--;
		if (bKey[2] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			nCurrentX++;
		if (bKey[1] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			nCurrentY++;

		if (bKey[3] && !bPrevKey[3])
		{
			int nNewRotation = (nCurrentRotation + 1) % 4;
			if (doesPieceFit(nCurrentPiece, nNewRotation, nCurrentX, nCurrentY))
				nCurrentRotation = nNewRotation;
		}

		// Gravity tick
		if (tickCount >= tickInterval)
		{
			bool canFall = doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1);
			if (canFall)
			{
				nCurrentY++;
			}
			else
			{
				// Lock piece in place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
					{
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = (unsigned char)(nCurrentPiece + 1);
					}

				// Check for lines
				// --- detect full lines (don't touch pField yet) ---
				std::vector<int> fullRows;
				for (int y = 0; y < nFieldHeight - 1; ++y)
				{
					bool full = true;
					for (int x = 1; x < nFieldWidth - 1; ++x)
						full &= (pField[y * nFieldWidth + x] != 0);
					if (full)
						fullRows.push_back(y);
				}

				if (!fullRows.empty())
				{
					// helper to render the current field immediately
					auto renderNow = [&]()
					{
						for (int i = 0; i < nScreenWidth * nScreenHeight; ++i)
							screen[i] = L' ';
						for (int yy = 0; yy < nFieldHeight; ++yy)
							for (int xx = 0; xx < nFieldWidth; ++xx)
								screen[(yy + 2) * nScreenWidth + (xx + 2)] = glyphs[pField[yy * nFieldWidth + xx]];
						COORD origin = {0, 0};
						WriteConsoleOutputCharacterW(hConsole, screen, (DWORD)(nScreenWidth * nScreenHeight), origin, &dwBytesWritten);
					};

					// --- flash the lines (modify pField temporarily + render each frame) ---
					for (int frame = 0; frame < 5; ++frame)
					{
						for (int y : fullRows)
						{
							for (int x = 1; x < nFieldWidth - 1; ++x)
							{
								// toggle between border '#' (8) and empty (0)
								pField[y * nFieldWidth + x] = (frame % 2 == 0) ? 8 : 0;
							}
						}
						renderNow();
						this_thread::sleep_for(chrono::milliseconds(60));
					}

					// --- clear and collapse ---
					for (int y : fullRows)
					{
						// clear the row
						for (int x = 1; x < nFieldWidth - 1; ++x)
							pField[y * nFieldWidth + x] = 0;

						// move everything above down by one
						for (int ny = y; ny > 0; --ny)
							for (int nx = 1; nx < nFieldWidth - 1; ++nx)
								pField[ny * nFieldWidth + nx] = pField[(ny - 1) * nFieldWidth + nx];
					}

					// --- score once per lock ---
					int cleared = (int)fullRows.size();
					if (cleared == 1)
						score += 100;
					else if (cleared == 2)
						score += 250;
					else if (cleared == 3)
						score += 450;
					else if (cleared >= 4)
						score += 750;
				}

				// Spawn new piece
				nCurrentPiece = rand() % 7;
				nCurrentRotation = 0;
				nCurrentX = nFieldWidth / 2 - 2;
				nCurrentY = 0;

				// Game over?
				if (!doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY))
					bGameOver = true;
			}

			tickCount = 0;
		}

		// -------- RENDER (every frame) --------
		// Clear screen area
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
			screen[i] = L' ';

		// Draw field
		for (int y = 0; y < nFieldHeight; y++)
			for (int x = 0; x < nFieldWidth; x++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = glyphs[pField[y * nFieldWidth + x]];
		// Draw score
		wstring scoreText = L"SCORE: " + to_wstring(score);
		for (size_t i = 0; i < scoreText.size(); i++)
		{
			screen[1 * nScreenWidth + (nFieldWidth + 6 + i)] = scoreText[i];
			// Row 1 (near top), column offset so it's right of the field
		}

		// Draw current piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
			{
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = (wchar_t)(nCurrentPiece + L'A');
			}

		// Flush to console
		COORD origin = {0, 0};
		WriteConsoleOutputCharacterW(
		    hConsole,
		    screen,
		    (DWORD)(nScreenWidth * nScreenHeight),
		    origin,
		    &dwBytesWritten);
		for (int i = 0; i < 4; i++)
			bPrevKey[i] = bKey[i];
	}

	// Clear screen
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
		screen[i] = L' ';

	// Copy ASCII art into screen buffer
	int startY = (nScreenHeight / 2) - 3; // roughly center vertically
	int startX = 5;			      // small horizontal offset

	int row = 0, col = 0;
	for (const wchar_t *p = gameOverArt; *p; ++p)
	{
		if (*p == L'\n')
		{
			row++;
			col = 0;
		}
		else
		{
			if (startY + row >= 0 && startY + row < nScreenHeight &&
			    startX + col >= 0 && startX + col < nScreenWidth)
			{
				screen[(startY + row) * nScreenWidth + (startX + col)] = *p;
			}
			col++;
		}
	}

	// Push to console
	COORD origin = {0, 0};
	WriteConsoleOutputCharacterW(hConsole, screen, (DWORD)(nScreenWidth * nScreenHeight), origin, &dwBytesWritten);

	// Wait so player can see it
	this_thread::sleep_for(chrono::seconds(3));

	// Cleanup
	delete[] pField;
	delete[] screen;
	return 0;
}
