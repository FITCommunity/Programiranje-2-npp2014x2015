#include <iostream>
#include <cstring>
#include <memory>

using namespace std;

enum VrstaObaveze { Seminarski, Parcijalni1, Parcijalni2, Integralni, Prakticni };
struct Datum
{
    int * _dan = nullptr, *_mjesec = nullptr, *_godina = nullptr;
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
struct ObavezeNaPredmetu
{
    shared_ptr<VrstaObaveze> _vrstaObaveze;
    Datum _datumIzvrsenja;
    char * _napomena;
    int _ocjena; // 5 - 10 
    void Unos(VrstaObaveze vrsta, Datum datum, int ocjena, char * napomena)
    {
        _vrstaObaveze.reset(new VrstaObaveze(vrsta));
        _datumIzvrsenja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
        _ocjena = ocjena;
        _napomena = new char[strlen(napomena) + 1];
        strcpy_s(_napomena, strlen(napomena) + 1, napomena);
    }
    void Ispis()
    {
        cout << *_vrstaObaveze << " " << _ocjena << " " << _napomena;
        _datumIzvrsenja.Ispis();
        cout << endl;
    }
    void Dealociraj()
    {
        _datumIzvrsenja.Dealociraj();
        delete[] _napomena; _napomena = nullptr;
    }
};

char * _kljucneRijeci[15] = { NULL };

int brojDana(Datum datum)
{
    return *datum._godina * 365 + *datum._mjesec * 30 + *datum._dan;
}

struct PolozeniPredmet
{
    Datum _datumPolaganja;
    char * _nazivPredmeta;
    ObavezeNaPredmetu * _listaIzvrsenihObaveza;
    int _trenutnoIzvrsenihObaveza;
    int _minimalanBrojDana;
    int _konacnaOcjena;

    bool DodajIzvrsenuObavezu(VrstaObaveze obaveza, Datum datum, int ocjena, char * napomena);
    int FormirajKonacnuOcjenu();
    void PostaviDatumPolaganja(VrstaObaveze obaveza);
    bool ProvjeriNapomenu(char * napomena);
    int VratiOcjenu(VrstaObaveze obaveza);
    void Unos(char * naziv, int minimalanBrojDana)
    {
        _nazivPredmeta = new char[strlen(naziv) + 1];
        strcpy_s(_nazivPredmeta, strlen(naziv) + 1, naziv);
        _listaIzvrsenihObaveza = nullptr;
        _trenutnoIzvrsenihObaveza = 0;
        _minimalanBrojDana = minimalanBrojDana;
        _konacnaOcjena = 0;
    }
    void Dealociraj()
    {
        _datumPolaganja.Dealociraj();
        delete[] _nazivPredmeta; _nazivPredmeta = nullptr;
        for (size_t i = 0; i < _trenutnoIzvrsenihObaveza; i++)
            _listaIzvrsenihObaveza[i].Dealociraj();
        delete[]_listaIzvrsenihObaveza;
    }
};

bool PolozeniPredmet::DodajIzvrsenuObavezu(VrstaObaveze obaveza, Datum datum, int ocjena, char * napomena)
{
    if (ocjena < 5 || ocjena > 10)
        return false;

    for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
    {
        if (obaveza != Seminarski)
        {
            bool istaObaveza = obaveza == *_listaIzvrsenihObaveza[i]._vrstaObaveze;
            bool ocjenaJednaka5 = _listaIzvrsenihObaveza[i]._ocjena != 5;
            bool postojiNapomena = ProvjeriNapomenu(_listaIzvrsenihObaveza[i]._napomena);
            if (istaObaveza && ocjenaJednaka5 && postojiNapomena)
                return false;
        }
    }
    
    if (obaveza == Parcijalni2)
    {
        bool prviParc = false;
        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
            if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == Parcijalni1 && _listaIzvrsenihObaveza[i]._ocjena > 5)
                prviParc = true;

        if (!prviParc) return false;
    }


    if (_trenutnoIzvrsenihObaveza > 0)
    {
        if (brojDana(datum) - brojDana(_listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza - 1]._datumIzvrsenja) < _minimalanBrojDana)
            return false;
    }

    ObavezeNaPredmetu * temp = _listaIzvrsenihObaveza;
    _listaIzvrsenihObaveza = new ObavezeNaPredmetu[_trenutnoIzvrsenihObaveza + 1];
    for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
        _listaIzvrsenihObaveza[i] = temp[i];

    _listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza].Unos(obaveza, datum, ocjena, napomena);
    _trenutnoIzvrsenihObaveza++;
    return true;
}

bool PolozeniPredmet::ProvjeriNapomenu(char * napomena)
{
    if (napomena != NULL) return true;

    int i = 0;
    while (_kljucneRijeci[i] != NULL && i < 15)
    {
        if (strstr(_kljucneRijeci[i], napomena) != 0)
            return false;
        i++;
    }
    return true;
}

void PolozeniPredmet::PostaviDatumPolaganja(VrstaObaveze obaveza)
{
    for (int i = _trenutnoIzvrsenihObaveza - 1; i >= 0; i--)
    {
        if (obaveza == Integralni)
        {
            bool postojiObaveza = *_listaIzvrsenihObaveza[i]._vrstaObaveze == obaveza;
            bool postojiNapomena = ProvjeriNapomenu(_listaIzvrsenihObaveza[i]._napomena);
            bool ocjenaNije5 = _listaIzvrsenihObaveza[i]._ocjena != 5;

            if (postojiObaveza && postojiNapomena && ocjenaNije5)
            {
                _datumPolaganja.Unos
                (
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._dan,
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._mjesec,
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._godina
                );
            }
        }
        else
        {
            bool postojiNapomena = ProvjeriNapomenu(_listaIzvrsenihObaveza[i]._napomena);
            bool ocjenaNije5 = _listaIzvrsenihObaveza[i]._ocjena != 5;

            if (postojiNapomena && ocjenaNije5)
            {   
                _datumPolaganja.Unos
                (
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._dan,
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._mjesec,
                    *_listaIzvrsenihObaveza[i]._datumIzvrsenja._godina
                );
            }
        }
    }
}

int PolozeniPredmet::VratiOcjenu(VrstaObaveze obaveza)
{
    if (obaveza != Seminarski)
    {
        for (int i = _trenutnoIzvrsenihObaveza - 1; i >= 0; i--)
        {
            bool postojiObaveza = *_listaIzvrsenihObaveza[i]._vrstaObaveze == obaveza;
            bool postojiNapomena = ProvjeriNapomenu(_listaIzvrsenihObaveza[i]._napomena);
            bool ocjenaNije5 = _listaIzvrsenihObaveza[i]._ocjena != 5;

            if (postojiObaveza && postojiNapomena && ocjenaNije5)
                return _listaIzvrsenihObaveza[i]._ocjena;
        }
    }

    return 5;
}



int PolozeniPredmet::FormirajKonacnuOcjenu()
{
    int suma = 0, brojac = 0, tempOcjena = 0;
    
    tempOcjena = VratiOcjenu(Integralni);
    if (tempOcjena != 5)
    {
        PostaviDatumPolaganja(Integralni);
        _konacnaOcjena = tempOcjena;
        return _konacnaOcjena;
    }
    else
    {
        tempOcjena = VratiOcjenu(Parcijalni1);
        if (tempOcjena != 5)
        {
            suma += tempOcjena;
            brojac++;

            tempOcjena = VratiOcjenu(Parcijalni2);
            if (tempOcjena != 5)
            {
                suma += tempOcjena;
                brojac++;

                tempOcjena = VratiOcjenu(Prakticni);
                if (tempOcjena != 5)
                {
                    suma += tempOcjena;
                    brojac++;

                    for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
                    {
                        bool obavezaSeminarski = *_listaIzvrsenihObaveza[i]._vrstaObaveze == Seminarski;
                        bool postojiNapomena = ProvjeriNapomenu(_listaIzvrsenihObaveza[i]._napomena);
                        bool ocjenaNije5 = _listaIzvrsenihObaveza[i]._ocjena != 5;

                        if (obavezaSeminarski && postojiNapomena && ocjenaNije5)
                        {
                            suma += _listaIzvrsenihObaveza[i]._ocjena;
                            brojac++;
                        }
                    }

                    if (brojac >= 5)
                    {
                        this->PostaviDatumPolaganja(Parcijalni1);
                        _konacnaOcjena = int(round((suma / brojac) + 0.5));
                        return _konacnaOcjena;
                    }
                }
            }
        }
    }

    _konacnaOcjena = 5;
    return _konacnaOcjena;  
}

bool UsloviRekurzivnePretrage(PolozeniPredmet & predmet, ObavezeNaPredmetu & obaveze, VrstaObaveze obaveza, Datum & OD, Datum & DO)
{
    return brojDana(obaveze._datumIzvrsenja) > brojDana(OD) &&
           brojDana(obaveze._datumIzvrsenja) < brojDana(DO) &&
           predmet.ProvjeriNapomenu(obaveze._napomena) &&
           *obaveze._vrstaObaveze == obaveza &&
           obaveze._ocjena != 5;
            
}

float PretragaRekurzivno(PolozeniPredmet & predmet, VrstaObaveze obaveza, Datum & OD, Datum & DO, int i, int brojac)
{
    if (brojac == 0)
    {
        for (int j = 0; j < predmet._trenutnoIzvrsenihObaveza; j++)
            if (UsloviRekurzivnePretrage(predmet, predmet._listaIzvrsenihObaveza[j], obaveza, OD, DO))
                brojac++;

        if (brojac == 0)
            return 0;
        else if (brojac == 1)
        {
            for (int j = 0; j < predmet._trenutnoIzvrsenihObaveza; j++)
                if (UsloviRekurzivnePretrage(predmet, predmet._listaIzvrsenihObaveza[j], obaveza, OD, DO))
                    return predmet._listaIzvrsenihObaveza[j]._ocjena / 1.F;
        }
    }

    if (i == predmet._trenutnoIzvrsenihObaveza - 1)
    {
        if (UsloviRekurzivnePretrage(predmet, predmet._listaIzvrsenihObaveza[i], obaveza, OD, DO))
            return predmet._listaIzvrsenihObaveza[i]._ocjena;
        else
            return 0;
    }
    
    if (UsloviRekurzivnePretrage(predmet, predmet._listaIzvrsenihObaveza[i], obaveza, OD, DO))
        return predmet._listaIzvrsenihObaveza[i]._ocjena / 1.F / brojac +
               PretragaRekurzivno(predmet, obaveza, OD, DO, i + 1, brojac);
    else
        return PretragaRekurzivno(predmet, obaveza, OD, DO, i + 1, brojac);
}

int GetBrojObavezaIznadProsjeka(PolozeniPredmet & predmet, float prosjek, bool(*funkcija)(int, float))
{
    int brojac = 0;
    for (int i = 0; i < predmet._trenutnoIzvrsenihObaveza; i++)
        if (funkcija(predmet._listaIzvrsenihObaveza[i]._ocjena, prosjek))
            brojac++;
    return brojac;
}

void main()
{
    _kljucneRijeci[0] = "prepisivao";
    _kljucneRijeci[1] = "ometao";
    _kljucneRijeci[2] = "nije dostavio";

    Datum datumSeminarski1, datumSeminarski2, datumParcijalni1, datumParcijalni2, datumIntegralni;
    datumSeminarski1.Unos(10, 6, 2017);
    datumSeminarski2.Unos(18, 6, 2017);
    datumParcijalni1.Unos(22, 6, 2017);
    datumParcijalni2.Unos(30, 6, 2017);

    PolozeniPredmet prII;
    prII.Unos("Programiranje II", 7);


    //datumi zamjenjeni da bi zadatak radio (nadopuniti kasnije)
    if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski1, 8, "implementirani svi algoritmi"))
        cout << "Seminarski1...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski2, 6, "rad slican kao kod studenta IB150388"))
        cout << "Seminarski2...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Parcijalni1, datumParcijalni1, 6, "uslovno polozen ispit"))
        cout << "Parcijalni 1...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 5, "previse gresaka, a tokom ispita ometao druge studente"))
        cout << "Parcijalni 2...dodan" << endl;


    cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

    datumParcijalni2.Unos(22, 7, 2017);
    if (prII.DodajIzvrsenuObavezu(Integralni, datumParcijalni2, 6, ""))
        cout << "Integralni...dodan" << endl;
    cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;
    Datum OD, DO; OD.Unos(1, 1, 2017); DO.Unos(1, 7, 2017);

    /*Rekurzivna funkcija treba vratiti prosjecnu ocjenu uspjesno izvrsenih obaveze na
    predmetu odredjene vrste u definisanom periodu. Izbjegavati koristenje globalnih varijabli*/
    float prosjek = PretragaRekurzivno(prII, Seminarski, OD, DO, 0, 0);
    cout << "Prosjecna ocjena izvrsenih obaveza je: " << prosjek << endl;

    /*Koristeci lambda-u, unutar funkcije GetBrojObavezaIznadProsjeka pronaci broj izvrsenih obaveza
    koje imaju istu ili vecu ocjenu od prosjecne*/
    int iznadProsjeka = GetBrojObavezaIznadProsjeka(prII, prosjek, [](int x, float prosjek) {return x >= prosjek; });
    cout << "Broj izvrsenih obaveza iznad prosjeka je: " << iznadProsjeka << endl;

    datumParcijalni1.Dealociraj();
    datumParcijalni2.Dealociraj();
    datumSeminarski1.Dealociraj();
    datumSeminarski2.Dealociraj();
    datumParcijalni2.Dealociraj();
    prII.Dealociraj();

    system("pause>0");
}