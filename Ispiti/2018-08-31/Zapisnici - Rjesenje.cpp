#include <iostream>
#include <iomanip>
using namespace std;

//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

const char * crt = "\n-------------------------------------------\n";
const int MAX = 100;

char * AlocirajNizKaraktera(const char * sadrzaj) 
{
    if (sadrzaj == nullptr)
        return nullptr;
    int vel = strlen(sadrzaj) + 1;
    char * temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    return temp;
}
struct Datum 
{
    int _dan, _mjesec, _godina;
    void Unos(int d, int m, int g) 
    {
        _dan = d;
        _mjesec = m;
        _godina = g;
    }
    void Ispis() 
    {
        cout << _dan << "." << _mjesec << "." << _godina << " ";
    }
};
struct Student 
{
    char * _imePrezime;
    int _brojIndeksa;
    int _polozioECTS;

    void Unos(const char* ime, float indeks) 
    {
        _imePrezime = AlocirajNizKaraktera(ime);
        _brojIndeksa = indeks;
        _polozioECTS = 0;
    }
    void Dealociraj() 
    {
        delete[] _imePrezime; _imePrezime = nullptr;
    }
    void DodajECTS(float ects) { _polozioECTS += ects; }
    void UmanjiECTS(float ects) { _polozioECTS -= ects; }
    void Ispis() 
    {
        cout << _imePrezime << ", Indeks: " << _brojIndeksa << ", ECTS: " << _polozioECTS << " " << endl;
    }
};

struct ZapisnikStavka 
{
    int _ocjena;
    Student* _student;
    bool _ponisteno;
    bool _komisijskoPolaganje;
    int _brojIzlazaka;

    void Unos(Student* student, int ocjena, bool komisijskoPolaganje) 
    {
        _ocjena = ocjena;
        _student = student;
        _komisijskoPolaganje = komisijskoPolaganje;
        _ponisteno = false;
        _brojIzlazaka = 0;
    }
    void Ispis() 
    {
        cout << _student->_imePrezime << "\t" << _ocjena << "\t" << _student->_brojIndeksa << "\t";
        if (_ponisteno)
            cout << " Ponisteno";
        cout << endl;
    }
};
struct IspitniZapisnik
{
    Datum _datum;
    ZapisnikStavka * _stavke;
    int _brojStavki;
    int _ects;
    char * _predmet;
    void Unos(Datum & datum, char* predmet, int ects) 
    {
        _datum = datum;
        _brojStavki = 0;
        _stavke = nullptr;
        _ects = ects;
        _predmet = AlocirajNizKaraktera(predmet);
    }
    int najduziString(int i = 0);
    void Dealociraj() 
    {
        delete[] _predmet; _predmet = nullptr;
        delete[] _stavke; _stavke = nullptr;
    }
    float GetProsjecnaOcjena() 
    { 
        auto prosjek = [this]()
        {
            float suma = 0;
            for (int i = 0; i < _brojStavki; i++)
                suma += _stavke[i]._ocjena;
            return _brojStavki ? suma / _brojStavki : 0;
        };

        return prosjek();
    }
};

struct Fakultet 
{
    Student* _studenti[MAX] = { nullptr };
    IspitniZapisnik* _zapisnici[MAX] = { nullptr };

    bool printaj();
    void Dealociraj() 
    {
        for (size_t i = 0; i < MAX; i++) 
        {
            if (_studenti[i])
            {
                _studenti[i]->Dealociraj();
                delete _studenti[i]; _studenti[i] = NULL;
            }

            if (_zapisnici[i])
            {
                _zapisnici[i]->Dealociraj();
                delete _zapisnici[i]; _zapisnici[i] = NULL;
            }

            if (!_studenti[i] && !_zapisnici[i]) break;
        }
    }
};

void dodaj_student(Fakultet &fakultet, const char *imePrezime, int indeks)
{
    for (int i = 0; i < MAX; i++)
    {
        if (!fakultet._studenti[i])
        {
            fakultet._studenti[i] = new Student;
            fakultet._studenti[i]->Unos(imePrezime, indeks);
            return;
        }
        
        if (fakultet._studenti[i]->_brojIndeksa == indeks) return;
    } 
}

void kreiraj_zapisnik(Fakultet &fakultet, int dan, int mjesec, int godina, const char *naziv, int ects)
{
    for (int i = 0; i < MAX; i++)
    {
        if (!fakultet._zapisnici[i])
        {
            fakultet._zapisnici[i] = new IspitniZapisnik;
            Datum datum; datum.Unos(dan, mjesec, godina);
            fakultet._zapisnici[i]->Unos(datum, const_cast<char*>(naziv), ects);
            return;
        }
    }
}

bool dodaj_stavku_na_zapisnik(Fakultet &fakultet, int indeks, int ocjena)
{
    if (ocjena < 5) return false;

    for (int i = MAX - 1; i >= 0; i--)
    {
        if (fakultet._zapisnici[i])
        {
            for (int j = 0; j < MAX; j++)
            {
                if (fakultet._studenti[j]->_brojIndeksa == indeks)
                {
                    for (int k = 0; k < fakultet._zapisnici[i]->_brojStavki; k++)
                    {
                        if (fakultet._zapisnici[i]->_stavke[k]._student->_brojIndeksa == indeks)
                        {
                            fakultet._zapisnici[i]->_stavke[k]._brojIzlazaka++;
                            fakultet._zapisnici[i]->_stavke[k]._komisijskoPolaganje = fakultet._zapisnici[i]->_stavke[k]._brojIzlazaka > 3;

                            if(fakultet._zapisnici[i]->_stavke[k]._ocjena > 5 && ocjena == 5)
                                fakultet._zapisnici[i]->_stavke[k]._student->UmanjiECTS(fakultet._zapisnici[i]->_ects);
                            if(fakultet._zapisnici[i]->_stavke[k]._ocjena == 5 && ocjena > 5)
                                fakultet._zapisnici[i]->_stavke[k]._student->DodajECTS(fakultet._zapisnici[i]->_ects);
                            
                            fakultet._zapisnici[i]->_stavke[k]._ocjena = ocjena;
                            return false;
                        }
                    }

                    ZapisnikStavka *temp = fakultet._zapisnici[i]->_stavke;
                    fakultet._zapisnici[i]->_stavke = new ZapisnikStavka[fakultet._zapisnici[i]->_brojStavki + 1];
                    for (int k = 0; k < fakultet._zapisnici[i]->_brojStavki; k++)
                        fakultet._zapisnici[i]->_stavke[k] = temp[k];
                    fakultet._zapisnici[i]->_stavke[fakultet._zapisnici[i]->_brojStavki].Unos(fakultet._studenti[j], ocjena, false);
                    if(ocjena > 5) fakultet._zapisnici[i]->_stavke[fakultet._zapisnici[i]->_brojStavki]._student->DodajECTS(fakultet._zapisnici[i]->_ects);
                    fakultet._zapisnici[i]->_brojStavki++;
                    delete[] temp;
                    return true;
                }
            }
        }
    }
}

bool ponisti_stavku_rezultat(Fakultet &fakultet, int indeks)
{
    for (int i = MAX - 1; i >= 0; i--)
    {
        if (fakultet._zapisnici[i])
        {
            for (int j = 0; j < fakultet._zapisnici[i]->_brojStavki; j++)
            {
                if (fakultet._zapisnici[i]->_stavke[j]._student->_brojIndeksa == indeks)
                {
                    if(fakultet._zapisnici[i]->_stavke[j]._ocjena > 5)
                        fakultet._zapisnici[i]->_stavke[j]._student->UmanjiECTS(fakultet._zapisnici[i]->_ects);
                    fakultet._zapisnici[i]->_stavke[j]._ponisteno = true;
                    return true;
                }
            }

            return false;
        }
    }

    return false;
}

int IspitniZapisnik::najduziString(int i)
{
    if (i == _brojStavki) return 0;

    int d1 = _stavke[i]._student->_imePrezime ? strlen(_stavke[i]._student->_imePrezime) : 0;
    int d2 = najduziString(i + 1);

    return d1 > d2 ? d1 : d2;
}

bool Fakultet::printaj()
{
    for (int i = MAX - 1; i >= 0; i--)
    {
        if (_zapisnici[i])
        {
            for (int j = 0; j < _zapisnici[i]->_brojStavki; j++)
            {
                if (!_zapisnici[i]->_stavke[j]._ponisteno)
                {
                    int sirina1 = strlen("Br indeksa") + 3;
                    int sirina2 = _zapisnici[i]->najduziString() + 1;
                    int sirina3 = strlen("Komisijsko") + 2;

                    cout << crt << "Datum ispita: "; _zapisnici[i]->_datum.Ispis();
                    cout << endl << "Predmet: " << _zapisnici[i]->_predmet << crt;
                    cout << "IZVJESTAJ O ODRZANOM ISPITU" << crt;
                    cout << "Br indeksa  Ime prezime   Komisijsko  Ocjena";
                    int brojac = 0;
                    for(int k = 0; k < _zapisnici[i]->_brojStavki; k++)
                    {
                        cout << endl;
                        cout << setw(sirina1) << left << _zapisnici[i]->_stavke[k]._student->_brojIndeksa;
                        cout << setw(sirina2) << left << _zapisnici[i]->_stavke[k]._student->_imePrezime;
                        _zapisnici[i]->_stavke[k]._komisijskoPolaganje ? cout << "DA" : cout << "NE";
                        cout << setw(sirina3) << right << _zapisnici[i]->_stavke[k]._ocjena;
                        if (_zapisnici[i]->_stavke[k]._ponisteno) cout << " Ponisteno";

                        if (_zapisnici[i]->_stavke[k]._ocjena > 5 && !_zapisnici[i]->_stavke[k]._ponisteno) brojac++;
                    }

                    cout << crt << left << "UKUPNO POLOZILO:  " << brojac << endl;
                    cout << "PROSJECNA OCJENA:  " << _zapisnici[i]->GetProsjecnaOcjena() << crt;
                    return true;
                }
            }
        }
    }
    return false;
}

int main() 
{
    Fakultet fakultet;

    dodaj_student(fakultet, "A. Joldic", 16001);
    dodaj_student(fakultet, "D. Music", 15251);
    dodaj_student(fakultet, "E. Babovic    ", 16014);
    dodaj_student(fakultet, "E. Junuz", 16015);
    dodaj_student(fakultet, "J. Azemovic", 16014);

    kreiraj_zapisnik(fakultet, 6, 9, 2018, "Programiranje II", 7);

    if (dodaj_stavku_na_zapisnik(fakultet, 16001, 5))
        cout << "Stavka (rezultat) uspjesno dodata!" << endl;

    if (!dodaj_stavku_na_zapisnik(fakultet, 16001, 6))
        cout << "Student vec dodat na zapisnik. Ocjena je ispravljena!" << endl;

    dodaj_stavku_na_zapisnik(fakultet, 15251, 5);
    dodaj_stavku_na_zapisnik(fakultet, 16015, 6);
    dodaj_stavku_na_zapisnik(fakultet, 16014, 8);

    kreiraj_zapisnik(fakultet, 7, 9, 2018, "Raèunarske mreze", 5);

    dodaj_stavku_na_zapisnik(fakultet, 16015, 5);
    dodaj_stavku_na_zapisnik(fakultet, 16014, 10);


    if (ponisti_stavku_rezultat(fakultet, 16001))
        cout << "Stavka (rezultat) uspjesno ponisten!" << endl;

    if (!ponisti_stavku_rezultat(fakultet, 19001))
        cout << "Student nije pronadjen!" << endl;

    if (fakultet.printaj())
        cout << "Ispitni zapisnik uspjesno prikazan!" << endl;
    fakultet.Dealociraj();

    system("pause>0");
    return 0;
}