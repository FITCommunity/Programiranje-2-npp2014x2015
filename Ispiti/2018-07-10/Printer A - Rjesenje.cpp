#include <iostream>
#include <memory>

using namespace std;

//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

const char *crt = "\n-------------------------------------------\n";

enum vrstaDokumenta { PDF, DOC, TXT, HTML };
const char * vrstaDokumentaChar[] = { "PDF", "DOC", "TXT", "HTML" };

enum Prioritet { VISOK, SREDNJI, NIZAK };
const char * PrioritetChar[] = { "VISOK", "SREDNJI", "NIZAK" };

char * AlocirajNizKaraktera(const char * sadrzaj)
{
    int vel = strlen(sadrzaj) + 1;
    char * temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    return temp;
}

const int BROJ_ZNAKOVA_PO_STRANICI = 30;

struct DatumVrijeme
{
    int *_dan, *_mjesec, *_godina, *_sati, *_minuti;
    void Unos(int dan = 1, int mjesec = 1, int godina = 2000, int sati = 1, int minuti = 1)
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
};

int datecmp(const DatumVrijeme &d1, const DatumVrijeme &d2)
{
    //poredenje sati i minuta najmjerno izostavljeni
    if (*d1._godina != *d2._godina) return *d1._godina - *d2._godina;
    else if (*d1._mjesec != *d2._mjesec) return *d1._mjesec - *d2._mjesec;
    else if (*d1._dan != *d2._dan) return *d1._dan - *d2._dan;
    //else if (*d1._sati != *d2._sati) return *d1._sati - *d2._sati;
    //else if (*d1._minuti != *d2._minuti) return *d1._minuti - *d2._minuti;

    return 0;
}

struct Filter
{
    char * _sadrzaj;
    Prioritet _prioritet;
    void Unos(const char * sadrzaj, Prioritet prioritet)
    {
        _sadrzaj = AlocirajNizKaraktera(sadrzaj);
        _prioritet = prioritet;
    }
    void Dealociraj() { delete[] _sadrzaj; _sadrzaj = nullptr; }
    void Ispis() { cout << _sadrzaj << " (" << PrioritetChar[_prioritet] << ")" << endl; }
};

struct Dokument
{
    vrstaDokumenta _vrsta;
    char * _naziv;
    char * _sadrzaj;
    unique_ptr<DatumVrijeme> _kreiran;
    int _brojStranica;

    void DodajSadrzaj(const char * sadrzaj);
    void Unos(vrstaDokumenta vrsta, const char * naziv, DatumVrijeme kreiran)
    {
        _naziv = AlocirajNizKaraktera(naziv);
        _vrsta = vrsta;
        _kreiran = make_unique<DatumVrijeme>();
        _kreiran->Unos(*kreiran._dan, *kreiran._mjesec, *kreiran._godina, *kreiran._sati, *kreiran._minuti);
        _brojStranica = 0;
        _sadrzaj = nullptr;
    }
    void Dealociraj()
    {
        delete[] _naziv; _naziv = nullptr;
        delete[] _sadrzaj; _sadrzaj = nullptr;
        _kreiran->Dealociraj();
    }
    void Ispis()
    {
        cout << crt << _naziv << " " << vrstaDokumentaChar[_vrsta] << " kreiran ";
        _kreiran->Ispis();
        if (_sadrzaj) cout << crt << _sadrzaj << crt << " br.stranica (" << _brojStranica << ")" << crt;
    }
};
struct Printer
{
    char * _model;
    Dokument * _dokumenti;
    int _trenutnoDokumenata;
    Filter * _filteri;
    int _trenutnoFiltera;

    Filter *GetNakoristenijiFilter();
    float GetProsjecanBrojStranicaUPeriodu_Od_Do(DatumVrijeme OD, DatumVrijeme DO, int i = 0, int brojac = 0);
    bool ProvjeriDokument(const Dokument &dokument);
    bool IspravanNaziv(const Dokument &dokument);
    bool ProvjeriSadrzaj(const Dokument &dokument);
    bool DodajFilter(const char * rijec, Prioritet prioritet);
    bool Printaj(const Dokument &dokument);
    void Unos(const char * model)
    {
        _model = AlocirajNizKaraktera(model);
        _filteri = nullptr;
        _dokumenti = nullptr;
        _trenutnoDokumenata = 0;
        _trenutnoFiltera = 0;
    }
    void Dealociraj()
    {
        delete[] _model; _model = nullptr;
        for (size_t i = 0; i < _trenutnoDokumenata; i++)
            _dokumenti[i].Dealociraj();
        delete[]_dokumenti; _dokumenti = nullptr;
        for (size_t i = 0; i < _trenutnoFiltera; i++)
            _filteri[i].Dealociraj();
        delete[]_filteri; _filteri = nullptr;
    }
    void Ispis()
    {
        cout << _model << crt;
        for (size_t i = 0; i < _trenutnoDokumenata; i++)
            if (ProvjeriDokument(_dokumenti[i]))
                _dokumenti[i].Ispis();
    }
};

void Dokument::DodajSadrzaj(const char * sadrzaj)
{
    if (sadrzaj)
    {
        int velicina = 0;

        if (_sadrzaj)
        {
            velicina = strlen(_sadrzaj) + strlen(sadrzaj) + 1;
            char * temp = _sadrzaj;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, temp);
            strcat_s(_sadrzaj, velicina, sadrzaj);
            delete[] temp;
        }
        else
        {
            velicina = strlen(sadrzaj) + 1;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, sadrzaj);
            //Alternativno
            //_sadrzaj = AlocirajNizKaraktera(sadrzaj);
            //velicina = strlen(sadrzaj) + 1; ili velicina = strlen(_sadrzaj) + 1;
        }

        _brojStranica = velicina / BROJ_ZNAKOVA_PO_STRANICI + bool(velicina % BROJ_ZNAKOVA_PO_STRANICI);
    }
}

bool Printer::DodajFilter(const char * rijec, Prioritet prioritet)
{
    for (int i = 0; i < _trenutnoFiltera; i++)
        if (strcmp(_filteri[i]._sadrzaj, rijec) == 0)
            return true;

    Filter *temp = _filteri;
    _filteri = new Filter[_trenutnoFiltera + 1];
    for (int i = 0; i < _trenutnoFiltera; i++)
        _filteri[i] = temp[i];

    _filteri[_trenutnoFiltera].Unos(rijec, prioritet);
    _trenutnoFiltera++;
    delete[] temp;

    return true;
}

bool Printer::ProvjeriDokument(const Dokument &dokument)
{
    return IspravanNaziv(dokument) && ProvjeriSadrzaj(dokument);
}

bool Printer::IspravanNaziv(const Dokument &dokument)
{
    char *temp = dokument._naziv;
    while (*temp != '.')
    {
        if (*temp < 'A' || (*temp > 'Z' && *temp < 'a') || *temp > 'z')
            return false;
        temp++;
    }

    temp++;

    char *ekstenzija = new char[5];
    strcpy_s(ekstenzija, 5, vrstaDokumentaChar[dokument._vrsta]);
    int i = 0;
    while (ekstenzija[i] != '\0')
    {
        ekstenzija[i] = tolower(ekstenzija[i]);
        i++;
    }

    bool flag = strcmp(temp, ekstenzija) == 0;
    delete[] ekstenzija;
    return flag;
}

bool Printer::ProvjeriSadrzaj(const Dokument &dokument)
{
    if (!dokument._sadrzaj) return false;

    int brojac[3]{};
    for (int i = 0; i < _trenutnoFiltera; i++)
    {
        char *temp = dokument._sadrzaj;
        while (temp = strstr(temp, _filteri[i]._sadrzaj))
        {
            brojac[_filteri[i]._prioritet]++;
            temp++;
        }

        if (brojac[VISOK] > 0 || brojac[SREDNJI] > 2 || brojac[NIZAK] > 3) return false;
    }

    return true;
}

bool Printer::Printaj(const Dokument &dokument)
{
    Dokument *temp = _dokumenti;
    _dokumenti = new Dokument[_trenutnoDokumenata + 1];
    for (int i = 0; i < _trenutnoDokumenata; i++)
    {
        _dokumenti[i].Unos(temp[i]._vrsta, temp[i]._naziv, *temp[i]._kreiran);
        _dokumenti[i].DodajSadrzaj(temp[i]._sadrzaj);
    }
    _dokumenti[_trenutnoDokumenata].Unos(dokument._vrsta, dokument._naziv, *dokument._kreiran);
    _dokumenti[_trenutnoDokumenata].DodajSadrzaj(dokument._sadrzaj);
    _trenutnoDokumenata++;
    delete[] temp;

    if (ProvjeriDokument(dokument))
    {
        int duzina = strlen(dokument._sadrzaj);
        for (int i = 0; i < duzina; i++)
        {
            cout << dokument._sadrzaj[i];
            if (i != 0 && i % BROJ_ZNAKOVA_PO_STRANICI == 0) cout << crt;
        }
        cout << endl << endl;

        return true;
    }
    else return false;
}


float Printer::GetProsjecanBrojStranicaUPeriodu_Od_Do(DatumVrijeme OD, DatumVrijeme DO, int i, int brojac)
{
    auto Provjera = [&](const Dokument &dokument)
    {
        return datecmp(OD, *dokument._kreiran) <= 0 && datecmp(DO, *dokument._kreiran) >= 0;
    };

    if (brojac == 0)
    {
        brojac = 0;
        for (int j = 0; j < _trenutnoDokumenata; j++)
            if (Provjera(_dokumenti[j]))
                brojac++;

        if (brojac == 0) return 0;
    }


    if (i == _trenutnoDokumenata - 1)
    {
        if (Provjera(_dokumenti[i]))
            return _dokumenti[i]._brojStranica / 1.F / brojac;
        else
            return 0;
    }

    if (Provjera(_dokumenti[i]))
        return _dokumenti[i]._brojStranica / 1.F / brojac + GetProsjecanBrojStranicaUPeriodu_Od_Do(OD, DO, i + 1, brojac);
    else
        return GetProsjecanBrojStranicaUPeriodu_Od_Do(OD, DO, i + 1, brojac);
}

Filter *Printer::GetNakoristenijiFilter()
{
    int *brojac = new int[_trenutnoFiltera] {0};
    for (int i = 0; i < _trenutnoDokumenata; i++)
    {
        for (int j = 0; j < _trenutnoFiltera; j++)
        {
            if (_dokumenti[i]._sadrzaj)
            {
                char *temp = _dokumenti[i]._sadrzaj;
                while (temp = strstr(temp, _filteri[j]._sadrzaj))
                {
                    brojac[j]++;
                    temp++;
                }
            }
        }
    }

    int max = 0;
    for (int i = 1; i < _trenutnoFiltera; i++)
        if (brojac[max] < brojac[i])
            max = i;

    return brojac[max] ? &_filteri[max] : nullptr;
}

int main()
{
    DatumVrijeme prije3Dana; prije3Dana.Unos(3, 2, 2018, 10, 15);
    DatumVrijeme danas1; danas1.Unos(6, 2, 2018, 10, 15);
    DatumVrijeme danas2; danas2.Unos(6, 2, 2018, 10, 16);
    DatumVrijeme za10Dana; za10Dana.Unos(16, 2, 2018, 10, 15);

    Dokument ispitPRII, ispitMAT, ispitUIT, ispitUITDrugiRok;
    ispitPRII.Unos(DOC, "ispitPRII.doc", prije3Dana);
    ispitMAT.Unos(DOC, "ispitMAT.doc", danas1);
    ispitUIT.Unos(DOC, "ispitUIT.doc", danas2);
    ispitUITDrugiRok.Unos(PDF, "ispitUITDrugiRok.pdf", za10Dana);


    ispitPRII.DodajSadrzaj("Programiranje ili racunarsko programiranje (engl. programming) jeste vjestina pomocu koje ");
    ispitPRII.DodajSadrzaj("korisnik stvara i izvrsava algoritme koristeci odredjene programske jezike da bi ... ");
    cout << "Broj stranica -> " << ispitPRII._brojStranica << endl;

    ispitPRII.Ispis();

    ispitMAT.DodajSadrzaj("Matematika se razvila iz potrebe da se obavljaju proracuni u trgovini, vrse mjerenja zemljista i predvidjaju ");
    ispitMAT.DodajSadrzaj("astronomski dogadjaji, i ove tri primjene se mogu dovesti u vezu sa grubom podjelom matematike ");



    Printer hp3200; hp3200.Unos("HP 3200");
    if (hp3200.DodajFilter("RE", NIZAK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("RAT", VISOK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("UBITI", VISOK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("MRZITI", SREDNJI))
        cout << "Filter dodan!" << endl;
    if (!hp3200.DodajFilter("RE", SREDNJI))
        cout << "Filter nije dodan!" << endl;


    if (hp3200.Printaj(ispitPRII))
        cout << "Printam -> " << ispitPRII._naziv << endl;
    if (hp3200.Printaj(ispitMAT))
        cout << "Printam -> " << ispitMAT._naziv << endl;
    if (hp3200.Printaj(ispitUIT))
        cout << "Printam -> " << ispitUIT._naziv << endl;
    if (hp3200.Printaj(ispitUITDrugiRok))
        cout << "Printam -> " << ispitUITDrugiRok._naziv << endl;

    cout << "Prosjecan broj printanih stranica je -> " << hp3200.GetProsjecanBrojStranicaUPeriodu_Od_Do(prije3Dana, danas2) << crt;
    Filter * f = hp3200.GetNakoristenijiFilter();
    if (f != nullptr && f->_sadrzaj != nullptr)
        cout << "Najcesce koristeni sadrzaj filtera je -> " << f->_sadrzaj << crt;

    hp3200.Ispis();

    auto ZamijeniIPrintaj = [&hp3200](const char x, const char y, DatumVrijeme D)
    {
        int brojac = 0;
        for (int i = 0; i < hp3200._trenutnoDokumenata; i++)
        {
            if (hp3200.ProvjeriDokument(hp3200._dokumenti[i]) && datecmp(*hp3200._dokumenti[i]._kreiran, D) == 0)
            {
                int duzina = strlen(hp3200._dokumenti[i]._sadrzaj);
                for (int j = 0; j < duzina; j++)
                {
                    if (hp3200._dokumenti[i]._sadrzaj[j] == x)
                    {
                        hp3200._dokumenti[i]._sadrzaj[j] = y;
                        brojac++;
                    }
                }
            }
        }

        return brojac;
    };

    int zamijenjeno = ZamijeniIPrintaj(' ', '-', danas1);
    cout << "Zamijenjeno -> " << zamijenjeno << " znakova" << endl;
    hp3200.Ispis();

    prije3Dana.Dealociraj(); danas1.Dealociraj(); danas2.Dealociraj(); za10Dana.Dealociraj();
    ispitMAT.Dealociraj(); ispitPRII.Dealociraj(); ispitUIT.Dealociraj(); ispitUITDrugiRok.Dealociraj();

    hp3200.Dealociraj();

    system("pause>0");
    return 0;
}