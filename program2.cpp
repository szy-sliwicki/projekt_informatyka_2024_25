#include <iostream>
#include <cstring>

using namespace std;

int main()
{
	string imie;
	cout << "Podaj imie: ";
	cin >> imie;
	cin.ignore();
	cout << "Witaj szefie" << imie << endl;
	cout << "Gratulacje. To Twoj pierwszy program!" << endl;
	cout << "Nacisnij ENTER aby zakonczyc" << endl;
	getchar();
	return 0;
}