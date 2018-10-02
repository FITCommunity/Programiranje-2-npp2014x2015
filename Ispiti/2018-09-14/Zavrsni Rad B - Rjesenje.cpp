#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

const char * not_set = "NEMA_VRIJEDNOST";
const char * crt = "\n-------------------------------------------\n";
const int min_polgavlja = 3;
const int max_zavrsnih = 30;
const char * zabranjeneRijeci[] = { "ubiti","mrziti", "NC", "RE" };

char * AlocirajNizKaraktera(const char * sadrzaj) 
{
    if (sadrzaj == nullptr)
        return nullptr;
    int vel = strlen(sadrzaj) + 1;
    char * temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    return temp;
}

struct Poglavlje 
{
    char * _naslov;
    char * _sadrzaj;
    bool _prihvaceno;
    int _ocjena;//da bi se poglavlje smatralo prihvacenim ocjena mora biti u opsegu od 6 - 10
    void Unos(const char * naslov = nullptr, const char * sadrzaj = nullptr) 
    {
        _ocjena = 0; _prihvaceno = false;
        _naslov = AlocirajNizKaraktera(naslov);
        _sadrzaj = AlocirajNizKaraktera(sadrzaj);
    }
    void Dealociraj() 
    {
        delete[] _naslov; _naslov = nullptr;
        delete[] _sadrzaj; _sadrzaj = nullptr;
    }
    void Ispis() 
    {
        if (_naslov == nullptr || _sadrzaj == nullptr) return;
        cout << endl << "Poglavlje -> " << _naslov << endl << _sadrzaj << endl;
        if (_prihvaceno) cout << "Ocjena: " << _ocjena << endl;;
    }
    void OcijeniPoglavlje(int ocjena) 
    {
        _ocjena = ocjena;
        if (_ocjena > 5 && ocjena <= 10) _prihvaceno = true;
    }
};

struct ZavrsniRad 
{
    char _brojIndeksa[15];
    char * _tema;
    shared_ptr<Poglavlje> * _poglavljaRada;
    int _trenutnoPoglavlja;
    char * _datumOdbrane;
    float _konacnaOcjena; //konacna ocjena zavrsnog rada predstavlja prosjek ocjena svih poglavlja u zavrsnom radu koja se izracunava u momentu zakazivanja odbrane

    void DodajPoglavlje(const char *nazivPoglavlja, const char *sadrzajPoglavlja);
    void OcijeniPoglavlje(const char *nazivPoglavlja, int ocjena);
    void PostaviKonacnuOcjenu();
    void Unos(const char * brojIndeksa, const char * nazivTeme = nullptr) 
    {
        _konacnaOcjena = 0;
        strcpy_s(_brojIndeksa, 15, brojIndeksa);
        _tema = AlocirajNizKaraktera(nazivTeme);
        _trenutnoPoglavlja = 0;
        _datumOdbrane = AlocirajNizKaraktera(not_set);
        _poglavljaRada = nullptr;
    }
    void Dealociraj() 
    {
        delete[] _tema; _tema = nullptr;
        delete[] _datumOdbrane; _datumOdbrane = nullptr;
        for (size_t i = 0; i < _trenutnoPoglavlja; i++)
            _poglavljaRada[i]->Dealociraj();
        delete[] _poglavljaRada; _poglavljaRada = nullptr;
    }
    void Ispis() 
    {
        cout << "Tema rada: " << _tema << endl;
        cout << "Sadrzaj: " << endl;
        for (size_t i = 0; i < _trenutnoPoglavlja; i++)
            _poglavljaRada[i]->Ispis();
        cout << "Datum odbrane rada: " << _datumOdbrane << endl << " Ocjena: " << _konacnaOcjena << endl;
    }
};
struct Nastavnik 
{
    char * _imePrezime;
    shared_ptr<ZavrsniRad> _teme[max_zavrsnih] = { nullptr };

    bool DodajZavrsniRad(ZavrsniRad zavrsniRad);
    ZavrsniRad *ZakaziOdbranuRada(const char *indeks, const char *datum);
    float GetProsjecnuOcjenu();
    void Unos(const char * imePrezime)
    {
        _imePrezime = AlocirajNizKaraktera(imePrezime);
    }
    void Dealociraj() 
    {
        delete[] _imePrezime; _imePrezime = nullptr;
        for (size_t i = 0; i < max_zavrsnih; i++)
            if (_teme[i] != nullptr)
                _teme[i]->Dealociraj();
    }
    void Ispis()
    {
        cout << crt << _imePrezime << crt;
        for (size_t i = 0; i < max_zavrsnih; i++)
            if (_teme[i] != nullptr)
                _teme[i]->Ispis();
    }
};

void ZavrsniRad::DodajPoglavlje(const char *nazivPoglavlja, const char *sadrzajPoglavlja)
{
    if (!nazivPoglavlja || !sadrzajPoglavlja) return;

    for (int i = 0; i < _trenutnoPoglavlja; i++)
    {
        if (strcmp(_poglavljaRada[i]->_naslov, nazivPoglavlja) == 0)
        {
            if (_poglavljaRada[i]->_sadrzaj)
            {
                int duzina = strlen(_poglavljaRada[i]->_sadrzaj) + strlen(sadrzajPoglavlja) + 2;
                char *temp = _poglavljaRada[i]->_sadrzaj;
                _poglavljaRada[i]->_sadrzaj = new char[duzina];

                strcpy_s(_poglavljaRada[i]->_sadrzaj, duzina, temp);
                strcat_s(_poglavljaRada[i]->_sadrzaj, duzina, " ");
                strcat_s(_poglavljaRada[i]->_sadrzaj, duzina, sadrzajPoglavlja);

                delete[] temp; temp = nullptr;
            }
            else _poglavljaRada[i]->_sadrzaj = AlocirajNizKaraktera(sadrzajPoglavlja);

            return;
        }
    }

    shared_ptr<Poglavlje> *temp = _poglavljaRada;
    _poglavljaRada = new shared_ptr<Poglavlje>[_trenutnoPoglavlja + 1];
    for (int i = 0; i < _trenutnoPoglavlja; i++)
        _poglavljaRada[i] = temp[i];
    _poglavljaRada[_trenutnoPoglavlja] = make_shared<Poglavlje>();
    _poglavljaRada[_trenutnoPoglavlja]->Unos(nazivPoglavlja, sadrzajPoglavlja);
    _trenutnoPoglavlja++;   
}

void ZavrsniRad::OcijeniPoglavlje(const char *nazivPoglavlja, int ocjena)
{
    for (int i = 0; i < _trenutnoPoglavlja; i++)
        if (strcmp(_poglavljaRada[i]->_naslov, nazivPoglavlja) == 0)
            _poglavljaRada[i]->OcijeniPoglavlje(ocjena);
}

bool Nastavnik::DodajZavrsniRad(ZavrsniRad zavrsniRad)
{
    for (int i = 0; i < max_zavrsnih; i++)
    {
        if (_teme[i])
        {
            if (strcmp(_teme[i]->_tema, zavrsniRad._tema) == 0) return false;
            if (strcmp(_teme[i]->_brojIndeksa, zavrsniRad._brojIndeksa) == 0) return false;
        }
        else
        {
            _teme[i] = make_shared<ZavrsniRad>();
            _teme[i]->Unos(zavrsniRad._brojIndeksa, zavrsniRad._tema);
            for (int j = 0; j < zavrsniRad._trenutnoPoglavlja; j++)
            {
                _teme[i]->DodajPoglavlje(zavrsniRad._poglavljaRada[j]->_naslov, zavrsniRad._poglavljaRada[j]->_sadrzaj);
                _teme[i]->OcijeniPoglavlje(zavrsniRad._poglavljaRada[j]->_naslov, zavrsniRad._poglavljaRada[j]->_ocjena);
            }

            return true;
        }
    }

    return false;
}

void ZavrsniRad::PostaviKonacnuOcjenu()
{
    float suma = 0;
    for (int i = 0; i < _trenutnoPoglavlja; i++)
        suma += _poglavljaRada[i]->_ocjena;

    _konacnaOcjena = _trenutnoPoglavlja ? suma / _trenutnoPoglavlja : 0;
}

bool ProvjeriSadrzaj(const char *sadrzaj)
{
    int brojRijeci = sizeof(zabranjeneRijeci) / sizeof(zabranjeneRijeci[0]);
    for (int i = 0; i < brojRijeci; i++)
        if (strstr(sadrzaj, zabranjeneRijeci[i]) == 0)
            return false;
    return true;
}

ZavrsniRad *Nastavnik::ZakaziOdbranuRada(const char *indeks, const char *datum)
{
    for (int i = 0; i < max_zavrsnih; i++)
    {
        if (_teme[i])
        {
            if (strcmp(_teme[i]->_brojIndeksa, indeks) == 0)
            {
                if (_teme[i]->_trenutnoPoglavlja < min_polgavlja) return nullptr;

                for (int j = 0; j < _teme[i]->_trenutnoPoglavlja; j++)
                    if (!ProvjeriSadrzaj(_teme[i]->_poglavljaRada[j]->_sadrzaj) ||
                        !_teme[i]->_poglavljaRada[j]->_prihvaceno) return nullptr;

                _teme[i]->PostaviKonacnuOcjenu();
                _teme[i]->_datumOdbrane = AlocirajNizKaraktera(datum);

                return &*_teme[i];
            }
        }
        else return nullptr;
    }

    return nullptr;
}

float Nastavnik::GetProsjecnuOcjenu()
{
    float suma = 0;
    int brojRadova = 0;
    for (int i = 0; i < max_zavrsnih; i++)
    {
        if (_teme[i])
        {
            if (_teme[i]->_konacnaOcjena)
            {
                suma += _teme[i]->_konacnaOcjena;
                brojRadova++;
            }
        }
        else break;
    }

    return brojRadova ? suma / brojRadova : 0;
}

tuple<char *, float, float> PronadjiStudenta(Nastavnik *nastavnici, int brojNastavnika)
{
    int indeksNastavnika = 0;
    float prosjekNastavnika = nastavnici[indeksNastavnika].GetProsjecnuOcjenu();
    for (int i = 1; i < brojNastavnika; i++)
    {
        float tempProsjek = nastavnici[i].GetProsjecnuOcjenu();
        if (tempProsjek > prosjekNastavnika)
        {
            indeksNastavnika = i;
            prosjekNastavnika = tempProsjek;
        }
    }

    int indeksStudenta = 0;
    int brojPoglavlja = 0;
    for (int i = 0; i < max_zavrsnih; i++)
    {
        if (nastavnici[indeksNastavnika]._teme[i])
        {
            int brojNeodobrenih = 0;
            for (int j = 0; j < nastavnici[indeksNastavnika]._teme[i]->_trenutnoPoglavlja; j++)
                if (!nastavnici[indeksNastavnika]._teme[i]->_poglavljaRada[j]->_prihvaceno)
                    brojNeodobrenih++;

            if (brojNeodobrenih > brojPoglavlja)
            {
                brojPoglavlja = brojNeodobrenih;
                indeksStudenta = i;
            }
        }
    }
           
    return !prosjekNastavnika ? make_tuple(nullptr, 0, 0)
                              : make_tuple
                                (
                                    nastavnici[indeksNastavnika]._teme[indeksStudenta]->_brojIndeksa, 
                                    brojPoglavlja, 
                                    prosjekNastavnika
                                );
}

int main()
{
    const int max = 2;
    Nastavnik * nastavnici = new Nastavnik[max];

    nastavnici[0].Unos("Denis Music");
    nastavnici[1].Unos("Emina Junuz");

    ZavrsniRad multimedijalni;
    //parametri: brojIndeksa, tema
    multimedijalni.Unos("IB120021", "Multimedijalni informacijski sistem za visoko - obrazovnu ustanovu");
    ZavrsniRad podrsa_operaterima;
    podrsa_operaterima.Unos("IB130031", "Sistem za podršku rada kablovskog operatera");
    ZavrsniRad analiza_sigurnosti;
    analiza_sigurnosti.Unos("IB140041", "Prakticna analiza sigurnosti bežienih raeunarskih mreža");
    ZavrsniRad kriptografija;
    kriptografija.Unos("IB120021", "Primjena teorije informacija u procesu generisanja kriptografskih kljueeva");

    /*u zavrsni rad dodaje novo poglavlje i njegov sadrzaj. ukoliko poglavlje vec postoji u zavrsnom radu, funkcija tom poglavlju treba dodati novi sadrzaj i pri tome zadrzi postojeci (izmedju postojeceg i novog sadrzaja se dodaje prazan prostor). u slucaju da poglavlje ne postoji, ono se dodaje zajedno sa sadrzajem*/
    //parametri: nazivPoglavlja, sadrzajPoglavlja

    multimedijalni.DodajPoglavlje("Uvod", "U ovom poglavlju ce biti rijeci");
    multimedijalni.DodajPoglavlje("Uvod", "o multimedijalnim sistemima koji se danas koriste");
    multimedijalni.DodajPoglavlje("Uvod", "u savremenom poslovanju");
    multimedijalni.DodajPoglavlje("Vrste multimedijalnih sistema", "Danas se moze govoriti o nekoliko vrsta multimedijalnih sistema, a neke od najznacajnijih su ...");
    multimedijalni.DodajPoglavlje("Teorija multimedije", "Sadrzaj koji bi trebao stajati na pocetku treceg poglavlja zavrsnog rada o multimediji studenta IB130011");

    //parametri: nazivPoglavlja, ocjena
    multimedijalni.OcijeniPoglavlje("Uvod", 8);
    multimedijalni.OcijeniPoglavlje("Vrste multimedijalnih sistema", 8);
    multimedijalni.OcijeniPoglavlje("Teorija multimedije", 9);


    /*funkcija DodajZavrsniRad ima zadatak da odredjenom nastavniku dodijeli mentorstvo na zavrsnom radu.
    sprijeciti dodavanje zavrsnih radova sa istom temom kao i mogucnost da jedan student kod istog nastavnika
    posjeduje vise zavrsnih radova*/
    if (nastavnici[0].DodajZavrsniRad(multimedijalni))
        cout << "Zavrsni rad uspjesno dodat!" << endl;
    if (nastavnici[0].DodajZavrsniRad(podrsa_operaterima))
        cout << "Zavrsni rad uspjesno dodat!" << endl;
    if (!nastavnici[0].DodajZavrsniRad(podrsa_operaterima))//dupliranje rada, onemoguciti dodavanje
        cout << "Zavrsni rad nije dodat!" << endl;
    if (!nastavnici[0].DodajZavrsniRad(kriptografija)) //studentu vec dodijeljen rad, onemoguciti dodavanje
        cout << "Zavrsni rad nije dodat!" << endl;
    if (nastavnici[1].DodajZavrsniRad(analiza_sigurnosti))
        cout << "Zavrsni rad uspjesno dodat!" << endl;


    /*funkcija ZakaziOdbranuRada ima zadatak da studentu sa proslijedjenim brojem indeksa zakaze odbranu zavrsnog rada sto podrazumijeva
    izracunavanje konacne ocjene i definisanje datuma odbrane. odbrana rada se moze zakazati samo studentu koji je rad prethodno prijavio.
    Za zakazivanje odbrane rada moraju biti zadovoljeni sljedeci uslovi:
    1. zavrsni rad mora imati broj poglavlja veci od minimalnog
    2. sadrzaj bilo kojeg poglavlja u zavrsnom radu ne smije sadrzavati neku od zabranjenih rijeci (definisanih u nizu zabranjeneRijeci)
    3. svako poglavlje mora biti prihvaceno/odobreno
    ukoliko su zadovoljeni prethodni kriteriji, izracunava se konacna ocjena rada (prosjek ocjena svih poglavlja), postavlja datum odbrane rada i vraca pokazivac na rad kome je zakazana odbrana.
    u slucaju da student sa primljenim brojem indeksa nije prijavio zavrsni rad ili neki od postavljenih kriterija nije zadovoljen, funkcija vraca nullptr.
    */

    //paramteri: brojIndeksa, datumOdbrane
    ZavrsniRad * zr1 = nastavnici[0].ZakaziOdbranuRada("IB120021", "25.09.2018");
    if (zr1 != nullptr)
        zr1->Ispis();

    zr1 = nastavnici[0].ZakaziOdbranuRada("IB180081", "25.09.2018");//student sa brojem indeksa IB180081 jos uvijek nije prijavio rad
    if (zr1 == nullptr)
        cout << "Odbrana ne moze biti zakazana!" << endl;

    //ispisuje sve podatke o nastavniku i njegovim mentorstvima
    nastavnici[0].Ispis();
    nastavnici[1].Ispis();

    /*funkcija PronadjiNajStudenta ima zadatak da pronadje studenta koji kod nastavnika/mentora sa najvecom prosjecnom ocjenom radova
    (sumaSvihOcjenaZavrsnihRadova/brojZavrsnihRadova) ima najvise neodobrenih (negativno ocijenjenih) poglavlja*/
    int brojPoglavlja = -1;
    float prosjekMentora = 0;
    char * indeksStudenta;
    //parametri PronadjiStudenta: nastavnici, brojNastavnika
    tie(indeksStudenta, brojPoglavlja, prosjekMentora) = PronadjiStudenta(nastavnici, max);
    if (indeksStudenta != nullptr)
        cout << crt << indeksStudenta << " " << prosjekMentora << " " << brojPoglavlja << crt;

    for (int i = 0; i < max; i++)
        nastavnici[i].Dealociraj();
    delete[]nastavnici;
    nastavnici = nullptr;

    system("pause>0");
    return 0;
}