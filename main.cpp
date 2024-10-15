
/*
LocalPlayer[ac_client.exe + 0x0017E0A8]
Entity List[ac_client.exe + 0x18AC04]
FOV[ac_client.exe + 0x18A7CC]
PlayerCount[ac_client.exe + 0x18AC0C]

Position X[0x2C]
Position Y[0x30]
Position Z[0x28]

Head Position X[0x4]
Head Position Y[0xC]
Head Position Z[0x8]

Player Camera X[0x34]
Player Camera Y[0x38]

Assault Rifle Ammo[0x140]
Submachine Gun Ammo[0x138]
Sniper Ammo[0x13C]
Shotgun[0x134]
Pistol Ammo[0x12C]
Grenade Ammo[0x144]

Fast fire Assault Rifle[0x164]
Fast fire Sniper[0x160]
Fast fire Shotgun[0x158]

Auto shoot[0x204]
Health Value[0xEC]
Armor Value[0xF0]
Player Name[0x205]
*/

#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>

using namespace std;

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

int main() {
	int varInt = 123456;
	string varString = "DefaultString";
	char arrChar[128] = "Long char array right there";
	int* ptr2int = &varInt;
	int** ptr2ptr = &ptr2int;
	int*** ptr2ptr2 = &ptr2ptr;

	while(true){
		/*DWORD processID = GetCurrentProcessId();

		cout << endl << "Process ID: " << processID << endl << endl;
		cout << "varInt (0x" << &varInt << ") = " << varInt << endl;
		cout << "varString (0x" << &varString << ") = " << varString << endl;
		cout << "varChar[128] (0x" << &arrChar << ") = " << arrChar << endl << endl;
		cout << "ptr2int (0x" << &ptr2int << ") = " << ptr2int << endl;
		cout << "ptr2ptr (0x" << &ptr2ptr << ") = " << ptr2ptr << endl;
		cout << "ptr2ptr2 (0x" << &ptr2ptr2 << ") = " << ptr2ptr2 << endl << endl;
		
		cout << "Press ENTER to print again." << endl << endl;


		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
		if (!processHandle) {
			cout << "OpenProcess failed. GetLastError : " << dec << GetLastError() << endl;
			system("pause");
			return EXIT_FAILURE;
		}
		
		cout << "processHandle : " << processHandle << endl << endl;

		int intRead(0);

		cout << "intRead (0x" << &intRead << ") = " << intRead << endl;

		ReadProcessMemory(processHandle, (LPCVOID)ptr2int, &intRead, sizeof(int), NULL);
		
		cout << "intRead (0x" << &intRead << ") = " << intRead << endl << endl;

		char charRead[128];

		cout << "charRead (0x" << &charRead << ") = " << charRead << endl;

		ReadProcessMemory(processHandle, (LPCVOID)&arrChar, &charRead, sizeof(char[128]), NULL);

		cout << "charRead (0x" << &charRead << ") = " << charRead << endl << endl;

		int intToWrite(1111);

		cout << "Adding to (0x" << varInt << ") varInt " << intToWrite << endl;
		cout << "----------------------------------------" << endl << endl;

		cout << "varInt (0x" << &varInt << ") = " << varInt << endl;

		WriteProcessMemory(processHandle, (LPVOID)&varInt, &intToWrite, sizeof(int), NULL);

		cout << "varInt (0x" << &varInt << ") = " << varInt << endl << endl;

		CloseHandle(processHandle);*/
		
		// Ammo pointer : "ac_client.exe"+00109B74
		// ammo pointer with base : 0A8CA2A0
		// ammo offset for pointer : 0x150 

		DWORD assaultcube_PID;
		HWND assaultcube_Process = FindWindowA(0, "AssaultCube");
		if (assaultcube_Process == NULL) {
			cout << "Didn't found Assault Cube window. Last Error : " << dec << GetLastError() << endl;
			system("pause");
			return EXIT_FAILURE;
		}
		
		cout << "assaultcube_Process (" << &assaultcube_Process << ") : " << assaultcube_Process << endl;

		GetWindowThreadProcessId(assaultcube_Process, &assaultcube_PID);
		if (assaultcube_PID == NULL) {
			cout << "Didn't found Assault Cube PID (did you close the game ?). Last Error : " << dec << GetLastError() << endl;
			system("pause");
			return EXIT_FAILURE;
		}

		cout << "assaultcube_PID (" << &assaultcube_PID << ") : " << assaultcube_PID << endl;

		HANDLE assaultcube = OpenProcess(PROCESS_ALL_ACCESS, TRUE, assaultcube_PID);

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
		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + 0x109B74), &localplayer, sizeof(uintptr_t), NULL);

		cout << "localplayer (" << hex << &localplayer << ") : " << localplayer << endl;


		int PlayerAmmo(0);
		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + 0x150), &PlayerAmmo, sizeof(int), NULL);
		

		cout << "PlayerAmmo (" << hex << &PlayerAmmo << ") : " << dec << PlayerAmmo << endl;

		int ammoWanted(100);

		cout << endl << "Choose the number of ammo for the rifle : " << endl;
		cin >> ammoWanted;

		WriteProcessMemory(assaultcube, (LPVOID)(localplayer + 0x150), &ammoWanted, sizeof(int), NULL);


		system("pause > null");

		cout << "--------------------------------------" << endl;

	}
}