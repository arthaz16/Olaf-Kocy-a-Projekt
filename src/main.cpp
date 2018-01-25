#include <fstream>
#include <iostream>
#include <string>
#include "struktura.hpp"
#include "pomocnicze.hpp"

using namespace std;

void pomoc(void)
{
	cout << "Wpisz -i <plik wejœciowy> -o <plik wyjœciowy>" << endl;
}

int main(int argc, char **argv)
{
	// wskaŸnik do pocz¹tku listy etykiet
	etykieta_elem *struktura;

	// plik wejœciowy
	ifstream ifile;

	// plik wyjœciowy
	ofstream ofile;

	// aktualnie przetwarzana linia pliku wejœciowego
	string linia;

	// przetwarzanie parametrów przekazanych do programu z linii poleceñ
	string sciezka_wejsc; // scie¿ka do pliku wejœciowego
	string sciezka_wyjsc; // œcie¿ka do pliku wyjœciowego
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

	// jeœli nie wskazano któregoœ z plików to zmienne ifile_path i/lub ofile_path bêd¹ puste - wyrzuæ b³¹d
	if (sciezka_wejsc.empty() || sciezka_wyjsc.empty()) {
		cout << "nie wskazano wszystkich wymaganych parametrów" << endl;
		pomoc();
		return 1;
	}

	// w przeciwnym wypadku (podano oba pliki) spróbuj je otworzyæ
	ifile.open(sciezka_wejsc.c_str());
	ofile.open(sciezka_wyjsc.c_str());

	// upewnij siê, ¿e pliki s¹ gotowe do przetwarzania (mo¿e siê to nie udaæ np. jesli plik wejœciowy nie istnieje)
	if (!ifile.is_open() || !ofile.is_open()) {
		cout << "próba otwarcia wskazanych plików nie powiod³a siê" << endl;
		pomoc();
		return 1;
	}

	struktura = NULL;

	// przetwarzaj plik wejœciowy linijka po linicje - aktualnie przetwarzana linia zapisywana jest zawsze do zmiennej "line", numer linii do "num"
	int num = 0;
	while (getline(ifile, linia)) {
		num++;

		// flaga wskazuj¹ca na to, czy dla bie¿¹co przetwarzanej linijki przetworozono ju¿ wszystkie etykiety
		bool final = false;

		string autor;
		string tytul;

		// wskaŸnik na ksi¹¿kê opisywan¹ przez aktualnie przetwarzan¹ linijkê
		ksiazka *book;

		// pomiñ liniê, jeœli jest pusta (tj. zawiera wy³¹cznie bia³e znaki)
		linia = trim(linia);
		if (linia.length() == 0) {
			continue;
		}

		// odczytaj pierwszy segment (tj. napis a¿ do pierwszego znaku œrednika) i zapisz do zmiennej "author"
		bool znajdz_autora = separuj(linia, ';', autor);
		if (!znajdz_autora || autor.length() == 0) {
			cout << "nie uda³o siê odczytaæ autora (pierwszy segment) w linijce " << num << endl;
			return 1;
		}

		// odczytaj drugi segment (tj. napis a¿ do drugiego znaku œrednika) i zapisz do zmiennej "title"
		bool znajdz_tytul = separuj(linia, ';', tytul);
		if (!znajdz_tytul || tytul.length() == 0) {
			cout << "nie uda³o siê odczytaæ tytu³u (drugi segment) w linijce " << num << endl;
			return 1;
		}

		// utwórz ksi¹¿kê pod adresem na który wskazuje "*book"
		book = utworz_ksiazke(autor, tytul);

		// przetwarzaj etykiety opisuj¹ce ksi¹¿kê
		do {
			string etykieta;

			// odczytaj etykietê (tj. napis a¿ do nastêpnego przecinka)
			bool znajdz_etykiete = separuj(linia, ',', etykieta);

			// jeœli nie znaleziono etykiety (nie by³o kolejnego przecinka) to zak³adamy, ¿e do koñca linijki jest ju¿ tylko jedna ostatnia etykieta
			if (!znajdz_etykiete) {
				etykieta = trim(linia); // przytnij bia³e znaki
				final = true;       // oznacz, ¿e wczytana etkieta by³a ostatni¹ i bêdzie mo¿na zakoñczyæ pêtlê
			}

			if (etykieta.length() == 0) {
				cout << "znaleziono pust¹ etykietê w linijce " << num << endl;
				return 1;
			}

			// umieœæ ksi¹¿kê pod etykiet¹ odczytan¹ w bie¿¹cym przebiegu pêtli
			wstaw_ksiazke(&struktura, etykieta, book);
		} while (!final);
	}

	// zamknij plik wejœciowy (zakoñcz jego przetwarzanie)
	ifile.close();

	// przekszta³æ listê etykiet z ksi¹zkami na plik wyjœciowy i zapisz
	struktura_do_pliku(struktura, ofile);

	// zamknij plik wyjœciowy
	ofile.close();

	// zwolnij pamiêæ (usuñ wczytane ksi¹zki i etykiety z pamiêci)
	zwolnij_pamiec(&struktura);

	// zakoñcz program z powodzeniem
	cout << "plik wyjœciowy zapisano w lokalizacji " << sciezka_wyjsc << endl;
	return 0;
}
