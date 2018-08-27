#include <iostream>
#include <fstream>

using namespace std;

enum Opstina { Opstina1, Opstina2, Opstina3, Opstina4 };
char * OpstineChar[] = { "Opstina1", "Opstina2", "Opstina3", "Opstina4" };
enum Kandidati { Kandidat1, Kandidat2, Kandidat3, Kandidat4 };
char * KandidatiChar[] = { "Kandidat1", "Kandidat2", "Kandidat3", "Kandidat4" };

struct Datum
{
    int _dan, _mjesec, _godina;
    void Unos(int d, int m, int g) { _dan = d; _mjesec = m; _godina = g; }
    void Ispis() { cout << _dan << "/" << _mjesec << "/" << _godina << endl; }
};

int datecmp(const Datum &d1, const Datum &d2)
{
    if (d1._godina != d2._godina) return d1._godina - d2._godina;
    else if (d1._mjesec != d2._mjesec) return d1._mjesec - d2._mjesec;
    else if (d1._dan != d2._dan) return d1._dan - d2._dan;
    return 0;
}

struct Osoba
{
    Datum _datumRodjenja;
    char * _imePrezime;
    char _JMBG[14];
    Opstina _Prebivaliste;
    void unos(Datum d, char * ip, char jmbg[], Opstina p)
    {
        _datumRodjenja = d;
        int velicina = strlen(ip) + 1;
        _imePrezime = new char[velicina];
        strcpy_s(_imePrezime, velicina, ip);
        strcpy_s(_JMBG, jmbg);
        _Prebivaliste = p;
    }
    void Dealociraj() { delete[] _imePrezime; _imePrezime = nullptr; }
    void Info()
    {
        _datumRodjenja.Ispis();
        cout << _imePrezime << " " << _JMBG << " " <<
            OpstineChar[_Prebivaliste] << endl;
    }
};

struct Glas
{
    Osoba * _glasac;
    Kandidati * _glasZa;
    void Unos(Osoba o, Kandidati k)
    {
        _glasac = new Osoba;
        _glasac->unos(o._datumRodjenja, o._imePrezime, o._JMBG, o._Prebivaliste);
        _glasZa = new Kandidati(k);
    }
    void Ispis()
    {
        _glasac->Info();
        cout << KandidatiChar[*_glasZa] << endl;
    }
    void Dealociraj() { _glasac->Dealociraj(); delete _glasZa; _glasZa = nullptr; }
};

struct Izbori2016
{
    Opstina * _glasackoMjesto;
    Glas * _glasovi[1000];
    int _doSadaGlasalo;
    Datum _datumIzbora;

    void Unos(Opstina opstina, Datum datum)
    {
        _glasackoMjesto = new Opstina(opstina);
        _datumIzbora = datum;
        _doSadaGlasalo = 0;
    }

    void Dealociraj()
    {
        delete _glasackoMjesto; _glasackoMjesto = nullptr;
        for (int i = 0; i < _doSadaGlasalo; i++)
            _glasovi[i]->Dealociraj(), _glasovi[i] = nullptr;
    }

    bool isPunoljetan(Datum datum)
    {
        if (_datumIzbora._godina - datum._godina == 18)
            if (datum._mjesec < _datumIzbora._mjesec) 
                return false;
        
        return _datumIzbora._godina - datum._godina >= 18;
    }

    bool DodajGlas(Glas glas)
    {
        if (_doSadaGlasalo == 1000) return false;

        if (!isPunoljetan(glas._glasac->_datumRodjenja)) return false;

        if (glas._glasac->_Prebivaliste != *_glasackoMjesto) return false;

        for (int i = 0; i < _doSadaGlasalo; i++)
            if (strcmp(_glasovi[i]->_glasac->_JMBG, glas._glasac->_JMBG) == 0)
                return false;
        
        _glasovi[_doSadaGlasalo] = new Glas;
        _glasovi[_doSadaGlasalo]->Unos(*glas._glasac, *glas._glasZa);
        _doSadaGlasalo++;
        return true;
    }

    void Ispis()
    {
        const int brojKandidata = 4;
        int brojac[brojKandidata]{};
        int pozicija[brojKandidata]{};
        
        for(int i = 0; i < brojKandidata; i++)
            pozicija[i] = i;

        for (int i = 0; i < _doSadaGlasalo; i++)
            brojac[*_glasovi[i]->_glasZa]++;

        for(int i = 0; i < brojKandidata - 1; i++)
            if (brojac[i] < brojac[i + 1])
            {
                int temp;

                temp = brojac[i];
                brojac[i] = brojac[i + 1];
                brojac[i + 1] = temp;

                temp = pozicija[i];
                pozicija[i] = pozicija[i + 1];
                pozicija[i + 1] = temp;
            }

        cout << "Glasacko mjeseto: " << OpstineChar[*_glasackoMjesto] << endl;
        for (int i = 0; i < brojKandidata; i++)
            cout << i + 1 << ". " << KandidatiChar[pozicija[i]] << " " << brojac[i] / float(_doSadaGlasalo) << "%" << endl;
    }
    
    int BrojGlasacaPoRodjenju(const Datum &OD, const Datum &DO)
    {
        int brojac = 0;
        for (int i = 0; i < _doSadaGlasalo; i++)
            if (datecmp(OD, _glasovi[i]->_glasac->_datumRodjenja) > 0 && datecmp(DO, _glasovi[i]->_glasac->_datumRodjenja) < 0)
                brojac++;
        return brojac;
    }
};

char *formirajNaziv(const Izbori2016 &izbori)
{
    int velicina = strlen(OpstineChar[*izbori._glasackoMjesto]) + 1;
    char *naziv = new char[velicina];

    strcpy_s(naziv, velicina, OpstineChar[*izbori._glasackoMjesto]);
    return naziv;
}

void UpisiUBinarniFajl(Izbori2016 izbori)
{
    char *naziv = formirajNaziv(izbori);
    ofstream upis(naziv, ios::binary);

    if (!upis.fail())
    {
        upis.write((char*)(&izbori), sizeof(izbori));
        upis.close();
    }
    else cout << "Greska!" << endl;
}

void UcitajIzBinarnogFajla(Izbori2016 &izbori, const char *naziv)
{
    ifstream ispis(naziv, ios::binary);

    if (!ispis.fail())
    {
        ispis.read((char*)(&izbori), sizeof(izbori));
        ispis.close();
    }
    else cout << "Greska!" << endl;
}

int PretragaRekurzivno(Izbori2016 izbori, Kandidati kandidat, int i = 0)
{
    if (i == izbori._doSadaGlasalo) return 0;

    if (*izbori._glasovi[i]->_glasZa == kandidat) 
        return 1 + PretragaRekurzivno(izbori, kandidat, i + 1);
    else 
        return PretragaRekurzivno(izbori, kandidat, i + 1);
}

void main()
{
    Datum datumGlasac1, datumGlasac2, datumGlasac3, datumGlasac4, datumIzbora;
    datumIzbora.Unos(14, 7, 2016);
    datumGlasac1.Unos(18, 5, 1990);
    datumGlasac2.Unos(5, 3, 1982);
    datumGlasac3.Unos(12, 8, 1958);
    datumGlasac4.Unos(22, 6, 2000);

    Osoba a, b, c, d, e;
    a.unos(datumGlasac1, "Denis Music", "1111111111111", Opstina1);
    b.unos(datumGlasac2, "Jasmin Azemovic", "1111111111112", Opstina1);
    c.unos(datumGlasac3, "Indira Hamulic", "1111111111113", Opstina1);
    d.unos(datumGlasac4, "Adel Handzic", "1111111111114", Opstina2);
    e.unos(datumGlasac4, "Zanin Vejzovic", "1111111111115", Opstina1);


    Glas g1, g2, g3, g4, g5;
    g1.Unos(a, Kandidat1);
    g2.Unos(b, Kandidat1);
    g3.Unos(c, Kandidat2);
    g4.Unos(d, Kandidat1);
    g5.Unos(e, Kandidat2);

    Izbori2016 izbori, izbori_FromFile;
    izbori.Unos(Opstina1, datumIzbora);

    izbori.DodajGlas(g1);
    izbori.DodajGlas(g2);
    izbori.DodajGlas(g3);
    izbori.DodajGlas(g4);
    izbori.DodajGlas(g5);
    izbori.Ispis();

    Datum OD, DO;
    OD.Unos(17, 7, 1998);
    DO.Unos(17, 7, 1986);
    cout << "Glasaca u starost od 18 - 30 godina->" << izbori.BrojGlasacaPoRodjenju(OD, DO) << endl;
    cout << "Broj glasova za Kandidat1->" << PretragaRekurzivno(izbori, Kandidat1) << endl;
    UpisiUBinarniFajl(izbori);
    UcitajIzBinarnogFajla(izbori_FromFile, "Opstina1");
    izbori_FromFile.Ispis();

    a.Dealociraj(); b.Dealociraj(); c.Dealociraj(); d.Dealociraj(); e.Dealociraj();
    g1.Dealociraj(); g2.Dealociraj(); g3.Dealociraj(); g4.Dealociraj(); g5.Dealociraj();
    izbori.Dealociraj();
    
    system("pause>0");
}