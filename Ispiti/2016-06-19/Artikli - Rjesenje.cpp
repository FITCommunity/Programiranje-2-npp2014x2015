#include <iostream>
#include <fstream>

using namespace std;

char * crt = "\n----------------------------------------------------\n";
char brojRacunaTemplate[10] = { "000000000" };
int brojacRacuna = 1;

struct Datum
{
    int * _dan, *_mjesec, *_godina;
    void Unos(int d, int m, int g)
    {
        _dan = new int(d);
        _mjesec = new int(m);
        _godina = new int(g);
    }
    void Ispis() { cout << *_dan << "/" << *_mjesec << "/" << *_godina << endl; }
    void Dealociraj()
    {
        delete _dan; delete _mjesec; delete _godina;
        _dan = _mjesec = _godina = nullptr;
    }
};
struct Artikal
{
    char * _naziv;
    float _cijena;
    void Unos(char * naziv, float cijena)
    {
        int vel = strlen(naziv) + 1;
        _naziv = new char[vel];
        strcpy_s(_naziv, vel, naziv);
        _cijena = cijena;
    }
    void Dealociraj() { delete[]_naziv; _naziv = nullptr; }
    void Ispis()
    {
        if (this != nullptr)
            cout << _naziv << " " << _cijena << "KM";
        return;
    }
};
struct Racun
{
    //format broja racuna: 006005491
    //osigurati automatsko generisanje broja racuna pocevsi od 000000001
    char _brojRacuna[10];
    Datum _datumKreiranja;
    Artikal * _artikli;
    int * _kolicine;//cuva informaciju o kolicini svakog artikla na racunu
    int _brojArtikala;
    //Potpisi funkcija trebaju odgovarati onima u main-u
    //1. Unos
    void Unos(char * brojRacuna, Datum datum)
    {
        strcpy_s(_brojRacuna, 10, brojRacuna);
        _datumKreiranja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
        _artikli = nullptr;
        _brojArtikala = 0;
        _kolicine = nullptr;
    }
    //2. Dealociraj
    void Dealociraj()
    {
        _datumKreiranja.Dealociraj();
        for (int i = 0; i < _brojArtikala; i++)
            _artikli[i].Dealociraj();
        delete[] _artikli; _artikli = nullptr;
        delete[] _kolicine; _kolicine = nullptr;
    }
    //3. DodajArtikal - dodaje novi artikal u listu artikala zajedno sa njegovom kolicinom.
    //Onemoguciti ponavljanje artikala na nacin da se uvecava samo kolicina ukoliko
    //korisnik vise puta pokusa dodati isti artikal.
    void DodajArtikal(Artikal artikal, int kolicina)
    {
        for (int i = 0; i < _brojArtikala; i++)
            if (strcmp(_artikli[i]._naziv, artikal._naziv) == 0)
            {
                _kolicine[i] += kolicina;
                return;
            }

        Artikal * temp = new Artikal[_brojArtikala + 1];
        int * tempKolicina = new int[_brojArtikala + 1];
        for (int i = 0; i < _brojArtikala; i++)
        {
            temp[i].Unos(_artikli[i]._naziv, _artikli[i]._cijena);
            _artikli[i].Dealociraj();

            tempKolicina[i] = _kolicine[i];
        }
        temp[_brojArtikala].Unos(artikal._naziv, artikal._cijena);
        tempKolicina[_brojArtikala] = kolicina;
        delete[] _artikli; _artikli = temp;
        delete[] _kolicine; _kolicine = tempKolicina;
        _brojArtikala++;
    }
    //4. Ispis - ispisuje racun u formatu prikazanom na slici (nije obavezno da bude 
    //identican, ali je svakako pozeljno).Prilikom svakog ispisa, racun
    //je potrebno spasiti u tekstualni fajl sa istim nazivom kao i broj racuna.
    void Ispis()
    {
        cout << crt << _brojRacuna << " Datum: ";
        _datumKreiranja.Ispis();
        cout << crt << "Naziv Cijena\tKolicina\tIznos" << crt;

        float suma = 0;
        for (int i = 0; i < _brojArtikala; i++)
        {
            _artikli[i].Ispis();
            cout << "\t" << _kolicine[i] << "\t\t" << _kolicine[i] * _artikli[i]._cijena << "KM\n";
            suma += _kolicine[i] * _artikli[i]._cijena;
        }
        cout << crt;
        cout << "Ukupno:\t\t\t\t" << suma << "KM" << crt;

        char * nazivFajla = new char[10 + 4];
        strcpy_s(nazivFajla, 14, _brojRacuna);
        strcat_s(nazivFajla, 14, ".txt");

        ofstream upis; upis.open(nazivFajla);
        if (upis.fail())
            cout << "Greska!" << endl;
        else
        {
            upis << crt << _brojRacuna << " Datum: "
                << *_datumKreiranja._dan << " "
                << *_datumKreiranja._mjesec << " "
                << *_datumKreiranja._godina << crt
                << "Naziv Cijena\tKolicina\tIznos" << crt;


            for (int i = 0; i < _brojArtikala; i++)
            {
                upis << _artikli[i]._naziv << " " << _artikli[i]._cijena << "KM"
                    << "\t" << _kolicine[i] << "\t\t" << _kolicine[i] * _artikli[i]._cijena << "KM\n";
            }

            upis << crt << "Ukupno:\t\t\t\t" << suma << "KM" << crt;
            upis.close();
        }

    }
    //5. BinarnaPretraga - koristeci binarnu pretragu, na osnovu naziva, pronalazi i vraca 
    //pokazivac na artikal.Ukoliko trazeni artikal ne postoji funkcija vraca nullptr.
    void Sortiraj()
    {
        bool sortiran = true;
        while (sortiran)
        {

            for (int i = 0; i < _brojArtikala - 1; i++)
            {
                sortiran = false;
                if (strcmp(_artikli[i]._naziv, _artikli[i + 1]._naziv) > 0)
                {
                    Artikal temp;
                    int tempKolicina;

                    temp.Unos(_artikli[i]._naziv, _artikli[i]._cijena);
                    tempKolicina = _kolicine[i];

                    _artikli[i].Unos(_artikli[i + 1]._naziv, _artikli[i + 1]._cijena);
                    _kolicine[i] = _kolicine[i + 1];

                    _artikli[i + 1].Unos(temp._naziv, temp._cijena);
                    _kolicine[i + 1] = tempKolicina;

                    sortiran = true;
                }
            }
        }
    }
    Artikal * BinarnaPretraga(char * naziv)
    {
        Sortiraj();

        int pocetak = 0,
            kraj = _brojArtikala - 1,
            sidro;

        while (pocetak <= kraj)
        {
            sidro = (pocetak + kraj) / 2;

            if (strcmp(naziv, _artikli[sidro]._naziv) == 0)
                return &_artikli[sidro];
            else if (strcmp(naziv, _artikli[sidro]._naziv) < 0)
                kraj = sidro - 1;
            else if (strcmp(naziv, _artikli[sidro]._naziv) > 0)
                pocetak = sidro + 1;
        }

        return nullptr;
    }
};
//1. PronadjiNajskupljiArtikal - rekurzivna funkcija koja vraca pokazivac na artikal sa najvecom ukupnom cijenom (cijena*kolicina)
Artikal * PronadjiNajskupljiArtikal(Racun racun, int i)
{
    if (i == 0) return &racun._artikli[i];

    if (racun._artikli[i]._cijena * racun._kolicine[i] < racun._artikli[i - 1]._cijena * racun._kolicine[i - 1])
        return PronadjiNajskupljiArtikal(racun, i - 1);
    else
        return &racun._artikli[i];
}
//2. GenerisiSljedeciBrojRacuna - generise i vraca naredni broj racuna
char * GenerisiSljedeciBrojRacuna()
{
    int temp = brojacRacuna;
    int i = 8;

    while (i >= 0 && temp != 0)
    {
        brojRacunaTemplate[i] = char(temp % 10 + 48);
        temp /= 10;
        i--;
    }

    brojacRacuna++;
    return brojRacunaTemplate;
}

void main()
{
    //za eventualne nejasnoce analizirati testni program i ispise na slici
    Datum danas; danas.Unos(13, 6, 2016);
    Artikal cigarete, cokolada, sok;

    cigarete.Unos("Cigarete", (float)2.23);
    cokolada.Unos("Cokolada", (float)1.23);
    sok.Unos("Cappy", (float)2.10);

    Racun racun;
    racun.Unos(GenerisiSljedeciBrojRacuna(), danas);
    racun.DodajArtikal(cokolada, 5);
    racun.DodajArtikal(sok, 10);
    racun.DodajArtikal(cigarete, 2);
    racun.DodajArtikal(cigarete, 5);
    racun.Ispis();
    Artikal * p1 = racun.BinarnaPretraga("Cokolada");
    cout << "Binarnom pretragom pronadjen artikal -> ";
    p1->Ispis();
    racun.Ispis();
    Artikal * p2 = PronadjiNajskupljiArtikal(racun, racun._brojArtikala - 1);
    cout << "Rekurzivno pronadjen najskuplji artikal -> ";
    p2->Ispis();
    cout << endl;

    //dealocirati zauzetu memoriju
    danas.Dealociraj();
    cokolada.Dealociraj();
    sok.Dealociraj();
    cigarete.Dealociraj();
    racun.Dealociraj();

    system("pause");
}