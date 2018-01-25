#include <fstream>
#include <iostream>
#include <string>
#include "struktura.hpp"
#include "pomocnicze.hpp"

using namespace std;

void pomoc(void)
{
	cout << "Wpisz -i <plik wej�ciowy> -o <plik wyj�ciowy>" << endl;
}

int main(int argc, char **argv)
{
	// wska�nik do pocz�tku listy etykiet
	etykieta_elem *struktura;

	// plik wej�ciowy
	ifstream ifile;

	// plik wyj�ciowy
	ofstream ofile;

	// aktualnie przetwarzana linia pliku wej�ciowego
	string linia;

	// przetwarzanie parametr�w przekazanych do programu z linii polece�
	string sciezka_wejsc; // scie�ka do pliku wej�ciowego
	string sciezka_wyjsc; // �cie�ka do pliku wyj�ciowego
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];

		if (arg == "-i") {
			if (++i == argc) {
				break;
			}

			sciezka_wejsc = argv[i];
		} else if (arg == "-o") {
			if (++i == argc) {
				break;
			}

			sciezka_wyjsc = argv[i];
		} else if (arg == "-h") {
			pomoc();
			return 1;
		}
	}

	// je�li nie wskazano kt�rego� z plik�w to zmienne ifile_path i/lub ofile_path b�d� puste - wyrzu� b��d
	if (sciezka_wejsc.empty() || sciezka_wyjsc.empty()) {
		cout << "nie wskazano wszystkich wymaganych parametr�w" << endl;
		pomoc();
		return 1;
	}

	// w przeciwnym wypadku (podano oba pliki) spr�buj je otworzy�
	ifile.open(sciezka_wejsc.c_str());
	ofile.open(sciezka_wyjsc.c_str());

	// upewnij si�, �e pliki s� gotowe do przetwarzania (mo�e si� to nie uda� np. jesli plik wej�ciowy nie istnieje)
	if (!ifile.is_open() || !ofile.is_open()) {
		cout << "pr�ba otwarcia wskazanych plik�w nie powiod�a si�" << endl;
		pomoc();
		return 1;
	}

	struktura = NULL;

	// przetwarzaj plik wej�ciowy linijka po linicje - aktualnie przetwarzana linia zapisywana jest zawsze do zmiennej "line", numer linii do "num"
	int num = 0;
	while (getline(ifile, linia)) {
		num++;

		// flaga wskazuj�ca na to, czy dla bie��co przetwarzanej linijki przetworozono ju� wszystkie etykiety
		bool final = false;

		string autor;
		string tytul;

		// wska�nik na ksi��k� opisywan� przez aktualnie przetwarzan� linijk�
		ksiazka *book;

		// pomi� lini�, je�li jest pusta (tj. zawiera wy��cznie bia�e znaki)
		linia = trim(linia);
		if (linia.length() == 0) {
			continue;
		}

		// odczytaj pierwszy segment (tj. napis a� do pierwszego znaku �rednika) i zapisz do zmiennej "author"
		bool znajdz_autora = separuj(linia, ';', autor);
		if (!znajdz_autora || autor.length() == 0) {
			cout << "nie uda�o si� odczyta� autora (pierwszy segment) w linijce " << num << endl;
			return 1;
		}

		// odczytaj drugi segment (tj. napis a� do drugiego znaku �rednika) i zapisz do zmiennej "title"
		bool znajdz_tytul = separuj(linia, ';', tytul);
		if (!znajdz_tytul || tytul.length() == 0) {
			cout << "nie uda�o si� odczyta� tytu�u (drugi segment) w linijce " << num << endl;
			return 1;
		}

		// utw�rz ksi��k� pod adresem na kt�ry wskazuje "*book"
		book = utworz_ksiazke(autor, tytul);

		// przetwarzaj etykiety opisuj�ce ksi��k�
		do {
			string etykieta;

			// odczytaj etykiet� (tj. napis a� do nast�pnego przecinka)
			bool znajdz_etykiete = separuj(linia, ',', etykieta);

			// je�li nie znaleziono etykiety (nie by�o kolejnego przecinka) to zak�adamy, �e do ko�ca linijki jest ju� tylko jedna ostatnia etykieta
			if (!znajdz_etykiete) {
				etykieta = trim(linia); // przytnij bia�e znaki
				final = true;       // oznacz, �e wczytana etkieta by�a ostatni� i b�dzie mo�na zako�czy� p�tl�
			}

			if (etykieta.length() == 0) {
				cout << "znaleziono pust� etykiet� w linijce " << num << endl;
				return 1;
			}

			// umie�� ksi��k� pod etykiet� odczytan� w bie��cym przebiegu p�tli
			wstaw_ksiazke(&struktura, etykieta, book);
		} while (!final);
	}

	// zamknij plik wej�ciowy (zako�cz jego przetwarzanie)
	ifile.close();

	// przekszta�� list� etykiet z ksi�zkami na plik wyj�ciowy i zapisz
	struktura_do_pliku(struktura, ofile);

	// zamknij plik wyj�ciowy
	ofile.close();

	// zwolnij pami�� (usu� wczytane ksi�zki i etykiety z pami�ci)
	zwolnij_pamiec(&struktura);

	// zako�cz program z powodzeniem
	cout << "plik wyj�ciowy zapisano w lokalizacji " << sciezka_wyjsc << endl;
	return 0;
}
