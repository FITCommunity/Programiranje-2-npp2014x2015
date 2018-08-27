#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

char * crt = "\n----------------------------------------------------\n";
char templateID[9]{ "IB150000" };
int brojIndeksa = 1;

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
struct Izostanak
{
    char * _razlogIzostanka;
    Datum _datumIzostanka;
    int _brojSati;
    bool _opravdano;//pravdanje zahtijeva da se evidentira i razlog izostanka	
    void Unos(Datum datumIzostanka, int brojSati)
    {
        _datumIzostanka.Unos(*datumIzostanka._dan, *datumIzostanka._mjesec, *datumIzostanka._godina);
        _brojSati = brojSati;
        _opravdano = false;
        _razlogIzostanka = new char;
        _razlogIzostanka[0] = '\0';
    }
    void Dealociraj() { if (_razlogIzostanka) { delete[] _razlogIzostanka; _razlogIzostanka = nullptr; } }
    void Ispis() { _datumIzostanka.Ispis(); cout << _razlogIzostanka << " " << _razlogIzostanka; }
    void Opravdaj(char * razlogIzostanka)
    {
        if (_razlogIzostanka != nullptr)
            delete _razlogIzostanka;
        int vel = strlen(razlogIzostanka) + 1;
        _razlogIzostanka = new char[vel];
        strcpy_s(_razlogIzostanka, vel, razlogIzostanka);
        _opravdano = true;
    }
};

int datecmp(const Datum &d1, const Datum &d2)
{
    if (*d1._godina != *d2._godina) return *d1._godina - *d2._godina;
    else if (*d1._mjesec != *d2._mjesec) return *d1._mjesec - *d2._mjesec;
    else if (*d1._dan != *d2._dan) return *d1._dan - *d2._dan;
    return 0;
}



struct Student
{
    //format broja indeksa: IB150051
    //osigurati automatsko generisanje broja indeksa pocevsi od IB150001
    char _brojIndeksa[9];
    char * _imePrezime;
    Izostanak * _izostanci;
    int _brojIzostanaka;
    //Potpisi funkcija trebaju odgovarati onima u main-u
    //1. Unos
    void Unos(char *indeks, char *imePrezime)
    {
        strcpy_s(_brojIndeksa, 9, indeks);

        int velicina = strlen(imePrezime) + 1;
        _imePrezime = new char[velicina];
        strcpy_s(_imePrezime, velicina, imePrezime);

        _izostanci = nullptr;
        _brojIzostanaka = 0;
    }
    //2. Dealociraj
    void Dealociraj()
    {
        delete[] _imePrezime; _imePrezime = nullptr;
        for (int i = 0; i < _brojIzostanaka; i++)
            _izostanci[i].Dealociraj();
        delete[] _izostanci; _izostanci = nullptr;
    }
    /*3. DodajIzostanak - dodaje novi izostanak u listu izostanaka.	Onemoguciti ponavljanje
    izostanaka na nacin da se uvecava samo broj sati ukoliko
    korisnik (profesor) vise puta pokusa dodati izostanak na isti dan.*/
    void DodajIzostanak(Izostanak izostanak)
    {
        for (int i = 0; i < _brojIzostanaka; i++)
        {
            if (datecmp(_izostanci[i]._datumIzostanka, izostanak._datumIzostanka) == 0)
            {
                _izostanci[i]._brojSati += izostanak._brojSati;
                return;
            }
        }

        Izostanak *temp = _izostanci;
        _izostanci = new Izostanak[_brojIzostanaka + 1];
        for (int i = 0; i < _brojIzostanaka; i++) _izostanci[i] = temp[i];
        _izostanci[_brojIzostanaka] = izostanak;
        _brojIzostanaka++;

        delete[] temp;
    }
    //4. Ispis - ispisuje izostanke u formatu prikazanom na slici (nije obavezno da ispis bude identican,
    //ali je svakako pozeljno). Prilikom svakog ispisa, izostanke je potrebno spasiti u tekstualni fajl sa 
    //istim nazivom kao i broj indeksa.
    void Ispis()
    {
        char naziv[14];
        strcpy_s(naziv, 14, _brojIndeksa);
        strcat_s(naziv, 14, ".txt");

        ofstream upis; upis.open(naziv);

        if (!upis.fail())
        {
            upis << crt << _imePrezime << " " << _brojIndeksa << crt;
            upis << "Datum\tSati\tOpravdan\tRazlog" << crt;
            
            for (int i = 0; i < _brojIzostanaka; i++)
            {
                upis << setw(2) << *_izostanci[i]._datumIzostanka._dan << "/"
                     << *_izostanci[i]._datumIzostanka._mjesec << "/"
                     << *_izostanci[i]._datumIzostanka._godina << " ";
                upis << _izostanci[i]._brojSati << "\t";
                upis << (_izostanci[i]._opravdano ? "Opravdano" : "Neopravdano");
                upis << "\t" << _izostanci[i]._razlogIzostanka;

                i == _brojIzostanaka - 1 ? upis << crt : upis << endl;  
            }

            upis.close();
            ifstream ispis; ispis.open(naziv);

                
            if(!ispis.fail())
            {
                char znak;
                while (ispis.get(znak)) cout << znak;

                ispis.close();
            }
            else cout << "Greska!" << endl;
        }
        else cout << "Greska!" << endl;

    }
    //5. BinarnaPretraga - koristeci binarnu pretragu, na osnovu datuma, pronalazi i vraca pokazivac na izostanak.
    //Ukoliko trazeni izostanak ne postoji funkcija vraca nullptr.
    void SortirajPoDatumu()
    {
        bool flag = true;
        while (flag)
        {
            flag = false;

            for(int i = 0; i < _brojIzostanaka - 1; i++)
                if (datecmp(_izostanci[i]._datumIzostanka, _izostanci[i + 1]._datumIzostanka) > 0)
                {
                    Izostanak temp = _izostanci[i];
                    _izostanci[i] = _izostanci[i + 1];
                    _izostanci[i + 1] = temp;
                    flag = true;
                }
        }
    }

    Izostanak *BinarnaPretraga(const Datum &d)
    {
        int prvi = 0, zadnji = _brojIzostanaka - 1;
        while (prvi <= zadnji)
        {
            int srednji = (prvi + zadnji) / 2;

            if (datecmp(_izostanci[srednji]._datumIzostanka, d) < 0)
                zadnji = srednji - 1; 
            else if (datecmp(_izostanci[srednji]._datumIzostanka, d) > 0)
                prvi = srednji + 1;
            else
                return &_izostanci[srednji];
        }

        return nullptr;
    }
};
//1. PronadjiNajveciNeopravdaniIzostanak - rekurzivna funkcija koja vraca pokazivac na neopravdani izostanak koji ima najveci broj sati
Izostanak *PronadjiNajveciNeopravdaniIzostanak(Student student, int i = 0, int najveci = 0)
{
    if (i == student._brojIzostanaka)
    {
        if (!student._izostanci[najveci]._opravdano)
            return &student._izostanci[najveci];
        else
            return nullptr;
    }

    if (!student._izostanci[i]._opravdano)
    {
        if (!student._izostanci[najveci]._opravdano)
        {
            if (student._izostanci[i]._brojSati > student._izostanci[najveci]._brojSati)
                najveci = i;
        }
        else najveci = i;
    }

    PronadjiNajveciNeopravdaniIzostanak(student, i + 1, najveci);
}
//2. GenerisiSljedeciBrojIndeksa - generise i vraca naredni broj indeksa
char *GenerisiSljedeciBrojIndeksa()
{
    int temp = brojIndeksa;
    int i = 7;

    while (i >= 0 && temp != 0)
    {
        templateID[i] = char(temp % 10 + '0');
        temp /= 10;
        i--;
    }

    brojIndeksa++;
    return templateID;
}

void main()
{
    Datum jucer, prije5Dana, prije10Dana;
    jucer.Unos(12, 6, 2016);
    prije5Dana.Unos(8, 6, 2016);
    prije10Dana.Unos(3, 6, 2016);

    Student denis;
    denis.Unos(GenerisiSljedeciBrojIndeksa(), "Denis Music");

    Izostanak izostanakJucer, izostanakPrije5Dana, izostanakPrije10Dana;

    izostanakJucer.Unos(jucer, 5);
    denis.DodajIzostanak(izostanakJucer);

    izostanakPrije5Dana.Unos(prije5Dana, 3);
    izostanakPrije5Dana.Opravdaj("Odsutan zbog bolesti - gripa");
    denis.DodajIzostanak(izostanakPrije5Dana);
    izostanakPrije5Dana.Unos(prije5Dana, 2);
    denis.DodajIzostanak(izostanakPrije5Dana);

    izostanakPrije10Dana.Unos(prije10Dana, 1);
    denis.DodajIzostanak(izostanakPrije10Dana);
    izostanakPrije10Dana.Unos(prije10Dana, 1);
    denis.DodajIzostanak(izostanakPrije10Dana);

    denis.Ispis();
    Izostanak * p = denis.BinarnaPretraga(jucer);
    if (p)
    {
        cout << "Binarnom pretragom pronadjen izostanak -> ";
        p->Ispis();
    }

    denis.Ispis();
    Izostanak * p2 = PronadjiNajveciNeopravdaniIzostanak(denis);
    if (p2)
    {
        cout << "Rekurzivno pronadjen najveci neopravdani izostanak -> ";
        p2->Ispis();
    }
    cout << endl;
    
    jucer.Dealociraj();
    prije5Dana.Dealociraj();
    prije10Dana.Dealociraj();
    
    denis.Dealociraj();
    system("pause>0");
}