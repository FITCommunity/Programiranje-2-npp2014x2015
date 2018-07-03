#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

enum VrstaAktivnosti { Tehnike, Kata, Borba };
enum Pojas { Zuti, Narandzasti, Zeleni, Plavi, Smedji, Crni };
const char * vrstaAktivnosti[] = { "Tehnike", "Kata", "Borba" };
const char * pojasIspis[] = { "Zuti", "Narandzasti", "Zeleni", "Plavi", "Smedji", "Crni" };

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
    void Dealociraj() { delete _dan; delete _mjesec; delete _godina;}
    int DatumUDane() { return *_godina * 365 + *_mjesec * 30 + *_dan;}
};

struct Aktivnost
{
    unique_ptr<VrstaAktivnosti> _vrsta;
    Datum _datumIzvrsenja;
    char * _nazivOpis;
    int _ocjena; // 1 - 10 
    void Unos(VrstaAktivnosti vrsta, Datum * datum, int ocjena, const char * nazivOpis)
    {
        _vrsta = make_unique<VrstaAktivnosti>(vrsta);
        _datumIzvrsenja.Unos(*datum->_dan, *datum->_mjesec, *datum->_godina);
        _ocjena = ocjena;
        int size = strlen(nazivOpis) + 1;
        _nazivOpis = new char[size];
        strcpy_s(_nazivOpis, size, nazivOpis);
    }
    void Ispis()
    {
        cout << vrstaAktivnosti[int(*_vrsta)] << " " << _ocjena << " " << _nazivOpis << " ";
        _datumIzvrsenja.Ispis();
        cout << endl;
    }
    void Dealociraj()
    {
        _datumIzvrsenja.Dealociraj();
        delete[] _nazivOpis; _nazivOpis = nullptr;
    }
};
struct KaratePojas
{
    Datum * _datumPolaganja;//datum koji ce se evidentirati kao datum polaganja pojasa tj. kada su ispunjene sve aktivnosti/obaveze
    Pojas _pojas;
    Aktivnost * _listaIzvrsenihAktivnosti;
    int _trenutnoIzvrsenihAktivnosti;
    bool DodajIzvrsenuAktivnost(Aktivnost * aktivnost);
    void Sortiraj();
    bool DaLiJePolozen();
    float PretragaRekrzivno(char * parametar, int i = 0, int brojac = 0, int suma = 0);
    void Unos(Pojas pojas)
    {
        _listaIzvrsenihAktivnosti = nullptr;
        _trenutnoIzvrsenihAktivnosti = 0;
        _datumPolaganja = nullptr;
        _pojas = pojas;
    }
    void Dealociraj()
    {
        if (_datumPolaganja != nullptr)
        {
            _datumPolaganja->Dealociraj(); _datumPolaganja = nullptr;
            for (size_t i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
                _listaIzvrsenihAktivnosti[i].Dealociraj();
            delete[] _listaIzvrsenihAktivnosti;
            _listaIzvrsenihAktivnosti = nullptr;
        }
    }
    void Ispis()
    {
        if(_datumPolaganja != nullptr) _datumPolaganja->Ispis();
        cout << pojasIspis[int(_pojas)] << endl;
        for (size_t i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
            _listaIzvrsenihAktivnosti[i].Ispis();
    }
};

struct Kandidat
{
    char * _imePrezime;
    shared_ptr<KaratePojas> _pojasevi[6];
    bool DodajPojas(KaratePojas karatePojas);
    pair<Pojas, float> GetNajbolji();
    void Unos(const char * imePrezime)
    {
        int size = strlen(imePrezime) + 1;
        _imePrezime = new char[size];
        strcpy_s(_imePrezime, size, imePrezime);
        for (size_t i = 0; i < 6; i++)
            _pojasevi[i] = nullptr;
    }
    void Dealociraj()
    {
        delete[] _imePrezime; _imePrezime = nullptr;
        for (size_t i = 0; i < 6; i++)
            if (_pojasevi[i] != nullptr)
            {
                _pojasevi[i]->Dealociraj();
                _pojasevi[i].reset();
            }
    }
    void Ispis()
    {
        cout << _imePrezime << endl;
        for (size_t i = 0; i < 6; i++)
            if (_pojasevi[i] != nullptr)
                _pojasevi[i]->Ispis();
    }
};

bool KaratePojas::DodajIzvrsenuAktivnost(Aktivnost * aktivnost)
{
    if (_datumPolaganja != nullptr) return false;

    for (int i = _trenutnoIzvrsenihAktivnosti - 1; i >= 0; i--)
    {
        bool identicanDatum = aktivnost->_datumIzvrsenja.DatumUDane() == _listaIzvrsenihAktivnosti[i]._datumIzvrsenja.DatumUDane();
        bool identicnaVrsta = *_listaIzvrsenihAktivnosti[i]._vrsta == *aktivnost->_vrsta;
        bool prolaznaOcjena = _listaIzvrsenihAktivnosti[i]._ocjena > 5;

        if (identicanDatum && identicnaVrsta && prolaznaOcjena)
            return false;
        else if (identicanDatum && identicnaVrsta && !prolaznaOcjena)
            break;

        if (identicnaVrsta)
        {
            if (aktivnost->_datumIzvrsenja.DatumUDane() - _listaIzvrsenihAktivnosti[i]._datumIzvrsenja.DatumUDane() >= 15)
                break;
            else
                return false;
        }
    }


    Aktivnost * temp = _listaIzvrsenihAktivnosti;
    _listaIzvrsenihAktivnosti = new Aktivnost[_trenutnoIzvrsenihAktivnosti + 1];
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
        _listaIzvrsenihAktivnosti[i].Unos(*temp[i]._vrsta, &temp[i]._datumIzvrsenja, temp[i]._ocjena, temp[i]._nazivOpis);
    _listaIzvrsenihAktivnosti[_trenutnoIzvrsenihAktivnosti].Unos
    (
        *aktivnost->_vrsta,
        &aktivnost->_datumIzvrsenja,
        aktivnost->_ocjena,
        aktivnost->_nazivOpis
    );
    _trenutnoIzvrsenihAktivnosti++;

    return true;


}

void KaratePojas::Sortiraj()
{
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti - 1; i++)
    {
        bool zamjenjen = false;
        for (int j = i + 1; j < _trenutnoIzvrsenihAktivnosti; j++)
        {
            if (_listaIzvrsenihAktivnosti[i]._ocjena > _listaIzvrsenihAktivnosti[j]._ocjena)
            {
                Aktivnost temp;
                temp.Unos
                (
                    *_listaIzvrsenihAktivnosti[i]._vrsta,
                    &_listaIzvrsenihAktivnosti[i]._datumIzvrsenja,
                    _listaIzvrsenihAktivnosti[i]._ocjena,
                    _listaIzvrsenihAktivnosti[i]._nazivOpis
                );


                _listaIzvrsenihAktivnosti[i].Unos
                (
                    *_listaIzvrsenihAktivnosti[j]._vrsta,
                    &_listaIzvrsenihAktivnosti[j]._datumIzvrsenja,
                    _listaIzvrsenihAktivnosti[j]._ocjena,
                    _listaIzvrsenihAktivnosti[j]._nazivOpis
                );


                _listaIzvrsenihAktivnosti[j].Unos
                (
                    *temp._vrsta,
                    &temp._datumIzvrsenja,
                    temp._ocjena,
                    temp._nazivOpis
                );

                temp.Dealociraj();
                zamjenjen = true;
            }
        }

        if (!zamjenjen) return;
    }
}
bool KaratePojas::DaLiJePolozen()
{
    int brojacPozitivnih[3] = { 0 }, brojacNegativnih[3] = { 0 };
    for (int i = 0; i < _trenutnoIzvrsenihAktivnosti; i++)
    {
        if (_listaIzvrsenihAktivnosti[i]._ocjena < 6)
            brojacNegativnih[int(*_listaIzvrsenihAktivnosti[i]._vrsta)]++;
        if (_listaIzvrsenihAktivnosti[i]._ocjena > 5)
            brojacPozitivnih[int(*_listaIzvrsenihAktivnosti[i]._vrsta)]++;


        if (brojacNegativnih[0] == 3 || brojacNegativnih[1] == 3 || brojacNegativnih[2] == 3)
            return false;
    }

    if (brojacPozitivnih[0] + brojacPozitivnih[1] + brojacPozitivnih[2] == (int(_pojas) + 1) * 3)
    {
        for (int i = _trenutnoIzvrsenihAktivnosti - 1; i >= 0; i--)
        {
            if (_listaIzvrsenihAktivnosti[i]._ocjena > 5)
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
    else return false;

}


bool Kandidat::DodajPojas(KaratePojas karatePojas)
{
    auto ProvjeriPojas = [=]()
    {
        for (int i = 0; i < 6; i++)
            if (_pojasevi[i] != nullptr)
                if (int(_pojasevi[i]->_pojas) + 1 != int(karatePojas._pojas))
                    return false;
        return true;
    };

    if (ProvjeriPojas())
    {
       
        _pojasevi[int(karatePojas._pojas)] = make_shared<KaratePojas>();
        _pojasevi[int(karatePojas._pojas)]->_pojas = karatePojas._pojas;
        _pojasevi[int(karatePojas._pojas)]->_listaIzvrsenihAktivnosti = new Aktivnost[karatePojas._trenutnoIzvrsenihAktivnosti];
        for (int i = 0; i < karatePojas._trenutnoIzvrsenihAktivnosti; i++)
            _pojasevi[int(karatePojas._pojas)]->_listaIzvrsenihAktivnosti[i].Unos
            (
                *karatePojas._listaIzvrsenihAktivnosti[i]._vrsta,
                &karatePojas._listaIzvrsenihAktivnosti[i]._datumIzvrsenja,
                karatePojas._listaIzvrsenihAktivnosti[i]._ocjena,
                karatePojas._listaIzvrsenihAktivnosti[i]._nazivOpis
            );
        _pojasevi[int(karatePojas._pojas)]->_trenutnoIzvrsenihAktivnosti = karatePojas._trenutnoIzvrsenihAktivnosti;
        _pojasevi[int(karatePojas._pojas)]->DaLiJePolozen();

        return true;
    }

    return false;
}

float KaratePojas::PretragaRekrzivno(char * parametar, int i, int brojac, int suma)
{
    if (i == _trenutnoIzvrsenihAktivnosti)
    {
        if (brojac == 0)
            return 0;
        else
            return suma / brojac / 1.F;
    }

    if (_listaIzvrsenihAktivnosti[i]._ocjena > 5 && strstr(_listaIzvrsenihAktivnosti[i]._nazivOpis, parametar) != nullptr)
        return PretragaRekrzivno(parametar, i + 1, brojac + 1, suma + _listaIzvrsenihAktivnosti[i]._ocjena);
    else
        return PretragaRekrzivno(parametar, i + 1, brojac, suma);
}

pair<Pojas, float> Kandidat::GetNajbolji()
{
    float maxProsjek = 0;
    Pojas pojasMaxProsjeka;
    for (int i = 0; i < 6; i++)
    {
        if (_pojasevi[i] != nullptr)
        {
            float prosjek = 0;
            int brojac = 0;
            for (int j = 0; j < _pojasevi[i]->_trenutnoIzvrsenihAktivnosti; j++)
            {
                if (_pojasevi[i]->_listaIzvrsenihAktivnosti[j]._ocjena > 5)
                {
                    prosjek += _pojasevi[i]->_listaIzvrsenihAktivnosti[j]._ocjena;
                    brojac++;
                }
            }

            if (prosjek / brojac > maxProsjek)
            {
                maxProsjek = prosjek / brojac;
                pojasMaxProsjeka = _pojasevi[i]->_pojas;
            }
        }
    }

    return { pojasMaxProsjeka, maxProsjek };
}

void main()
{
    Datum datumPolaganja1, datumPolaganja2, datumPolaganja3, datumPolaganja4;
    datumPolaganja1.Unos(10, 6, 2018);
    datumPolaganja2.Unos(18, 6, 2018);
    datumPolaganja3.Unos(22, 3, 2018);
    datumPolaganja4.Unos(22, 7, 2018);

    Aktivnost aktivnost1, aktivnost2, aktivnost3, aktivnost4, aktivnost5;
    aktivnost1.Unos(Tehnike, &datumPolaganja1, 6, "Tehnike za zuti pojas");
    aktivnost2.Unos(Kata, &datumPolaganja1, 8, "Taiki joko shodan - zuti pojas");
    aktivnost3.Unos(Borba, &datumPolaganja1, 2, "Jednostavne borbene tehnike sa partnerom");
    aktivnost4.Unos(Borba, &datumPolaganja1, 6, "Jednostavne borbene tehnike sa partnerom");
    aktivnost5.Unos(Borba, &datumPolaganja4, 6, "Jednostavne borbene tehnike sa partnerom");

    KaratePojas pojasZuti, pojasNarandzasti, pojasZeleni;
    pojasZuti.Unos(Zuti);
    pojasNarandzasti.Unos(Narandzasti);
    pojasZeleni.Unos(Zeleni);

    if (pojasZuti.DodajIzvrsenuAktivnost(&aktivnost1))
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (pojasZuti.DodajIzvrsenuAktivnost(&aktivnost2))
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (pojasZuti.DodajIzvrsenuAktivnost(&aktivnost3))
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (pojasZuti.DodajIzvrsenuAktivnost(&aktivnost4))//15 dana...
        cout << "Aktivnost uspjesno dodana!" << endl;
    if (pojasZuti.DodajIzvrsenuAktivnost(&aktivnost5))
        cout << "Aktivnost uspjesno dodana!" << endl;

    pojasZuti.Sortiraj();
    

    if (pojasZuti.DaLiJePolozen())
        pojasZuti.Ispis();

    cout << "Prosjecna ocjena za zuti pojas -> " << pojasZuti.PretragaRekrzivno("pojas") << endl;
    
    pojasZuti.Ispis();
    
    Kandidat jasmin;
    jasmin.Unos("Jasmin Azemovic");
    
    if (jasmin.DodajPojas(pojasZuti))
        cout << "Pojas uspjesno dodan!" << endl;
    if (jasmin.DodajPojas(pojasZeleni))//prethodno je trebao biti dodan narandzasti pojas
        cout << "Pojas uspjesno dodan!" << endl;
    if (jasmin.DodajPojas(pojasNarandzasti))
        cout << "Pojas uspjesno dodan!" << endl;

    float prosjek = 0;
    Pojas pojas;
    tie(pojas, prosjek) = jasmin.GetNajbolji();
    cout << "Najbolji rezultat od " << prosjek << " je ostvaren tokom stjecanja pojasa " << pojasIspis[int(pojas)] << endl;

    
    datumPolaganja1.Dealociraj(), datumPolaganja2.Dealociraj(), datumPolaganja3.Dealociraj(), datumPolaganja4.Dealociraj();
    aktivnost1.Dealociraj(), aktivnost2.Dealociraj(), aktivnost3.Dealociraj(), aktivnost4.Dealociraj(), aktivnost5.Dealociraj();
    pojasZuti.Dealociraj(), pojasNarandzasti.Dealociraj(), pojasZeleni.Dealociraj();
    jasmin.Dealociraj();

    system("pause");
}