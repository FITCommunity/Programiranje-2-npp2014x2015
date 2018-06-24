#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

char * crt = "\n-------------------------------------------\n";

struct Datum 
{
    int _dan;
    int _mjesec;
    int _godina;
    void Unos(int d, int m, int g)
    {
        _dan = d;
        _mjesec = m;
        _godina = g;
    }
    void Ispis() {
        cout << _dan << "." << _mjesec << "." << _godina << " ";
    }
};

struct Proizvod 
{
    char _naziv[20];
    float _cijena;
    float _stanjeSkladista;

    void Unos(char* naziv, float cijena) 
    {
        strcpy_s(_naziv, naziv);
        _cijena = cijena;
        _stanjeSkladista = 0;
    }
    void DodajUSkladiste(float kolicina)
    { 
        _stanjeSkladista += kolicina; 
    }
    void UkloniSaSkladista(float kolicina)
    { 
        _stanjeSkladista -= kolicina; 
    }
    void Ispis() 
    { 
        cout << _naziv << ", cijena: " << _cijena << "KM, stanje: " << _stanjeSkladista << " " << endl;
    }
};

struct StavkaFakture 
{
    float _kolicina;
    Proizvod * _proizvod; 
    bool _stornirana;
    void Unos(Proizvod* proizvod, float kolicina) 
    {
        _kolicina = kolicina;
        _proizvod = proizvod;
        _stornirana = false;
    }
    float GetIznos() { return _kolicina * _proizvod->_cijena; }
    void Ispis()
    {
        cout << _proizvod->_naziv << "\t" << _kolicina << "\t" << _proizvod->_cijena << "\t" << GetIznos() << "BAM";
        if (_stornirana)
            cout << " Storno";
        cout << endl;
    }
};
struct Faktura 
{
    Datum _datum;
    StavkaFakture ** _stavke;//svaki put se dodaje po jedna stavka
    int _brojStavki;
    void Unos(Datum datum)
    {
        _datum = datum;
        _brojStavki = 0;
        _stavke = nullptr;
    }

    /*iznos fakture izracunati koristeci lambda funkciju koja se poziva putem objekta
    tipa auto. u iznos fakture ne racunati stornirane stavke*/
    float GetIznosSuma()
    {
        auto suma = [this]()
        {
            float temp = 0;
            for (int i = 0; i < this->_brojStavki; i++)
                if (!this->_stavke[i]->_stornirana)
                    temp += this->_stavke[i]->_kolicina * this->_stavke[i]->_proizvod->_cijena;

            return temp;
        };

        return suma();
    }
};
const int Max = 100;

struct Market 
{
    Proizvod* Proizvodi[Max] = { NULL };
    Faktura* Fakture[Max] = { NULL };

    void Dealociraj()
    {
        for (size_t i = 0; i < Max; i++)
        {
            delete Proizvodi[i]; Proizvodi[i] = NULL;
        }
        for (size_t i = 0; i < Max; i++) 
        {
            delete Fakture[i]; Fakture[i] = NULL;
        }
    }
};

void dodaj_proizvod(Market & market, char * naziv, float cijena, float kolicina)
{
    for (int i = 0; i < Max; i++)
    {
        if (market.Proizvodi[i] != NULL)
        {
            if (strcmp(market.Proizvodi[i]->_naziv, naziv) == 0)
            {
                market.Proizvodi[i]->DodajUSkladiste(kolicina);
                return;
            }
        }
        else
        {
            market.Proizvodi[i] = new Proizvod;
            market.Proizvodi[i]->Unos(naziv, cijena);
            market.Proizvodi[i]->DodajUSkladiste(kolicina);
            return;
        }
    }
}

void kreiraj_fakturu(Market & market, int dan, int mjesec, int godina)
{
    for (int i = 0; i < Max; i++)
    {
        if (market.Fakture[i] == NULL)
        {
            market.Fakture[i] = new Faktura;
            market.Fakture[i]->Unos({ dan, mjesec, godina });
            return;
        }
    }
}

void dodaj_stavku_fakture(Market & market, char * naziv, float kolicina)
{
    for (int i = Max - 1; i >= 0; i--)
    {
        if (market.Fakture[i] != NULL)
        {
            bool dodaj = true;
            for (int j = 0; j < market.Fakture[i]->_brojStavki; j++)
            {
                if (strcmp(market.Fakture[i]->_stavke[j]->_proizvod->_naziv, naziv) == 0)
                {
                    if (!market.Fakture[i]->_stavke[j]->_stornirana)
                    {
                        market.Fakture[i]->_stavke[j]->_kolicina += kolicina;
                        dodaj = false;
                        break;
                    }
                }

                if (!dodaj) break;
            }

            if (dodaj)
            {
                for (int k = 0; k < Max; k++)
                {
                    if (market.Proizvodi[k] != NULL)
                    {
                        bool proizvodUBazi = strcmp(market.Proizvodi[k]->_naziv, naziv) == 0;
                        bool odgovarajucaKolicina = market.Proizvodi[k]->_stanjeSkladista - kolicina >= 0;

                        if (proizvodUBazi && odgovarajucaKolicina)
                        {
                            StavkaFakture ** temp = market.Fakture[i]->_stavke;
                            market.Fakture[i]->_stavke = new StavkaFakture*[market.Fakture[i]->_brojStavki + 1];
                            for (int j = 0; j < market.Fakture[i]->_brojStavki; j++)
                                market.Fakture[i]->_stavke[j] = temp[j];

                            market.Fakture[i]->_stavke[market.Fakture[i]->_brojStavki] = new StavkaFakture;
                            market.Fakture[i]->_stavke[market.Fakture[i]->_brojStavki]->Unos(market.Proizvodi[k], kolicina);
                            market.Fakture[i]->_brojStavki++;
                            market.Proizvodi[k]->UkloniSaSkladista(kolicina);
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool storniraj_stavku(Market & market, char * parametar)
{
    bool flag = false;
    for (int i = 0; i < Max; i++)
    {
        if (market.Fakture[i] != NULL)
        {
            for (int j = 0; j < market.Fakture[i]->_brojStavki; j++)
            {
                if (strstr(market.Fakture[i]->_stavke[j]->_proizvod->_naziv, parametar) != NULL)
                {
                    market.Fakture[i]->_stavke[j]->_stornirana = true;
                    market.Fakture[i]->_stavke[j]->_proizvod->DodajUSkladiste(market.Fakture[i]->_stavke[j]->_kolicina);
                    flag = true;
                }
            }
        }
        
        if (market.Fakture[i] == NULL) break;
    }

    return flag;

}

char * FormirajImeFajla()
{
    int broj;
    cout << "Unesite broj indeksa: IB";
    cin >> broj;
    char ID[7];
    _itoa_s(broj, ID, 10);

    int velicina = 2 + 6 + 4 + 1; //2(IB) + 6(170000) + 4(.txt) + 1(\0)
    char * naziv = new char[velicina];
    
    strcpy_s(naziv, velicina, "IB");
    strcat_s(naziv, velicina, ID);
    strcat_s(naziv, velicina, ".txt");

    return naziv;
}

void upisi(ofstream & upis, Faktura * faktura)
{
    upis << crt << "Datum: ";
    if (faktura->_datum._dan < 10) upis << "0" << faktura->_datum._dan;
    else upis << faktura->_datum._dan;
    upis << ".";

    if (faktura->_datum._mjesec < 10) upis << "0" << faktura->_datum._mjesec;
    else upis << faktura->_datum._mjesec;

    upis << "." << faktura->_datum._godina
         << crt << "FAKTURE / RACUN" << crt
         << "RB Naziv\tKol.\tCij.\tIznos" << endl;

    for (int j = 0; j < faktura->_brojStavki; j++)
    {
        upis << j + 1 << "." << faktura->_stavke[j]->_proizvod->_naziv << "\t"
                             << faktura->_stavke[j]->_kolicina << "\t"
                             << faktura->_stavke[j]->_proizvod->_cijena << "\t"
                             << setw(5) << left << faktura->_stavke[j]->GetIznos() << "BAM " << right;

        if (faktura->_stavke[j]->_stornirana) upis << "(Storno)";

        if (j != faktura->_brojStavki - 1) upis << endl;
    }

    upis << crt << "UKUPNO:\t\t\t\t" << faktura->GetIznosSuma() << "BAM" << crt;
}

bool printaj(const Market & market)
{
    for (int i = Max - 1; i >= 0; i--)
    {
        if (market.Fakture[i] != NULL)
        {
            for (int j = 0; j < market.Fakture[i]->_brojStavki; j++)
            {
                if (market.Fakture[i]->_stavke[j]->_stornirana == false)
                {
                    char * naziv = FormirajImeFajla();
                    ofstream upis;
                    upis.open(naziv);

                    if (upis.fail())
                        cout << "Greska!" << endl;
                    else
                    {                        
                        upisi(upis, market.Fakture[i]);
                        upis.close();
                    }

                    ifstream ispis;
                    ispis.open(naziv);

                    if (ispis.fail())
                        cout << "Greska!" << endl;
                    else
                    {
                        char znak;
                        while (ispis.get(znak))
                            cout << znak;
                        ispis.close();
			
			delete[] naziv; naziv = nullptr;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void main() {

    Market market;

    dodaj_proizvod(market, "Mlijeko (L)", 1.45, 50);
    dodaj_proizvod(market, "Mlijeko (L)", 1.45, 5);
    dodaj_proizvod(market, "Jabuke (kg)", 1.25, 80.4);
    dodaj_proizvod(market, "Lubenica (kg)", 0.45, 189);

    kreiraj_fakturu(market, 6, 9, 2017);

    dodaj_stavku_fakture(market, "Mlijeko (L)", 3);
    dodaj_stavku_fakture(market, "Mlijeko (L)", 2);
    dodaj_stavku_fakture(market, "Lubenica (kg)", 5);
    

    kreiraj_fakturu(market, 7, 9, 2017);
    dodaj_stavku_fakture(market, "Jabuke (kg)", 5);
    dodaj_stavku_fakture(market, "Lubenica (kg)", 3);
    
    if (storniraj_stavku(market, "Lub"))
        cout << "Stavka uspjesno stornirana!" << endl;
    if (!storniraj_stavku(market, "TEST"))
        cout << "Trazena stavka ne postoji na fakturi!" << endl;

    if (printaj(market))
        cout << "Faktura uspjesno prikazana i sacuvana!" << endl;
    market.Dealociraj();

    system("pause>0");
}