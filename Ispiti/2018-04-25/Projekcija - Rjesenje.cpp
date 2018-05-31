#include <iostream>
#include <tuple>
#include <cstring>
#include <iomanip>

using namespace std;

const char * crt = "\n-------------------------------------\n";
const int brojRedova = 10;

struct Posjetitelj
{
    char * _imePrezime;
    char _ozankaSjedista[5];
};

struct Projekcija
{
    char * _film;
    Posjetitelj * _posjetitelji[brojRedova];
    int _brojPosjetiteljaURedu[brojRedova];
    int _brojKolonaUSali;
};

void Dealociraj(Projekcija & projekcija)
{
    delete[] projekcija._film; projekcija._film = nullptr;
    for (size_t i = 0; i < brojRedova; i++)
    {
        for (size_t j = 0; j < projekcija._brojPosjetiteljaURedu[i]; j++)
        {
            delete[] projekcija._posjetitelji[i][j]._imePrezime;
            projekcija._posjetitelji[i][j]._imePrezime = nullptr;
        }
        delete[] projekcija._posjetitelji[i];
        projekcija._posjetitelji[i] = nullptr;
    }
}

void Alociraj(Projekcija & projekcija, int red)
{
    Posjetitelj * temp = projekcija._posjetitelji[red];
    projekcija._posjetitelji[red] = new Posjetitelj[projekcija._brojPosjetiteljaURedu[red] + 1];
    
    for (int i = 0; i < projekcija._brojPosjetiteljaURedu[red]; i++)
    {
        projekcija._posjetitelji[red][i]._imePrezime = temp[i]._imePrezime;
        strcpy_s(projekcija._posjetitelji[red][i]._ozankaSjedista, 5, temp[i]._ozankaSjedista);
    }
}

void Unos(Projekcija & projekcija, const char * film, int broj_kolona)
{
    if (broj_kolona < 5 || broj_kolona > 10 || broj_kolona % 2 != 0)
        return;

    int duzina = strlen(film) + 1;
    projekcija._film = new char[duzina];
    strcpy_s(projekcija._film, duzina, film);

    projekcija._brojKolonaUSali = broj_kolona;

    for (int i = 0; i < brojRedova; i++)
    {
        projekcija._posjetitelji[i] = nullptr;
        projekcija._brojPosjetiteljaURedu[i] = 0;
    }
}

Posjetitelj GetNovogPosjetitelja(const char * sjediste, const char * imePrezime)
{
    Posjetitelj temp = { " ", " " };
    int duzina = strlen(imePrezime) + 1;
    temp._imePrezime = new char[duzina];
    strcpy_s(temp._imePrezime, duzina, imePrezime);
    strcpy_s(temp._ozankaSjedista, 5, sjediste);

    return temp;
}

pair<int, int> GetPozicijuUReduIKoloni(const char * sjediste)
{
    int red = sjediste[0] - 'A',
        kolona = 10 * (sjediste[2] - 48) + sjediste[3] - 48 - 1;

    return { red, kolona };
}

bool DodajPosjetitelja(Projekcija & projekcija, Posjetitelj posjetitelj)
{
    int red, kolona;
    tie(red, kolona) = GetPozicijuUReduIKoloni(posjetitelj._ozankaSjedista);

    if (red < 0 || red > 10 || kolona < 0 || kolona > projekcija._brojKolonaUSali) return false;

    if (projekcija._brojPosjetiteljaURedu[red] == projekcija._brojKolonaUSali) return false;

    int suma = 0;
    for (int i = 0; i < brojRedova; i++)
        suma += projekcija._brojPosjetiteljaURedu[i];
    if (suma == brojRedova * projekcija._brojKolonaUSali) return false;

    for (int i = 0; i < brojRedova; i++)
        for (int j = 0; j < projekcija._brojPosjetiteljaURedu[i]; j++)
            if (projekcija._posjetitelji[i][j]._imePrezime != nullptr)
                if (!strcmp(projekcija._posjetitelji[i][j]._imePrezime, posjetitelj._imePrezime))
                    return false;


    Alociraj(projekcija, red);
    projekcija._posjetitelji[red][projekcija._brojPosjetiteljaURedu[red]]._imePrezime = posjetitelj._imePrezime;
    strcpy_s(projekcija._posjetitelji[red][projekcija._brojPosjetiteljaURedu[red]]._ozankaSjedista, posjetitelj._ozankaSjedista);
    projekcija._brojPosjetiteljaURedu[red]++;

    return true;
}

int najduzeImePrezime(const Projekcija & projekcija)
{
    int max = 0;
    int duzina;
    for (int i = 0; i < brojRedova; i++)
    {
        for (int j = 0; j < projekcija._brojPosjetiteljaURedu[i]; i++)
        {
            if (projekcija._posjetitelji[i][j]._imePrezime != nullptr)
            {
                duzina = strlen(projekcija._posjetitelji[i][j]._imePrezime);
                if (duzina > max) max = duzina;
            }
        }
    }


    return max;
}


bool ZauzetoMjesto(const Projekcija & projekcija, const int i, const int j, const int pozicija)
{
    return GetPozicijuUReduIKoloni(projekcija._posjetitelji[i][pozicija]._ozankaSjedista).second == j;
}

char * SlobodnoSjediste(const Projekcija & projekcija)
{
    char * slobodno = new char[5];

    int suma = 0;
    for (int i = 0; i < brojRedova; i++)
        suma += projekcija._brojPosjetiteljaURedu[i];
    if (suma == brojRedova * projekcija._brojKolonaUSali) return nullptr;

    for (int i = 0; i < brojRedova; i++)
    {
        for (int j = 0; j < projekcija._brojKolonaUSali; j++)
        {
            bool slobodnoMjesto = true;
            for (int k = 0; k < projekcija._brojPosjetiteljaURedu[i]; k++)
            {
                if (ZauzetoMjesto(projekcija, i, j, k))
                {
                    slobodnoMjesto = false;
                    break;
                }
            }

            if (slobodnoMjesto)
            {
                if (projekcija._brojPosjetiteljaURedu[i] == brojRedova) { i++; j = 0; }

                slobodno[0] = i + 'A';
                slobodno[1] = '-';
                slobodno[2] = j / 10 + '0';
                slobodno[3] = j % 10 + 1 + '0';
                slobodno[4] = '\0';
                return slobodno;
            }
        }
    }

    return nullptr;

}


void PrikaziRasporedSjedenja(const Projekcija & projekcija)
{
    int sirina = najduzeImePrezime(projekcija);
    for (int i = 0; i <= brojRedova; i++)
    {

        for (int j = 0; j < projekcija._brojKolonaUSali; j++)
        {
            cout << "+";
            for (int k = 0; k <= sirina; k++)
                cout << "-";
        }
        cout << "+" << endl;

        if (i < brojRedova)
        {

            int j = 0;
            for (int l = 0; l < projekcija._brojKolonaUSali * (sirina + 2) + 1; l++)
            {

                if (l % (sirina + 2) == 0 || l == projekcija._brojKolonaUSali * (sirina + 2)) cout << "|";
                else
                {
                    bool zauzeto = false;
                    for (int m = 0; m < projekcija._brojPosjetiteljaURedu[i]; m++)
                    {
                        if (ZauzetoMjesto(projekcija, i, j, m))
                        {
                            cout << setw(sirina + 1) << projekcija._posjetitelji[i][m]._imePrezime;
                            zauzeto = true;
                            break;
                        }
                    }

                    if (!zauzeto) cout << setw(sirina + 1) << " ";

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
    Projekcija Leo;

    Unos(Leo, "Leo da Vinci: Mission Mona Lisa", 6);

    tie(oznakaR, oznakaK) = GetPozicijuUReduIKoloni("B-15");
    cout << crt << "GetPozicijuUReduIKoloni(B-15) ->" << oznakaR << "/" << oznakaK << endl;

    tie(oznakaR, oznakaK) = GetPozicijuUReduIKoloni("C-01");
    cout << "GetPozicijuUReduIKoloni(C-01) ->" << oznakaR << "/" << oznakaK << crt;

    if (DodajPosjetitelja(Leo, GetNovogPosjetitelja("A-02", "Denis Music")))
        cout << crt << "Posjetitelj uspjesno dodan!" << crt;
    if (DodajPosjetitelja(Leo, GetNovogPosjetitelja("C-03", "Zanin Vejzovic")))
        cout << crt << "Posjetitelj uspjesno dodan!" << crt;
    if (DodajPosjetitelja(Leo, GetNovogPosjetitelja("D-05", "Adel Handzic")))
        cout << crt << "Posjetitelj uspjesno dodan!" << crt;


    PrikaziRasporedSjedenja(Leo);

    char * p = SlobodnoSjediste(Leo);

    if (p != nullptr)
        cout << crt << "Sljedece slobodno sjediste -> " << p << crt;
    else
        cout << crt << "Sva sjedista su zauzeta!" << crt;

    delete[] p; p = nullptr;

    Dealociraj(Leo);

    system("pause>0");
    return 0;
}
