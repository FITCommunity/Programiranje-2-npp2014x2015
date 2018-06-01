/* ---Pseudo kod--- */
/*
POÈETAK

prepisivanje polja a[n] u polje A[N]
N = n+1
for (i = 1; i <= N; i++)
    A[i] = a[i-1]
kraj_prepisivanja

sortiranje polja A[N]
for (i = 2; i <= N; i++)
    PromatraniA = A[i]
    pretrazivanje
    A[0] = PromatraniA
    lokacija = i-1
    while (PromatraniA < A[lokacija]))
        A[lokacija+1] = A[lokacija]
        lokacija = lokacija-1
    kraj_ponavljanja

    lokacija = lokacija+1
    kraj_pretrazivanja
    A[lokacija] = PromatraniA
kraj_for
kraj_sortiranja

KRAJ
*/

#include <iostream>

using namespace std;

template<typename Tip>
void sortiranje(Tip * niz, int duzina)
{
    for (int i = 2; i < duzina; i++)
    {
        Tip promatrani = niz[i];
        niz[0] = promatrani;
        int lokacija = i - 1;

        while (promatrani < niz[lokacija]) // zamjeniti znak za opadajuci poredak
        {
            niz[lokacija + 1] = niz[lokacija];
            lokacija--;
        }

        lokacija++;
        niz[lokacija] = promatrani;
    }
}

int main()
{
    // ----------------------------------------------------------------------------------------------- //

    int niz[10] = { 3, 4, 12, 8, 1, 6, 15, 7, 0, 2 };

    int noviNiz[11];
    for (int i = 0; i < 10; i++)
        noviNiz[i + 1] = niz[i];

    sortiranje(noviNiz, 11);
    for (int i = 1; i < 11; i++) std::cout << noviNiz[i] << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //

    char nizChar[10] = "aBbAjTt+1";

    char noviNizChar[11];
    for (int i = 0; i < 10; i++)
        noviNizChar[i + 1] = nizChar[i];

    sortiranje(noviNizChar, 10);
    for (int i = 1; i < 11; i++) std::cout << noviNizChar[i] << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //


    system("pause>0");
    return 0;
}