#include <iostream>
#include <fstream>

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
    void Ispis() { cout << _dan << "." << _mjesec << "." << _godina << " "; }
};
struct Student
{
    char _imePrezime[20];
    int _brojIndeksa;
    int _polozioECTS;

    void Unos(char* naziv, int cijena)
    {
        strcpy_s(_imePrezime, naziv);
        _brojIndeksa = cijena;
        _polozioECTS = 0;
    }
    void DodajECTS(int ects)
    {
        _polozioECTS += ects;
    }
    void UmanjiECTS(int ects)
    {
        _polozioECTS -= ects;
    }
    void Ispis()
    {
        cout << _imePrezime << ", Indeks: " << _brojIndeksa << ", ECTS: " << _polozioECTS << " " << endl;
    }
};

struct ZapisnikStavka
{
    int _ocjena;
    Student * _student;
    //ZapisnikStavka ce pokazivati na objekat tipa student koji se nalazi u instanci tipa Fakultet,
    //pa nije potrebno vršiti nove alokacije
    int _brojacPonistenih;
    bool _ponisteno;
    bool _komisijskoPolaganje;

    void Unos(Student * proizvod, int ocjena, bool komisijskoPolaganje)
    {
        _ocjena = ocjena;
        _student = new Student;
        _student->Unos(proizvod->_imePrezime, proizvod->_brojIndeksa);
        _komisijskoPolaganje = komisijskoPolaganje;
        _ponisteno = false;
        _brojacPonistenih = 0;
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
    ZapisnikStavka ** _stavke;//svaki put se dodaje po jedna stavka
    int _brojStavki;
    int _ects;
    char _predmet[20];
    void Unos(Datum datum, char* predmet, int ects)
    {
        _datum = datum;
        _brojStavki = 0;
        _stavke = nullptr;
        _ects = ects;
        strcpy_s(_predmet, predmet);
    }
    /*Prosjecnu ocjenu izracunati koristeci lambda funkciju koja se poziva koristeci objekat
    tipa auto. u prosjek racunati i poništene rezultate (stavke)*/
    float GetProsjecnaOcjena()
    {
        auto prosjek = [this]()
        {
            float suma = 0;
            for (int i = 0; i < _brojStavki; i++)
                suma += _stavke[i]->_ocjena;
            return suma / _brojStavki;
        };

        return prosjek();
    }
};
const int Max = 100;

/*unutar objekta tipa Fakultet ce se cuvati podaci o svim studentima i ispitnim zapisnicima, a dodaju se pomocu globalnih funkcija*/
struct Fakultet
{
    //umjesto brojaca koristiti default.ne vrijednosti
    Student* Studenti[Max] = { NULL };
    IspitniZapisnik* Zapisnici[Max] = { NULL };

    void Dealociraj()
    {
        for (size_t i = 0; i < Max; i++)
        {
            delete Studenti[i]; Studenti[i] = NULL;
        }
        for (size_t i = 0; i < Max; i++)
        {
            delete Zapisnici[i]; Zapisnici[i] = NULL;
        }
    }
};


void dodaj_student(Fakultet & fakultet, char * imePrezime, int indeks)
{
    int i = 0;
    while (fakultet.Studenti[i] != NULL && i < Max)
    {
        if (fakultet.Studenti[i]->_brojIndeksa == indeks)
            return;
        i++;
    }

    if (i < Max)
    {
        fakultet.Studenti[i] = new Student;
        fakultet.Studenti[i]->Unos(imePrezime, indeks);
    }
}


void kreiraj_zapisnik(Fakultet & fakultet, int dan, int mjesec, int godina, char * predmet, int ectsKrediti)
{
    for (int i = 0; i < Max; i++)
    {
        if (fakultet.Zapisnici[i] == NULL)
        {
            fakultet.Zapisnici[i] = new IspitniZapisnik;
            fakultet.Zapisnici[i]->Unos({ dan, mjesec, godina }, predmet, ectsKrediti);
            fakultet.Zapisnici[i]->_stavke = nullptr;
            return;
        }
    }
}

/*dodaje stavku na posljednji kreiran zapisnik, a ects studenta uveca ako je student položio ispit. */
/*ako je student vec dodat u zapisnik, onda se vrsi samo modifikacija njegove ocjene*/
bool dodaj_stavku_na_zapisnik(Fakultet & fakultet, int indeks, int ocjena)
{
    for (int i = Max - 1; i >= 0; i--)
    {
        if (fakultet.Zapisnici[i] != NULL)
        {
            bool dodaj = true;
            for (int j = 0; j < fakultet.Zapisnici[i]->_brojStavki; j++)
            {
                if (fakultet.Zapisnici[i]->_stavke[j]->_student->_brojIndeksa == indeks)
                {
                    fakultet.Zapisnici[i]->_stavke[j]->_ocjena = ocjena;
                    fakultet.Zapisnici[i]->_stavke[j]->_brojacPonistenih++;

                    if (fakultet.Zapisnici[i]->_stavke[j]->_ocjena > 5) 
                        fakultet.Zapisnici[i]->_stavke[j]->_student->DodajECTS(fakultet.Zapisnici[i]->_ects);

                    if (fakultet.Zapisnici[i]->_stavke[j]->_brojacPonistenih == 4)
                        fakultet.Zapisnici[i]->_stavke[j]->_komisijskoPolaganje = true;

                    dodaj = false;
                    break;
                }
            }

            if (dodaj)
            {

                for (int k = 0; k < Max; k++)
                {
                    if (fakultet.Studenti[k] != NULL)
                    {
                        if (fakultet.Studenti[k]->_brojIndeksa == indeks)
                        {
                            ZapisnikStavka ** temp = fakultet.Zapisnici[i]->_stavke;
                            fakultet.Zapisnici[i]->_stavke = new ZapisnikStavka*[fakultet.Zapisnici[i]->_brojStavki + 1];
                            for (int j = 0; j < fakultet.Zapisnici[i]->_brojStavki; j++)
                                fakultet.Zapisnici[i]->_stavke[j] = temp[j];

                            fakultet.Zapisnici[i]->_stavke[fakultet.Zapisnici[i]->_brojStavki] = new ZapisnikStavka;
                            fakultet.Zapisnici[i]->_stavke[fakultet.Zapisnici[i]->_brojStavki]->Unos
                            (
                                fakultet.Studenti[k],
                                ocjena,
                                false
                            );
                            
                            fakultet.Zapisnici[i]->_brojStavki++;
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool ponisti_stavku_rezultat(Fakultet & fakultet, int indeks)
{
    for (int i = Max - 1; i >= 0; i--)
    {
        if (fakultet.Zapisnici[i] != NULL)
        {
            for (int j = 0; j < fakultet.Zapisnici[i]->_brojStavki; j++)
            {
                if (fakultet.Zapisnici[i]->_stavke[j]->_student->_brojIndeksa == indeks)
                {
                    fakultet.Zapisnici[i]->_stavke[j]->_student->UmanjiECTS(fakultet.Zapisnici[i]->_ects);
                    fakultet.Zapisnici[i]->_stavke[j]->_ponisteno = true;
                    return true;
                }
            }
        }
    }

    return false;
}

char * formirajNazivFajla(const Fakultet & fakultet, int i)
{
    char nazivFajla[100];
    strcpy_s(nazivFajla, fakultet.Zapisnici[i]->_predmet);
    strcat_s(nazivFajla, "_");

    char godina[5], mjesec[3], dan[3];
    _itoa_s(fakultet.Zapisnici[i]->_datum._godina, godina, 10);
    _itoa_s(fakultet.Zapisnici[i]->_datum._mjesec, mjesec, 10);
    _itoa_s(fakultet.Zapisnici[i]->_datum._dan, dan, 10);

    strcat_s(nazivFajla, godina);
    strcat_s(nazivFajla, "-");

    if(fakultet.Zapisnici[i]->_datum._mjesec < 10) strcat_s(nazivFajla, "0");
    strcat_s(nazivFajla, mjesec);
    strcat_s(nazivFajla, "-");

    if (fakultet.Zapisnici[i]->_datum._dan < 10) strcat_s(nazivFajla, "0");
    strcat_s(nazivFajla, dan);
    strcat_s(nazivFajla, "-");
    strcat_s(nazivFajla, ".txt");

    int duzina = strlen(nazivFajla) + 1;
    char * pok = new char[duzina];
    strcpy_s(pok, duzina, nazivFajla);
    return pok;
}

void upisi(ofstream & upis, IspitniZapisnik * zapisnik)
{
    upis << crt << "Datum ispita: "; 
    if (zapisnik->_datum._dan < 10) upis << "0" << zapisnik->_datum._dan;
    else upis << zapisnik->_datum._dan;
    upis << "."; 

    if (zapisnik->_datum._mjesec < 10) upis << "0" << zapisnik->_datum._mjesec;
    else upis << zapisnik->_datum._mjesec;

    upis << "." << zapisnik->_datum._godina
         << crt << "Predmet: " << zapisnik->_predmet
         << crt << "IZVJESTAJ O ODRZANOM ISPITU"
         << crt << "Br indeksa Ime prezime Komisijsko Ocjena" << endl;

    int brojac = 0;
    for (int j = 0; j < zapisnik->_brojStavki; j++)
    {
        upis << zapisnik->_stavke[j]->_student->_brojIndeksa << " " 
             << zapisnik->_stavke[j]->_student->_imePrezime  << " ";

        if (zapisnik->_stavke[j]->_komisijskoPolaganje == true) upis << "DA ";
        else upis << "NE ";

        upis << zapisnik->_stavke[j]->_ocjena << " ";
        if (zapisnik->_stavke[j]->_ocjena > 5 && zapisnik->_stavke[j]->_komisijskoPolaganje == true) brojac++;

        if (zapisnik->_stavke[j]->_ponisteno) upis << "Ponisteno ";

        if (j != zapisnik->_brojStavki - 1) upis << endl;
    }

    upis << crt << "UKUPNO POLOZILO: " << brojac << "\n"
         << "PROSJECNA OCJENA: " << zapisnik->GetProsjecnaOcjena() << crt;
}

bool printaj(const Fakultet & fakultet)
{
    for (int i = Max - 1; i >= 0; i--)
    {
        if (fakultet.Zapisnici[i] != NULL)
        {
            if (fakultet.Zapisnici[i]->_brojStavki != 0)
            {
                char * nazivFajla = formirajNazivFajla(fakultet, i);

                ofstream upis;
                upis.open(nazivFajla);
                if (upis.fail())
                    cout << "Greska!" << endl;
                else
                {
                    upisi(upis, fakultet.Zapisnici[i]);
                    upis.close();
                }

               
                ifstream ispis;
                ispis.open(nazivFajla);

                if (ispis.fail())
                    cout << "Greska!" << endl;
                else
                {
                    char znak;
                    while (ispis.get(znak))
                        cout << znak;
                    ispis.close();
                    
		    delete[] nazivFajla; nazivFajla = nullptr;
                    return true;
                }      
            }
        }
    }

    return false;
}

void main()
{
    Fakultet fakultet;

    
    dodaj_student(fakultet, "A. Joldic", 16001);
    dodaj_student(fakultet, "D. Music", 15251);
    dodaj_student(fakultet, "E. Babovic ", 16014);
    dodaj_student(fakultet, "E. Junuz", 16015);
    dodaj_student(fakultet, "J. Azemovic", 16014);

    kreiraj_zapisnik(fakultet, 6, 9, 2017, "Programiranje 2", 7);

    if (dodaj_stavku_na_zapisnik(fakultet, 16001, 5))
        cout << "Stavka (rezultat) uspjesno dodata!" << endl;

    if (!dodaj_stavku_na_zapisnik(fakultet, 16001, 6))
        cout << "Student vec dodat na zapisnik. Ocjena je ispravljena!" << endl;

    dodaj_stavku_na_zapisnik(fakultet, 15251, 5);
    dodaj_stavku_na_zapisnik(fakultet, 16015, 6);
    dodaj_stavku_na_zapisnik(fakultet, 16014, 8);

    kreiraj_zapisnik(fakultet, 7, 9, 2017, "Racunarske mreze", 5);
    dodaj_stavku_na_zapisnik(fakultet, 16015, 5);
    dodaj_stavku_na_zapisnik(fakultet, 16014, 10);



    if (ponisti_stavku_rezultat(fakultet, 16001))
        cout << "Stavka (rezultat) uspjesno ponisten!" << endl;

    if (!ponisti_stavku_rezultat(fakultet, 19001))
        cout << "Student nije pronadjen!" << endl;

    if (printaj(fakultet))
        cout << "Ispitni zapisnik uspjesno prikazan i spasen u fajl!" << endl;
    fakultet.Dealociraj();

    system("pause>0");
}