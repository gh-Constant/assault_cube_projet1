
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <vector>

using namespace std;

struct Vector3
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};



uintptr_t getModuleBaseAdress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

void setPlayerViewAngles() {

}

int aimbot() {
	DWORD assaultcube_PID;
	HANDLE assaultcube;

	HWND assaultcube_Process = FindWindowA(0, "AssaultCube");
	if (assaultcube_Process == NULL) {
		cout << "Didn't found Assault Cube window. Last Error : " << dec << GetLastError() << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	GetWindowThreadProcessId(assaultcube_Process, &assaultcube_PID);
	if (assaultcube_PID == NULL)
		cout << "Didn't found Assault Cube PID (did you close the game ?). Last Error : " << dec << GetLastError() << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	while (true) {

		cout << "assaultcube_PID (" << &assaultcube_PID << ") : " << assaultcube_PID << endl;

		assaultcube = OpenProcess(PROCESS_ALL_ACCESS, TRUE, assaultcube_PID);

		if (!assaultcube) {
			cout << "OpenProcess failed. GetLastError : " << dec << GetLastError() << endl;
			system("pause");
			return EXIT_FAILURE;
		}
		cout << "assaultcube (" << &assaultcube << ") : " << assaultcube << endl;

		uintptr_t assaultcubeBaseAdrr = getModuleBaseAdress(assaultcube_PID, L"ac_client.exe");
		if (assaultcubeBaseAdrr == NULL) {
			cout << "Didn't found Assault Cube base address. Last Error : " << hex << GetLastError() << endl;
			system("pause");
			return EXIT_FAILURE;
		}

		cout << "assaultcube_BaseAdrr (" << hex << &assaultcubeBaseAdrr << ") : " << assaultcubeBaseAdrr << endl;

		uintptr_t localplayer;
		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + 0x18AC00), &localplayer, sizeof(uintptr_t), NULL);

		uintptr_t PlayerCountPTR = 0x18AC0C;
		uintptr_t entitylistPTR = 0x18AC04;

		uintptr_t EntityList;

		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + entitylistPTR), &EntityList, 4, NULL);
		cout << "Boucle Segment : " << endl << endl;

		int playercounter = 0;
		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + 0x18AC0C), &playercounter, sizeof(int), NULL);
		cout << "Le nombre de joueurs dans la partie : " << dec << playercounter << endl;

		
		Vector3 headpos;
		Vector2 viewAngles;
		uintptr_t headPosPTR = 0x4;
		uintptr_t viewAnglesPTR = 0x34;

		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + headPosPTR), &headpos, sizeof(Vector3), NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + viewAnglesPTR), &viewAngles, sizeof(Vector2), NULL);


		for (int i = 1; i < playercounter; i++) {
			cout << "Player ID : (" << &i << ") :" << i;
			int playerPointer;
			ReadProcessMemory(assaultcube, (LPCVOID)(EntityList + i * 4), &playerPointer, 4, NULL);
			cout << "   entity (" << hex << &playerPointer << ") :" << playerPointer;

			char EntityName[20];
			ReadProcessMemory(assaultcube, (LPCVOID)(playerPointer + 0x205), &EntityName, sizeof(char[20]), NULL);
			cout << "   Name (" << &EntityName << ") : " << EntityName << endl;

			system("pause > null");

		}



		cout << "--------------------------------------" << endl;

	}
}