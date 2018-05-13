#include <iostream>
#include <cstring>
#include <tuple>
#include <iomanip>
 
const char * crt = "\n-------------------------------------------\n";
 
using namespace std;
 
struct Putnik
{
    char * _imePrezime;
    char _oznakaSjedista[4];
};
 
struct Let
{
    char * _relacija;//Mostar -> Sarajevo
    Putnik * _putnici;
    int _trenutnoPutnika;
    int _brojRedovaUAvionu;
    int _brojKolonaUAvionu;//broj kolona mora biti paran >=4 i <=10;
};
 
void Dealociraj(Let & let)
{
    delete[] let._relacija; let._relacija = nullptr;
    for (size_t i = 0; i < let._trenutnoPutnika; i++)
    {
        delete[] let._putnici[i]._imePrezime;
        let._putnici[i]._imePrezime = nullptr;
    }
    delete[] let._putnici;
    let._putnici = nullptr;
}
 
void Alokacija(Let & let)
{
    Putnik * kopija = let._putnici;
    let._putnici = new Putnik[let._trenutnoPutnika + 1];
    for (int i = 0; i < let._trenutnoPutnika; i++)
    {
        //let._putnici[i]._imePrezime = new char[strlen(kopija[i]._imePrezime)];
        let._putnici[i]._imePrezime = kopija[i]._imePrezime;
        strcpy_s(let._putnici[i]._oznakaSjedista, strlen(kopija[i]._oznakaSjedista) + 1, kopija[i]._oznakaSjedista);
    }
   
    delete[] kopija; kopija = nullptr;
}
 
 
void Unos(Let & let, char * relacija, int broj_redova, int broj_kolona)
{
    if (broj_kolona % 2 != 0 && (broj_kolona > 10 || broj_kolona < 4)) return;
   
    let._relacija = new char[strlen(relacija) + 1];
    strcpy_s(let._relacija, strlen(relacija) + 1, relacija);
 
    let._putnici = new Putnik[1];
    //let._putnici[0]._imePrezime = new char[1];
    //let._putnici[0]._imePrezime = nullptr;
 
    let._brojRedovaUAvionu = broj_redova;
    let._brojKolonaUAvionu = broj_kolona;
    let._trenutnoPutnika = 0;
}
 
pair <int, int> GetPozicijuUReduIKoloni(char * pozicija)
{
    int red = 10 * (pozicija[0] - '0') + (pozicija[1] - '0');
    int kolona = int(pozicija[2] - 'A');
 
    return make_pair(red - 1, kolona);
}
 
Putnik GetNovogPutnika(char * sjediste, char * ime_prezime)
{
    Putnik novi;
    novi._imePrezime = new char[strlen(ime_prezime) + 1];
    strcpy_s(novi._imePrezime, strlen(ime_prezime) + 1, ime_prezime);
    strcpy_s(novi._oznakaSjedista, strlen(sjediste) + 1, sjediste);
   
    return novi;
}
 
 
bool DodajPutnika(Let & let, Putnik noviPutnik)
{
    int kolona = GetPozicijuUReduIKoloni(noviPutnik._oznakaSjedista).second;
    // Nepravilno uneseno sjediste
    if (kolona < 0 || kolona > 10) return false;
 
    //popunjen
    if (let._trenutnoPutnika == let._brojKolonaUAvionu * let._brojRedovaUAvionu) return false;
 
    //Registracija za isto ime
    for (int i = 0; i < let._trenutnoPutnika; i++)
        if (strcmp(let._putnici[i]._imePrezime, noviPutnik._imePrezime) == 0) return false;
 
    //Dodavanje novog putnika
    if(let._putnici != nullptr) Alokacija(let);
    let._putnici[let._trenutnoPutnika]._imePrezime = noviPutnik._imePrezime;
    strcpy_s(let._putnici[let._trenutnoPutnika]._oznakaSjedista, noviPutnik._oznakaSjedista);
 
    let._trenutnoPutnika++;
    return true;
}
 
int rekBrojacKaraktera(const Let & let, int brPutnika, int i)
{
    if (i == brPutnika - 1)
        return strlen(let._putnici[i]._imePrezime);
 
    if (strlen(let._putnici[i]._imePrezime) > rekBrojacKaraktera(let, brPutnika, i + 1))
        return strlen(let._putnici[i]._imePrezime);
    else
        return rekBrojacKaraktera(let, brPutnika, i + 1);
}
 
void IspisKolone(const int kolona, const int sirina)
{
    for (int j = 0; j < kolona; j++)
    {
        cout << "+";
        for (int k = 0; k <= sirina; k++)
            cout << "-";
    }
    cout << "+" << endl;
}
 
bool ZauzetoMjesto(const Let & let, const int i, const int j, const int pozicija)
{
    return GetPozicijuUReduIKoloni(let._putnici[pozicija]._oznakaSjedista).first == i &&
           GetPozicijuUReduIKoloni(let._putnici[pozicija]._oznakaSjedista).second == j;
}
 
//char * SlobodnoSjediste(const Let & let) ovo nije koristeno da nebi doslo do curenja memorije
Putnik SlobodnoSjediste(const Let & let)
{
    Putnik temp{ " ", " " };
    for (int i = 0; i < let._brojRedovaUAvionu; i++)
    {
        for (int j = 0; j < let._brojKolonaUAvionu; j++)
        {
            bool slobodno = true;
            for (int k = 0; k < let._trenutnoPutnika; k++)
            {
                if (ZauzetoMjesto(let, i, j, k))
                {
                    slobodno = false;
                    break;
                }
            }
 
            if (slobodno)
            {
                temp._oznakaSjedista[0] = i / 10 + '0';
                temp._oznakaSjedista[1] = i % 10 + 1 + '0';
                temp._oznakaSjedista[2] = j + 'A';
                temp._oznakaSjedista[3] = '\0';
                return temp;
            }
        }    
    }
 
    return temp;
}
 
void PrikaziRasporedSjedenja(const Let & let)
{
    cout << crt << let._relacija << crt;
    int sirina = rekBrojacKaraktera(let, let._trenutnoPutnika, 0) + 1;
    for (int i = 0; i <= let._brojRedovaUAvionu; i++)
    {
        IspisKolone(let._brojKolonaUAvionu, sirina);
 
        if (i < let._brojRedovaUAvionu)
        {
            int j = 0;
            for (int l = 0; l < let._brojKolonaUAvionu * (sirina + 2) + 1; l++)
            {
                if (l % (sirina + 2) == 0 || l == let._brojKolonaUAvionu * (sirina + 2)) cout << "|";
                else
                {
                    bool zauzeto = false;
                    for (int m = 0; m < let._trenutnoPutnika; m++)
                    {
                        if (ZauzetoMjesto(let, i, j, m))
                        {
                            cout << right << setw(sirina + 1) << let._putnici[m]._imePrezime << left;
                            zauzeto = true;
                            break;
                        }
                    }
 
                    if (!zauzeto)
                        for (int n = 0; n < sirina + 1; n++)
                            cout << " ";
                    j++;
                    l += sirina;
                }
            }
            cout << endl;
        }
    }
}
 
int main()
{
 
    int oznakaR = 0, oznakaK = 0;
    Let mostar_sarajevo;
 
    
    Unos(mostar_sarajevo, "Mostar -> Sarajevo", 10, 4); 
 
    
    tie(oznakaR, oznakaK) = GetPozicijuUReduIKoloni("15B");
    cout << crt << "GetPozicijuUReduIKoloni(15B) ->" << oznakaR << "/" << oznakaK << endl;//ispisuje 14/1
 
    tie(oznakaR, oznakaK) = GetPozicijuUReduIKoloni("01B");
    cout << crt << "GetPozicijuUReduIKoloni(01B) ->" << oznakaR << "/" << oznakaK << crt;//ispisuje 0/1

 
    if (DodajPutnika(mostar_sarajevo, GetNovogPutnika("01A", "Denis Music")))
        cout << crt << "Putnik uspjesno dodan!" << crt;
    if (DodajPutnika(mostar_sarajevo, GetNovogPutnika("07C", "Zanin Vejzovic")))
        cout << crt << "Putnik uspjesno dodan!" << crt;
    if (DodajPutnika(mostar_sarajevo, GetNovogPutnika("10D", "Adel Handzic")))
        cout << crt << "Putnik uspjesno dodan!" << crt;
   
    PrikaziRasporedSjedenja(mostar_sarajevo);
 
 
    cout << crt << "Maksimalan broj karaktera u imenu i prezimenu putnika je ->"
         << rekBrojacKaraktera(mostar_sarajevo, mostar_sarajevo._trenutnoPutnika, 0)
         << crt;
 
    if (mostar_sarajevo._trenutnoPutnika != mostar_sarajevo._brojKolonaUAvionu * mostar_sarajevo._brojRedovaUAvionu)
        cout << crt << "Sljedece slobodno sjediste -> " << SlobodnoSjediste(mostar_sarajevo)._oznakaSjedista << crt;
    else
        cout << crt << "Sva sjedista su zauzeta!" << crt;
 
    Dealociraj(mostar_sarajevo);
 
    system("pause>0");
    return 0;
}
