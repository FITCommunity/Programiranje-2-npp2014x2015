#include <iostream>

using namespace std;

enum enumPredmeti { Matematika, Historija, Geografia, Hemija, Biologija, Engleski };
enum enumRazredi { I, II, III, IV };
char *crt = "\n--------------------------------------------\n";
const char *ispisPredmeti[]{ "Matematika", "Historija", "Geografia", "Hemija", "Biologija", "Engleski" };
const char *ispisRazredi[]{ "I", "II", "III", "IV" };

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
    void Dealociraj() { delete _dan; delete _mjesec; delete _godina; }
    int DatumUDane() { return *_godina * 365 + *_mjesec * 30 + *_dan; }
};

int datecmp(const Datum &datum1, const Datum &datum2)
{
    if (*datum1._godina != *datum2._godina) return *datum1._godina - *datum2._godina;
    else if (*datum1._mjesec != *datum2._godina) return *datum1._mjesec - *datum2._mjesec;
    else if (*datum1._dan != *datum2._dan) return *datum1._dan - *datum2._dan;
    
    return 0;
}

struct Kurs
{
    enumPredmeti _predmet;
    enumRazredi _razredi;
    Datum * _pocetak;
    Datum * _kraj;
    char * _imePredavaca;
    void Unos(enumPredmeti predmet, enumRazredi razred, Datum pocetak, Datum kraj, char * imePredavaca)
    {
        _pocetak = new Datum; 
        _kraj = new Datum;
        _pocetak->Unos(*pocetak._dan, *pocetak._mjesec, *pocetak._godina);
        _kraj->Unos(*kraj._dan, *kraj._mjesec, *kraj._godina);
        _predmet = predmet;
        _razredi = razred;
        int size = strlen(imePredavaca) + 1;
        _imePredavaca = new char[size];
        strcpy_s(_imePredavaca, size, imePredavaca);
    }
    void Ispis() 
    {
        cout << "Predmet: " << ispisPredmeti[_predmet] << endl;
        cout << "Razred: " << ispisRazredi[_razredi] << endl;
        cout << "Ime predavaca: " << _imePredavaca << endl;
        cout << "Datum pocetka: "; _pocetak->Ispis();
        cout << "Datum kraja: "; _kraj->Ispis();
    }
    void Dealociraj()
    {
        delete[] _imePredavaca; _imePredavaca = nullptr;
        _pocetak->Dealociraj();
        _kraj->Dealociraj();
    }
};

struct Polaznik
{
    Datum * _datumRodjenja;
    char * _imePrezime;
    void Unos(Datum datumRodjenja, char * imePrezime)
    {
        int size = strlen(imePrezime) + 1;
        _imePrezime = new char[size];
        strcpy_s(_imePrezime, size, imePrezime);
        _datumRodjenja = new Datum;
        _datumRodjenja->Unos(*datumRodjenja._dan, *datumRodjenja._mjesec, *datumRodjenja._godina);
    }
    void Dealociraj() { delete[] _imePrezime; _imePrezime = nullptr; _datumRodjenja->Dealociraj(); }
    void Ispis() 
    {
        cout << "Ime i prezime: " << _imePrezime << " Datum rodenja: ";
        _datumRodjenja->Ispis();
    }
};
struct Polaganja
{
    Polaznik * _polaznik;
    Kurs * _kurs;
    Datum * _datumPolaganja;
    int _ocjena; //ocjene su u rasponu od 1 do 5	

    void Unos(Polaznik polaznik, Kurs kurs, Datum datum, int ocjena)
    {
        if (ocjena < 1 || ocjena > 5)
        {
            _polaznik = nullptr;
            _kurs = nullptr;
            _datumPolaganja = nullptr;
            return;
        }

        _polaznik = new Polaznik;
        _polaznik->Unos(*polaznik._datumRodjenja, polaznik._imePrezime);
        _kurs = new Kurs;
        _kurs->Unos(kurs._predmet, kurs._razredi, *kurs._pocetak, *kurs._kraj, kurs._imePredavaca);
        _datumPolaganja = new Datum;
        _datumPolaganja->Unos(*datum._dan, *datum._mjesec, *datum._godina);
        _ocjena = ocjena;
    }

    void Ispis()
    {
        cout << "Polaznik: "; _polaznik->Ispis();
        cout << "Kurs: ";  _kurs->Ispis();
        cout << "Datum polaganja: "; _datumPolaganja->Ispis();
        cout << "Ocjena: " << _ocjena << endl;
    }

    void Dealociraj()
    {
        if(_polaznik) _polaznik->Dealociraj();
        if(_kurs) _kurs->Dealociraj();
        if(_datumPolaganja) _datumPolaganja->Dealociraj();
    }
};

struct EdukacijskiCentar
{
    char * _nazivCentra;
    Kurs * _kursevi[50];//JEDAN EDUKACIJSKI CENTAR MOZE NUDITI NAJVISE 50 KURSEVA
    int _trenutnoKurseva;
    Polaganja * _polaganja;
    int _trenutnoPolaganja;

    void Unos(const char *nazivCentra);
    bool DodajKurs(Kurs kurs);
    bool DodajPolaganje(Polaganja denisMatematika);
    Polaganja *PolaganjaByDatumIRazred(Datum OD, Datum DO, enumRazredi razred, int &brojPolaganja);
    void SortirajKurseve(bool (*Funkcija)(Kurs *, Kurs *));
    void Ispis()
    {
        SortirajKurseve([](Kurs *x, Kurs *y) { return datecmp(*x->_pocetak, *y->_pocetak) > 0; });
        //SortirajKurseve([](Kurs *x, Kurs *y) { return datecmp(*x->_kraj, *y->_kraj) > 0; });
        //SortirajKurseve([](Kurs *x, Kurs *y) { return x->_razredi > y->_razredi; });
        cout << crt << _nazivCentra << crt;
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            _kursevi[i]->Ispis();
            cout << crt;
        }
    }

    void Dealociraj()
    {
        delete[] _nazivCentra; _nazivCentra = nullptr;
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            _kursevi[i]->Dealociraj();
            delete _kursevi[i];
            _kursevi[i] = nullptr;
        }
        for (int i = 0; i < _trenutnoPolaganja; i++)
            _polaganja[i].Dealociraj();
        delete[] _polaganja; _polaganja = nullptr;
    }
};

void EdukacijskiCentar::SortirajKurseve(bool(*Funkcija)(Kurs *, Kurs *))
{
    bool zamjena = true;
    while (zamjena)
    {
        zamjena = false;
        for (int i = 0; i < _trenutnoKurseva - 1; i++)
        {
            if (Funkcija(_kursevi[i], _kursevi[i + 1]))
            {
                Kurs *temp = _kursevi[i];
                _kursevi[i] = _kursevi[i + 1];
                _kursevi[i + 1] = temp;
                zamjena = true;
            }
        }
    }
}

void EdukacijskiCentar::Unos(const char *nazivCentra)
{
    int velicina = strlen(nazivCentra) + 1;
    _nazivCentra = new char[velicina];
    strcpy_s(_nazivCentra, velicina, nazivCentra);
    _trenutnoKurseva = 0;
    _trenutnoPolaganja = 0;
    _polaganja = nullptr;
}

bool EdukacijskiCentar::DodajKurs(Kurs kurs)
{
    if (_trenutnoKurseva == 50) return false;
    
    for (int i = _trenutnoKurseva - 1; i >= 0; i--)
        if (_kursevi[i]->_predmet == kurs._predmet)
            if (datecmp(*_kursevi[i]->_kraj, *kurs._pocetak) >= 0)
                return false;
            else
                break;

    _kursevi[_trenutnoKurseva] = new Kurs;
    _kursevi[_trenutnoKurseva]->Unos(kurs._predmet, kurs._razredi, *kurs._pocetak, *kurs._kraj, kurs._imePredavaca);
    _trenutnoKurseva++;
}

bool EdukacijskiCentar::DodajPolaganje(Polaganja polaganje)
{
    if (!polaganje._polaznik || !polaganje._kurs || !polaganje._datumPolaganja) return false;

    bool flag = false;
    for (int i = 0; i < _trenutnoKurseva; i++)
    {
        bool isPredmet = _kursevi[i]->_predmet == polaganje._kurs->_predmet;
        bool isDatum = datecmp(*_kursevi[i]->_pocetak, *polaganje._kurs->_pocetak) == 0;
        
        if (isPredmet && isDatum)
        {
            flag = true;
            break;
        }
    }

    if (!flag) return false;

    for (int i = _trenutnoPolaganja - 1; i >= 0; i--)
        if (strcmp(_polaganja[i]._polaznik->_imePrezime, polaganje._polaznik->_imePrezime) == 0)
            if (_polaganja[i]._datumPolaganja->DatumUDane() - polaganje._datumPolaganja->DatumUDane() < 15)
                return false;

    
    for (int i = _trenutnoPolaganja - 1; i >= 0; i--)
        if (_polaganja[i]._kurs->_predmet == polaganje._kurs->_predmet && _polaganja[i]._kurs->_razredi == polaganje._kurs->_razredi)
        {
            if (_polaganja[i]._ocjena >= polaganje._ocjena)
                return false;
        }


    Polaganja *temp = _polaganja;
    _polaganja = new Polaganja[_trenutnoPolaganja + 1];
    for (int i = 0; i < _trenutnoPolaganja; i++)
        _polaganja[i] = temp[i];
    _polaganja[_trenutnoPolaganja].Unos(*polaganje._polaznik, *polaganje._kurs, *polaganje._datumPolaganja, polaganje._ocjena);
    _trenutnoPolaganja++;

    delete[] temp;

    return true;
}

Polaganja *EdukacijskiCentar::PolaganjaByDatumIRazred(Datum OD, Datum DO, enumRazredi razred, int &brojPolaganja)
{
    if (brojPolaganja < 0) return nullptr;

    Polaganja *niz = nullptr;
    
    for (int i = 0; i < _trenutnoPolaganja; i++)
    {
        bool isRazred = _polaganja[i]._kurs->_razredi == razred;
        bool veciOD = datecmp(OD, *_polaganja[i]._datumPolaganja) <= 0;
        bool manjiOD = datecmp(DO, *_polaganja[i]._datumPolaganja) >= 0;

        if (isRazred && veciOD && manjiOD)
        {
            Polaganja *temp = niz;
            niz = new Polaganja[brojPolaganja + 1];
            for (int i = 0; i < brojPolaganja; i++)
                niz[i] = temp[i];
            niz[brojPolaganja] = _polaganja[i];
            brojPolaganja++;

            delete[] temp;
        }
    }

    return niz;
}

float PretragaRekurzivno(const EdukacijskiCentar &centar, enumPredmeti predmet, int godina, int i = 0, int brojac = 0, float suma = 0)
{
    if (i == centar._trenutnoPolaganja)
        return suma / brojac;

    bool isPredmet = centar._polaganja[i]._kurs->_predmet == predmet;
    bool isGodina = *centar._polaganja[i]._datumPolaganja->_godina == godina;
    bool isOcjena = centar._polaganja[i]._ocjena > 1;
    if (isPredmet && isGodina && isOcjena)
        return PretragaRekurzivno(centar, predmet, godina, i + 1, brojac + 1, suma + centar._polaganja[i]._ocjena);
    else
        return PretragaRekurzivno(centar, predmet, godina, i + 1, brojac, suma);
}

void main()
{
    Datum datum1, datum2, datum3, datum4, datum5, datum6;
    datum1.Unos(26, 11, 2015);
    datum2.Unos(29, 5, 2016);
    datum3.Unos(5, 6, 2016);
    datum4.Unos(15, 8, 2016);
    datum5.Unos(13, 7, 2016);
    datum6.Unos(22, 9, 2016);

    Kurs matematika, hemija, biologija, engleski;
    matematika.Unos(Matematika, III, datum1, datum2, "Nina Bijedic");
    hemija.Unos(Hemija, III, datum2, datum3, "Murat Praso");
    biologija.Unos(Biologija, IV, datum3, datum4, "Jasmin Azemovic");
    engleski.Unos(Engleski, II, datum3, datum4, "Iris Memic");

    Datum rodjenje1, rodjenje2;
    rodjenje1.Unos(22, 8, 1982);
    rodjenje2.Unos(11, 8, 2018);
    Polaznik denis, zanin, indira;
    denis.Unos(rodjenje1, "Denis Music");
    zanin.Unos(rodjenje2, "Zanin Vejzovic");


    Polaganja denisMatematika, zaninHemija, denisBiologija;
    denisMatematika.Unos(denis, matematika, datum5, 3);
    zaninHemija.Unos(zanin, hemija, datum6, 93);

    EdukacijskiCentar mostar;
    mostar.Unos("ECM - Edukacijski Center Mostar");

    if (mostar.DodajKurs(matematika))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        matematika.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(engleski))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        matematika.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(hemija))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        matematika.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(biologija))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        matematika.Ispis();
        cout << crt;
    }

    if (mostar.DodajPolaganje(denisMatematika)) 
    {
        cout << crt << "POLAGANJE EVIDENTIRANO" << crt;
        denisMatematika.Ispis();
    }
    if (mostar.DodajPolaganje(zaninHemija))
    {
        cout << crt << "POLAGANJE EVIDENTIRANO" << crt;
        denisMatematika.Ispis();
    }
    Datum OD, DO;
    int brojPolaganja = 0;
    OD.Unos(1, 6, 2016);
    DO.Unos(1, 8, 2016);

    cout << crt;
    Polaganja * polaganjaByDatumiRazred = mostar.PolaganjaByDatumIRazred(OD, DO, II, brojPolaganja);
    cout << "U periodu od ";
    OD.Ispis();
    cout << " do ";
    DO.Ispis();
    cout << " uspjesno je polozeno " << brojPolaganja << " predmeta --->>> " << crt;
    for (size_t i = 0; i < brojPolaganja; i++)
        polaganjaByDatumiRazred[i].Ispis();
    cout << crt;

    cout << "Prosjecan uspjeh na kursu iz predmeta Matematika u 2016 godini je " << PretragaRekurzivno(mostar, Matematika, 2016) << endl;

    mostar.Ispis();

    datum1.Dealociraj(); datum2.Dealociraj(); datum3.Dealociraj(); 
    datum4.Dealociraj(); datum5.Dealociraj(); datum6.Dealociraj();
    matematika.Dealociraj(); hemija.Dealociraj(); biologija.Dealociraj(); engleski.Dealociraj();
    rodjenje1.Dealociraj(); rodjenje2.Dealociraj();
    denis.Dealociraj(); zanin.Dealociraj();
    denisMatematika.Dealociraj(); zaninHemija.Dealociraj();
    OD.Dealociraj(); DO.Dealociraj();
    delete[] polaganjaByDatumiRazred;
    mostar.Dealociraj();

    system("pause");
}