#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

char *crt = "\n-------------------------------------------\n";

enum eNacinStudiranja { REDOVAN, DL };
enum eRazred { PRVI = 1, DRUGI, TRECI, CETVRTI };

const char *nacinStudiranja[]{ "REDOVAN", "DL" };

struct DatumVrijeme
{
    int *_dan, *_mjesec, *_godina, *_sati, *_minuti;
    void Unos(int dan = 1, int mjesec = 1, int godina = 2000, int sati = 0, int minuti = 0)
    {
        _dan = new int(dan);
        _mjesec = new int(mjesec);
        _godina = new int(godina);
        _sati = new int(sati);
        _minuti = new int(minuti);
    }
    void Dealociraj()
    {
        delete _dan; _dan = nullptr;
        delete _mjesec; _mjesec = nullptr;
        delete _godina; _godina = nullptr;
        delete _sati; _sati = nullptr;
        delete _minuti; _minuti = nullptr;
    }
    void Ispis()
    {
        cout << *_dan << "." << *_mjesec << "." << *_godina << " " << *_sati << ":" << *_minuti << endl;
    }
    char *GetDatumKaoNizKaraktera()
    {
        int velicina = strlen("00.00.0000 00:00") + 1;
        char *temp = new char[velicina];
        char dan[3], mjesec[3], godina[5], sati[3], minuti[3];
        _itoa_s(*_dan, dan, 10);
        _itoa_s(*_mjesec, mjesec, 10);
        _itoa_s(*_godina, godina, 10);
        _itoa_s(*_sati, sati, 10);
        _itoa_s(*_minuti, minuti, 10);

        strcpy_s(temp, velicina, dan);
        strcat_s(temp, velicina, ".");
        strcat_s(temp, velicina, mjesec);
        strcat_s(temp, velicina, ".");
        strcat_s(temp, velicina, godina);
        strcat_s(temp, velicina, " ");
        strcat_s(temp, velicina, sati);
        strcat_s(temp, velicina, ":");
        strcat_s(temp, velicina, minuti);

        return temp;
    }
};

int datecmp(const DatumVrijeme &d1, const DatumVrijeme &d2)
{
    if (*d1._godina != *d2._godina) return *d1._godina - *d2._godina;
    else if (*d1._mjesec != *d2._mjesec) return *d1._mjesec - *d2._mjesec;
    else if (*d1._dan != *d2._dan) return *d1._dan - *d2._dan;
    else if (*d1._sati != *d2._sati) return *d1._sati - *d2._sati;
    else if (*d1._minuti != *d2._minuti) return *d1._minuti - *d2._minuti;
    return 0;
}

const DatumVrijeme rokZaPrijavu = { new int(5), new int(7), new int(2017), new int(12), new int(30) };

struct Predmet
{
    char * _naziv;
    int _ocjena;
    DatumVrijeme * _datumUnosa;
    void Unos(char * naziv, int ocjena, DatumVrijeme datumUnosa)
    {
        int vel = strlen(naziv) + 1;
        _naziv = new char[vel];
        strcpy_s(_naziv, vel, naziv);
        _ocjena = ocjena;
        _datumUnosa = new DatumVrijeme;
        _datumUnosa->Unos(*datumUnosa._dan, *datumUnosa._mjesec, *datumUnosa._godina, *datumUnosa._sati, *datumUnosa._minuti);
    }

    void Dealociraj()
    {
        delete[] _naziv; _naziv = nullptr;
        _datumUnosa->Dealociraj(); delete _datumUnosa;
    }

    void Ispis()
    {
        //kreirati funkciju GetDatumKaoNizKaraktera() koja vraca vrijednosti atributa strukture datum kao niz karaktera
        char *datumUNiz = _datumUnosa->GetDatumKaoNizKaraktera();
        cout << _naziv << " (" << _ocjena << ") " << datumUNiz << endl;
        delete[] datumUNiz;
    }
};

struct Uspjeh
{
    eRazred _razred;
    Predmet * _predmeti;
    int _brojPredmeta;
    void Unos(eRazred razred)
    {
        _razred = razred;
        _predmeti = nullptr;
        _brojPredmeta = 0;
    }
    void Dealociraj()
    {
        for (size_t i = 0; i < _brojPredmeta; i++)
            _predmeti[i].Dealociraj();
        delete[] _predmeti; _predmeti = nullptr;
    }

    void Ispis()
    {
        cout << crt << "Razred -> " << _razred << crt;
        for (size_t i = 0; i < _brojPredmeta; i++)
            _predmeti[i].Ispis();
    }
};

struct Kandidat
{
    eNacinStudiranja _nacinStudiranja;
    char * _imePrezime;
    shared_ptr<Uspjeh> _uspjeh[4];

    void Unos(eNacinStudiranja nacinStudiranja, char * imePrezime)
    {
        int vel = strlen(imePrezime) + 1;
        _imePrezime = new char[vel];
        strcpy_s(_imePrezime, vel, imePrezime);
        _nacinStudiranja = nacinStudiranja;
        for (size_t i = 0; i < 4; i++)
            _uspjeh[i] = nullptr;
    }
    void Dealociraj()
    {
        delete[] _imePrezime; _imePrezime = nullptr;
        for (size_t i = 0; i < 4; i++)
            if(_uspjeh[i])
                _uspjeh[i]->Dealociraj();
    }
    void Ispis()
    {
        cout << crt << _imePrezime << " " << nacinStudiranja[_nacinStudiranja];
        for (size_t i = 0; i < 4; i++)
            if(_uspjeh[i])
                _uspjeh[i]->Ispis();
    }
    bool DodajPredmet(eRazred razred, Predmet predmet);
};

bool Kandidat::DodajPredmet(eRazred razred, Predmet predmet)
{
    if(_uspjeh[razred - 1])
        for (int i = 0; i < _uspjeh[razred - 1]->_brojPredmeta; i++)
            if (strcmp(_uspjeh[razred - 1]->_predmeti[i]._naziv, predmet._naziv) == 0)
                return false;

    if (datecmp(*predmet._datumUnosa, rokZaPrijavu) > 0) return false;

    if (!_uspjeh[razred - 1])
    {
        _uspjeh[razred - 1] = make_shared<Uspjeh>();
        _uspjeh[razred - 1]->_razred = razred;
    }

    Predmet *temp = _uspjeh[razred - 1]->_predmeti;
    _uspjeh[razred - 1]->_predmeti = new Predmet[_uspjeh[razred - 1]->_brojPredmeta + 1];
    for (int i = 0; i < _uspjeh[razred - 1]->_brojPredmeta; i++)
        _uspjeh[razred - 1]->_predmeti[i] = temp[i];
    _uspjeh[razred - 1]->_predmeti[_uspjeh[razred - 1]->_brojPredmeta].Unos(predmet._naziv, predmet._ocjena, *predmet._datumUnosa);
    _uspjeh[razred - 1]->_brojPredmeta++;

    delete[] temp;
    return true;
}

int getMaxUGodini(Uspjeh *uspjeh, const char *naziv, int i = 0, int pozicija = 0)
{
    if (i == uspjeh->_brojPredmeta) return pozicija;

    if (strcmp(uspjeh->_predmeti[i]._naziv, naziv) == 0)
    {
        if (strcmp(uspjeh->_predmeti[pozicija]._naziv, naziv) == 0)
        {
            if (uspjeh->_predmeti[i]._ocjena > uspjeh->_predmeti[pozicija]._ocjena) pozicija = i;
        }
        else pozicija = i;
    }

    return getMaxUGodini(uspjeh, naziv, i + 1, pozicija);
}

pair<int, int> getGodinu(Kandidat kandidat, const char *naziv, int godina = 0, int brPredmeta = 0, int i = 0, int j = 0)
{
    if (i == 4) return { godina, brPredmeta };

    if (kandidat._uspjeh[i])
    {
        brPredmeta = getMaxUGodini(kandidat._uspjeh[i].get(), naziv);
        
        if (strcmp(kandidat._uspjeh[i]->_predmeti[j]._naziv, naziv) == 0)
        {
            int ocjena1 = kandidat._uspjeh[i]->_predmeti[j]._ocjena;
            int ocjena2 = kandidat._uspjeh[godina]->_predmeti[brPredmeta]._ocjena;
            bool flag = datecmp
            (
                *kandidat._uspjeh[i]->_predmeti[j]._datumUnosa,
                *kandidat._uspjeh[godina]->_predmeti[brPredmeta]._datumUnosa
            ) > 0;

            if (ocjena1 > ocjena2 || (ocjena1 == ocjena2 && flag)) godina = i, brPredmeta = j;
        }
    }

    return getGodinu(kandidat, naziv, godina, brPredmeta, i + 1);
}

Kandidat *rekNajboljaOcjena(Kandidat *kandidati, int broj, char *naziv, int i = 0, int kandidat = 0, int x = 0, int y = 0)
{
    if (i == broj) return (&kandidati[kandidat] ? &kandidati[kandidat] : nullptr);

    int j, k;
    tie(j, k) = getGodinu(kandidati[i], naziv);
    
    if (strcmp(kandidati[i]._uspjeh[j]->_predmeti[k]._naziv, naziv) == 0)
    {
        if (strcmp(kandidati[kandidat]._uspjeh[x]->_predmeti[y]._naziv, naziv) == 0)
        {
            int ocjena1 = kandidati[i]._uspjeh[j]->_predmeti[k]._ocjena;
            int ocjena2 = kandidati[kandidat]._uspjeh[x]->_predmeti[y]._ocjena;
            bool flag = datecmp
            (
                *kandidati[i]._uspjeh[j]->_predmeti[k]._datumUnosa,
                *kandidati[kandidat]._uspjeh[x]->_predmeti[y]._datumUnosa
            ) > 0;

            if (ocjena1 > ocjena2 || (ocjena1 == ocjena2 && flag)) kandidat = i, x = j, y = k;
        }
        else kandidat = i, x = j, y = k;
    }
    

    return rekNajboljaOcjena(kandidati, broj, naziv, i + 1, kandidat, x, y);
}

void main()
{
    DatumVrijeme datum19062017_1015, datum20062017_1115, datum30062017_1215, datum05072017_1231;
    datum19062017_1015.Unos(19, 6, 2017, 10, 15);
    datum20062017_1115.Unos(20, 6, 2017, 11, 15);
    datum30062017_1215.Unos(30, 6, 2017, 12, 15);
    datum05072017_1231.Unos(5, 7, 2017, 12, 31);

    char *datumUNiz = datum19062017_1015.GetDatumKaoNizKaraktera();
    cout << datumUNiz << endl;//9.6.2017 10:15
    delete[] datumUNiz;

    Predmet Matematika, Fizika, Hemija, Engleski;
    //2 - ocjena na predmetu; datum - datum evidentiranja uspjeha na predmetu jer postoji krajnji rok za evidentiranje
    Matematika.Unos("Matematika", 2, datum19062017_1015);
    Fizika.Unos("Fizika", 5, datum20062017_1115);
    Hemija.Unos("Hemija", 2, datum20062017_1115);
    Engleski.Unos("Engleski", 5, datum05072017_1231);

    int brojKandidata = 2;

    Kandidat * prijave2017 = new Kandidat[brojKandidata];
    prijave2017[0].Unos(DL, "Jasmin Azemovic");
    prijave2017[1].Unos(REDOVAN, "Indira Hamulic");

    /*
    uspjeh (tokom srednjoskolskog obrazovanja) se dodaje za svaki predmet na nivou razreda.
    prilikom dodavanja onemoguciti:
    - dodavanje predmeta za razrede koji nisu definisani enumeracijom,
    - dodavanje istoimenih predmeta na nivou jednog razreda,
    - dodavanje predmeta nakon dozvoljenog roka za prijavu (rokZaPrijavu).
    razredi (predmeti ili uspjeh) ne moraju biti dodavani sortiranim redoslijedom
    (npr. prvo se moze dodati uspjeh za II razred, pa onda za I razred i sl.).
    Funkcija vraca true ili false u zavisnosti od (ne)uspjesnost izvrsenja
    */
    if (prijave2017[0].DodajPredmet(DRUGI, Engleski))//ne bi trebao dodati jer je prosao postavljeni rok za dodavanje predmeta
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[0].DodajPredmet(DRUGI, Matematika))
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[0].DodajPredmet(PRVI, Fizika))
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[0].DodajPredmet(PRVI, Hemija))
        cout << "Predmet uspjesno dodan!" << crt;

    Matematika._ocjena = 5;
    Hemija._ocjena = 3;

    if (prijave2017[1].DodajPredmet(PRVI, Matematika))
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[1].DodajPredmet(PRVI, Matematika))//ne bi trebalo ponovo dodati Matematiku!
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[1].DodajPredmet(TRECI, Hemija))
        cout << "Predmet uspjesno dodan!" << crt;
    if (prijave2017[1].DodajPredmet(DRUGI, Engleski))
        cout << "Predmet uspjesno dodan!" << crt;

    /*
    koristeci Lambda izraz kreirati funkciju koja ce vratiti uspjeh kandidata koji je
    ostvario najveci prosjek (na nivou razreda, a ne ukupni prosjek). ukoliko vise kandidata
    ima isti prosjek funkcija vraca uspjeh (najboljeg razreda) prvog pronadjenog kandidata
    */
    auto najboljiUspjeh = [prijave2017, brojKandidata]()
    {
        float max = 0;
        int kandidat = 0, razred = 0;
        for (int i = 0; i < brojKandidata; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (prijave2017[i]._uspjeh[j])
                {
                    float suma = 0;
                    for (int k = 0; k < prijave2017[i]._uspjeh[j]->_brojPredmeta; k++)
                        suma += prijave2017[i]._uspjeh[j]->_predmeti[k]._ocjena;
                    suma /= prijave2017[i]._uspjeh[j]->_brojPredmeta;


                    if (suma > max)
                    {
                        max = suma;
                        kandidat = i;
                        razred = j;
                    }
                }
            }
        }
        return prijave2017[kandidat]._uspjeh[razred];
    };
    shared_ptr<Uspjeh> najbolji = najboljiUspjeh();
    najbolji->Ispis();

    /*
    napisati rekurzivnu funkciju koja ce vratiti pokazivac na kandidata sa
    najvecom ocjenom na predmetu koji je proslijedjen kao parametar. ukoliko je vise
    kandidata ostvarilo istu ocjenu, funkcija treba da vrati onog kandidata koji je prvi
    evidentirao tu ocjenu (ako je isto vrijeme evidentiranja, onda funkcija vraca kandidata
    koji je prvi u nizu).	u slucaju da niti jedan kandidat nije evidentirao trazeni predmet
    funkcija vraca nullptr. u nastavku je prikazan primjer poziva rekurzivne funkcije,
    a ostale parametre dodajte po potrebi.
    */
    Kandidat * kandidatSaNajboljomOcjenom = rekNajboljaOcjena(prijave2017, brojKandidata, "Matematika");
    if (kandidatSaNajboljomOcjenom) 
        cout << crt << "Kandidat sa najboljom ocjenom iz matematike je: " << kandidatSaNajboljomOcjenom->_imePrezime << crt;

    

    for (size_t i = 0; i < brojKandidata; i++)
    {
        prijave2017[i].Ispis();
        prijave2017[i].Dealociraj();
    }
    delete[] prijave2017;
    prijave2017 = nullptr;

    datum19062017_1015.Dealociraj();
    datum20062017_1115.Dealociraj();
    datum30062017_1215.Dealociraj();
    datum05072017_1231.Dealociraj();

    system("pause>0");
}