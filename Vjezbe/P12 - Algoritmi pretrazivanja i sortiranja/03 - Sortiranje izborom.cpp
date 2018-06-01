/* -- Pseudo kod--- */
/*
POCETAK_SORTIRANJA(rastuæi redoslij.)

ponavljanje(i = 0; i < N - 1; i++)
    lokacija = i
    ponavljanje(j = i + 1; j < N; j++)
        ako(clan[j] < clan[lokacija])
        lokacija = j
        kraj_ispitivanja
    kraj_ponavljanja
    ako(clan[i] > clan[lokacija])
        spremi = clan[i]
        clan[i] = clan[lokacija]
        clan[lokacija] = spremi
        kraj_ispitivanja
kraj_ponavljanja

KRAJ_SORTIRANJA
*/

#include <iostream>

template<typename Tip>
void zamjeni(Tip &x, Tip &y)
{
    Tip temp = x;
    x = y;
    y = temp;
}

template<typename Tip>
void sortirajRP(Tip * niz, int duzina) //Rastuci poredak
{
    for (int i = 0; i < duzina - 1; i++)
    {
        int lokacija = i;
        for (int j = i + 1; j < duzina; j++)
            if (niz[j] < niz[lokacija])
                lokacija = j;

        if (niz[i] > niz[lokacija])
            zamjeni(niz[i], niz[lokacija]);
    }
}

template<typename Tip>
void sortirajOP(Tip * niz, int duzina) //Opadajuci poredak
{
    for (int i = 0; i < duzina - 1; i++)
    {
        int lokacija = i;
        for (int j = i + 1; j < duzina; j++)
            if (niz[j] > niz[lokacija])
                lokacija = j;

        if (niz[i] < niz[lokacija])
            zamjeni(niz[i], niz[lokacija]);
    }
}

int main()
{
    // ----------------------------------------------------------------------------------------------- //

    int niz[10] = { 3, 4, 12, 8, 1, 6, 15, 7, 0, 2 };
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    sortirajOP(niz, 10);
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    sortirajRP(niz, 10);
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------- //

    char nizChar[10] = "aBbAjTt+1";
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    sortirajOP(nizChar, 9);
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    sortirajRP(nizChar, 9);
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //


    system("pause>0");
    return 0;
}