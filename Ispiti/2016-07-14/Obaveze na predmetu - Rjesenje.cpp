#include <iostream>
#include <fstream>

using namespace std;

enum VrstaObaveze { Seminarski, Parcijalni1, Parcijalni2, Integralni, Prakticni };
const char * vrstaObavezeIspis[]{ "Seminarski", "Parcijalni1", "Parcijalni2", "Integralni", "Prakticni" };

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
    void Dealociraj() { delete _dan; delete _mjesec; delete _godina; }
    int DatumUDane() { return *_dan + 30 * *_mjesec + 365 * *_godina; }
};
struct ObavezeNaPredmetu
{
    VrstaObaveze * _vrstaObaveze;
    Datum * _datumIzvrsenja;
    char * _napomena;
    int _ocjena; // 5 - 10 
    void Unos(VrstaObaveze vrsta, Datum * datum, int ocjena, char * napomena)
    {
        _vrstaObaveze = new VrstaObaveze(vrsta);
        _datumIzvrsenja = new Datum;
        _datumIzvrsenja->Unos(*datum->_dan, *datum->_mjesec, *datum->_godina);
        _ocjena = ocjena;
        int size = strlen(napomena) + 1;
        _napomena = new char[size];
        strcpy_s(_napomena, size, napomena);
    }
    void Ispis()
    {
        cout << vrstaObavezeIspis[*_vrstaObaveze] << " " << _ocjena << " " << _napomena << " ";
        _datumIzvrsenja->Ispis();
        cout << endl;
    }
    void Dealociraj()
    {
        _datumIzvrsenja->Dealociraj();
        delete _vrstaObaveze; _vrstaObaveze = nullptr;
        delete _datumIzvrsenja; _datumIzvrsenja = nullptr;
        delete[] _napomena; _napomena = nullptr;
    }
};
struct PolozeniPredmet
{
    Datum _datumPolaganja;/*datum koji ce se evidentirati kao datum kada je predmet polozen tj. kada je formirana konacna ocjena*/
    char * _nazivPredmeta;
    ObavezeNaPredmetu * _listaIzvrsenihObaveza[10];
    int _trenutnoIzvrsenihObaveza;
    int _konacnaOcjena; //formira se na osnovu ocjena izvrsenih obaveza

    void Unos(const char * naziv)
    {
        int duzina = strlen(naziv) + 1;
        _nazivPredmeta = new char[duzina];
        strcpy_s(_nazivPredmeta, duzina, naziv);
        for (size_t i = 0; i < 10; i++)
        {
            _listaIzvrsenihObaveza[i] = nullptr;
        }
        _trenutnoIzvrsenihObaveza = 0;
    }

    void Dealociraj()
    {
        delete[] _nazivPredmeta; _nazivPredmeta = nullptr;
        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
        {
            _listaIzvrsenihObaveza[i]->Dealociraj();
            _listaIzvrsenihObaveza[i] = nullptr;
        }
    }

    bool DodajIzvrsenuObavezu(VrstaObaveze vrsta, Datum datum, int ocjena, char * napomena)
    {
        if (_trenutnoIzvrsenihObaveza == 10) return false;

        if (ocjena < 5 || ocjena > 10) return false;

        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
        {
            if (vrsta == Seminarski)
                break;

            bool identicnaVrsta = vrsta == *_listaIzvrsenihObaveza[i]->_vrstaObaveze;
            bool prolaznaOcjena = _listaIzvrsenihObaveza[i]->_ocjena > 5;
            
            if (identicnaVrsta && prolaznaOcjena)
                return false;
        }


        if (_trenutnoIzvrsenihObaveza > 0)
        {
            if (datum.DatumUDane() - _listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza - 1]->_datumIzvrsenja->DatumUDane() < 7)
                return false;
        }

        _listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza] = new ObavezeNaPredmetu;
        _listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza]->Unos(vrsta, &datum, ocjena, napomena);
        _trenutnoIzvrsenihObaveza++;
        return true;
    }

    int GetPozicijuOcjene(VrstaObaveze vrsta)
    {
        if (vrsta == Seminarski)
            return -1;

        for (int i = _trenutnoIzvrsenihObaveza - 1; i >= 0; i--)
            if (vrsta == *_listaIzvrsenihObaveza[i]->_vrstaObaveze && _listaIzvrsenihObaveza[i]->_ocjena > 5)
                return i;

        return -1;
    }

    bool ProvjeriNapomene()
    {
        int brojac = 0;
        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
        {
            bool prepisivo = strstr(_listaIzvrsenihObaveza[i]->_napomena, "prepisivao") != 0;
            bool izbacen = strstr(_listaIzvrsenihObaveza[i]->_napomena, "izbacen") != 0;

            if (prepisivo || izbacen) brojac++;

            if (brojac > 1) return false;
        }

        return true;
    }

    int FormirajKonacnuOcjenu()
    {
        if (ProvjeriNapomene()) return (_konacnaOcjena = 5);

        int indeks;
        if ((indeks = GetPozicijuOcjene(Integralni)) == -1)
        {
            if ((indeks = GetPozicijuOcjene(Parcijalni1)) != -1)
            {
                int suma = _listaIzvrsenihObaveza[indeks]->_ocjena;
                int brojac = 1;

                if ((indeks = GetPozicijuOcjene(Parcijalni2)) != -1)
                {
                    suma += _listaIzvrsenihObaveza[indeks]->_ocjena;
                    brojac++;

                    if ((indeks = GetPozicijuOcjene(Prakticni)) != -1)
                    {
                        suma += _listaIzvrsenihObaveza[indeks]->_ocjena;
                        brojac++;

                        int brojacSeminarskih = 0;
                        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
                        {
                            if (*_listaIzvrsenihObaveza[i]->_vrstaObaveze == Seminarski && _listaIzvrsenihObaveza[i]->_ocjena > 5)
                            {
                                suma += _listaIzvrsenihObaveza[i]->_ocjena;
                                brojacSeminarskih++;
                            }
                        }

                        if (brojacSeminarskih >= 2)
                        {
                            
                            for (int i = _trenutnoIzvrsenihObaveza - 1; i >= 0; i--)
                            {
                                if (_listaIzvrsenihObaveza[i]->_ocjena > 5)
                                {
                                    _datumPolaganja.Unos
                                    (
                                        *_listaIzvrsenihObaveza[i]->_datumIzvrsenja->_dan,
                                        *_listaIzvrsenihObaveza[i]->_datumIzvrsenja->_mjesec,
                                        *_listaIzvrsenihObaveza[i]->_datumIzvrsenja->_godina
                                    );
                                    return (_konacnaOcjena = suma / (brojac + brojacSeminarskih) );
                                }
                            }
                        }     
                    }
                }
            }

            return (_konacnaOcjena = 5);  
        }
        else
        {
            
            _datumPolaganja.Unos
            (
                *_listaIzvrsenihObaveza[indeks]->_datumIzvrsenja->_dan,
                *_listaIzvrsenihObaveza[indeks]->_datumIzvrsenja->_mjesec,
                *_listaIzvrsenihObaveza[indeks]->_datumIzvrsenja->_godina
            );

            return (_konacnaOcjena = _listaIzvrsenihObaveza[indeks]->_ocjena);
        }
    }

    void Ispis()
    {
        cout << "Naziv predmeta: " << _nazivPredmeta << endl;
        cout << "Polozen: "; _datumPolaganja.Ispis();
        for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
            _listaIzvrsenihObaveza[i]->Ispis();
    }
};

char * FormirajNazivFajla(PolozeniPredmet predmet)
{
    int duzina = strlen(predmet._nazivPredmeta) + 4;
    char * nazivFajla = new char[duzina];

    char broj[2];
    _itoa_s(predmet._konacnaOcjena, broj, 10);
    
    strcpy_s(nazivFajla, duzina, predmet._nazivPredmeta);
    strcat_s(nazivFajla, duzina, "_");
    strcat_s(nazivFajla, duzina, broj);

    return nazivFajla;
}

void UpisiUBinarniFajl(PolozeniPredmet predmet)
{
    char * nazivFajla = FormirajNazivFajla(predmet);
    ofstream upis(nazivFajla, ios::binary);

    if (upis.fail())
        cout << "Greska!" << endl;
    else
    {
        upis.write((char*)(&predmet), sizeof(predmet));
        upis.close();
    }

    delete[] nazivFajla; nazivFajla = nullptr;
}

void UcitajIzBinarnogFajla(PolozeniPredmet & predmet, const char * nazivFajla)
{
    ifstream ispis(nazivFajla, ios::binary);

    if (ispis.fail())
        cout << "Greska!" << endl;
    else
    {
        ispis.read((char*)(&predmet), sizeof(predmet));
        ispis.close();
    }
}

int PretragaRekurzivno(PolozeniPredmet predmet, int i, int suma)
{
    if (i == predmet._trenutnoIzvrsenihObaveza - 1)
    {
        if (predmet._listaIzvrsenihObaveza[i]->_ocjena == 5)
            return suma + 1;
        else
            return suma;
    }

    if (predmet._listaIzvrsenihObaveza[i]->_ocjena == 5)
        return PretragaRekurzivno(predmet, i + 1, suma + 1);
    else
        return PretragaRekurzivno(predmet, i + 1, suma);
}

void main()
{
    Datum datumSeminarski1, datumSeminarski2, datumParcijalni1, datumParcijalni2, datumIntegralni;
    datumSeminarski1.Unos(10, 6, 2016);
    datumSeminarski2.Unos(18, 6, 2016);
    datumParcijalni1.Unos(22, 3, 2016);
    datumParcijalni2.Unos(22, 6, 2016);

    //polozeni predmet
    PolozeniPredmet prII, prII_FromFile;
    prII.Unos("Programiranje II");
    if (prII.DodajIzvrsenuObavezu(Parcijalni1, datumParcijalni1, 6, "uslovno polozen"))
        cout << "Parcijalni 1...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 5, "previse gresaka"))
        cout << "Parcijalni 2...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski1, 8, "implementirani svi algoritmi"))
        cout << "Seminarski1...dodan" << endl;
    if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski2, 6, "rad slican kao kod studenta IB150388"))
        cout << "Seminarski2...dodan" << endl;

    cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

    datumParcijalni2.Unos(22, 7, 2016);
    if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 6, ""))
        cout << "Parcijalni 2...dodan" << endl;
    cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

    datumIntegralni.Unos(22, 9, 2022);
    if (prII.DodajIzvrsenuObavezu(Prakticni, datumIntegralni, 7, ""))
        cout << "Integralni...dodan" << endl;
    cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

    //rekurzija
    cout << "Broj negativnih ocjena je: " << PretragaRekurzivno(prII, 0, 0) << endl;
    //manipulacija fajlovima
    UpisiUBinarniFajl(prII);
    prII.Ispis();
    UcitajIzBinarnogFajla(prII_FromFile, "Programiranje II_6");
    prII_FromFile.Ispis();
    
    datumParcijalni1.Dealociraj();
    datumParcijalni2.Dealociraj();
    datumSeminarski1.Dealociraj();
    datumSeminarski2.Dealociraj();
    datumIntegralni.Dealociraj();
    
    prII.Dealociraj();
    
    system("pause");
}