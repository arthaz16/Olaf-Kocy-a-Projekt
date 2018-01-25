#include "struktura.hpp"

using namespace std;

// definicja struktury w�z�a drzewa z ksi��kami
// (ka�da etykieta na li�cie ma drzewo z ksi�zkami)


// tworzenie nowego "pustego" w�z�a dla ksi�zki (tj. bez dzieci)
static ksiazka_wezel* utworz_wezel(ksiazka *ksiazka)
{
	ksiazka_wezel *wezel;

	wezel = new ksiazka_wezel;
	wezel->ksiazka_ptr = ksiazka;
	wezel->left = NULL;
	wezel->right = NULL;

	ksiazka->licznik_referencji++; // zliczaj referencje do ksi��ki (mo�e by� umieszczona w wielu drzewach, bo mo�e mie� wiele etykiet)

	return wezel;
}

// wstaw ksi��k� do drzewa z ksi��kami, kt�rego korzeniem jest "**korzen"
static void wstaw_wezel_ksiazki(ksiazka_wezel **korzen, ksiazka *ksiazka)
{
	ksiazka_wezel *wezel;		// aktualnie odwiedzany w�ze� drzewa (podczas poszukiwania w�a�ciwego miejsca dla ksi��ki)

	// je�li korze� drzewa jest pusty (pierwsza ksi�zka dla danej etykiety) to utw�rz nowy w�ze� i umie�� go jako korze�
	if (!*korzen) {
		*korzen = utworz_wezel(ksiazka);
		return;
	}

	// ustaw korze� jako aktualnie odwiedzany - od tego miejsca zdecydujemy, czy i�� w prawo, czy w lewo
	wezel = *korzen;

	if (ksiazka->autor < wezel->ksiazka_ptr->autor) { // autor ksi��ki jest alfabetycznie przed autorem ksi�zki w aktualnie odwiedzanym w�le - idziemy w lew� stron�
		if (wezel->left) { // je�li w lewym w�le jest ju� ksi�zka, to ustawiamy lewy w�ze� jako aktualnie odwiedzany
			wezel = wezel->left;
		} else { // je�li lewy w�ze� nie istnieje - znale�li�my miejsce dla nowej ksi�zki, tworzymy dla niej w�ze� i zapisujemy do drzewa, konczymy dzia�anie funkcji
			wezel->left = utworz_wezel(ksiazka);
			return;
		}
	} else { // autor ksi��ki jest alfabetycznie po autorze ksi�zki w aktualnie odwiedzanym w�le - idziemy w praw� stron� i dalej analogicznie jak poprzednio
		if (wezel->right) {
			wezel = wezel->right;
		} else {
			wezel->right = utworz_wezel(ksiazka);
			return;
		}
	}

	// je�li nie znaleziono jeszcze miejsca dla ksi��ki, a jedynie wskazano poddrzewo to rekursj� przechodzimy do wyzanczonego poddrzewa i tam szukamy miejsca
	wstaw_wezel_ksiazki(&wezel, ksiazka);
}

// przejd� drzewo z ksi��kami wg��b za pomoc� rekursji i zapisz alfabetycznie do pliku wyj�ciowego
static void przejdz_drzewo(ksiazka_wezel *wezel, ostream& os)
{
	// je�li dotarto do pustego w�z�a to zako�cz drukowanie poddrzewa
	if (!wezel) {
		return;
	}

	// przejd� przez lewe poddrzewo
	przejdz_drzewo(wezel->left, os);

	// wydrukuj bie��cy w�ze�
	os << wezel->ksiazka_ptr->autor << "; " << wezel->ksiazka_ptr->tytul << endl;

	// przejd� przez prawe poddrzewo
	przejdz_drzewo(wezel->right, os);
}

// usu� w�ze� z ksi��k� (czyszczenie pami�ci)
static void kasuj_wezel(ksiazka_wezel *wezel)
{
	// je�li dotarto do pustego w�z�a to usuni�to wszystkie referencja z poddrzewa
	if (!wezel) {
		return;
	}

	// wyczy�� lewe poddrzewo
	kasuj_wezel(wezel->left);

	// wyczy�� prawe poddrzewo
	kasuj_wezel(wezel->right);

	// zmniejsz licznik referencji do ksi��ki, jesli usuni�to wszystkie referencje to pozb�d� si� ksi�zki ca�kowicie
	wezel->ksiazka_ptr->licznik_referencji--;
	if (wezel->ksiazka_ptr->licznik_referencji == 0) {
		delete wezel->ksiazka_ptr;
	}

	// po wyczyszczeniu poddrzew usu� bie��co przetwarzany w�ze�
	delete wezel;
}

// dodaje etykiet� o zadanej nazwie do listy etykiet, kt�rej czo�em jest "**head"
static etykieta_elem* wstaw_etykiete(etykieta_elem **head, const string& name)
{
	etykieta_elem *sprawdzany;	// aktualnie sprawdzany element listy z etykietamy

	// zainicjuj aktualnie sprawdzany element listy wskazaniem na czo�o listy
	sprawdzany = *head;

	// przechod� przez list� tak d�ugo jak s� na niej elementy
	while (sprawdzany) {
		if (sprawdzany->nazwa == name) {	// nazwa elementu jest taka sama jak nazwa etyiety - mo�na sko�czy� przechodzenie przez list�
			return sprawdzany;			// zwr�� element listy - do niego b�dziemy wpisywa� ksi��k�
		}

		sprawdzany = sprawdzany->nastepny;			// je�li element na li�cie nie by� tak� sam� etykiet�, jak wstawiana to przejd� do kolejnego elementu
	}

	// nie znaleziono etykiety - nale�y j� doda� na pocz�tek listy
	sprawdzany = new etykieta_elem;
	sprawdzany->nazwa = name;
	sprawdzany->ksiazka_korzen = NULL;		// korze� drzewa z ksi��kami, kt�re maj� t� etykiet�
	sprawdzany->nastepny = *head;			// ustaw czo�o listy jako element kolejny po w�a�nie wstawionym (bo wstawiamy na pocz�tek)

	*head = sprawdzany;				// zaktualizuj wska�nik na czo�o - teraz czo�em jest nowy dodany element

	return sprawdzany;
}

// utw�rz now� ksi�zk� o zadanym tytule i autorze, ustaw jej licznik referencji na 0 - b�dziemy go zwi�ksza� w miar� umieszczania ksi��ki pod etykietami
ksiazka* utworz_ksiazke(const string& autor, const string& tytul)
{
	ksiazka *b = new ksiazka;
	b->autor = autor;
	b->tytul = tytul;
	b->licznik_referencji = 0;

	return b;
}

// umie�� ksi��k� w drzewie
// funkcja przyjmuje parametry opisuj�ce czo�o listy z etykietami (**korzen), nazw� etykiety (etykieta) i referencj� do ksi�zki, kt�r� trzeba doda�
// ksi��ke przekazujemy przez referencj�, bo jesli trafi do kilku drzew to w ka�dym zapami�tamy wska�nik do tej samej ksi��ki zapisanej w pami�ci, a nie b�dziemy duplikowa� danych o niej
void wstaw_ksiazke(etykieta_elem **korzen, const string& etykieta, ksiazka *ksiazka)
{
	etykieta_elem *item;

	// znajd� lub dodaj etykiet� i zwr�� j� - dodamy pod ni� ksi��k�
	item = wstaw_etykiete(korzen, etykieta);

	// wstaw ksi��k� do drzewa z ksi��kami dla etykiety
	wstaw_wezel_ksiazki(&item->ksiazka_korzen, ksiazka);
}

// przekszta�canie listy etykiet na plik wyj�ciowy
void struktura_do_pliku(etykieta_elem *root, ostream& os)
{
	while (root) {
		// wypisz nazw� etykiety
		os << root->nazwa << ":" << endl;

		// przejd� przez drzewo z ksi��kami i wypisz je w kolejno�ci alfabetycznej
		przejdz_drzewo(root->ksiazka_korzen, os);
		os << endl;

		// przesu� wska�nik na kolejny element listy
		root = root->nastepny;
	}
}

// zwalanianie pami�ci (usuwanie etykiet z listy)
void zwolnij_pamiec(etykieta_elem **root)
{
	etykieta_elem *item;	// aktualnie czyszczony element listy z etykietami
	etykieta_elem *tymczasowy;		// zmienna pomocnicza (do podmiany element�w)

	item = *root;				// zainicjuj aktualnie czyszczony element na czo�o listy

	while (item) {
		kasuj_wezel(item->ksiazka_korzen);	// przejd� przez drzewo z ksi��kami i usuwaj referencje do nich

		// usu� aktualnie przetwarzany w�ze� i przejd� do kolejnego
		tymczasowy = item;
		item = item->nastepny;
		delete tymczasowy;

		// uzasadnienie u�ycie zmiennej tymczasowej "tymczasowy":
		// nie robimy bezpo�rednio "delete item", bo w ten spos�b stracimy referencj� do kolejnego elementu
		// nie mo�emy te� od razu przesun�� wska�nika na item-> next, bo wtedy stracimy referencj� do elementu, kt�ry chcemy usun��
	}

	// po usuni�ciu wszystkich element�w usu� tak�e korze�
	*root = NULL;
}
