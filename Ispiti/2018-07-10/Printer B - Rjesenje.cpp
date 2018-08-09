#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

const char *crt = "\n-------------------------------------------\n";

enum vrstaDokumenta { PDF, DOC, TXT, HTML };
const char * vrstaDokumentaChar[] = { "PDF", "DOC", "TXT", "HTML" };

enum vrstaAktivnosti { KREIRANJE, DODAVANJE, ZAMJENA, PRINTANJE };
const char * vrstaAktivnostiChar[] = { "KREIRANJE", "DODAVANJE", "ZAMJENA", "PRINTANJE" };

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

char * AlocirajNizKaraktera(const char * sadrzaj)
{
    int vel = strlen(sadrzaj) + 1;
    char * temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    return temp;
}

struct Modifikacija
{
    char * _korisnicnkoIme;
    vrstaAktivnosti _aktivnost;
    DatumVrijeme _vrijemeModifikacije;
    void Unos(const char * sadrzaj, vrstaAktivnosti aktivnost, DatumVrijeme vm)
    {
        _korisnicnkoIme = AlocirajNizKaraktera(sadrzaj);
        _aktivnost = aktivnost;
        _vrijemeModifikacije.Unos(*vm._dan, *vm._mjesec, *vm._godina, *vm._dan, *vm._minuti);
    }
    void Dealociraj()
    {
        delete[] _korisnicnkoIme; _korisnicnkoIme = nullptr; 
        _vrijemeModifikacije.Dealociraj();
    }
    void Ispis() { cout << _korisnicnkoIme << " (" << vrstaAktivnostiChar[_aktivnost] << ")"; _vrijemeModifikacije.Ispis(); }
};

Modifikacija *AlocirajNizModifikacija(Modifikacija *modifikacije, int broj)
{
    Modifikacija *temp = new Modifikacija[broj + 1];
    for (int i = 0; i < broj; i++) 
        temp[i].Unos(modifikacije[i]._korisnicnkoIme, modifikacije[i]._aktivnost, modifikacije[i]._vrijemeModifikacije);
    
    return temp;
}

struct Dokument
{
    unique_ptr<vrstaDokumenta> _vrsta;
    char * _naziv;
    char * _sadrzaj;
    Modifikacija * _modifikacije;
    int _brojModifikacija;

    void DodajSadrzaj(const char *sadrzaj, const char *korisnicnoIme, DatumVrijeme vrijme);
    void KopirajDokument(Dokument &d)
    {
        int duzina = strlen(d._naziv) + 1;
        _naziv = new char[duzina];
        strcpy_s(_naziv, duzina, d._naziv);

        if (d._sadrzaj)
        {
            duzina = strlen(d._sadrzaj) + 1;
            _sadrzaj = new char[duzina];
            strcpy_s(_sadrzaj, duzina, d._sadrzaj);
        }
        else
            _sadrzaj = nullptr;

        _vrsta = make_unique<vrstaDokumenta>(*d._vrsta);
        _modifikacije = AlocirajNizModifikacija(d._modifikacije, d._brojModifikacija);
        _brojModifikacija = d._brojModifikacija;
    }
    void Unos(vrstaDokumenta vrsta, const char * naziv, DatumVrijeme kreiran)
    {
        _naziv = AlocirajNizKaraktera(naziv);
        _vrsta = make_unique<vrstaDokumenta>(vrsta);
        _brojModifikacija = 0;
        _modifikacije = new Modifikacija[_brojModifikacija + 1];
        _modifikacije->Unos("", KREIRANJE, kreiran);
        _brojModifikacija++;
        _sadrzaj = nullptr;
    }
    void Dealociraj()
    {
        if(_naziv) { delete[] _naziv; _naziv = nullptr;}
        if(_sadrzaj) { delete[] _sadrzaj; _sadrzaj = nullptr; }
        for (size_t i = 0; i < _brojModifikacija; i++)
            _modifikacije[i].Dealociraj();
        delete[] _modifikacije;
        _modifikacije = nullptr;
    }
    void Ispis()
    {
        cout << crt << _naziv << " " << vrstaDokumentaChar[*_vrsta] << " kreiran ";
        if(_sadrzaj) cout << crt << _sadrzaj << crt;
        for (size_t i = 0; i < _brojModifikacija; i++)
            _modifikacije[i].Ispis();
    }
};


struct Printer
{
    char * _model;
    Dokument * _dokumenti;
    int _trenutnoDokumenata;
    pair<Modifikacija *, int> GetModifikacijeByDatum(DatumVrijeme datum1, DatumVrijeme datum2);
    float GetProsjecanBrojModifikacijaByVrsta(vrstaAktivnosti vrsta, int i = 0, int brojac = 0);
    bool IspravanNaziv(Dokument &dokument);
    bool ProvjeriModifikacije(Dokument &dokument);
    bool ProvjeriKorisnika(Dokument &dokument, const char *korisnickoIme);
    bool ProvjeriDokument(Dokument &dokument, const char *korisnickoIme);
    bool Printaj(Dokument &dokument, const char *korisnickoIme, DatumVrijeme datum);
    void Unos(const char * model)
    {
        _model = AlocirajNizKaraktera(model);
        _dokumenti = nullptr;
        _trenutnoDokumenata = 0;
    }
    void Dealociraj()
    {
        delete[] _model; _model = nullptr;
        for (size_t i = 0; i < _trenutnoDokumenata; i++)
            _dokumenti[i].Dealociraj();
        delete[]_dokumenti; _dokumenti = nullptr;
    }
    void Ispis()
    {
        cout << _model << crt;
        for (size_t i = 0; i < _trenutnoDokumenata; i++)
            for(int j = 0; j < _dokumenti[i]._brojModifikacija; j++)
                if (_dokumenti[i]._modifikacije[j]._aktivnost == PRINTANJE)
                    _dokumenti[i].Ispis();
    }
};

void Dokument::DodajSadrzaj(const char *sadrzaj, const char *korisnicnoIme, DatumVrijeme vrijme)
{
    if (sadrzaj)
    {
        if (_sadrzaj)
        {
            char *temp = _sadrzaj;
            int velicina = strlen(_sadrzaj) + strlen(sadrzaj) + 1;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, temp);
            strcat_s(_sadrzaj, velicina, sadrzaj);
            delete[] temp;
        }
        else
        {
            int velicina = strlen(sadrzaj) + 1;
            _sadrzaj = new char[velicina];
            strcpy_s(_sadrzaj, velicina, sadrzaj);
        }

        
        _modifikacije = AlocirajNizModifikacija(_modifikacije, _brojModifikacija);
        _modifikacije[_brojModifikacija].Unos(korisnicnoIme, DODAVANJE, vrijme);
        _brojModifikacija++;

    }
}

bool Printer::IspravanNaziv(Dokument &dokument)
{
    char *temp = dokument._naziv;
    while (*temp != '.')
    {
        if (!isalpha(*temp)) return false;
        temp++;
    }

    temp++;

    char *ekstenzija = new char[5];
    strcpy_s(ekstenzija, 5, vrstaDokumentaChar[*dokument._vrsta]);
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
bool Printer::ProvjeriModifikacije(Dokument &dokument)
{
    int brojac[4]{};
    for (int i = 0; i < dokument._brojModifikacija; i++)
        brojac[dokument._modifikacije[i]._aktivnost]++;

    return brojac[KREIRANJE] > 0 && brojac[DODAVANJE] > 0 && brojac[ZAMJENA] > 0;
}

bool Printer::ProvjeriKorisnika(Dokument &dokument, const char *korisnickoIme)
{
    for (int i = 0; i < dokument._brojModifikacija; i++)
        if (strcmp(dokument._modifikacije[i]._korisnicnkoIme, korisnickoIme) == 0)
            return true;
    return false;
}

bool Printer::ProvjeriDokument(Dokument &dokument, const char *korisnickoIme)
{
    return IspravanNaziv(dokument) && ProvjeriModifikacije(dokument) && ProvjeriKorisnika(dokument, korisnickoIme);
}

bool Printer::Printaj(Dokument &dokument, const char *korisnickoIme, DatumVrijeme datum)
{
    Dokument *temp = _dokumenti;
    _dokumenti = new Dokument[_trenutnoDokumenata + 1];
    for (int i = 0; i < _trenutnoDokumenata; i++)
        _dokumenti[i].KopirajDokument(temp[i]);
    _dokumenti[_trenutnoDokumenata].KopirajDokument(dokument);
    
    delete[] temp;

    if (ProvjeriDokument(dokument, korisnickoIme))
    {
        _dokumenti[_trenutnoDokumenata]._modifikacije = AlocirajNizModifikacija
        (
            _dokumenti[_trenutnoDokumenata]._modifikacije,
            _dokumenti[_trenutnoDokumenata]._brojModifikacija
        );
        _dokumenti[_trenutnoDokumenata]._modifikacije[_dokumenti[_trenutnoDokumenata]._brojModifikacija].Unos
        (
            korisnickoIme, 
            PRINTANJE, 
            datum
        );
        _dokumenti[_trenutnoDokumenata]._brojModifikacija++;
        _trenutnoDokumenata++;
        
        int duzina = strlen(dokument._sadrzaj);
        for (int i = 0; i < duzina; i++)
        {
            cout << dokument._sadrzaj[i];
            if (i != 0 && i % BROJ_ZNAKOVA_PO_STRANICI == 0) cout << crt;
        }

        return true;
    }

    _trenutnoDokumenata++;
    return false;
}

float Printer::GetProsjecanBrojModifikacijaByVrsta(vrstaAktivnosti vrsta, int i, int brojac)
{
    if (brojac == 0)
    {
        for (int j = 0; j < _trenutnoDokumenata; j++) brojac += _dokumenti[j]._brojModifikacija;
                    
        if (brojac == 0) return 0;
    }

    int brMod = 0;

    if (i == _trenutnoDokumenata - 1)
    {
        for (int j = 0; j < _dokumenti[i]._brojModifikacija; j++)
            if (_dokumenti[i]._modifikacije[j]._aktivnost == vrsta)
                brMod++;
        return brMod / 1.F / brojac;
    }

    for (int j = 0; j < _dokumenti[i]._brojModifikacija; j++)
        if (_dokumenti[i]._modifikacije[j]._aktivnost == vrsta)
            brMod++;
    return brMod / 1.F / brojac + GetProsjecanBrojModifikacijaByVrsta(vrsta, i + 1, brojac);
}

pair<Modifikacija *, int> Printer::GetModifikacijeByDatum(DatumVrijeme datum1, DatumVrijeme datum2)
{
    auto DatumIzmedu = [&](DatumVrijeme &D)
    {
        return datecmp(datum1, D) <= 0 && datecmp(datum2, D) >= 0;
    };

    int brojac = 0;
    Modifikacija *modifikacije = nullptr;
    for (int i = 0; i < _trenutnoDokumenata; i++)
        for (int j = 0; j < _dokumenti[i]._brojModifikacija; j++)
            if (DatumIzmedu(_dokumenti[i]._modifikacije[j]._vrijemeModifikacije))
            {
                modifikacije = AlocirajNizModifikacija(modifikacije, brojac);
                modifikacije[brojac] = _dokumenti[i]._modifikacije[j];
                brojac++;
            }

    return { modifikacije, brojac };
    
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

    ispitPRII.DodajSadrzaj("Programiranje ili racunarsko programiranje (engl. programming) jeste vjestina pomocu koje ", "denis.music", danas1);
    ispitPRII.DodajSadrzaj("korisnik stvara i izvrsava algoritme koristeci odredjene programske jezike da bi ... ", "indira.hamulic", danas2);

    ispitPRII.Ispis();

    ispitMAT.DodajSadrzaj("Matematika se razvila iz potrebe da se obavljaju proracuni u trgovini, vrse mjerenja zemljista i predvidjaju ", "jasmin.azemovic", danas1);
    ispitMAT.DodajSadrzaj("astronomski dogadjaji, i ove tri primjene se mogu dovesti u vezu sa grubom podjelom matematike ", "adel.handzic", danas2);

    Printer hp3200; hp3200.Unos("HP 3200");

    if (hp3200.Printaj(ispitPRII, "denis.music", za10Dana))
        cout << "Printam -> " << ispitPRII._naziv << endl;
    if (hp3200.Printaj(ispitMAT, "denis.music", za10Dana))
        cout << "Printam -> " << ispitMAT._naziv << endl;
    if (hp3200.Printaj(ispitUIT, "iris.memic", za10Dana))
        cout << "Printam -> " << ispitUIT._naziv << endl;
    if (hp3200.Printaj(ispitUITDrugiRok, "goran.skondric", za10Dana))
        cout << "Printam -> " << ispitUITDrugiRok._naziv << endl;

    cout << "Prosjecan broj modifikacija odrejdne vrste printanih dokumenata je -> " << hp3200.GetProsjecanBrojModifikacijaByVrsta(DODAVANJE) << crt;

    Modifikacija * pok = nullptr;
    int brojac = 0;
    tie(pok, brojac) = hp3200.GetModifikacijeByDatum(danas1, za10Dana);
    if (brojac > 0 && pok != nullptr)
        for (size_t i = 0; i < brojac; i++)
            pok[i].Ispis();
    delete[] pok;

    //ISPISUJE INFORMACIJE O SVIM DOKUMENTIMA KOJI SU ISPRINTANI
    hp3200.Ispis();

    
    auto Zamijeni = [&hp3200](const char x, const char y, const char *korisnickoIme, DatumVrijeme &datum)
    {
        int brojac = 0;
        for (int i = 0; i < hp3200._trenutnoDokumenata; i++)
        {
            for (int j = 0; j < hp3200._dokumenti[i]._brojModifikacija; j++)
            {
                bool isPrintan = hp3200._dokumenti[i]._modifikacije[j]._aktivnost == PRINTANJE;
                bool isDatumJednak = datecmp(hp3200._dokumenti[i]._modifikacije[j]._vrijemeModifikacije, datum) == 0;
                if (isPrintan && isDatumJednak)
                {
                    int duzina = strlen(hp3200._dokumenti[i]._sadrzaj);
                    for (int k = 0; k < duzina; k++)
                    {
                        if (hp3200._dokumenti[i]._sadrzaj[k] == x)
                        {
                            hp3200._dokumenti[i]._sadrzaj[k] = y;
                            brojac++;
                        }
                    }

                    hp3200._dokumenti[i]._modifikacije = AlocirajNizModifikacija
                    (
                        hp3200._dokumenti[i]._modifikacije, 
                        hp3200._dokumenti[i]._brojModifikacija
                    );
                    hp3200._dokumenti[i]._modifikacije[hp3200._dokumenti[i]._brojModifikacija].Unos(korisnickoIme, ZAMJENA, datum);
                    hp3200._dokumenti[i]._brojModifikacija++;
                }
            }
        }

        return brojac;

    };

    int zamijenjeno = Zamijeni(' ', '-', "denis.music", za10Dana);
    cout << "Zamijenjeno -> " << zamijenjeno << " znakova" << endl;
    hp3200.Ispis();

    prije3Dana.Dealociraj(); danas1.Dealociraj(); danas2.Dealociraj(); za10Dana.Dealociraj();
    ispitMAT.Dealociraj();
    ispitPRII.Dealociraj(); 
    ispitUIT.Dealociraj(); 
    ispitUITDrugiRok.Dealociraj();

    hp3200.Dealociraj();
    
    system("pause>0");
    return 0;
}