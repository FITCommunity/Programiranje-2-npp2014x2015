#include <iostream>

template<typename Tip1, typename Tip2>
struct Imenik 
{
    Tip1 * _niz1; //pokazivaè na niz tipa Tip1
    Tip2 * _niz2; //pokazivaè na niz tipa Tip2
    int _trenutno;
    void Inicijaliziraj()
    {
        _niz1 = nullptr;
        _niz2 = nullptr;
        _trenutno = 0;
    }
    
    void Dodaj(Tip1 n1, Tip2 n2)
    {
        Tip1 * niz1 = _niz1;
        Tip2 * niz2 = _niz2;
        _niz1 = new Tip1[_trenutno + 1];
        _niz2 = new Tip2[_trenutno + 1];

        for (int i = 0; i < _trenutno; i++)
        {
            zamjena(niz1 + i, _niz1 + i);
            zamjena(niz2 + i, _niz2 + i);
        }

        _niz1[_trenutno] = n1;
        _niz2[_trenutno] = n2;
        _trenutno++;
    }

    void Ispis()
    {
        for (int i = 0; i < _trenutno; i++)
            std::cout << i + 1 << ". " << _niz1[i] << " " << _niz2[i] << std::endl;
    }

    void Dealociraj()
    {
        delete[] _niz1; _niz1 = nullptr;
        delete[] _niz2; _niz2 = nullptr;
    }

};

template<typename Tip>
void zamjena(Tip * x, Tip * y)
{
    Tip temp = *x;
    *x = *y;
    *y = temp;
}

int main()
{

    Imenik<char*, int> Imenik1;
    Imenik1.Inicijaliziraj();
    Imenik1.Dodaj("Studentska", 281160);
    Imenik1.Dodaj("Dekan", 281171);
    Imenik1.Dodaj("Tehnicka podrksa", 281170);
    Imenik1.Ispis();
    Imenik1.Dealociraj();

    std::cout << std::endl << std::endl;

    Imenik<int, int> Imenik2;
    Imenik2.Inicijaliziraj();
    Imenik2.Dodaj(160, 281160);
    Imenik2.Dodaj(171, 281171);
    Imenik2.Dodaj(170, 281170);
    Imenik2.Ispis();
    Imenik2.Dealociraj();

    system("pause>0");
    return 0;
}