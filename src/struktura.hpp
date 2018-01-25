#ifndef STRUKTURA_HPP
#define STRUKTURA_HPP

#include <string>
#include <ostream>

using namespace std;

struct ksiazka {
	string autor;
	string tytul;
	int licznik_referencji;
};

struct ksiazka_wezel {
	ksiazka *ksiazka_ptr;				// wskaźnik na książkę zapisaną w węźle 
	struct ksiazka_wezel *left;		// książka w lewym węźle-dziecku (tj. nazwisko autora alfabetycznie przed autorem książki w węźle)
	struct ksiazka_wezel *right;	// książka w prawym węźle-dziecku (tj. nazwisko autora alfabetycznie po autorze książki w węźle)
};

struct etykieta_elem {
	string nazwa;
	struct ksiazka_wezel *ksiazka_korzen;
	struct etykieta_elem *nastepny;
};

ksiazka* utworz_ksiazke(const string& autor, const string& tytul);
void wstaw_ksiazke(etykieta_elem **pierwszy_elem, const string& etykieta, ksiazka *ksiazka_ptr); //wstaw ksiazke do etykiety
void struktura_do_pliku(etykieta_elem *pierwszy_elem, ostream& os); //zapisz strukture do pliku
void zwolnij_pamiec(etykieta_elem **pierwszy_elem); //zwalnianie pamieci

#endif
