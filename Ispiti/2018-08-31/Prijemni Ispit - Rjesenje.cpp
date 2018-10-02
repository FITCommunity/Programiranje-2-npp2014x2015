#include <iostream>
#include <tuple>

using namespace std;

//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

const char * crt = "\n-------------------------------\n";
const int MAX_ODGOVORA = 10;

char * AlocirajNizKaraktera(const char * sadrzaj) {
    if (sadrzaj == nullptr)
        return nullptr;
    int vel = strlen(sadrzaj) + 1;
    char * temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    return temp;
}
struct Kandidat 
{
    char * _imePrezime;
    char _JMBG[14];
    void Unos(const char * ip, const char * jmbg) 
    {
        _imePrezime = AlocirajNizKaraktera(ip);
        strcpy_s(_JMBG, jmbg);
    }
    void Dealociraj()
    {
        delete[] _imePrezime; _imePrezime = nullptr;
    }
    void Ispis()
    {
        cout << _imePrezime << " " << _JMBG << " ";
    }
};
struct Pitanje 
{
    char * _tekstPitanja;
    char * _odgovori[MAX_ODGOVORA];//maksimalno 10 odgovora
    /* svako pitanje moze imati jedan ili vise tacnih odgovora, a njihove lokacije se pohranjuju u niz _tacni*/
    int _tacni[MAX_ODGOVORA];
    /*ako pitanje ima vise tacnih odgovora, onda se broj osvojenih bodova (tokom izrade testa) odredjuje 
    na osnovu broj tacnih odgovora (npr. ako pitanje nosi 5 boda i ima 2 tacna odgovora, a student oznaci 1 
    tacan odgovor onda ce osvojeni broj bodova na tom pitanju iznositi 2.5. drugim rijecima, vrijednost ovog 
    atributa je bitna zbog  odredjivanja uspjeha kandidata prilikom izrade ispita)*/
    int _bodova;

    bool AddOdgovor(const char *odgovor, bool tacan);
    bool RemoveOdgovor(int i);
    void Ispis();
    int BrojOdgovora();
    int BrojTacnihOdgovora();
    void Unos(const char * tekstPitanja, int bodova) 
    {
        _tekstPitanja = AlocirajNizKaraktera(tekstPitanja);
        /*umjesto dodavanja brojaca iskoristite defaultne vrijednosti clanova niza kako biste
        manipulisali njihovim vrijednostima (dodavanje, uklanjanje)*/
        for (size_t i = 0; i < MAX_ODGOVORA; i++) 
        {
            _odgovori[i] = nullptr;
            _tacni[i] = -1;
        }
        _bodova = bodova;
    }
    void Dealociraj() 
    {
        delete[] _tekstPitanja; _tekstPitanja = nullptr;
        for (size_t i = 0; i < MAX_ODGOVORA; i++) 
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
    Kandidat * _prijavljeniKandidati;
    /*nakon sto urade ispit tj. nakon sto odgovore na pitanja za svakog od kandidata se cuva ostvareni 
    uspjeh u okviru niza _uspjehKandidata. broj elemenata u nizu _uspjehKandidata bi trebao biti identican 
    broju elemenata u nizu _prijavljeniKandidati*/
    float * _uspjehKandidata;
    bool * _ispitPolagan;
    int _trenutnoKandidata;
    //prag prolaznosti na prijemnom ispitu npr. 60.5 procenata
    float _pragProlaznosti;
    float _kazneniBodovi;
    /*vrijednost kazenog boda (npr. 0.30) se mnozi sa svakim netacnim odgovorom na pitanje i oduzima od osvojenih
    bodova za to pitanje
    npr. ako pitanje nosi 5 bodova i ima 2 tacna odgovora, a student odabere 1 tacan i 2 netacna odgovora, broj osvojenih 
    bodova ce biti 1.9
    (1 tacan = 2.5; 2 netacna * 0.30 kaznenih = 0.6; 2.5 - 0.6 = 1.9)*/
    bool DodajKandidata(Kandidat kandidat);
    bool PokreniIspit(const char * JMBG, Pitanje **pitanja, int brojPitanja);
    void Unos(float pragProlaznosti, float kazneniBodovi)
    {
        _pragProlaznosti = pragProlaznosti;
        _kazneniBodovi = kazneniBodovi;
        _prijavljeniKandidati = nullptr;
        _uspjehKandidata = nullptr;
        _ispitPolagan = nullptr;
        _trenutnoKandidata = 0;
    }
    void Dealociraj() 
    {
        for (size_t i = 0; i < _trenutnoKandidata; i++)
            _prijavljeniKandidati[i].Dealociraj();
        delete[] _prijavljeniKandidati; _prijavljeniKandidati = nullptr;
        delete[] _uspjehKandidata; _uspjehKandidata = nullptr;
    }
};

bool Pitanje::AddOdgovor(const char *odgovor, bool tacan)
{
    for (int i = 0; i < MAX_ODGOVORA; i++)
    {
        if (_odgovori[i] != nullptr)
        {
            if (strcmp(_odgovori[i], odgovor) == 0) return false;
        }
        else
        {
            _odgovori[i] = AlocirajNizKaraktera(odgovor);
            _tacni[i] = tacan;
            return true;
        }
    }

    return false;
}

bool Pitanje::RemoveOdgovor(int i)
{
    if (_odgovori[i] != nullptr)
    {
        while (_odgovori[i] != nullptr/* && _odgovori[i + 1] != nullptr*/)
        {
            delete[] _odgovori[i];
            _odgovori[i] = AlocirajNizKaraktera(_odgovori[i + 1]);
            _tacni[i] = _tacni[i + 1];
            i++;
        }

        delete[] _odgovori[i]; _odgovori[i] = nullptr;
        _tacni[i] = -1;

        return true;
    }

    return false;
}

bool PrijemniIspit::DodajKandidata(Kandidat kandidat)
{
    for (int i = 0; i < _trenutnoKandidata; i++)
        if (strcmp(_prijavljeniKandidati[i]._JMBG, kandidat._JMBG) == 0)
            return false;

    Kandidat *tempK = _prijavljeniKandidati;
    float *tempU = _uspjehKandidata;
    bool *tempP = _ispitPolagan;
    _prijavljeniKandidati = new Kandidat[_trenutnoKandidata + 1];
    _uspjehKandidata = new float[_trenutnoKandidata + 1];
    _ispitPolagan = new bool[_trenutnoKandidata + 1];
    for (int i = 0; i < _trenutnoKandidata; i++)
    {
        _prijavljeniKandidati[i] = tempK[i];
        _uspjehKandidata[i] = tempU[i];
        _ispitPolagan[i] = tempP[i];
    }
    _prijavljeniKandidati[_trenutnoKandidata].Unos(kandidat._imePrezime, kandidat._JMBG);
    _uspjehKandidata[_trenutnoKandidata] = 0;
    _ispitPolagan[_trenutnoKandidata] = false;
    _trenutnoKandidata++;

    delete[] tempK; tempK = nullptr;
    delete[] tempU; tempU = nullptr;

    return true;

}

int Pitanje::BrojOdgovora()
{
    for (int i = 0; i < MAX_ODGOVORA; i++)
        if (_odgovori[i] == nullptr && i != 0)
            return i + 1;
        else if (_odgovori[i] == nullptr && i == 0)
            return 0;

}
int Pitanje::BrojTacnihOdgovora()
{
    int brojac = 0;
    for (int i = 0; i < MAX_ODGOVORA; i++)
        if (_odgovori[i] != nullptr && _tacni[i] == 1)
            brojac++;
    return brojac;
}

void Pitanje::Ispis()
{
    cout << crt << _tekstPitanja << crt;
    for (int i = 0; i < MAX_ODGOVORA; i++)
    {
        if (_odgovori[i] != nullptr) cout << i + 1 << ". " << _odgovori[i];

        if (_odgovori[i + 1] == nullptr)
        {
            cout << crt;
            return;
        }

        cout << endl;
    }
}

bool PrijemniIspit::PokreniIspit(const char * JMBG, Pitanje **pitanja, int brojPitanja)
{
    if (brojPitanja < 1 || brojPitanja > 10) return false;

    if (pitanja[brojPitanja - 1]->_tekstPitanja == nullptr) return false;

    for (int i = 0; i < _trenutnoKandidata; i++)
    {
        if (strcmp(_prijavljeniKandidati[i]._JMBG, JMBG) == 0)
        {
            if (!_ispitPolagan[i])
            {
                float osvojeniBodovi = 0;
                float maxBodova = 0;

                for (int j = 0; j < brojPitanja; j++)
                {
                    int brOdgovora = pitanja[j]->BrojOdgovora();
                    if (brOdgovora != 0)
                    {
                        pitanja[j]->Ispis();
                        int * odgovori = new int[brOdgovora];

                        int izbor = 0;
                        while (cout << "Izaberite broj odgovora kojih zelite unjeti: ", cin >> izbor, izbor < 1 || izbor >= brOdgovora);

                        cout << "Unesite odgovore: ";
                        float bodoviPoTacnom = pitanja[j]->_bodova / 1.F / pitanja[j]->BrojTacnihOdgovora();

                        for (int k = 0; k < izbor; k++)
                        {
                            while (cin >> odgovori[k], odgovori[k] < 1 || odgovori[k] >= brOdgovora);

                            if (pitanja[j]->_tacni[odgovori[k] - 1])
                                osvojeniBodovi += bodoviPoTacnom;
                            else
                                osvojeniBodovi -= _kazneniBodovi;
                        }

                        maxBodova += pitanja[j]->_bodova;
                        delete[] odgovori; odgovori = nullptr;
                    }
                }

                _uspjehKandidata[i] = 100 * (osvojeniBodovi / maxBodova);
                _ispitPolagan[i] = true;

                return true;
            }
            else return false;
        }
    }
}

int main() 
{

    Kandidat jasmin, adel;
    jasmin.Unos("Jasmin Azemovic", "1111111111111");
    adel.Unos("Adel Handzic", "2222222222222");

    const int brojPitanja = 3;
    Pitanje * pitanja[brojPitanja];
    for (size_t i = 0; i < brojPitanja; i++)
        pitanja[i] = new Pitanje;

    //parametri: tekstPitanja, brojBodova
    pitanja[0]->Unos("U kojem gradu se nalazi FIT?", 4);

    /*dodaje novi odgovor u listu ponudjenih odgovora. onemoguciti ponavljanje odgovora, te voditi racuna da 
    vise odgovora moze biti tacno*/
    //parametri: tekstOdgovora, daLiJeTacanTajOdgovor
    if (pitanja[0]->AddOdgovor("Mostar", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0]->AddOdgovor("Sarajevo", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (!pitanja[0]->AddOdgovor("Sarajevo", false))//Sarajevo je vec dodano
        cout << "Odgovor nije uspjesno dodan!" << crt;
    if (pitanja[0]->AddOdgovor("Tuzla", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0]->AddOdgovor("Fojnica", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[0]->AddOdgovor("Zenica", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    /*na osnovu primljene lokacije uklanja podatke o ponudjenom odgovoru. prilikom uklanjanja ocuvati redoslijed 
    dodavanja odgovora. takodjer, potrebno je onemoguciti uklanjanje svih tacnih odgovora na nacin da se prije 
    njegovog uklanjanja mora unijeti novi tacan odgovor */
    if (pitanja[0]->RemoveOdgovor(4))//uklanja odgovor Fojnica
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
    pitanja[0]->Ispis();//prethodno je prikazan ocekivani ispis pitanja

    pitanja[1]->Unos("Odaberite tacne odgovore.", 5);

    if (pitanja[1]->AddOdgovor("FIT se nalazi u Mostaru", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[1]->AddOdgovor("FIT je clanica UNMO-a", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[1]->AddOdgovor("U Mostaru je uvijek jako vruce", false))
        cout << "Odgovor uspjesno dodan!" << crt;

    pitanja[2]->Unos("Studij na FIT-u traje 4 godine?", 3);
    if (pitanja[2]->AddOdgovor("Ne", true))
        cout << "Odgovor uspjesno dodan!" << crt;
    if (pitanja[2]->AddOdgovor("Da", false))
        cout << "Odgovor uspjesno dodan!" << crt;
    /*....*/


    PrijemniIspit prijemni2018;
    //parametri: pragProlaznosti, kazneniBodovi
    prijemni2018.Unos(60, 0.30);
    /*dodaje novog kandidata u listu prijavljenih. onemoguciti dodavanje kandidata sa istim jmbg-om*/
    if (prijemni2018.DodajKandidata(jasmin))
        cout << "Kandidat uspjesno dodan" << crt;
    if (prijemni2018.DodajKandidata(adel))
        cout << "Kandidat uspjesno dodan" << crt;

    /*kandidatu sa JMBG 1111111111111 pokrece ispit na nacin da mu se prikazuju pitanja, od njega zahtijevaju odgovori, 
    te izracunava ostvareni uspjeh (da li je polozio ili ne, te sa kojim procentom uspjesnosti). prilikom 
    izracuvanja uspjesnosti voditi racuna o kaznenim bodovima. po okoncanju ispita, uspjeh kandidata se pohranjuje u niz _uspjehKandidata. za pokretanje ispita kandidat prethodno mora biti dodan u listu. po
    vlastitoj zelji implementirajte nacin oznacavanja veceg broja tacnih odgovoran na postavljeno pitanje 
    (jedno pitanje moze imati vise tacnih odgovora)*/
    if (prijemni2018.PokreniIspit("1111111111111", pitanja, brojPitanja))
        cout << "Ispit uspjesno okoncan!" << crt;
    if (!prijemni2018.PokreniIspit("1111111111111", pitanja, brojPitanja))//kandidat jasmin je vec radio ispit
        cout << "Kandidat je vec polagao prijemni ispit!" << crt;

    if (prijemni2018.PokreniIspit("2222222222222", pitanja, brojPitanja))
        cout << "Ispit uspjesno okoncan!" << crt;

    Kandidat * najbolji = nullptr;
    float uspjeh = 0;
    /*getNajbolji student je lambda funkcija koja treba vratiti podatke o studentu koji je ostvario najbolji uspjeh na prijemnom ispitu*/
    auto getNajboljiStudent = [&prijemni2018]()
    {
        int max = 0;
        for (int i = 0; i < prijemni2018._trenutnoKandidata; i++)
            if (prijemni2018._uspjehKandidata[i] > prijemni2018._uspjehKandidata[max])
                max = i;

        return make_pair(&prijemni2018._prijavljeniKandidati[max], prijemni2018._uspjehKandidata[max]);
    };
    tie(najbolji, uspjeh) = getNajboljiStudent();
    if (najbolji != nullptr)
        cout << "Najbolji uspjeh od " << uspjeh << " % ostvario je kandidat " << najbolji->_imePrezime << "." << endl;


    jasmin.Dealociraj();
    adel.Dealociraj();
    for (size_t i = 0; i < brojPitanja; i++) {
        pitanja[i]->Dealociraj();
        pitanja[i] = nullptr;
    }

    system("pause>0");
    return 0;
}