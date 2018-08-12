#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int dimenzija = 20;

void Horizontalno()
{
    cout << "+";
    for (int j = 0; j < 2 * dimenzija - 1; j++)
        cout << "-";
    cout << "+" << endl;
}

struct Kocka
{
    int lokacijaX;
    int lokacijaY;
    int velicina;
    int zivot;

    Kocka(int d = 1, int v = 1)
    {
        lokacijaX = d;
        lokacijaY = d;
        velicina = v;
        zivot = 3;
    }
    
    bool OutOfBound()
    {
        bool flag = lokacijaX > dimenzija || lokacijaX < 0 || lokacijaY > dimenzija || lokacijaY < 0;

        if (flag) zivot--;
        return flag;
    }

    void Dealociraj(char **&matrica)
    {
        for (int i = 0; i < dimenzija; i++)
            delete[] matrica[i];
        delete matrica; matrica = nullptr;
    }

    void Alociraj(char **&matrica, int velicinaMatrice)
    {
        if (matrica) Dealociraj(matrica);

        dimenzija = velicinaMatrice;
        matrica = new char*[dimenzija];
        for (int i = 0; i < dimenzija; i++)
            matrica[i] = new char[dimenzija];

        for (int i = 0; i < dimenzija; i++)
            for (int j = 0; j < dimenzija; j++)
                if (i >= lokacijaX && j >= lokacijaY && i <= lokacijaX + velicina && j <= lokacijaY + velicina)
                    matrica[i][j] = 'X';
                else
                    matrica[i][j] = ' ';
    }

    void Ispis(const char *const *matrica)
    {
        Horizontalno();

        for (int i = 0; i < dimenzija; i++)
        {
            cout << "|";
            for (int j = 0; j < dimenzija; j++)
                if (j < dimenzija - 1)
                    cout << matrica[i][j] << " ";
                else
                    cout << matrica[i][j];
            cout << "|" << endl;

        }

        Horizontalno();

        cout << "Zivoti: ";
        for (int i = 0; i < zivot; i++)
            cout << "O ";
        cout << endl;
    }
};


void main()
{
    char ** matrica = nullptr;
    Kocka kocka;
    
    do
    {
        kocka.Alociraj(matrica, dimenzija);
        kocka.Ispis(matrica);

        char komanda;
        cout << "/Lijevo - L/Desno - R/Gore - U/Dole - D/" << endl;
        cout << "/Povecaj - P/ Smanji - S/" << endl;
        cout << "Komanda -> ";
        cin >> komanda;

        komanda = toupper(komanda);
        
        switch (komanda)
        {
            case 'L':
                kocka.lokacijaY--;
                if (kocka.OutOfBound()) kocka.lokacijaY++;
                break;
            case 'R':
                kocka.lokacijaY++;
                if (kocka.OutOfBound()) kocka.lokacijaY--;
                break;
            case 'U':
                kocka.lokacijaX--;
                if (kocka.OutOfBound()) kocka.lokacijaX++;
                break;
            case 'D':
                kocka.lokacijaX++;
                if (kocka.OutOfBound()) kocka.lokacijaX--;
                break;
            case 'P':
                kocka.velicina++;
                kocka.Alociraj(matrica, dimenzija + 1);
                break;
            case 'S':
                if (kocka.velicina - 1 > -1)
                {
                    kocka.velicina--;
                    kocka.Alociraj(matrica, dimenzija - 1);
                }
                break;
            default:
                cout << "Pogresna komanda!" << endl;
        }
        system("cls");
    } while (kocka.zivot);

    kocka.Dealociraj(matrica);
}