
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>

using namespace std;

boolean Aimbot = true;

struct Vector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3() {}

	Vector3 operator+ (Vector3& a) {
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator- (Vector3& a) {
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	Vector3 operator/ (Vector3& a) {
		return Vector3(x / a.x, y / a.y, z / a.z);
	}

	Vector3 operator* (Vector3& a) {
		return Vector3(x * a.x, y * a.y, z * a.z);
	}

	float hypo3() {
		return sqrt(((x * x) + (y * y) + (z * z)));
	}
};

struct Vector2 {
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

auto todegree = [](const float radians) {
	return radians * 180.f / (atan(1) * 4);
};
void aimbot(HANDLE assaultcube, uintptr_t assaultcubeBaseAdrr) {





	while (Aimbot == true) {

		// pointers

		uintptr_t PlayerCountPTR = 0x18AC0C;
		uintptr_t entitylistPTR = 0x18AC04;
		uintptr_t headPosPTR = 0x4;
		uintptr_t viewAnglesPTR = 0x34;
		uintptr_t playerTeamPTR = 0x30C;
		uintptr_t EntityList;

		// variables

		int playercounter(0);
		int playerTeam(0);
		Vector3 headpos;
		Vector2 viewAngles;
		uintptr_t localplayer;
		// variables setup

		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + entitylistPTR), &EntityList, 4, NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + 0x18AC0C), &playercounter, sizeof(int), NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(assaultcubeBaseAdrr + 0x18AC00), &localplayer, sizeof(uintptr_t), NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + headPosPTR), &headpos, sizeof(Vector3), NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + playerTeamPTR), &playerTeam, sizeof(int), NULL);
		ReadProcessMemory(assaultcube, (LPCVOID)(localplayer + viewAnglesPTR), &viewAngles, sizeof(Vector2), NULL);
		

		if (playercounter <= 1) {
			cout << "For the aimbot to start, you must be more than 1 player in the game" << endl;
			system("pause");
			return;
		}

		// Finding nearest enemy

		Vector3 nearestCoords;
		float NearestDistance(1999999);
		int nearestPointer;
		string NearestName("");

		while (true) {

			for (int i = 1; i < playercounter; i++) {
				int playerPointer;
				ReadProcessMemory(assaultcube, (LPCVOID)(EntityList + i * 4), &playerPointer, 4, NULL);

				char EntityName[20];
				ReadProcessMemory(assaultcube, (LPCVOID)(playerPointer + 0x205), &EntityName, sizeof(char[20]), NULL);

				int team;
				ReadProcessMemory(assaultcube, (LPCVOID)(playerPointer + 0x30C), &team, sizeof(int), NULL);

				int health;
				ReadProcessMemory(assaultcube, (LPCVOID)(playerPointer + 0xEC), &health, sizeof(int), NULL);

				Vector3 headcoords;
				ReadProcessMemory(assaultcube, (LPCVOID)(playerPointer + 0x4), &headcoords, sizeof(Vector3), NULL);
				cout << "   Name (" << &EntityName << ") : " << EntityName << "  Headcoords (" << &headcoords << ") : " << endl << headcoords.x << endl << headcoords.y << endl << headcoords.z << endl;

				float deltaX = headcoords.x - headpos.x;
				float deltaY = headcoords.y - headpos.y;
				float deltaZ = headcoords.z - headpos.z;

				float Distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2));

				cout << "Distance : " << Distance;

				if (Distance < NearestDistance && Distance < 1000 && health > 0 && health < 200 && team != playerTeam) {
					NearestDistance = Distance;
					nearestCoords = headcoords;
					NearestName = (string)EntityName;
					nearestPointer = playerPointer;
				}
			}
			Sleep(1);
		}


		Vector3 headcoords;
		ReadProcessMemory(assaultcube, (LPCVOID)(nearestPointer + 0x4), &headcoords, sizeof(Vector3), NULL);


		Vector3 delta = (nearestCoords - headpos);

		cout << "Player coords : " << headpos.x << " " << headpos.y << " " << headpos.z << endl;
		cout << "Entity coords : " << nearestCoords.x << " " << nearestCoords.y << " " << nearestCoords.z << endl;
		
		float yaw = todegree(atan2f(delta.y, delta.x));         // Find angle for x direction
		float hyp = sqrt(delta.x * delta.x + delta.y * delta.y );   // Find angle for y direction
		float pitch = todegree(atan2f(delta.z, hyp));
;
		Vector2 newAngles;
		newAngles.x = yaw + 90;
		newAngles.y = pitch + 0.5;

		// Set angles

		WriteProcessMemory(assaultcube, (LPVOID)(localplayer + viewAnglesPTR), &newAngles, sizeof(Vector2), NULL);

	}
}

int main() {


	while(true){
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

		aimbot(assaultcube, assaultcubeBaseAdrr);

		cout << "--------------------------------------" << endl;

	}
}	