#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <time.h>

using namespace std;

#define N 5

#define LEFT (i-1)%N //Левый сосед философа с номером i - 1
#define RIGHT (i+1)%N //Правый сосед философа с номером i + 1

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int philosopher_state[N];

struct Philosopher
{
	int number;
};

CRITICAL_SECTION cs; //Для критических секций: синхрон. процессов(философов)    
CRITICAL_SECTION cs_forks; //и синхр. вилок

HANDLE philMutex[N];
HANDLE forkMutex[N];

void think(int i)
{
	EnterCriticalSection(&cs);
	cout << "Philosopher " << i << " thinking" << endl;
	LeaveCriticalSection(&cs);
}

void eat(int i)
{
	EnterCriticalSection(&cs);
	cout << "Philosopher " << i << " eating" << endl;
	LeaveCriticalSection(&cs);
}

void test_for_dinner(int i)
{
	if (philosopher_state[i] == HUNGRY && philosopher_state[LEFT] != EATING && philosopher_state[RIGHT] != EATING)
	{
		philosopher_state[i] = EATING;
		ReleaseMutex(philMutex[i]);
	}
}

void take_forks(int i)
{
	EnterCriticalSection(&cs_forks);
	philosopher_state[i] = HUNGRY;
	test_for_dinner(i);
	LeaveCriticalSection(&cs_forks);
	WaitForSingleObject(philMutex[i], INFINITE);
}

void put_forks(int i)
{
	EnterCriticalSection(&cs_forks);  
	philosopher_state[i] = THINKING;
	test_for_dinner(LEFT);
	test_for_dinner(RIGHT);
	LeaveCriticalSection(&cs_forks);
}

DWORD WINAPI philosopher(void* lParam)
{
	Philosopher phil = *((Philosopher*)lParam);
	int i = phil.number;

	while (true)
	{
		think(phil.number);
		take_forks(phil.number);
		eat(phil.number);
		put_forks(phil.number);

		Sleep(10);
	}
}

int main()
{
	Philosopher phil[N];
	for (int i = 0; i < N; i++)
	{
		phil[i].number = i;
	}

	for (int i = 0; i < N; i++)
	{
		philMutex[i] = CreateMutex(NULL, FALSE, NULL);
		forkMutex[i] = CreateMutex(NULL, FALSE, NULL);
	}

	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&cs_forks);

	DWORD id[N];
	HANDLE hThread[N];

	for (int i = 0; i < N; i++)
	{
		hThread[i] = CreateThread(NULL, NULL, &philosopher, &phil[i], NULL, &id[i]);
	}

	Sleep(INFINITE); 
	while (true);
}
