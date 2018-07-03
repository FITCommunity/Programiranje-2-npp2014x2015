#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

enum VrstaAktivnosti { Poligon, Gradska, Nocna, Parkiranje };
enum Kategorija { A, B, C, D, E };

const char * IspisAktivnost[] = { "Poligon", "Gradska", "Nocna", "Parkiranje" };
const int maxKategorija = 5;

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
    void Dealociraj() { delete  _dan; delete  _mjesec; delete _godina; }
    int DatumUDane() { return *_dan + *_mjesec * 30 + *_godina * 365; }
};
struct Aktivnost
{
    unique_ptr<VrstaAktivnosti> _vrsta;
    Datum _datumIzvrsenja;
    char * _komentar;
    int _ocjena; // 1 - 5
    void Unos(VrstaAktivnosti vrsta, Datum * datum, int ocjena, const char * komentar)
    {
        _vrsta = make_unique<VrstaAktivnosti>(vrsta);
        _datumIzvrsenja.Unos(*datum->_dan, *datum->_mjesec, *datum->_godina);
        _ocjena = ocjena;
        int size = strlen(komentar) + 1;
        _komentar = new char[size];
        strcpy_s(_komentar, size, komentar);
    }
    void Ispis()
    {
        cout << IspisAktivnost[int(*_vrsta)] << " " << _ocjena << " " << _komentar;
        _datumIzvrsenja.Ispis();
        cout << endl;
    }
    void Dealociraj()
    {
        _datumIzvrsenja.Dealociraj();
        delete[] _komentar; _komentar = nullptr;
    }
};
struct VozackaKategorija
{
    Datum * _datumPolaganja;//datum koji ce se evidentirati kao datum polaganja kategorije tj. kada su ispunjene sve aktivnosti/obaveze
    Kategorija _kategorija;
    Aktivnost * _listaIzvrsenihAktivnosti;
    int _trenutnoIzvrsenihAktivnosti;
    bool DodajIzvrsenuAktivnost(VrstaAktivnosti vrsta, Datum * datumPolaganja, int ocjena, const char * napomena);
    void Sortiraj();
    bool DaLiJePolozena();
    float PretragaRekrzivno(const char * parametar, int i = 0, int brojac = 0, float suma = 0);
    void Unos(Kategorija kategorija)
    {
        _listaIzvrsenihAktivnosti = nullptr;
        _trenutnoIzvrsenihAktivnosti = 0;
        _datumPolaganja = nullptr;
        _kategorija = kategorija;
    }
    void Dealociraj()
    {
        if (_datumPolaganja != nullptr)
            _datumPolaganja->Dealociraj();
        for (size_t i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
            _listaIzvrsenihAktivnosti[i].Dealociraj();
        delete[] _listaIzvrsenihAktivnosti;
        _listaIzvrsenihAktivnosti = nullptr;
    }
    void Ispis()
    {
        if (_datumPolaganja != nullptr)
        {
            _datumPolaganja->Ispis();
            cout << _kategorija << endl;
            for (size_t i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
                _listaIzvrsenihAktivnosti[i].Ispis();
        }
    }
};

struct Kandidat
{
    char * _imePrezime;
    shared_ptr<VozackaKategorija> _kategorije[maxKategorija];
    int _trenutnoKategorija;
    bool DodajKategoriju(VozackaKategorija kategorija);
    pair<Kategorija, float> GetNajboljuKategoriju();
    void Unos(const char * imePrezime)
    {
        int size = strlen(imePrezime) + 1;
        _imePrezime = new char[size];
        strcpy_s(_imePrezime, size, imePrezime);
        _trenutnoKategorija = 0;
    }
    void Dealociraj()
    {
        delete[] _imePrezime; _imePrezime = nullptr;
        for (size_t i = 0; i < _trenutnoKategorija; i++)
        {
            _kategorije[i]->Dealociraj();
            _kategorije[i].reset();
        }
    }
    void Ispis()
    {
        cout << _imePrezime << endl;
        for (size_t i = 0; i < _trenutnoKategorija; i++)
            _kategorije[i]->Ispis();
    }
};

bool VozackaKategorija::DodajIzvrsenuAktivnost(VrstaAktivnosti vrsta, Datum * datumPolaganja, int ocjena, const char * napomena)
{
    if (_datumPolaganja != nullptr) return false;

    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
    {
        bool identicanDatum = datumPolaganja->DatumUDane() == _listaIzvrsenihAktivnosti[i]._datumIzvrsenja.DatumUDane();
        bool identicnaVrsta = *_listaIzvrsenihAktivnosti[i]._vrsta == vrsta;
        bool prolaznaOcjena = _listaIzvrsenihAktivnosti[i]._ocjena > 2;

        if (identicanDatum && identicnaVrsta && prolaznaOcjena)
            return false;
        else if (identicanDatum && identicnaVrsta && !prolaznaOcjena)
            break;

        if (identicnaVrsta)
        {
            if (datumPolaganja->DatumUDane() - _listaIzvrsenihAktivnosti[i]._datumIzvrsenja.DatumUDane() < 30)
                return false;
        }
    }

    Aktivnost * temp = _listaIzvrsenihAktivnosti;
    _listaIzvrsenihAktivnosti = new Aktivnost[_trenutnoIzvrsenihAktivnosti + 1];
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
        _listaIzvrsenihAktivnosti[i].Unos(*temp[i]._vrsta, &temp[i]._datumIzvrsenja, temp[i]._ocjena, temp[i]._komentar);
    _listaIzvrsenihAktivnosti[_trenutnoIzvrsenihAktivnosti].Unos(vrsta, datumPolaganja, ocjena, napomena);
    _trenutnoIzvrsenihAktivnosti++;

    return true;
}

void VozackaKategorija::Sortiraj()
{
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
    {
        bool zamjenjen = false;
        for (int j = i + 1; j < _trenutnoIzvrsenihAktivnosti; j++)
        {
            if (_listaIzvrsenihAktivnosti[i]._ocjena < _listaIzvrsenihAktivnosti[j]._ocjena)
            {
                Aktivnost temp;
                temp.Unos
                (
                    *_listaIzvrsenihAktivnosti[i]._vrsta,
                    &_listaIzvrsenihAktivnosti[i]._datumIzvrsenja,
                    _listaIzvrsenihAktivnosti[i]._ocjena,
                    _listaIzvrsenihAktivnosti[i]._komentar
                );

                _listaIzvrsenihAktivnosti[i].Unos
                (
                    *_listaIzvrsenihAktivnosti[j]._vrsta,
                    &_listaIzvrsenihAktivnosti[j]._datumIzvrsenja,
                    _listaIzvrsenihAktivnosti[j]._ocjena,
                    _listaIzvrsenihAktivnosti[j]._komentar
                );

                _listaIzvrsenihAktivnosti[j].Unos
                (
                    *temp._vrsta,
                    &temp._datumIzvrsenja,
                    temp._ocjena,
                    temp._komentar
                );

                temp.Dealociraj();
                zamjenjen = true;
            }

            if (!zamjenjen) return;
        }
    }
}

bool VozackaKategorija::DaLiJePolozena()
{
    int brojacPozitivnih[4] = { 0 }, brojacNegativnih[4] = { 0 };
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
    {
        if (_listaIzvrsenihAktivnosti[i]._ocjena < 3)
            brojacNegativnih[int(*_listaIzvrsenihAktivnosti[i]._vrsta)]++;
        if (_listaIzvrsenihAktivnosti[i]._ocjena > 2)
            brojacPozitivnih[int(*_listaIzvrsenihAktivnosti[i]._vrsta)]++;

        if (brojacNegativnih[0] == 3 || brojacNegativnih[1] == 3 || brojacNegativnih[2] == 3 || brojacNegativnih[3] == 3)
            return false;
    }

    if (brojacPozitivnih[0] + brojacPozitivnih[1] + brojacPozitivnih[2] + brojacPozitivnih[3] == 4 * (int(_kategorija) + 1))
    {
        for (int i = _trenutnoIzvrsenihAktivnosti; i >= 0; i--)
        {
            if (_listaIzvrsenihAktivnosti[i]._ocjena > 2)
            {
                _datumPolaganja = new Datum;
                _datumPolaganja->Unos
                (
                    *_listaIzvrsenihAktivnosti[i]._datumIzvrsenja._dan,
                    *_listaIzvrsenihAktivnosti[i]._datumIzvrsenja._mjesec,
                    *_listaIzvrsenihAktivnosti[i]._datumIzvrsenja._godina
                );

                return true;
            }
        }
    }

    return false;
}

float VozackaKategorija::PretragaRekrzivno(const char * parametar, int i, int brojac, float suma)
{
    if (i == _trenutnoIzvrsenihAktivnosti)
    {
        if (brojac == 0)
            return 0;
        else
            return suma / brojac;
    }

    if (_listaIzvrsenihAktivnosti[i]._ocjena > 2 && strstr(_listaIzvrsenihAktivnosti[i]._komentar, parametar) != nullptr)
        return PretragaRekrzivno(parametar, i + 1, brojac + 1, suma + _listaIzvrsenihAktivnosti[i]._ocjena);
    else
        return PretragaRekrzivno(parametar, i + 1, brojac, suma);

}

bool Kandidat::DodajKategoriju(VozackaKategorija kategorija)
{
    auto provjeriKategoriju = [=]()
    {
        for (int i = 0; i < maxKategorija; i++)
            if (_kategorije[i] != nullptr)
                if (int(_kategorije[i]->_kategorija) + 1 != int(kategorija._kategorija))
                    return false;
        return true;
    };

    if (provjeriKategoriju())
    {
        int indeks = int(kategorija._kategorija);
        _kategorije[indeks] = make_shared<VozackaKategorija>();
        _kategorije[indeks]->_kategorija = kategorija._kategorija;
        _kategorije[indeks]->_listaIzvrsenihAktivnosti = new Aktivnost[kategorija._trenutnoIzvrsenihAktivnosti];
        for (int i = 0; i < kategorija._trenutnoIzvrsenihAktivnosti; i++)
            _kategorije[indeks]->_listaIzvrsenihAktivnosti[i].Unos
            (
                *kategorija._listaIzvrsenihAktivnosti[i]._vrsta,
                &kategorija._listaIzvrsenihAktivnosti[i]._datumIzvrsenja,
                kategorija._listaIzvrsenihAktivnosti[i]._ocjena,
                kategorija._listaIzvrsenihAktivnosti[i]._komentar
            );
        _kategorije[indeks]->_trenutnoIzvrsenihAktivnosti = kategorija._trenutnoIzvrsenihAktivnosti;
        _kategorije[indeks]->DaLiJePolozena();
        _trenutnoKategorija++;

        return true;
    }

    return false;
}

pair<Kategorija, float> Kandidat::GetNajboljuKategoriju()
{
    float maxProsjek = 0;
    Kategorija kategorijaMaxProsjeka;
    for (int i = 0; i < _trenutnoKategorija; i++)
    {
        float prosjek = 0;
        int brojac = 0;
        for (int j = 0; j < _kategorije[i]->_trenutnoIzvrsenihAktivnosti; j++)
        {
            if (_kategorije[i]->_listaIzvrsenihAktivnosti[j]._ocjena > 2)
            {
                prosjek += _kategorije[i]->_listaIzvrsenihAktivnosti[j]._ocjena;
                brojac++;
            }
        }

        if (prosjek / brojac > maxProsjek)
        {
            maxProsjek = prosjek / brojac;
            kategorijaMaxProsjeka = _kategorije[i]->_kategorija;
        }
    }

    return make_pair(kategorijaMaxProsjeka, maxProsjek);
}


void main()
{
    Datum datumPolaganja1, datumPolaganja2, datumPolaganja3, datumPolaganja4;
    datumPolaganja1.Unos(10, 6, 2018);
    datumPolaganja2.Unos(18, 6, 2018);
    datumPolaganja3.Unos(22, 3, 2018);
    datumPolaganja4.Unos(28, 7, 2018);

    VozackaKategorija kategorijaA, kategorijaB, kategorijaC;
    kategorijaA.Unos(A);
    kategorijaB.Unos(B);
    kategorijaC.Unos(C);

    if (kategorijaA.DodajIzvrsenuAktivnost(Poligon, &datumPolaganja1, 3, "izuzetno dobra orijentacija"))
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (kategorijaA.DodajIzvrsenuAktivnost(Gradska, &datumPolaganja1, 2, "nesigurnost u raskrsnicama"))
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (kategorijaA.DodajIzvrsenuAktivnost(Gradska, &datumPolaganja2, 4, "raskrsnice savladane"))//datum?
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (kategorijaA.DodajIzvrsenuAktivnost(Gradska, &datumPolaganja4, 4, "raskrsnice savladane"))
        cout << "Aktivnost uspjesno dodana!" << endl;


    kategorijaA.Sortiraj();


    if (kategorijaA.DaLiJePolozena())
        kategorijaA.Ispis();

    cout << "Prosjecna ocjena za kategoriju A -> " << kategorijaA.PretragaRekrzivno("nesigurno") << endl;

    kategorijaA.Ispis();

    Kandidat jasmin;
    jasmin.Unos("Jasmin Azemovic");


    if (jasmin.DodajKategoriju(kategorijaA))
        cout << "Kategorija uspjesno dodan!" << endl;
    if (jasmin.DodajKategoriju(kategorijaC))
        cout << "Kategorija uspjesno dodan!" << endl;
    if (jasmin.DodajKategoriju(kategorijaB))
        cout << "Kategorija uspjesno dodan!" << endl;

    float prosjek = 0;
    Kategorija kategorija;
    tie(kategorija, prosjek) = jasmin.GetNajboljuKategoriju();
    cout << "Najbolji rezultat od " << prosjek << " je ostvaren tokom polaganja kategorije " << char(kategorija + 'A') << endl;


    datumPolaganja1.Dealociraj(), datumPolaganja2.Dealociraj(), datumPolaganja3.Dealociraj(), datumPolaganja4.Dealociraj();
    kategorijaA.Dealociraj(), kategorijaB.Dealociraj(), kategorijaC.Dealociraj();
    jasmin.Dealociraj();

    system("pause");
}