#include <iostream>

using namespace std;

char * crt = "\n-------------------------------------------\n";
char * vrstaDok[] = { ".pdf", ".doc", ".txt", ".html" };

enum vrstaDokumenta { PDF, DOC, TXT, HTML };
enum prioritet { VISOK, SREDNJI, NIZAK };
struct DatumVrijeme
{
    int *_dan , *_mjesec, *_godina, *_sati, *_minuti;
    int DatumUMinute()
    {
        return *_godina * 365 * 24 * 60 + *_mjesec * 30 * 24 * 60 + *_dan * 24 * 60 + *_sati * 60 + *_minuti;
    }
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
};

struct Filter
{
    char * _sadrzaj;
    prioritet _prioritet;
    void Unos(char * sadrzaj, prioritet prioritet)
    {
        int vel = strlen(sadrzaj) + 1;
        _sadrzaj = new char[vel];
        strcpy_s(_sadrzaj, vel, sadrzaj);
        _prioritet = prioritet;
    }
    void Dealociraj() { delete[] _sadrzaj; _sadrzaj = nullptr; }
    void Ispis() { cout << _sadrzaj << " (" << _prioritet << ")" << endl; }

};

struct Dokument
{
    vrstaDokumenta _vrsta;
    char * _naziv = nullptr;
    char * _sadrzaj = nullptr;
    DatumVrijeme _kreiran;
    int _brojStranica;

    void DodajSadrzaj(char * sadrzaj);
    bool ProvjeriNaziv();
    void Unos(vrstaDokumenta vrsta, char * naziv, DatumVrijeme kreiran)
    {
        int vel = strlen(naziv) + 1;
        _naziv = new char[vel];
        strcpy_s(_naziv, vel, naziv);
        _vrsta = vrsta;
        _kreiran.Unos(*kreiran._dan, *kreiran._mjesec, *kreiran._godina, *kreiran._sati, *kreiran._minuti);
        _brojStranica = 0;//broj stranica se odredjuje prilikom dodavanja svakog novog sadrzaja dokumentu
    }
    void Dealociraj()
    {
        delete[] _naziv; _naziv = nullptr;
        delete[] _sadrzaj; _sadrzaj = nullptr;
        _kreiran.Dealociraj();
    }

    void Ispis()
    {
        cout << crt << _naziv << " " << _vrsta << " kreiran ";
        _kreiran.Ispis();
        cout << crt << _sadrzaj << crt << " br.stranica (" << _brojStranica << ")" << crt;
    }
};

struct Printer
{
    char * _proizvodjac;
    char * _model;
    Dokument * _dokumenti;
    int _trenutnoDokumenata;
    Filter * _filteri;
    int _trenutnoFiltera;

    bool DodajFilter(char * rijec, prioritet _prioritet);
    void Printaj(Dokument & dokument);
    bool ProvjeraDokumenta(Dokument & dokument);
    bool ProvjeriSadrzajDokumenta(Dokument dokument);
    Filter * GetNakoristenijiFilter();
    float GetProsjecanBrojStranicaPoDatumuIVrsti(DatumVrijeme d1, DatumVrijeme d2, vrstaDokumenta vrsta);
    void Ispis();
    void Unos(char * proizvodjac, char * model)
    {
        int vel = strlen(proizvodjac) + 1;
        _proizvodjac = new char[vel];
        strcpy_s(_proizvodjac, vel, proizvodjac);
        vel = strlen(model) + 1;
        _model = new char[vel];
        strcpy_s(_model, vel, model);
        _trenutnoDokumenata = 0;
        _trenutnoFiltera = 0;
        _filteri = nullptr;
        _dokumenti = nullptr;
    }
    void Dealociraj()
    {
        delete[] _proizvodjac; _proizvodjac = nullptr;
        delete[] _model; _model = nullptr;
        for (size_t i = 0; i < _trenutnoDokumenata; i++)
            _dokumenti[i].Dealociraj();
        delete[]_dokumenti; _dokumenti = nullptr;
        for (size_t i = 0; i < _trenutnoFiltera; i++)
            _filteri[i].Dealociraj();
        delete[]_filteri; _filteri = nullptr;
    }
};

void Dokument::DodajSadrzaj(char * sadrzaj)
{
    if (sadrzaj != nullptr)
    {
        int velicina = 0;
        if (_sadrzaj == nullptr)
        {
            velicina = strlen(sadrzaj) + 1;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, sadrzaj);
        }
        else
        {
            velicina = strlen(_sadrzaj) + strlen(sadrzaj) + 1;
            char * temp = _sadrzaj;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, temp);
            strcat_s(_sadrzaj, velicina, sadrzaj);
        }


        _brojStranica = velicina / 30;
    }
}

bool Printer::DodajFilter(char * rijec, prioritet _prioritet)
{
    for (int i = 0; i < _trenutnoFiltera; i++)
        if (strcmp(_filteri[i]._sadrzaj, rijec) == 0)
            return false;

    Filter * temp = _filteri;
    _filteri = new Filter[_trenutnoFiltera + 1];
    for (int i = 0; i < _trenutnoFiltera; i++)
        _filteri[i] = temp[i];

    _filteri[_trenutnoFiltera].Unos(rijec, _prioritet);
    _trenutnoFiltera++;
    return true;
}

bool Printer::ProvjeriSadrzajDokumenta(Dokument dokument)
{
    for (int i = 0; i < _trenutnoFiltera; i++)
        if(dokument._sadrzaj != nullptr)
            if (strstr(dokument._sadrzaj, _filteri[i]._sadrzaj) != 0 && _filteri[i]._prioritet != NIZAK)
                return false;
    return true;
}

bool Dokument::ProvjeriNaziv()
{
    int velicina = 5;
    char * nastavak = new char[velicina];
    int duzinaNaziva = strlen(_naziv) + 1;

    int k = 0;
    for (int j = duzinaNaziva - velicina; j < duzinaNaziva; j++)
    {
        nastavak[k] = _naziv[j];
        k++;   
    }

    bool flag = false;
    for (int i = 0; i < 4; i++)
    {
        
        if (strcmp(vrstaDok[i], nastavak) == 0)
        {
            k = 0;
            while (k < duzinaNaziva - velicina)
            {
                if (_naziv[k] < 'A' || (_naziv[k] > 'Z' && _naziv[k] < 'a') || _naziv[k] > 'z')
                    return false;

                k++;
            }

            flag = true;
            break;
        }
    }

    return flag;
}

bool Printer::ProvjeraDokumenta(Dokument & dokument)
{
    return dokument.ProvjeriNaziv() && ProvjeriSadrzajDokumenta(dokument) && dokument._sadrzaj != nullptr;
}

void Printer::Printaj(Dokument & dokument)
{
    Dokument * temp = _dokumenti;
    _dokumenti = new Dokument[_trenutnoDokumenata + 1];
    for (int i = 0; i < _trenutnoDokumenata; i++)
        _dokumenti[i] = temp[i];
    _dokumenti[_trenutnoDokumenata].Unos(dokument._vrsta, dokument._naziv, dokument._kreiran);
    _dokumenti[_trenutnoDokumenata].DodajSadrzaj(dokument._sadrzaj);
    _trenutnoDokumenata++;

    if (ProvjeraDokumenta(dokument))
    {
        int i = 0;
        cout << endl;
        while (dokument._sadrzaj[i] != '\0')
        {
            cout << dokument._sadrzaj[i];
            if (i != 0 && i % 30 == 0)
                cout << crt;
            i++;
        }
        cout << crt << endl;
    }
}

Filter * Printer::GetNakoristenijiFilter()
{
    int * brojac = new int[_trenutnoFiltera] {0};
    for (int i = 0; i < _trenutnoDokumenata; i++)
    {
        if (_dokumenti[i]._sadrzaj != nullptr)
        {
            for (int j = 0; j < _trenutnoFiltera; j++)
            {
                char * rezultat = _dokumenti[i]._sadrzaj;
                while ((rezultat = strstr(rezultat, _filteri[j]._sadrzaj)) != NULL)
                {
                    rezultat++;
                    brojac[j]++;
                }
            }
        }
    }

    int max = 0;
    for (int i = 1; i < _trenutnoFiltera; i++)
        if (brojac[max] < brojac[i])
            max = i;

    delete[] brojac; brojac = nullptr;
    return &_filteri[max];
}

float Printer::GetProsjecanBrojStranicaPoDatumuIVrsti(DatumVrijeme d1, DatumVrijeme d2, vrstaDokumenta vrsta)
{
    int suma = 0, brojac = 0;
    for (int i = 0; i < _trenutnoDokumenata; i++)
    {
        bool veciOdPrvog = d1.DatumUMinute() <= _dokumenti[i]._kreiran.DatumUMinute();
        bool manjiOdDrugog = d2.DatumUMinute() >= _dokumenti[i]._kreiran.DatumUMinute();
        bool vrstaJednaka = _dokumenti[i]._vrsta == vrsta;
        bool flag = ProvjeraDokumenta(_dokumenti[i]);
        if (veciOdPrvog && manjiOdDrugog && vrstaJednaka && flag)
        {
            suma += _dokumenti[i]._brojStranica;
            brojac++;
        }
    }

    if (brojac == 0) return 0;
    
    return suma / brojac / 1.F;
}

void Printer::Ispis()
{
    cout << crt << "\t\t" << _proizvodjac << _model << crt;
    cout << "Trenutno dokumenata: " << _trenutnoDokumenata << crt;
    for (int i = 0; i < _trenutnoDokumenata; i++)
    {
        if (_dokumenti[i]._sadrzaj != nullptr && ProvjeraDokumenta(_dokumenti[i]))
        {
            _dokumenti[i].Ispis();
        }
    }
}

void ZamijeniIPrintaj(Printer & printer, char znak)
{
    for (int i = 0; i < printer._trenutnoDokumenata; i++)
    {
        if (printer._dokumenti[i]._sadrzaj != nullptr)
        {
            for (int j = 0; j < printer._trenutnoFiltera; j++)
            {
                char * rezultat = printer._dokumenti[i]._sadrzaj;
                while ((rezultat = strstr(rezultat, printer._filteri[j]._sadrzaj)) != NULL)
                {
                    int k = rezultat - printer._dokumenti[i]._sadrzaj;
                    int duzina = k + strlen(printer._filteri[j]._sadrzaj);
                    while (k < duzina)
                        printer._dokumenti[i]._sadrzaj[k++] = znak;

                    rezultat += k;

                }
            }
        }
    }
}


void main()
{
    DatumVrijeme prije3Dana; prije3Dana.Unos(3, 2, 2017, 10, 15);
    DatumVrijeme danas1; danas1.Unos(6, 2, 2017, 10, 15);
    DatumVrijeme danas2; danas2.Unos(6, 2, 2017, 10, 16);
    DatumVrijeme za10Dana; za10Dana.Unos(16, 2, 2017, 10, 15);

    Dokument ispitPRII, ispitMAT, ispitUIT, ispitUITDrugiRok;
    ispitPRII.Unos(DOC, "ispitPRII.doc", prije3Dana);
    ispitMAT.Unos(DOC, "ispitMAT.doc", danas1);
    ispitUIT.Unos(DOC, "ispitUIT.doc", danas2);
    ispitUITDrugiRok.Unos(PDF, "ispitUITDrugiRok.pdf", za10Dana);

    
    ispitPRII.DodajSadrzaj("Programiranje ili racunarsko programiranje (engl. programming) jeste vjestina pomocu koje ");
    ispitPRII.DodajSadrzaj("korisnik stvara i izvrsava algoritme koristeci odredene programske jezike da bi ... ");
    cout << "Broj stranica -> " << ispitPRII._brojStranica << endl;

    ispitPRII.Ispis();
    
    ispitMAT.DodajSadrzaj("matematika se razvila iz potrebe da se obavljaju proracuni u trgovini, vrse mjerenja zemljista i predvidaju ");
    ispitMAT.DodajSadrzaj("astronomski dogadaji, i ove tri primjene se mogu dovesti u vezu sa grubom podjelom matematike na RAT i ");
    
    Printer hp3200; hp3200.Unos("HP", "3200");
    
    if (hp3200.DodajFilter("RE", NIZAK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("RAT", VISOK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("UBITI", VISOK))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("MRZITI", SREDNJI))
        cout << "Filter dodan!" << endl;
    if (hp3200.DodajFilter("RE", SREDNJI))
        cout << "Filter dodan!" << endl;

    
    hp3200.Printaj(ispitPRII);
    hp3200.Printaj(ispitMAT);
    hp3200.Printaj(ispitUIT);
    hp3200.Printaj(ispitUITDrugiRok);
    

    cout << "Prosjecan broj printanih stranica je -> " << hp3200.GetProsjecanBrojStranicaPoDatumuIVrsti(prije3Dana, danas2, DOC) << crt;
    
    Filter * f = hp3200.GetNakoristenijiFilter();
    cout << "Najcesce koristeni sadrzaj filtera je -> " << f->_sadrzaj << crt;
    hp3200.Ispis();
    /*MIJENJA ZNAKOVE ZABRANJENOG SADRŽAJA DOKUMENTA SA POSLANIM KARAKTEROM
    (BEZ OBZIRA NA PRIORITET) I NAKON TOGA PRINTA SADRŽAJ DOKUMENATA; npr.
    UBITI POSTAJE -----.
    RIJEŠITI REKURZIJOM*/
    ZamijeniIPrintaj(hp3200, '-'); //OSTALE PARAMETRE DODAJTE PO POTREBI*/
    
    prije3Dana.Dealociraj(); 
    danas1.Dealociraj(); 
    danas2.Dealociraj(); 
    za10Dana.Dealociraj();
    ispitMAT.Dealociraj(); 
    ispitPRII.Dealociraj(); 
    ispitUIT.Dealociraj(); 
    ispitUITDrugiRok.Dealociraj();
    
    hp3200.Dealociraj();

    system("pause>0");
}