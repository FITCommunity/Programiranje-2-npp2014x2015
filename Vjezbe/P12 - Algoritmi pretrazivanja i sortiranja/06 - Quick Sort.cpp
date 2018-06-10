/* ---Pseudo kod--- */
/*
QuickSort (a[],N)
    Sort (1,N)
kraj_QuickSort

Sort (lijevo,desno)
    i = lijevo, j = desno
    sidro = a[(lijevo + desno) / 2]
    ponavljanje
        sve dok je (a[i] < sidro) 
            i=i+1
        sve dok je (sidro < a[j]) 
            j=j-1
        ako (i <= j)
            spremi = a[i]
            a[i] = a[j]
            a[j] = spremi
            i=i+1
            j=j-1
    sve dok je (i <= j)
    
    if (lijevo < j) 
        Sort (lijevo,j)
    if (i < desno)
        Sort (i,desno)
kraj_Sort
*/

#include <iostream>

template<typename Tip>
void zamjeni(Tip & x, Tip & y)
{
    Tip temp = x;
    x = y;
    y = temp;
}

template<typename Tip>
void sortiraj(Tip niz[], int lijevo, int desno)
{
    int i = lijevo, j = desno;
    Tip sidro = niz[(lijevo + desno)/2];
    
    while(i <= j)
    {
        while(niz[i] < sidro)
            i++;
        while(niz[j] > sidro)
            j--;
        
        if(i <= j)
        {
            zamjeni(niz[i], niz[j]);
            i++;
            j--;
        }
    }
    
    if(lijevo < j)
        sortiraj(niz, lijevo, j);
    if(desno > i)
        sortiraj(niz, i, desno);
}

int main()
{
     // ----------------------------------------------------------------------------------------------- //

    int niz[10] = { 3, 4, 12, 8, 1, 6, 15, 7, 0, 2 };
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj(niz, 0, 10);
    for (int i : niz) std::cout << i << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //

    char nizChar[10] = "aBbAjTt+1";
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    sortiraj(nizChar, 0, 8);
    for (char i : nizChar) std::cout << i << " ";
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------- //
	
    system("pause>0");
    return 0;
}