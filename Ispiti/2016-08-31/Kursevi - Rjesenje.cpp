#include <iostream>

using namespace std;

enum enumKursevi { HtmlCSSJavaScript, SoftwareEngeneeringFundamentals, MasteringSQL, WindowsSecurity };
const char * Kursevi[] = { "HtmlCSSJavaScript", "SoftwareEngeneeringFundamentals", "MasteringSQL", "WindowsSecurity" };
char *crt = "\n--------------------------------------------\n";

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
    void Dealociraj() { delete _dan; delete _mjesec; delete _godina; _dan = _mjesec = _godina = nullptr; }
};

struct Kurs
{
    enumKursevi _kurs;
    Datum _pocetak;
    Datum _kraj;
    char * _imePredavaca;
    bool _aktivan;//SVAKI KURS JE NA POCETKU AKTIVAN
    void Unos(enumKursevi kurs, Datum pocetak, Datum kraj, char *imePrezime)
    {
        _kurs = kurs;
        _pocetak.Unos(*pocetak._dan, *pocetak._mjesec, *pocetak._godina);
        _kraj.Unos(*kraj._dan, *kraj._mjesec, *kraj._godina);
        int size = strlen(imePrezime) + 1;
        _imePredavaca = new char[size];
        strcpy_s(_imePredavaca, size, imePrezime);
        _aktivan = true;
    }
    void Dealociraj()
    {
        delete[] _imePredavaca; _imePredavaca = nullptr;
        _pocetak.Dealociraj();
        _kraj.Dealociraj();
    }
    void Ispis()
    {
        cout << Kursevi[int(_kurs)] << endl;
        cout << "Pocetak: "; _pocetak.Ispis();
        cout << "Kraj: "; _kraj.Ispis();
        cout << "Ime predavaca: " << _imePredavaca << endl;
        cout << "Aktivan: " << boolalpha << _aktivan;
    }
};
struct Polaznik
{
    int _polaznikID;
    char * _imePrezime;
    void Unos(int polaznikID, char * imePrezime)
    {
        _polaznikID = polaznikID;
        int size = strlen(imePrezime) + 1;
        _imePrezime = new char[size];
        strcpy_s(_imePrezime, size, imePrezime);
    }
    void Ispis() { cout << _polaznikID << " " << _imePrezime << endl; }
    void Dealociraj() { delete[] _imePrezime; _imePrezime = nullptr; }
};
struct Polaganja
{
    Polaznik _polaznik;
    Kurs _kurs;
    Datum _datumPolaganja;
    float _ostvareniUspjeh;
    void Unos(const Polaznik & polaznik, const Kurs & kurs, const Datum & datum, float uspjeh)
    {
        _polaznik.Unos(polaznik._polaznikID, polaznik._imePrezime);
        _kurs.Unos(kurs._kurs, kurs._pocetak, kurs._kraj, kurs._imePredavaca);
        _datumPolaganja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
        _ostvareniUspjeh = uspjeh;
    }
    void Dealociraj()
    {
        _polaznik.Dealociraj();
        _kurs.Dealociraj();
        _datumPolaganja.Dealociraj();
    }
    void Ispis()
    {
        cout << "Polaznik: "; _polaznik.Ispis();
        cout << "Kurs: " << Kursevi[int(_kurs._kurs)] << endl;
        cout << "Datum polaganja: "; _datumPolaganja.Ispis();
        cout << "Ostvaren uspjeh: " << _ostvareniUspjeh << "%" << endl;
    }
};
struct SkillsCentar
{
    char * _nazivCentra;
    Kurs * _kursevi[50];//JEDAN EDUKACIJSKI CENTAR MOZE NUDITI NAJVISE 50 KURSEVA
    int _trenutnoKurseva;
    Polaganja * _polaganja;
    int _trenutnoPolaganja;
    void Unos(const char * naziv)
    {
        int size = strlen(naziv) + 1;
        _nazivCentra = new char[size];
        strcpy_s(_nazivCentra, size, naziv);
        _trenutnoKurseva = 0;
        _polaganja = nullptr;
        _trenutnoPolaganja = 0;
    }
    int PretvoriUDane(const Datum & datum) { return *datum._godina * 365 + *datum._mjesec * 30 + *datum._dan; }
    bool DodajKurs(const Kurs & kurs)
    {
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            if (_kursevi[i]->_kurs == kurs._kurs)
            {
                if (PretvoriUDane(_kursevi[i]->_kraj) > PretvoriUDane(kurs._pocetak))
                    return false;
                else
                    break;
            }
        }

        _kursevi[_trenutnoKurseva] = new Kurs;
        _kursevi[_trenutnoKurseva]->Unos(kurs._kurs, kurs._pocetak, kurs._kraj, kurs._imePredavaca);
        _trenutnoKurseva++;
        return true;
    }

    bool DodajPolaganje(const Polaganja & polaganje)
    {
        bool postojiAktivanKurs = false;
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            if (_kursevi[i]->_kurs == polaganje._kurs._kurs && _kursevi[i]->_aktivan)
            {
                postojiAktivanKurs = true;
                break;
            }
        }

        if (!postojiAktivanKurs) return false;

        for (int i = 0; i < _trenutnoPolaganja; i++)
        {
            bool istiKurs = _polaganja[i]._kurs._kurs == polaganje._kurs._kurs;
            bool istiPolaznik = strcmp(_polaganja[i]._polaznik._imePrezime, polaganje._polaznik._imePrezime) == 0;
            bool manjiUspjeh = _polaganja[i]._ostvareniUspjeh > polaganje._ostvareniUspjeh;
            if (istiKurs && istiPolaznik && manjiUspjeh)
                return false;
        }

        AlokacijaPolaganja(_polaganja, _trenutnoPolaganja);

        _polaganja[_trenutnoPolaganja].Unos(polaganje._polaznik, polaganje._kurs, polaganje._datumPolaganja, polaganje._ostvareniUspjeh);
        _trenutnoPolaganja++;
        return true;
    }
    void Ispis()
    {
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            for (int j = i + 1; j < _trenutnoKurseva; j++)
            {
                if (PretvoriUDane(_kursevi[i]->_pocetak) > PretvoriUDane(_kursevi[j]->_pocetak))
                {
                    Kurs * temp = _kursevi[i];
                    *_kursevi[i] = *_kursevi[j];
                    *_kursevi[j] = *temp;
                }
            }
        }

        cout << crt << _nazivCentra << endl;
        cout << "Trenutno kurseva: " << _trenutnoKurseva << crt;
        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            _kursevi[i]->Ispis();
            cout << crt;
        }

    }

    void AlokacijaPolaganja(Polaganja *& niz, int brojPolaganja)
    {
        Polaganja * temp = niz;
        niz = new Polaganja[brojPolaganja + 1];
        for (int i = 0; i < brojPolaganja; i++)
            niz[i] = temp[i];
    }

    void Dealociraj()
    {
        delete[] _nazivCentra; _nazivCentra = nullptr;

        for (int i = 0; i < _trenutnoKurseva; i++)
        {
            _kursevi[i]->Dealociraj();
            _kursevi[i] = nullptr;
        }

        for (int i = 0; i < _trenutnoPolaganja; i++)
            _polaganja[i].Dealociraj();
        delete[] _polaganja; _polaganja = nullptr;
    }

    Polaganja *PolaganjaByDatum(Datum OD, Datum DO, int &brojPolaganja)
    {
        Polaganja * niz = nullptr;
        for (int i = 0; i < _trenutnoPolaganja; i++)
        {
            bool veciOD = PretvoriUDane(OD) < PretvoriUDane(_polaganja[i]._datumPolaganja);
            bool manjiOD = PretvoriUDane(_polaganja[i]._datumPolaganja) < PretvoriUDane(DO);
            if (veciOD && manjiOD)
            {
                AlokacijaPolaganja(niz, brojPolaganja);
                niz[brojPolaganja].Unos
                (
                    _polaganja[i]._polaznik,
                    _polaganja[i]._kurs,
                    _polaganja[i]._datumPolaganja,
                    _polaganja[i]._ostvareniUspjeh
                );
                brojPolaganja++;
            }
        }

        return niz;
    }
};

float PretragaRekurzivno(SkillsCentar centar, enumKursevi kurs, int godina, int i = 0, int brojac = 0)
{
    if (i == 0)
    {

        for (int j = 0; j < centar._trenutnoPolaganja; j++)
            if (centar._polaganja[j]._kurs._kurs == kurs && *centar._polaganja[j]._kurs._pocetak._godina == godina)
                brojac++;


        if (brojac == 0)
            return 0;
        else if (brojac == 1)
        {
            for (int j = 0; j < centar._trenutnoPolaganja; j++)
                if (centar._polaganja[j]._kurs._kurs == kurs && *centar._polaganja[j]._kurs._pocetak._godina == godina)
                    return centar._polaganja[j]._ostvareniUspjeh;
        }
    }


    if (i == centar._trenutnoPolaganja - 1)
    {
        if (centar._polaganja[i]._kurs._kurs == kurs && *centar._polaganja[i]._kurs._pocetak._godina == godina)
            return centar._polaganja[i]._ostvareniUspjeh;
        else
            return 0;
    }

    if (centar._polaganja[i]._kurs._kurs == kurs && *centar._polaganja[i]._kurs._pocetak._godina == godina)
        return centar._polaganja[i]._ostvareniUspjeh / brojac + PretragaRekurzivno(centar, kurs, godina, i + 1, brojac) / brojac;
    else
        return PretragaRekurzivno(centar, kurs, godina, i + 1, brojac);
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

    Kurs softverski, html, sql, security;
    softverski.Unos(SoftwareEngeneeringFundamentals, datum1, datum2, "Emina Junuz");
    html.Unos(HtmlCSSJavaScript, datum2, datum3, "Larisa Tipura");
    sql.Unos(MasteringSQL, datum3, datum4, "Jasmin Azemovic");
    security.Unos(WindowsSecurity, datum3, datum4, "Adel Handzic");

    Polaznik denis, zanin, indira;
    denis.Unos(1, "Denis Music");
    zanin.Unos(2, "Zanin Vejzovic");
    indira.Unos(3, "Indira Hamulic");

    Polaganja denisHtml, zaninHtml, indiraSql;
    denisHtml.Unos(denis, html, datum5, 61);
    //61 predstavlja ostvareni uspjeh/procenat, a uspjesno polozenim se smatra svaki kurs na kome je polaznik ostvari vise od 55%
    zaninHtml.Unos(zanin, html, datum6, 93);
    indiraSql.Unos(indira, sql, datum3, 77);

    SkillsCentar mostar;
    mostar.Unos("Skills Center Mostar");
    //jedan edukacijski centar moze nuditi vise istih kurseva (npr. MasteringSQL), ali se oni moraju realizovati u razlictim periodima (pocetak-kraj) tj. ne smiju se preklapati
    //novi kurs istog tipa (npr. MasteringSQL) se mora realizovati nakon prethodno dodanog npr. ako prethodni kurs MasteringSQL traje 01.03.2016 - 01.04.2016, onda se novi kurs MasteringSQL moze dodati jedino ako pocinje nakon 01.04.2016
    if (mostar.DodajKurs(softverski))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        softverski.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(html))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        html.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(sql))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        sql.Ispis();
        cout << crt;
    }
    if (mostar.DodajKurs(security))
    {
        cout << crt << "KURS USPJESNO REGISTROVAN" << crt;
        security.Ispis();
        cout << crt;
    }
    //nije moguce dodati polaganje onog kursa koji nije evidentiran (registrovan, ponudjen) u tom edukacijskom centru
    //moguce je polagati samo aktivne kurseve i to najkasnije 5 dana od dana zavrsetka (kraja) posljednje dodanog (aktivnog) kursa iste vrste (npr. MasteringSQL)
    //jedan polaznik moze vise puta polagati isti kurs, ali novi procenat mora biti veci od prethodno dodanog (na istom kursu)
    if (mostar.DodajPolaganje(denisHtml))
    {
        cout << crt << "POLAGANJE EVIDENTIRANO" << crt;
        denisHtml.Ispis();
        cout << crt;
    }
    if (mostar.DodajPolaganje(zaninHtml))
    {
        cout << crt << "POLAGANJE EVIDENTIRANO" << crt;
        zaninHtml.Ispis();
    }
    if (mostar.DodajPolaganje(indiraSql))
    {
        cout << crt << "POLAGANJE EVIDENTIRANO" << crt;
        indiraSql.Ispis();
    }
    Datum OD, DO;
    int brojPolaganja = 0;
    OD.Unos(1, 6, 2016);
    DO.Unos(1, 8, 2016);
    //PolaganjaByDatum – vraca niz polaganja koja su uspjesno realizovana u periodu OD-DO
    Polaganja * polaganjaByDatum = mostar.PolaganjaByDatum(OD, DO, brojPolaganja);
    cout << crt << "U periodu od ";
    OD.Ispis();
    cout << " do ";
    DO.Ispis();
    cout << " uspjesno je realizovano " << brojPolaganja << " polaganja--->>>" << endl << endl;
    for (size_t i = 0; i < brojPolaganja; i++)
    {
        polaganjaByDatum[i].Ispis();
        cout << crt;
    }
    cout << crt;

    //PretragaRekurzivno - rekurzivna funkcija pronalazi prosjecni uspjeh koji su polaznici tokom godine (npr.2016) ostvarili 
    //na odredjenom kursu (npr. MasteringSQL)
    cout << "Prosjecan uspjeh na kursu MasteringSQL u 2016 godini je " << PretragaRekurzivno(mostar, MasteringSQL, 2016) << endl;
    ////Ispis - ispisuje sve informacije o edukacijskom centru. prije ispisa sortirati sve kurseve na osnovu pocetka odrzavanja kursa
    mostar.Ispis();

    //izvrsiti potrebne dealokacije
    for (size_t i = 0; i < brojPolaganja; i++)
        polaganjaByDatum[i].Dealociraj();
    delete[] polaganjaByDatum; polaganjaByDatum = nullptr;

    datum1.Dealociraj();
    datum2.Dealociraj();
    datum3.Dealociraj();
    datum4.Dealociraj();
    datum5.Dealociraj();
    datum6.Dealociraj();
    OD.Dealociraj();
    DO.Dealociraj();

    softverski.Dealociraj();
    html.Dealociraj();
    sql.Dealociraj();
    security.Dealociraj();

    denis.Dealociraj();
    zanin.Dealociraj();
    indira.Dealociraj();

    denisHtml.Dealociraj();
    zaninHtml.Dealociraj();
    indiraSql.Dealociraj();

    mostar.Dealociraj();

    system("pause");
}