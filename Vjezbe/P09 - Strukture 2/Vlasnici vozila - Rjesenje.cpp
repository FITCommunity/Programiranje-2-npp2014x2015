#include <iostream>
#include <cstring>

using namespace std;

char crt[] = "\n======================================================\n";

enum VrstaVozila { PUTNICKO = 0, TERETNO };
struct Datum
{
    int _dan, _mjesec, _godina;
    void Unos(int dan, int mjesec, int godina) { _dan = dan; _mjesec = mjesec; _godina = godina; };
    void Prikazi() { cout << _dan << "/" << _mjesec << "/" << _godina; };
};

struct Vozilo
{
    char _regOznaka[10];
    char * _markaTip = nullptr;
    int _godinaProizvodnje;
    Datum _registrovanDO; //objekat druge strukture
    VrstaVozila _vrsta; //enumeracija
    void Unos();
    void Prikazi();
    void Dealociraj();
};

struct Vlasnik
{
    char * _imePrezime;
    char _JMBG[14];
    Datum _datumRodjenja;
    Vozilo * _vozila[10];
    int _brojacVozila;
    void Unos();
    void Prikazi();
    void DodajVozilo();
    void UkloniVozilo();
    Vozilo * GetVozilo(char * regOznaka);
    void Dealociraj();
};

// ------------------------------------ Vozilo ------------------------------------ //

void Vozilo::Prikazi()
{
    char * vrstaVozila[] = { "PUTNICKO", "TERETNO" };
    int i = int(_vrsta);

    cout << crt << "\t\t::ISPIS PODATAKA VOZILA::";
    cout << crt << "Registarska oznaka: \t\t" << _regOznaka;
    cout << "\nMarka i tip vozila: \t\t" << _markaTip;
    cout << "\nGodina proizvodnje: \t\t" << _godinaProizvodnje;
    cout << "\nRegistrovan do: \t\t";  _registrovanDO.Prikazi();
    cout << "\nVrsta vozila: \t\t\t" << vrstaVozila[i] << crt;
}

void Vozilo::Unos()
{
    cout << crt << "\t\t::UNOS PODATAKA::" << crt;

    cout << "Registarska oznaka vozila: ";
    cin.getline(_regOznaka, 10);

    char temp[30];
    cout << "Marka i tip vozila: ";
    cin.getline(temp, 30);
    int velicina = strlen(temp) + 1;
    _markaTip = new char[velicina];
    strcpy_s(_markaTip, velicina, temp);

    cout << "Godina proizvodnje: ";
    cin >> _godinaProizvodnje;

    int d, m, g;
    cout << "Datum do kada je vozilo registrovano: ";
    cin >> d >> m >> g;
    _registrovanDO.Unos(d, m, g);
    int izbor = 0;
    while (cout << "Vrsta vozila: 0 za putnicko, 1 za teretno: ", cin >> izbor, izbor < 0 && izbor > 1);
    _vrsta = (VrstaVozila)izbor;

    cin.ignore();

}

void Vozilo::Dealociraj()
{
    delete[] _markaTip;
    _markaTip = nullptr;
}

// ------------------------------------ Vozilo ------------------------------------ //
// ----------------------------------- Vlasnik ------------------------------------ //

void Vlasnik::Unos()
{
    cout << crt << "\t\t::UNOS PODATAKA::" << crt;

    char temp[40];
    cout << "Ime i prezime vlasnika: ";
    cin.getline(temp, 40);
    int velicina = strlen(temp) + 1;
    _imePrezime = new char[velicina];
    strcpy_s(_imePrezime, velicina, temp);

    cout << "JMBG vlasnika: ";
    cin.getline(_JMBG, 14);

    int d, m, g;
    cout << "Datum rodenja vlasnika:";
    cin >> d >> m >> g;
    _datumRodjenja.Unos(d, m, g);

    for (int i = 0; i < 10; i++)
        _vozila[i] = nullptr;
    _brojacVozila = 0;

    int izbor;
    do
    {
        cout << "1. Unesite vozilo za korisnika" << endl;
        cout << "2. Uklonite vozilo za korisnika" << endl;
        cout << "3. Izlaz" << endl;
        cin >> izbor;
        cin.ignore();


        if (izbor == 1)
        {
            if (_brojacVozila == 10)
                cout << "Vlasnik ima maksimalan broj dozvoljenih vozila" << endl;
            else
                DodajVozilo();
        }
        if (izbor == 2)
        {
            if (_brojacVozila == 0)
                cout << "Vlasnik nema vozila" << endl;
            else
                UkloniVozilo();
        }


    } while (izbor != 3);

}

void Vlasnik::Prikazi()
{
    cout << crt << "\t\t::PRIKAZ PODATAKA::" << crt;
    cout << "\nIme i prezime: " << _imePrezime;
    cout << "\nJMBG: " << _JMBG;
    cout << "\nDatum rodenja: "; _datumRodjenja.Prikazi();
    cout << "\nVozila: " << _brojacVozila << endl;
    for (int i = 0; i < _brojacVozila; i++)
        _vozila[i]->Prikazi();

}

void Vlasnik::Dealociraj()
{
    delete[] _imePrezime; _imePrezime = nullptr;
    for (int i = 0; i < _brojacVozila; i++)
    {
        delete _vozila[i];
        _vozila[i] = nullptr;
    }
}

void Vlasnik::DodajVozilo()
{
    _vozila[_brojacVozila] = new Vozilo;
    _vozila[_brojacVozila]->Unos();

    if (GetVozilo(_vozila[_brojacVozila]->_regOznaka) != nullptr)
    {
        cout << "Vozilo sa tom registracijom vec postoji" << endl;
        delete _vozila[_brojacVozila]; _vozila[_brojacVozila] = nullptr;
        return;
    }
    else
    {
        _brojacVozila++;
    }

}

void Vlasnik::UkloniVozilo()
{
    for (int i = 0; i < _brojacVozila; i++)
        _vozila[i]->Prikazi();
    cout << endl;
    
    char regOznaka[10];
    cout << "Unesite registarsku oznaku vozila: ";
    cin.getline(regOznaka, 10);

    Vozilo * pok = GetVozilo(regOznaka);
    if (pok == nullptr)
    {
        cout << "Vozilo sa tom registracijom ne postoji" << endl;
        return;
    }
    else
    {
        for (int i = 0; i < _brojacVozila; i++)
        {
            if (strcmp(_vozila[i]->_regOznaka, regOznaka) == 0)
            {
                while (i < _brojacVozila)
                {
                    _vozila[i] = _vozila[i + 1];
                    i++;
                }

                _brojacVozila--;
            }
        }
    }
}

Vozilo * Vlasnik::GetVozilo(char * regOznaka)
{
    for (int i = 0; i < _brojacVozila; i++)
        if (_vozila[i]->_regOznaka != nullptr)
            if (strcmp(_vozila[i]->_regOznaka, regOznaka) == 0)
                return _vozila[i];

    return nullptr;
}

// ----------------------------------- Vlasnik ------------------------------------ //

int prikaziMeni()
{
    int izbor = 1;
    do
    {
        cout << crt << "\t\t::MENI::" << crt;
        cout << "1.Unos novih podataka o vlasniku." << endl;
        cout << "2.Prikazi listu unesenih podataka." << endl;
        cout << "3.Pretraga po registarskoj oznaci." << endl;
        cout << "4.Pretraga po vlasniku vozila." << endl;
        cout << "5.Pretraga po JMBG vlasnika vozila." << endl;
        cout << "6.Pretraga po vrsti vozila." << endl;
        cout << "7.Pretraga po datumu vazenja registracije." << endl;
        cout << "8.Napravi izmjene na podacima." << endl;
        cout << "9.Kraj rada." << crt;
        cout << "Unesite vas izbor: ";
        cin >> izbor;
        cin.ignore();
        system("cls");
    } while (izbor < 1 || izbor > 9);
    return izbor;
}

void unosNovihPodataka(Vlasnik * niz, int &uneseno, int max)
{
    if (uneseno < max) //ako je broj unesenih podataka manji od max
    {
        niz[uneseno].Unos();
        uneseno++;
    }
    else
        cout << crt << "Nije moguce upisati nove podatke o vozilu!" << crt;
}

//funkcija koja ispisuje listu svih unesenih podataka 
int prikaziListuPodataka(Vlasnik * niz, int uneseno)
{
    int izbor = 0;
    do
    {
        cout << crt << "RB.\tIme i prezime\tJMBG\t\tBroj Vozila" << crt;
        for (int i = 0; i < uneseno; i++)
            cout << i + 1 << ".\t" << niz[i]._imePrezime << "\t" << niz[i]._JMBG << "\t" << niz[i]._brojacVozila << endl;

        cout << crt << "Unesite RB vlasnika(0-Izlaz):";
        cin >> izbor;
        cin.ignore();
        if (izbor == 0) return 0;

    } while (izbor < 1 || izbor > uneseno);
    return izbor;
}
void prikaziPodatkeDetaljno(Vlasnik * niz, int uneseno)
{
    int izbor = prikaziListuPodataka(niz, uneseno);
    if (izbor != 0) niz[izbor - 1].Prikazi();
}

// ----------------------------------- Pretraga ------------------------------------ //
void pretragaPoRegOznaci(Vlasnik * niz, int uneseno)
{
    char temp[10];
    bool pronadjeno = false;
    cout << crt << "Unesite reg.oznaku koju trazite: ";
    cin.getline(temp, 10);
    for (int i = 0; i < uneseno; i++)
    {
        for (int j = 0; j < niz[i]._brojacVozila; j++)
        {
            if (strcmp(niz[i]._vozila[j]->_regOznaka, temp) == 0)
            {
                niz[i]._vozila[j]->Prikazi();
                pronadjeno = true;
            }
        }
    }

    if (pronadjeno == false)
        cout << crt << "Trazeni podaci ne postoje u bazi!" << crt;
}

void pretragaPoVlasniku(Vlasnik * niz, int uneseno)
{
    char temp[40];
    bool pronadjen = false;
    cout << crt << "Unesite ime i prezime vlasnika: ";
    cin.getline(temp, 40);
    for (int i = 0; i<uneseno; i++)
    {
        if (strstr(niz[i]._imePrezime, temp) != 0)
        {
            niz[i].Prikazi();
            pronadjen = true;
        }
    }
    if (pronadjen == false)
        cout << crt << "Trazeni podaci ne postoje u bazi!" << crt;
}

void pretragaPoJMBGVlasnika(Vlasnik * niz, int uneseno)
{
    char temp[14];
    bool pronadjen = false;
    cout << crt << "Unesite JMBG vlasnika: ";
    cin.getline(temp, 14);
    for (int i = 0; i < uneseno; i++)
    {
        if (niz[i]._JMBG != nullptr)
        {
            if (strcmp(niz[i]._JMBG, temp) == 0)
            {
                niz[i].Prikazi();
                pronadjen = true;
            }
        }
    }
    if (pronadjen == false)
        cout << crt << "Trazeni podaci ne postoje u bazi!" << crt;
}
void pretragaPoDatumuRegistracije(Vlasnik * niz, int uneseno)
{
    int d, m, g;
    bool pronadjen = false;
    cout << crt << "Unesite datum isteka registracije: ";
    cin >> d >> m >> g;
    for (int i = 0; i < uneseno; i++)
    {
        for (int j = 0; j < niz[i]._brojacVozila; j++)
        {
            if (d == niz[i]._vozila[j]->_registrovanDO._dan &&
                m == niz[i]._vozila[j]->_registrovanDO._mjesec &&
                g == niz[i]._vozila[j]->_registrovanDO._godina)
            {
                niz[i]._vozila[j]->Prikazi();
                pronadjen = true;
            }

        }
    }
    if (pronadjen == false)
        cout << crt << "Trazeni podaci ne postoje u bazi!" << crt;
}
void pretragaPoVrstiVozila(Vlasnik * niz, int uneseno)
{
    int izbor;
    bool pronadjen = false;
    cout << crt << "Unesite vrstu vozila\n 0 - Putnicko 1 - Transportno: ";
    cin >> izbor;
    for (int i = 0; i < uneseno; i++)
    {
        for (int j = 0; j < niz[i]._brojacVozila; j++)
        {
            if (niz[i]._vozila[j]->_vrsta == (VrstaVozila)izbor)
            {
                niz[i]._vozila[j]->Prikazi();
                pronadjen = true;
            }

        }
    }
    if (pronadjen == false)
        cout << crt << "Trazeni podaci ne postoje u bazi!" << crt;
}

// ----------------------------------- Pretraga ------------------------------------ //

void izmjeneNaPodacima(Vlasnik * niz, int uneseno)
{
    int izbor = prikaziListuPodataka(niz, uneseno);
    if (izbor != 0) niz[izbor - 1].Unos();
}

void Alokacija(Vlasnik *& niz, int & max)
{
    do
    {
        cout << "Za koliko vlasnika unosite podatke: ";
        cin >> max;
        cin.ignore();
    } while (max <= 0);
    niz = new Vlasnik[max];
}

void Dealokacija(Vlasnik * niz, int uneseno)
{
    for (int i = 0; i < uneseno; i++)
        niz[i].Dealociraj();
    delete[] niz; niz = nullptr;
}
void main()
{
    int max = 0;
    int izbor = 0;
    int uneseno = 0;

    Vlasnik * niz = nullptr;
    Alokacija(niz, max);

    system("cls");
    do
    {
        cout << crt << "\t\t::EVIDENCIJA VLASNIKA::" << crt;
        izbor = prikaziMeni();
        switch (izbor)
        {
        case 1:
            unosNovihPodataka(niz, uneseno, max); break;
        case 2:
            prikaziPodatkeDetaljno(niz, uneseno); break;
        case 3:
            pretragaPoRegOznaci(niz, uneseno); break;
        case 4:
            pretragaPoVlasniku(niz, uneseno); break;
        case 5:
            pretragaPoJMBGVlasnika(niz, uneseno); break;
        case 6:
            pretragaPoVrstiVozila(niz, uneseno); break;
        case 7:
            pretragaPoDatumuRegistracije(niz, uneseno); break;
        case 8:
            izmjeneNaPodacima(niz, uneseno); break;
        }
    } while (izbor < 9);
    Dealokacija(niz, uneseno);
    system("pause");
}