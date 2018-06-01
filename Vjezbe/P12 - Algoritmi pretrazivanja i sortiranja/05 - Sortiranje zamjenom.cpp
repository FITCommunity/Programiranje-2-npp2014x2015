/* ---Presudo kod--- */
/*
POÈETAK

do
    IndikatorZamjene = 1
    for (i = 0; i < N-1; i++)
        if (clan[i] > clan[i+1])
            spremi = clan[i]
            clan[i] = clan[i+1]
            clan[i+1] = spremi
            IndikatorZamjene = 0
        end_if
    end_for
    N = N-1
while (N > 1 && IndikatorZamjene == 0)

KRAJ
*/

#include <iostream>

template<typename Tip>
void zamjeni(Tip &x, Tip &y)
{
    Tip temp = x;
    x = y;
    y = temp;
}

//Sortira u opadajucem poretku
template<typename TipKontejner>
void sortiraj(TipKontejner * niz, int duzina)
{
    for (int i = 0; i < duzina; i++)
        for (int j = i + 1; j < duzina; j++)
            if (niz[i] > niz[j])
                zamjeni(niz[i], niz[j]);
}

//Sortira prema zadanom parametru
template<typename TipKontejner, typename Tip>
void sortiraj(TipKontejner * niz, int duzina, bool(*nacinSortiranja)(Tip, Tip))
{
    for (int i = 0; i < duzina; i++)
        for (int j = i + 1; j < duzina; j++)
            if (nacinSortiranja(niz[i], niz[j]))
                zamjeni(niz[i], niz[j]);
}

int main()
{
    // ----------------------------------------------------------------------------------------------- //

    int niz[10] = { 3, 4, 12, 8, 1, 6, 15, 7, 0, 2 };
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj(niz, 10);
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj<int, int>(niz, 10, [](int x, int y)->bool {return x < y; });
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------- //

    char nizChar[10] = "aBbAjTt+1";
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj(nizChar, 9);
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj<char, char>(nizChar, 9, [](char x, char y)->bool {return x < y; });
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //


    system("pause>0");
    return 0;
}