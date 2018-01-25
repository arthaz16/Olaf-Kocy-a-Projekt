#include <string>

using namespace std;

string trim(string linia)//przytnij spacje (trim - od przycinania)
{
	int pozycja;

	pozycja = linia.find_first_not_of(' '); //funkcja find_first_not_of - znajdz pierwsza która nie jest (czymœ)
	if (pozycja != string::npos) {
		linia.erase(0, pozycja);
	}

	pozycja = linia.find_last_not_of(' ');
	if (pozycja != string::npos) {
		linia.erase(pozycja + 1, string::npos);
	}

	return linia;
}

bool separuj(string& linia, char separator, string& wynik)
{
	int pozycja;

	pozycja = linia.find_first_of(separator);
	if (pozycja == string::npos) {
		return false;
	}

	wynik = trim(linia.substr(0, pozycja)); //zmienna linia nie jest przekazywana jako const, bo jej wartoœæ siê zmienia w wyniku dzia³ania funkcji
	linia.erase(0, pozycja + 1);

	return true;
}