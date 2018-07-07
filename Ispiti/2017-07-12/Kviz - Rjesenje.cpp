#include <iostream>

using namespace std;

char * crt = "\n-------------------------------\n";

struct Kandidat
{
    char * _imePrezime;
    char _JMBG[14];
    void Unos(char * ip, char * jmbg)
    {
        _imePrezime = new char[strlen(ip) + 1];
        strcpy_s(_imePrezime, strlen(ip) + 1, ip);
        strcpy_s(_JMBG, jmbg);
    }
    void Dealociraj() { delete[] _imePrezime; _imePrezime = nullptr; }
    void Ispis() { cout << _imePrezime << " " << _JMBG << " "; }
};

struct Pitanje
{
    char * _tekstPitanja;
    char * _odgovori[10];
    int _tacni[10];
    int _bodova;
    bool AddOdgovor(const char * odgovor, bool tacan);
    bool RemoveOdgovor(int indeks);
    void Ispis();
    int BrojOdgovora();
    int BrojTacnihOdgovora();
    void Unos(char * tekstPitanja, int bodova)
    {
        _tekstPitanja = new char[strlen(tekstPitanja) + 1];
        strcpy_s(_tekstPitanja, strlen(tekstPitanja) + 1, tekstPitanja);
        for (size_t i = 0; i < 10; i++)
        {
            _odgovori[i] = nullptr;
            _tacni[i] = -1;
        }
        _bodova = bodova;
    }
    void Dealociraj()
    {

        delete[] _tekstPitanja; _tekstPitanja = nullptr;
        for (size_t i = 0; i < 10; i++)
        {
            if (_odgovori[i] == nullptr)
                break;
            delete[] _odgovori[i]; _odgovori[i] = nullptr;
        }
    }
};
struct PrijemniIspit
{
    //kandidati prijavljeni za prijemni ispit 
    Kandidat * _prijavljeniKandidati[100];
    /*uspjeh ostvaren na prijemnom ispitu za svakog pojedinog kandidata(prijemnom
    ispitu moze pristupiti maksimalno 100 kandidata i za svakog od njih se cuva ostvareni
    uspjeh u okviru niza _uspjehKandidata (nakon sto urade ispit tj. nakon sto odgovore na pitanja)*/
    float * _uspjehKandidata[100];
    //prag prolaznosti na prijemnom ispitu npr. 60 procenata
    float _pragProlaznosti;
    float _kazneniBodovi;
    /*vrijednost kazenog boda (npr. 0.30) se mnozi sa svakim netacnim odgovorom
    na pitanje i oduzima od osvojenih bodova za to pitanje (npr. ako pitanje nosi 5
    bodova i ima 2 tacna odgovora, a student odabere 1 tacan i 2 netacna odgovora,
    broj osvojenih bodova ce biti 1.9 (1 tacan = 2.5; 2 netacna * 0.30 kaznenih = 0.6; 2.5 - 0.6 = 1.9)*/
    bool PokreniIspit(const char * JMBG, Pitanje * pitanja, int brojPitanja);
    bool DodajKandidata(Kandidat kandidat);
    void Unos(float pragProlaznosti, float kazneniBodovi)
    {
        _pragProlaznosti = pragProlaznosti;
        _kazneniBodovi = kazneniBodovi;
        for (size_t i = 0; i < 100; i++)
        {
            _prijavljeniKandidati[i] = nullptr;
            _uspjehKandidata[i] = nullptr;
        }
    }

    void Ispis()
    {
        cout << crt << "Uspje kandidata: " << crt;
        for (size_t i = 0; i < 100; i++)
        {
            if (_prijavljeniKandidati[i] != nullptr)
            {
                if (_uspjehKandidata[i] != nullptr)
                {
                    _prijavljeniKandidati[i]->Ispis();
                    cout << "Uspjeh: " << *_uspjehKandidata[i] << crt;
                }
            }
            else break;
            
        }
    }

    void Dealociraj()
    {
        for (size_t i = 0; i < 100; i++)
        {
            if (_prijavljeniKandidati[i] != nullptr)
            {
                _prijavljeniKandidati[i]->Dealociraj();
                delete _prijavljeniKandidati[i]; _prijavljeniKandidati[i] = nullptr;
                if (_uspjehKandidata[i] != nullptr)
                {
                    delete _uspjehKandidata[i]; _uspjehKandidata[i] = nullptr;
                }
            }
            else break;
        }
    }
};

bool Pitanje::AddOdgovor(const char * odgovor, bool tacan)
{
    for (size_t i = 0; i < 10; i++)
    {
        if (_odgovori[i] != nullptr)
        {
            if (strcmp(_odgovori[i], odgovor) == 0)
                return false;
        }
        else
        {
            int duzina = strlen(odgovor) + 1;
            _odgovori[i] = new char[duzina];
            strcpy_s(_odgovori[i], duzina, odgovor);
            _tacni[i] = tacan;

            return true;
        }
    }
    
    return false;
}

bool Pitanje::RemoveOdgovor(int indeks)
{
    if (_odgovori[indeks] != nullptr)
    {
        int i = indeks;
        while (_odgovori[i] != nullptr && _odgovori[i + 1] != nullptr)
        {
            delete[] _odgovori[i];
            int duzina = strlen(_odgovori[i + 1]);
            _odgovori[i] = new char[duzina];
            strcpy_s(_odgovori[i], duzina, _odgovori[i + 1]);
            _tacni[i] = _tacni[i + 1];
            i++;
        }

        delete[] _odgovori[i]; _odgovori[i] = nullptr;
        _tacni[i] = -1;

        return true;
    }
    
    return false;
}

void Pitanje::Ispis()
{
    cout << crt << _tekstPitanja << crt;
    for (int i = 0; i < 10; i++)
    {
        if (_odgovori[i] != nullptr)
        {
            cout << i + 1 << ". " << _odgovori[i];
        }

        if(_odgovori[i + 1] == nullptr)
        {
            cout << crt;
            return;
        }

        cout << endl;
    }
}

int Pitanje::BrojOdgovora()
{
    for (int i = 0; i < 10; i++)
        if (_odgovori[i] == nullptr && i != 0)
            return i + 1;
        else if (_odgovori[i] == nullptr && i == 0)
            return 0;
    
}
int Pitanje::BrojTacnihOdgovora()
{
    int brojac = 0;
    for (int i = 0; i < 10; i++)
        if (_odgovori[i] != nullptr && _tacni[i] == 1)
            brojac++;
    return brojac;
}

bool PrijemniIspit::DodajKandidata(Kandidat kandidat)
{
    for (size_t i = 0; i < 100; i++)
    {
        if (_prijavljeniKandidati[i] != nullptr)
        {
            if (strcmp(_prijavljeniKandidati[i]->_JMBG, kandidat._JMBG) == 0)
                return false;
        }
        else
        {
            _prijavljeniKandidati[i] = new Kandidat;
            _prijavljeniKandidati[i]->Unos(kandidat._imePrezime, kandidat._JMBG);
            return true;
        }
    }
}

bool PrijemniIspit::PokreniIspit(const char * JMBG, Pitanje * pitanja, int brojPitanja)
{
    if (brojPitanja < 1 || brojPitanja > 10) return false;

    if (pitanja[brojPitanja - 1]._tekstPitanja == nullptr) return false;

    for (size_t i = 0; i < 100; i++)
    {
        if (_prijavljeniKandidati[i] != nullptr)
        {
            if (strcmp(_prijavljeniKandidati[i]->_JMBG, JMBG) == 0)
            {
                float osvojeniBodovi = 0;
                float maxBodova = 0;

                for (size_t j = 0; j < brojPitanja; j++)
                {
                    int brOdgovora = pitanja[j].BrojOdgovora();
                    if (brOdgovora != 0)
                    {
                        pitanja[j].Ispis();
                        int * odgovori = new int[brOdgovora];

                        int izbor = 0;
                        while (cout << "Izaberite broj odgovora kojih zelite unjeti: ", cin >> izbor, izbor < 1 || izbor >= brOdgovora);

                        cout << "Unesite odgovore: ";
                        float bodoviPoTacnom = pitanja[j]._bodova / 1.F / pitanja[j].BrojTacnihOdgovora();
                        
                        for (size_t k = 0; k < izbor; k++)
                        {
                            while (cin >> odgovori[k], odgovori[k] < 1 || odgovori[k] >= brOdgovora);

                            if (pitanja[j]._tacni[odgovori[k] - 1])
                                osvojeniBodovi += bodoviPoTacnom;
                            else
                                osvojeniBodovi -= _kazneniBodovi;
                        }

                        maxBodova += pitanja[j]._bodova;
                        delete[] odgovori; odgovori = nullptr;
                    }
                }

                _uspjehKandidata[i] = new float;
                *_uspjehKandidata[i] = 100 * (osvojeniBodovi / maxBodova);

                return true;
            }
        }
        else return false;
    }  
}

int main()
{

    Kandidat jasmin, adel;
    jasmin.Unos("Jasmin Azemovic", "1111111111111");
    adel.Unos("Adel Handzic", "2222222222222");

    int brojPitanja = 3;
    Pitanje * pitanja = new Pitanje[brojPitanja];
    //parametri: tekstPitanja, brojBodova
    pitanja[0].Unos("U kojem gradu se nalazi FIT?", 4);

    /*dodaje novi odgovor u listu ponudjenih odgovora. onemoguciti ponavljanje odgovora,
    te voditi racuna da vise odgovora moze biti tacno*/
    //parametri: tekstOdgovora,daLiJeTacanTajOdgovor
    if (pitanja[0].AddOdgovor("Mostar", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0].AddOdgovor("Sarajevo", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (!pitanja[0].AddOdgovor("Sarajevo", false))//Sarajevo je vec dodano
        cout << "Odgovor nije uspjesno dodan!" << crt;
    if (pitanja[0].AddOdgovor("Tuzla", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0].AddOdgovor("Fojnica", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0].AddOdgovor("Zenica", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    /*
    na osnovu primljene lokacije uklanja podatke o ponudjenom odgovoru. prilikom uklanjanja
    ocuvati redoslijed dodavanja odgovora. onemoguciti uklanjanje svih tacnih odgovora tj.
    osigurati da pitanje u svakom momentu posjeduje najmanje jedan tacan odgovor
    */
    if (pitanja[0].RemoveOdgovor(4))//uklanja odgovor Fojnica
        cout << "Odgovor uspjesno uklonjen!" << crt;

    /*
    -------------------------------
    1. U kojem gradu se nalazi FIT?
    -------------------------------
    1. Mostar
    2. Sarajevo
    3. Tuzla
    4. Zenica
    -------------------------------
    */
    pitanja[0].Ispis();//prethodno je prikazan ocekivani ispis pitanja

    pitanja[1].Unos("Odaberite tacne odgovore.", 5);

    if (pitanja[1].AddOdgovor("FIT se nalazi u Mostaru", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[1].AddOdgovor("FIT je clanica UNMO-a", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[1].AddOdgovor("U Mostaru je uvijek jako vruce", false))
        cout << "Odgovor uspjesno dodan!" << crt;

    pitanja[2].Unos("Studij na FIT-u traje 4 godine?", 2);
    if (pitanja[2].AddOdgovor("Ne", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[2].AddOdgovor("Da", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    /*....*/


    PrijemniIspit prijemni2017;
    //parametri: pragProlaznosti, kazneniBodovi
    prijemni2017.Unos(55, 0.30);
    /*dodaje novog kandidata u listu prijavljenih. onemoguciti dodavanje kandidata sa istim jmbg-om*/
    if (prijemni2017.DodajKandidata(jasmin))
        cout << "Kandidat uspjesno dodan" << crt;
    if (prijemni2017.DodajKandidata(adel))
        cout << "Kandidat uspjesno dodan" << crt;

    /*kandidatu sa JMBG 1111111111111 pokrece ispit na nacin da mu se prikazuju pitanja,
    od njega zahtijevaju odgovori, te izracunava ostvareni uspjeh (da li je polozio ili ne,
    te sa kojim procentom uspjesnosti). prilikom izracuvanja uspjesnosti voditi racuna o kaznenim bodovima.
    po okoncanju ispita, uspjeh kandidata se pohranjuje u niz _uspjehKandidata. za pokretanje ispita
    kandidat prethodno mora biti dodan u listu. po vlastitoj zelji implementirajte naci oznacavanja
    veceg broja tacnih odgovoran na postavljeno pitanje (jedno pitanje moze imati vise tacnih odgovora)*/
    if (prijemni2017.PokreniIspit("1111111111111", pitanja, brojPitanja))
        cout << "Ispit uspjesno okoncan!" << crt;

    prijemni2017.Ispis();
    jasmin.Dealociraj();
    adel.Dealociraj();
    for (size_t i = 0; i < brojPitanja; i++)
        pitanja[i].Dealociraj();
    delete[] pitanja;

    system("pause");
    return 0;
}