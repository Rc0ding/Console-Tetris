#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;

int nFieldHeight = 18;

int nScreenWidth = 80;

int nScreenHeight = 30;

const wchar_t *glyphs = L" ABCDEFG#"; // index 8 == '#'

unsigned char *pField = nullptr;

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{

	case 0:
		return py * 4 + px;

	case 1:
		return 12 + py - (px * 4);

	case 2:
		return 15 - (py * 4) - px;

	case 3:
		return 3 - py + (px * 4);
	};

	return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{

			int pi = Rotate(px, py, nRotation);

			int fx = nPosX + px;
			int fy = nPosY + py;
			int fi = fy * nFieldWidth + fx;

			// Check bounds before accessing arrays
			if (fx >= 0 && fx < nFieldWidth && fy >= 0 && fy < nFieldHeight)
			{
				if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
				{
					return false;
				}
			}
			else
			{
				if (tetromino[nTetromino][pi] == L'X')
				{
					return false;
				}
			}
		}
	}
	return true;
}

int main()
{

		tetromino[0].append(L"..X.");
		tetromino[0].append(L"..X.");
		tetromino[0].append(L"..X.");
		tetromino[0].append(L"..X.");

		tetromino[1].append(L".XX.");
		tetromino[1].append(L".X..");
		tetromino[1].append(L".X..");
		tetromino[1].append(L"....");

		tetromino[2].append(L".XX.");
		tetromino[2].append(L"..X.");
		tetromino[2].append(L"..X.");
		tetromino[2].append(L"....");

		tetromino[3].append(L"....");
		tetromino[3].append(L".XX.");
		tetromino[3].append(L".XX.");
		tetromino[3].append(L"....");

		tetromino[4].append(L".X..");
		tetromino[4].append(L".XX.");
		tetromino[4].append(L"..X.");
		tetromino[4].append(L"....");

		tetromino[5].append(L"..X.");
		tetromino[5].append(L".XX.");
		tetromino[5].append(L".X..");
		tetromino[5].append(L"....");

		tetromino[6].append(L"..X.");
		tetromino[6].append(L".XX.");
		tetromino[6].append(L"..X.");
		tetromino[6].append(L"....");

		// creating the Field

		pField = new unsigned char[nFieldHeight * nFieldWidth];
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
			{
				if (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1)
				{
					pField[y * nFieldWidth + x] = 8;
				}
				else
				{
					pField[y * nFieldWidth + x] = 0;
				};
			}

		// creating the Screen

		wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];

		for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
			screen[i] = L' ';
		HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(hConsole);
		DWORD dwBytesWritten = 0;
		SetConsoleScreenBufferSize(hConsole, {(SHORT)nScreenWidth,
						      (SHORT)nScreenHeight});
		// creating the GameLoop
		SMALL_RECT win = {0, 0,
				  (SHORT)(nScreenWidth - 1),
				  (SHORT)(nScreenHeight - 1)};
		SetConsoleWindowInfo(hConsole, TRUE, &win);
		bool bGameOver = false;
		bool bKey[4];
		int nCurrentPiece = 0;
		int nCurrentRotation = 0;
		int nCurrentX = nFieldWidth / 2;
		int nCurrentY = 0;

		while (!bGameOver)

		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			// GAME TIMING
			this_thread::sleep_for(50ms);


			

			// INPUT
			for(int i=0; i<4;i++){
				bKey[i]= (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x28\x27R"[i]))) != 0;
			}
			// LOGIC
			if (bKey[0]) // move left
			{
				if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
				{
					nCurrentX--;
				}
			}
			if (bKey[1]) // move down
			{
				if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				{
					nCurrentY++;
				}
			}
			
			if (bKey[2]) // move right
			{
				if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
				{
					nCurrentX++;
				}
			}
			if (bKey[3]) // rotate
			{
				int nNewRotation = (nCurrentRotation + 1) % 4;
				if (doesPieceFit(nCurrentPiece, nNewRotation, nCurrentX, nCurrentY))
				{
					nCurrentRotation = nNewRotation;
				}
			}

			// RENDER OUTPUT

			for (int x = 0; x < nFieldWidth; x++)
				for (int y = 0; y < nFieldHeight; y++)
					screen[(y + 2) * nScreenWidth + (x + 2)] = glyphs[pField[y * nFieldWidth + x]];
			;

			for (int px = 0; px < 4; px++)
			{
				for (int py = 0; py < 4; py++)
				{

					if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					{
						screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
					}
				};

				


				
			}
			WriteConsoleOutputCharacterW(hConsole, screen, nScreenHeight * nScreenWidth, {0, 0}, &dwBytesWritten);
		}
		
		return 0;
}