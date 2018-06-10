#include <iostream>

using namespace std;

template<typename Tip>
void zamjeni(Tip & x, Tip & y)
{
    Tip temp = x;
    x = y;
    y = temp;
}

template<typename Tip>
void sortiranje(Tip * niz, int duzina)
{
    for (int i = 1; i < duzina; i++)
    {
        Tip posmatrani = niz[i];

        int j = i - 1;
        while (posmatrani < niz[j] && j >= 0) // zamjeniti znak za opadajuci poredak
        {
            zamjeni(niz[j], niz[j + 1]);
            j--;
        }
    }
}

int main()
{
    // ----------------------------------------------------------------------------------------------- //

    int niz[10] = { 3, 4, 12, 8, 1, 6, 15, 7, 0, 2 };

    sortiranje(niz, 10);
    for (int i = 0; i < 10; i++) std::cout << niz[i] << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //

    char nizChar[10] = "aBbAjTt+1";

    sortiranje(nizChar, 9);
    for (int i = 0; i < 10; i++) std::cout << nizChar[i] << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //


    system("pause>0");
    return 0;
}