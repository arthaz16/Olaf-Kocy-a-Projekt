#include "struktura.hpp"

using namespace std;

// definicja struktury wêz³a drzewa z ksi¹¿kami
// (ka¿da etykieta na liœcie ma drzewo z ksi¹zkami)


// tworzenie nowego "pustego" wêz³a dla ksi¹zki (tj. bez dzieci)
static ksiazka_wezel* utworz_wezel(ksiazka *ksiazka)
{
	ksiazka_wezel *wezel;

	wezel = new ksiazka_wezel;
	wezel->ksiazka_ptr = ksiazka;
	wezel->left = NULL;
	wezel->right = NULL;

	ksiazka->licznik_referencji++; // zliczaj referencje do ksi¹¿ki (mo¿e byæ umieszczona w wielu drzewach, bo mo¿e mieæ wiele etykiet)

	return wezel;
}

// wstaw ksi¹¿kê do drzewa z ksi¹¿kami, którego korzeniem jest "**korzen"
static void wstaw_wezel_ksiazki(ksiazka_wezel **korzen, ksiazka *ksiazka)
{
	ksiazka_wezel *wezel;		// aktualnie odwiedzany wêze³ drzewa (podczas poszukiwania w³aœciwego miejsca dla ksi¹¿ki)

	// jeœli korzeñ drzewa jest pusty (pierwsza ksi¹zka dla danej etykiety) to utwórz nowy wêze³ i umieœæ go jako korzeñ
	if (!*korzen) {
		*korzen = utworz_wezel(ksiazka);
		return;
	}

	// ustaw korzeñ jako aktualnie odwiedzany - od tego miejsca zdecydujemy, czy iœæ w prawo, czy w lewo
	wezel = *korzen;

	if (ksiazka->autor < wezel->ksiazka_ptr->autor) { // autor ksi¹¿ki jest alfabetycznie przed autorem ksi¹zki w aktualnie odwiedzanym wêŸle - idziemy w lew¹ stronê
		if (wezel->left) { // jeœli w lewym wêŸle jest ju¿ ksi¹zka, to ustawiamy lewy wêze³ jako aktualnie odwiedzany
			wezel = wezel->left;
		} else { // jeœli lewy wêze³ nie istnieje - znaleŸliœmy miejsce dla nowej ksi¹zki, tworzymy dla niej wêze³ i zapisujemy do drzewa, konczymy dzia³anie funkcji
			wezel->left = utworz_wezel(ksiazka);
			return;
		}
	} else { // autor ksi¹¿ki jest alfabetycznie po autorze ksi¹zki w aktualnie odwiedzanym wêŸle - idziemy w praw¹ stronê i dalej analogicznie jak poprzednio
		if (wezel->right) {
			wezel = wezel->right;
		} else {
			wezel->right = utworz_wezel(ksiazka);
			return;
		}
	}

	// jeœli nie znaleziono jeszcze miejsca dla ksi¹¿ki, a jedynie wskazano poddrzewo to rekursj¹ przechodzimy do wyzanczonego poddrzewa i tam szukamy miejsca
	wstaw_wezel_ksiazki(&wezel, ksiazka);
}

// przejdŸ drzewo z ksi¹¿kami wg³¹b za pomoc¹ rekursji i zapisz alfabetycznie do pliku wyjœciowego
static void przejdz_drzewo(ksiazka_wezel *wezel, ostream& os)
{
	// jeœli dotarto do pustego wêz³a to zakoñcz drukowanie poddrzewa
	if (!wezel) {
		return;
	}

	// przejdŸ przez lewe poddrzewo
	przejdz_drzewo(wezel->left, os);

	// wydrukuj bie¿¹cy wêze³
	os << wezel->ksiazka_ptr->autor << "; " << wezel->ksiazka_ptr->tytul << endl;

	// przejdŸ przez prawe poddrzewo
	przejdz_drzewo(wezel->right, os);
}

// usuñ wêze³ z ksi¹¿k¹ (czyszczenie pamiêci)
static void kasuj_wezel(ksiazka_wezel *wezel)
{
	// jeœli dotarto do pustego wêz³a to usuniêto wszystkie referencja z poddrzewa
	if (!wezel) {
		return;
	}

	// wyczyœæ lewe poddrzewo
	kasuj_wezel(wezel->left);

	// wyczyœæ prawe poddrzewo
	kasuj_wezel(wezel->right);

	// zmniejsz licznik referencji do ksi¹¿ki, jesli usuniêto wszystkie referencje to pozb¹dŸ siê ksi¹zki ca³kowicie
	wezel->ksiazka_ptr->licznik_referencji--;
	if (wezel->ksiazka_ptr->licznik_referencji == 0) {
		delete wezel->ksiazka_ptr;
	}

	// po wyczyszczeniu poddrzew usuñ bie¿¹co przetwarzany wêze³
	delete wezel;
}

// dodaje etykietê o zadanej nazwie do listy etykiet, której czo³em jest "**head"
static etykieta_elem* wstaw_etykiete(etykieta_elem **head, const string& name)
{
	etykieta_elem *sprawdzany;	// aktualnie sprawdzany element listy z etykietamy

	// zainicjuj aktualnie sprawdzany element listy wskazaniem na czo³o listy
	sprawdzany = *head;

	// przechodŸ przez listê tak d³ugo jak s¹ na niej elementy
	while (sprawdzany) {
		if (sprawdzany->nazwa == name) {	// nazwa elementu jest taka sama jak nazwa etyiety - mo¿na skoñczyæ przechodzenie przez listê
			return sprawdzany;			// zwróæ element listy - do niego bêdziemy wpisywaæ ksi¹¿kê
		}

		sprawdzany = sprawdzany->nastepny;			// jeœli element na liœcie nie by³ tak¹ sam¹ etykiet¹, jak wstawiana to przejdŸ do kolejnego elementu
	}

	// nie znaleziono etykiety - nale¿y j¹ dodaæ na pocz¹tek listy
	sprawdzany = new etykieta_elem;
	sprawdzany->nazwa = name;
	sprawdzany->ksiazka_korzen = NULL;		// korzeñ drzewa z ksi¹¿kami, które maj¹ tê etykietê
	sprawdzany->nastepny = *head;			// ustaw czo³o listy jako element kolejny po w³aœnie wstawionym (bo wstawiamy na pocz¹tek)

	*head = sprawdzany;				// zaktualizuj wskaŸnik na czo³o - teraz czo³em jest nowy dodany element

	return sprawdzany;
}

// utwórz now¹ ksi¹zkê o zadanym tytule i autorze, ustaw jej licznik referencji na 0 - bêdziemy go zwiêkszaæ w miarê umieszczania ksi¹¿ki pod etykietami
ksiazka* utworz_ksiazke(const string& autor, const string& tytul)
{
	ksiazka *b = new ksiazka;
	b->autor = autor;
	b->tytul = tytul;
	b->licznik_referencji = 0;

	return b;
}

// umieœæ ksi¹¿kê w drzewie
// funkcja przyjmuje parametry opisuj¹ce czo³o listy z etykietami (**korzen), nazwê etykiety (etykieta) i referencjê do ksi¹zki, któr¹ trzeba dodaæ
// ksi¹¿ke przekazujemy przez referencjê, bo jesli trafi do kilku drzew to w ka¿dym zapamiêtamy wskaŸnik do tej samej ksi¹¿ki zapisanej w pamiêci, a nie bêdziemy duplikowaæ danych o niej
void wstaw_ksiazke(etykieta_elem **korzen, const string& etykieta, ksiazka *ksiazka)
{
	etykieta_elem *item;

	// znajdŸ lub dodaj etykietê i zwróæ j¹ - dodamy pod ni¹ ksi¹¿kê
	item = wstaw_etykiete(korzen, etykieta);

	// wstaw ksi¹¿kê do drzewa z ksi¹¿kami dla etykiety
	wstaw_wezel_ksiazki(&item->ksiazka_korzen, ksiazka);
}

// przekszta³canie listy etykiet na plik wyjœciowy
void struktura_do_pliku(etykieta_elem *root, ostream& os)
{
	while (root) {
		// wypisz nazwê etykiety
		os << root->nazwa << ":" << endl;

		// przejdŸ przez drzewo z ksi¹¿kami i wypisz je w kolejnoœci alfabetycznej
		przejdz_drzewo(root->ksiazka_korzen, os);
		os << endl;

		// przesuñ wskaŸnik na kolejny element listy
		root = root->nastepny;
	}
}

// zwalanianie pamiêci (usuwanie etykiet z listy)
void zwolnij_pamiec(etykieta_elem **root)
{
	etykieta_elem *item;	// aktualnie czyszczony element listy z etykietami
	etykieta_elem *tymczasowy;		// zmienna pomocnicza (do podmiany elementów)

	item = *root;				// zainicjuj aktualnie czyszczony element na czo³o listy

	while (item) {
		kasuj_wezel(item->ksiazka_korzen);	// przejdŸ przez drzewo z ksi¹¿kami i usuwaj referencje do nich

		// usuñ aktualnie przetwarzany wêze³ i przejdŸ do kolejnego
		tymczasowy = item;
		item = item->nastepny;
		delete tymczasowy;

		// uzasadnienie u¿ycie zmiennej tymczasowej "tymczasowy":
		// nie robimy bezpoœrednio "delete item", bo w ten sposób stracimy referencjê do kolejnego elementu
		// nie mo¿emy te¿ od razu przesun¹æ wskaŸnika na item-> next, bo wtedy stracimy referencjê do elementu, który chcemy usun¹æ
	}

	// po usuniêciu wszystkich elementów usuñ tak¿e korzeñ
	*root = NULL;
}
