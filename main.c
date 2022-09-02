// Zofia Tunova, DSA zadanie 3: Popolvar
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct Policko
{
    int meno;
    int vaha;
    struct Policko* susediaci;
}POLICKO;

typedef struct Graf
{
    int pocet_vrcholov;
    POLICKO** pole;
}GRAF;

typedef struct HaldaUzol
{
    int vrchol;
    int vzdialenost;
}HALDAUZOL;

typedef struct MinHalda
{
    int velkost;
    int prvkov;
    int* pozicie;
    HALDAUZOL **halda;
}MINHALDA;

typedef struct Ostatne
{
    int poc_vrcholov;
    int pozicia_draka;
    int cena_k_drakovi;
    int pocet_princezien;
    int prejdenych_vrcholov;
    int* pozicie_princezien;
    int* vzdialenosti_od_draka;
    int* cela_cesta;
    int* vsetky_cesty;
    int* vzdialenosti;
    int** tabulka;
}OSTATNE;

//funkcia alokuje strukturu OSTATNE a alokuje vsetky polia ktore sa v strukture nachadzaju
//dvojrozmerne pole tabulka este okrem toho aj vynuluje
OSTATNE* init_ostatne(int max_vrcholov)
{
    int i, j;
    OSTATNE* ostatne= (OSTATNE*) malloc(sizeof(OSTATNE));
    ostatne->vzdialenosti_od_draka= (int*) malloc(5*sizeof(int));
    ostatne->cela_cesta= (int*) malloc(100*max_vrcholov*sizeof(int));
    ostatne->vsetky_cesty= (int*) malloc(max_vrcholov*sizeof(int));
    ostatne->vzdialenosti= (int*) malloc(max_vrcholov*sizeof(int));
    ostatne->pozicie_princezien= (int*) malloc(5*sizeof(int));
    ostatne->pocet_princezien=0;
    ostatne->prejdenych_vrcholov=0;
    ostatne->poc_vrcholov= max_vrcholov;

    ostatne->tabulka= (int**) malloc(5*sizeof(int*));
    for(i=0; i<5; i++){
        ostatne->tabulka[i] = (int *) malloc(5 * sizeof(int));
    }

    for(i=0; i<5; i++){
        for(j=0; j<5; j++){
            ostatne->tabulka[i][j]= 0;
        }
    }

    return ostatne;
}

//inicializuje strukturu GRAF a celu ju vynuluje
//do premennej pocet_vrcholov zapise celkovy pocet vrcholov v grafe
GRAF* init_graf(int vrcholov)
{
    GRAF* graf= (GRAF*) malloc(sizeof(GRAF));
    graf->pocet_vrcholov= vrcholov;
    graf->pole=(POLICKO**) malloc(vrcholov*sizeof(POLICKO*));
    int i;
    for(i=0; i<vrcholov; i++){
        graf->pole[i]= NULL;
    }
    return graf;
}

//pomocna funkcia na vypisanie vytvoreneho grafu
void vypis_graf(GRAF* graf)
{
    int i;
    POLICKO* akt;
    int vrcholov= graf->pocet_vrcholov;
    for(i=0; i<vrcholov; i++){
        akt= graf->pole[i];
        printf("(%d) ", i);
        if(akt== NULL){
            printf("---\n");
        }
        else{
            while(akt != NULL){
                printf("[%d, %d] ->", akt->meno, akt->vaha);
                akt= akt->susediaci;
            }
            printf("\n");
        }
    }
}

//funkcia ktora pridava zaznam do zoznamu susedov
void pridaj_prechod(GRAF* graf, int odkial, int kam, int cena)
{
    //alokuje sa nova struktura typu policko, pricom sa zaplni prislusnymi hodnotami
    POLICKO* nove_policko= (POLICKO*) malloc(sizeof(POLICKO));
    nove_policko->meno= kam;
    nove_policko->vaha= cena;
    nove_policko->susediaci= NULL;

    //smernik sa v poli nastavi na taky index, ake je oznacenie policka pretoze na tomto indexe sa nachadza zoznam jeho susedov
    //ak policko ziadneho suseda este zaznaceneho nema, akt == NULL, a struktura sa zapise hned na zaciatok
    //inak sa prejde na koniec spajaneho zoznamu susedov a nova struktura sa pripoji na koniec
    POLICKO* akt= graf->pole[odkial];
    if(akt == NULL){
        graf->pole[odkial] = nove_policko;
    }
    else{
        while(akt->susediaci != NULL){
            akt= akt->susediaci;
        }
        akt->susediaci= nove_policko;
    }

    //rovnakym sposobom sa prechod pridava aj v opacnom smere
    nove_policko= (POLICKO*) malloc(sizeof(POLICKO));
    nove_policko->meno= odkial;
    nove_policko->vaha= cena;
    nove_policko->susediaci= NULL;

    akt= graf->pole[kam];
    if(akt== NULL){
        graf->pole[kam]= nove_policko;
    }
    else{
        while(akt->susediaci != NULL){
            akt= akt->susediaci;
        }
        akt->susediaci= nove_policko;
    }
}

//funkcia ktora postupne prechadza mapou, kontroluje typ policka podla coho urci cenu prechodu cez toto policko: H 2, ostatne okrem N 1
//vola funkciu pridaj_prechod, cim sa vytvara zoznam susediacich vrcholov
void prerob_mapu(char** mapa, int riadkov, int stlpcov, GRAF* graf, OSTATNE* ostatne)
{
    int i, j, pocitadlo, oznacenie, cena, princezien, poz;
    char policko, sused;
    pocitadlo=0;
    princezien=0;
    poz=0;
    for (i=0; i<riadkov; i++)
    {
        for(j=0; j<stlpcov; j++)
        {
            policko= mapa[i][j];

            //polickam typu N sa prechod nepridava, preto by ani nemalo zmysel pokracovat dalej
            if(policko != 'N'){
                //ak najde draka jeho poziciu ulozi
                if(policko == 'D'){
                    ostatne->pozicia_draka= pocitadlo;
                }
                //spocitava pocet princezien a ich pozicie uklada do pola
                if(policko== 'P'){
                    princezien= princezien+1;
                    ostatne->pocet_princezien= princezien;
                    ostatne->pozicie_princezien[poz]= pocitadlo;
                    poz=poz+1;
                }
                //kontroluje sa, ci nie je v poslednom riadku a stlpci v mape, pretoze policka by potom mali menej susedov
                if(i != riadkov-1){
                    if(j != stlpcov-1){
                        //najskor sa kontroluje pravy sused policka
                        sused= mapa[i][j+1];
                        oznacenie= pocitadlo+1;
                        //nepridava sa prechod ani ked je sused policka policko typu N
                        if(sused != 'N'){
                            if(sused == 'H'){
                                cena= 2;
                            }
                            else{
                                cena= 1;
                            }
                            //printf("graf %d %d %d\n", pocitadlo, oznacenie, cena);
                            //volanie funkcie, ktora pridava suseda do zoznamu
                            pridaj_prechod(graf, pocitadlo, oznacenie, cena);
                        }
                    }
                    //ako druhy sa kontroluje spodny sused policka
                    //kontrola aj vsetko prebieha rovnakym sposobom ako pri pravom susedovi
                    sused= mapa[i+1][j];
                    oznacenie= pocitadlo+ stlpcov;
                    if(sused != 'N'){
                        if(sused == 'H'){
                            cena= 2;
                        }
                        else{
                            cena= 1;
                        }
                        //printf("graf %d %d %d\n", pocitadlo, oznacenie, cena);
                        pridaj_prechod(graf, pocitadlo, oznacenie, cena);
                    }
                }
                else{
                    if(j != stlpcov-1){
                        sused= mapa[i][j+1];
                        oznacenie= pocitadlo+1;
                        if(sused != 'N'){
                            if(sused== 'H'){
                                cena= 2;
                            }
                            else{
                                cena= 1;
                            }
                            //printf("graf %d %d %d\n", pocitadlo, oznacenie, cena);
                            pridaj_prechod(graf, pocitadlo, oznacenie, cena);
                        }
                    }
                }
            }
            pocitadlo= pocitadlo+1;
        }
    }
}

//alokuje strukturu typu MINHALDA spolu s polom do ktoreho sa ukladaju pozicie prvkov v poli haldy a samotne pole haldy ako pole struktur
//typu HALDAUZOL
//ostatne premenne sa zaplnia prislusnymi udajmi, pricom pocet prvkov sa na zaciatok nastavi na 0
MINHALDA* init_halda(int kapacita)
{
    MINHALDA* minhalda= (MINHALDA*) malloc(sizeof(MINHALDA));
    minhalda->pozicie= (int*) malloc(kapacita*sizeof(int));
    minhalda->prvkov=0;
    minhalda->velkost= kapacita;
    minhalda->halda= (HALDAUZOL**) malloc(kapacita*sizeof(HALDAUZOL*));
    return minhalda;
}

//vytvara novy zaznam do haldy - alokuje strukturu ktoru funkcia aj vrati a zaplni ju prislusnymi udajmi
//vstupny parameter aky_vrchol je oznacenie vrcholu ktory sa pridava do haldy, za_kolko je vzdialenost od vychodzieho bodu z dijkstra
HALDAUZOL* novy_uzol(int aky_vrchol, int za_kolko)
{
    HALDAUZOL* novy= (HALDAUZOL*) malloc(sizeof(HALDAUZOL));
    novy->vrchol= aky_vrchol;
    novy->vzdialenost= za_kolko;
    return novy;
}

//pomocna funkcia na vypisanie celeho pola haldy
void vypis_haldu(MINHALDA* minhalda)
{
    int pocet_pr= minhalda->prvkov;
    int i;
    for(i=0; i< pocet_pr; i++){
        printf("[%d  %d]", minhalda->halda[i]->vrchol, minhalda->halda[i]->vzdialenost);
    }
    printf("\n");
}

//funkcia na utriedenie a zoradenie haldy tak, aby rodicovsky prvok bol vzdy mensi ako jeho potomkovia
void uprac_haldu(MINHALDA* minhalda, int index)
{
    int pocet_pr= minhalda->prvkov;
    int lavy, pravy, minimalny;
    HALDAUZOL* pomocny;
    //zoradovanie prebieha kym sa nepride na koniec haldy alebo kym nie je zoradena (vid vetvu else)
    while(index< pocet_pr)
    {
        //urcenie na akej pozicii v poli sa nachadzaju potomkovia zvoleneho vrcholu
        //zvoleny vrchol sa predbezne oznaci ako minimalny
        lavy= 2*index+1;
        pravy= 2*index+2;
        minimalny= index;
        //tu sa kontroluje, ci lavy a pravy potomok vobec v halde su
        if(lavy< pocet_pr)
        {
            //nasledne sa kontroluje ci vzdialenost potomka je mensia ako vzdialenost rodicovskeho vrcholu
            //ak je bude to treba riesit, takze si zapamatame mensi prvok - teraz uz jeden z potomkov
            if(minhalda->halda[lavy]->vzdialenost < minhalda->halda[minimalny]->vzdialenost){
                minimalny= lavy;
            }
        }
        if(pravy< pocet_pr){
            if(minhalda->halda[pravy]->vzdialenost < minhalda->halda[minimalny]->vzdialenost){
                minimalny= pravy;
            }
        }
        //ak je minimalny iny ako povodne zvoleny vrchol znamena to, ze jeden z potomkov bol mensi ako jeho rodic
        //v takom pripade sa potomok s jeho rodicom vymeni
        // a tiez treba tuto vymenu spravit v poli, v ktorom si pamatam poziciu prvku v halde
        if(minimalny != index){
            HALDAUZOL* najmensi= minhalda->halda[minimalny];
            HALDAUZOL* akt= minhalda->halda[index];

            //tu sa meni pole s poziciami v halde
            minhalda->pozicie[najmensi->vrchol]= index;
            minhalda->pozicie[akt->vrchol]= minimalny;

            //tu sa vymiena v samotnej halde prvok s jeho rodicom
            pomocny= minhalda->halda[index];
            minhalda->halda[index] = minhalda->halda[minimalny];
            minhalda->halda[minimalny] = pomocny;

            index= minimalny;
        } else{
            //ak minimalny prvok zostal na povodne zvolenom prvku, znamena to ze rodic je mensi ako jeho potomkovia a moze sa skoncit
            break;
        }
    }
}

//funkcia ktora vrati vrchol haldy = najmensi prvok zo vsetkych
HALDAUZOL* delete_najmensi(MINHALDA* minhalda)
{
    int pocet_pr= minhalda->prvkov;
    if(pocet_pr== 0){
        return NULL;
    }

    //zapamata si vrchol haldy
    HALDAUZOL* vysledok= minhalda->halda[0];

    //nahradi ho posledne pridanym prvkom
    HALDAUZOL* posledny= minhalda->halda[pocet_pr-1];
    minhalda->halda[0]= posledny;

    //zmeni este hodnoty v poli pozicii v halde
    minhalda->pozicie[vysledok->vrchol]= minhalda->prvkov-1;
    minhalda->pozicie[posledny->vrchol]= 0;

    //v halde uz bude menej prvkov
    minhalda->prvkov= minhalda->prvkov-1;
    // a treba este znovu upratat haldu pretoze po pridani posledneho prvku sa rozhadzala
    uprac_haldu(minhalda, 0);
    return  vysledok;
}

//funkcia ktora meni hodnotu vzdialenosti v halde ked dijkstra vypocita lepsiu
void zmen_vzdialenost(MINHALDA* minhalda, int akemu_vrcholu, int nova_vzdialenost)
{
    //najprv si zisti poziciu meneneho prvku v halde a zapise mu novu vzdialenost
    int pozicia_vhlade= minhalda->pozicie[akemu_vrcholu];
    minhalda->halda[pozicia_vhlade]->vzdialenost= nova_vzdialenost;

    //znovu treba haldu upratat ale tentokrat sa vzdialenost mohla zmenit len na mensiu, takze prvok sa dostane vyssie v halde
    // => treba porovnavat s rodicom
    int rodic= (pozicia_vhlade-1)/2;
    int meneny, skym;
    HALDAUZOL* pomocny;
    //cyklus bezi kym sa prvku nenajde spravna pozicia v halde
    while(pozicia_vhlade != 0 && minhalda->halda[pozicia_vhlade]->vzdialenost < minhalda->halda[rodic]->vzdialenost )
    {
        meneny= minhalda->halda[pozicia_vhlade]->vrchol;
        skym= minhalda->halda[rodic]->vrchol;
        //znovu treba zmenit aj pole pozicii v halde
        minhalda->pozicie[meneny]= rodic;
        minhalda->pozicie[skym]= pozicia_vhlade;

        //a vymenit prvok s jeho rodicom
        pomocny= minhalda->halda[pozicia_vhlade];
        minhalda->halda[pozicia_vhlade]= minhalda->halda[rodic];
        minhalda->halda[rodic]= pomocny;

        //posuva sa po rodicoch
        pozicia_vhlade= rodic;
        rodic= (pozicia_vhlade-1)/2;
    }
}

//pomocne funkcie na vypis
void vypis_vzdialenosti(int *vzdialenosti, int pocet_vrcholov)
{
    int i;
    printf("vrchol     vzdialenost od pociatocneho\n");
    for(i=0; i<pocet_vrcholov; i++)
    {
        printf("%d    %d\n", i, vzdialenosti[i]);
    }
}

void vypis_cestu(OSTATNE* ostatne)
{
    int i;
    printf("cela cesta: ");
    for(i=0; i< ostatne->prejdenych_vrcholov; i++)
    {
        printf("%d  ", ostatne->cela_cesta[i]);
    }
    printf("\n");
}

void dijkstra(GRAF* graf, int zaciatocny, OSTATNE* ostatne)
{
    int vrcholov_vgrafe= graf->pocet_vrcholov;

    //inicializuje sa halda
    MINHALDA* minhalda= init_halda(vrcholov_vgrafe);
    int vrchol;
    //cela halda sa zaplni vsetkymi vrcholmi v grafe
    //vzdialenost vrcholov je na zaciatok INT_MAX (namiesto nekonecna)
    for(vrchol=0; vrchol<vrcholov_vgrafe; ++vrchol){
        ostatne->vzdialenosti[vrchol]= INT_MAX;
        minhalda->halda[vrchol]= novy_uzol(vrchol, ostatne->vzdialenosti[vrchol]);
        minhalda->pozicie[vrchol]= vrchol;
    }

    //v poli do ktoreho si ukladam z akoeho vrcholu som sa do vrcholu dostala sa zaciatocnemu vrcholu nastavi predchodca na -1
    ostatne->vsetky_cesty[zaciatocny]= -1;

    //vzdialenost zaciatocneho vrcholu sa nastavi na 0, takze bude z haldy vybraty ako prvy
    minhalda->halda[zaciatocny]= novy_uzol(zaciatocny, ostatne->vzdialenosti[zaciatocny]);
    minhalda->pozicie[zaciatocny]= zaciatocny;
    ostatne->vzdialenosti[zaciatocny]= 0;

    //vzdialensot sa meni prostrednictvom tejto funkcie, cize zaciatocny bod hned aj prebuble navrch
    zmen_vzdialenost(minhalda, zaciatocny, ostatne->vzdialenosti[zaciatocny]);

    minhalda->prvkov= vrcholov_vgrafe;

    //vypis_haldu(minhalda);

    HALDAUZOL* najlacnejsi;
    POLICKO* akt;
    int ktory_to_je, akt_vrchol;

    //kym nie je halda prazdna => kym neboli navstivene vsetky vrcholi
    while(minhalda->prvkov != 0){
        //vypis_haldu(minhalda);

        //vyberie sa najblizsi prvok - je na vrchu haldy
        najlacnejsi= delete_najmensi(minhalda);

        //toto je oznacenie vybrateho vrcholu, na tom indexe v grafe je zoznam jeho susedov
        ktory_to_je= najlacnejsi->vrchol;
        akt= graf->pole[ktory_to_je];

        //prechadza sa vsetkymi susedmi vybrateho vrcholu
        //ak je sused este v halde (nepreslo sa este cez neho) skontorluje sa, ci cena cesty cez vybraty vrchol nie je mensia ako doterajsia cena
        //ak nahodou je, vybraty vrchol sa zaznaci ako jeho predchodca a aktualizuje sa vzdialenost tohoto suseda
        while (akt != NULL){
            akt_vrchol= akt->meno;
            if(minhalda->pozicie[akt_vrchol] < minhalda->prvkov){
                if(ostatne->vzdialenosti[ktory_to_je] != INT_MAX && akt->vaha + ostatne->vzdialenosti[ktory_to_je] < ostatne->vzdialenosti[akt_vrchol]){
                    ostatne->vsetky_cesty[akt_vrchol]= ktory_to_je;
                    ostatne->vzdialenosti[akt_vrchol] = ostatne->vzdialenosti[ktory_to_je] + akt->vaha;

                    zmen_vzdialenost(minhalda, akt_vrchol, ostatne->vzdialenosti[akt_vrchol]);
                }
            }
            akt= akt->susediaci;
        }
    }

    /*vypis_vzdialenosti(ostatne->vzdialenosti, vrcholov_vgrafe);
    printf("\n");*/
}

//funkcia ktora vytvara postupnost vrcholov, ktorymi ma Popolvar na svojej ceste prechadzat
void rekonstrukcia_cesty(OSTATNE* ostatne, int ciel)
{
    int pocitadlo=0;
    //aby som vedela odkial mozem dopisovat cestu do zatial zapamatanej
    int odkial= ostatne->prejdenych_vrcholov;
    int vrcholov= ostatne->poc_vrcholov;
    //pomocne pole, do neho sa uklada aktualne spracovavany usek cesty
    int* pomocne= (int*) malloc(vrcholov*sizeof(int));
    int i= ostatne->vsetky_cesty[ciel];

    //prechadza sa polom predchodcov kym nenajde -1 cize vychodzi bod a do pomocneho pola uklada vrcholi ktorymi sa preslo
    while(i != -1)
    {
        pomocne[pocitadlo] = i;
        i= ostatne->vsetky_cesty[i];
        pocitadlo= pocitadlo+1;
    }

    ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+pocitadlo;

    //cesta je v pomocnom poli zapisana odzadu, takze este sa v spravnom poradi doplni do celkovej cesty
    for(i= pocitadlo-1; i>= 0; i--){
        ostatne->cela_cesta[odkial]= pomocne[i];
        odkial= odkial+1;
    }
    //printf("\n");
}

int* zachran_princezne(char **mapa, int n, int m, int t, int* dlzka_cesty)
{
    int v= n*m;
    int celkova_cena=0;
    int min_cena= INT_MAX;
    int i,j,k,l,o, poc=0;
    int usek1, usek2, usek3, usek4, usek5;
    int p1, p2, p3, p4, p5;
    int cena=0;

    //inicializacia struktur a pretvorenie mapy do zoznamu susedov
    GRAF* graf= init_graf(v);
    OSTATNE* ostatne= init_ostatne(v);

    prerob_mapu(mapa, n,m, graf, ostatne);
    //vypis_graf(graf);

    int drak= ostatne->pozicia_draka;

    //kontroluje sa pristupnost draka, ak je nepristupny funkcia vrati NULL
    if(graf->pole[drak] == NULL){
        printf("nepristupny drak\n");
        *dlzka_cesty=0;
        return NULL;
    }

    int princezien= ostatne->pocet_princezien;
    int princezna;

    //tu sa kontroluje pristupnost princezien, opat ak je jedna z nich nedostupna funkcia skonci vratenim NULL
    for(i=0; i< princezien; i++){
        //printf("%d ", ostatne->pozicie_princezien[i]);
        princezna= ostatne->pozicie_princezien[i];
        if(graf->pole[princezna] == NULL){
            printf("\nnepristupna princezna\n");
            *dlzka_cesty= 0;
            return NULL;
        }
    }
    printf("\n");

    //ak su vsetky postavicky dostupne zavola sa dijkstra, ktory zisti dlzku cesty z bodu 0,0 k drakovi
    dijkstra(graf, 0, ostatne);
    ostatne->cena_k_drakovi= ostatne->vzdialenosti[drak];

    //moze sa stat, ze k drakovi sa Popolvar nestihne dostat vcas, teda funkcia tiez skonci vratenim NULL
    if(ostatne->cena_k_drakovi > t){
        printf("princezne nie je mozne zachranit, nestiham zabit draka\n");
        *dlzka_cesty=0;
        return NULL;
    }

    //ak sa teda stiha dostat k drakovi ulozi sa cesta k drakovi, lebo ta je rovnaka bez ohladu na pocet princezien
    rekonstrukcia_cesty(ostatne, drak);


    //tu sa uz rozlisuje kolko je princezien pretoze podla toho treba riesit permutacie
    //ak je princezna jedna je len jedna moznost, cize zavola sa dijkstra od draka
    //ulozi sa cesta od draka po princeznu a ulozi si dlzku cesty
    if(princezien == 1){
        princezna= ostatne->pozicie_princezien[0];
        celkova_cena= celkova_cena+ ostatne->vzdialenosti[drak];

        dijkstra(graf, drak, ostatne);
        rekonstrukcia_cesty(ostatne, princezna);

        int koniec= ostatne->prejdenych_vrcholov;

        ostatne->cela_cesta[koniec] = princezna;
        celkova_cena= celkova_cena+ ostatne->vzdialenosti[princezna];
        ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+1;

        min_cena= celkova_cena;
        //vypis_cestu(ostatne);
    }
    //ak su princezne 2 stale nie je tolko vela moznosti, takze ich kontrolujem rucne bez cyklov
    //spocitaju sa dlzky ciest v 2 moznostiach, vyberie sa ta kratsia cesta a podla toho sa ulozi cesta ktorou treba postupovat
    else if(princezien == 2)
    {
        int pr1= ostatne->pozicie_princezien[0];
        int pr2= ostatne->pozicie_princezien[1];
        int c1=0, c2=0;

        c1= c1+ostatne->vzdialenosti[drak];
        c2= c2+ostatne->vzdialenosti[drak];

        dijkstra(graf, drak, ostatne);
        c1= c1+ostatne->vzdialenosti[pr1];

        dijkstra(graf, pr1, ostatne);
        c1= c1+ostatne->vzdialenosti[pr2];
        //printf("cena1: %d\n",c1);


        dijkstra(graf, drak, ostatne);
        c2= c2+ostatne->vzdialenosti[pr2];

        dijkstra(graf, pr2, ostatne);
        c2= c2+ostatne->vzdialenosti[pr1];
       // printf("cena2: %d\n",c2);

        if(c1< c2){
            dijkstra(graf, drak, ostatne);
            rekonstrukcia_cesty(ostatne, pr1);
            dijkstra(graf, pr1, ostatne);
            rekonstrukcia_cesty(ostatne, pr2);
            int koniec= ostatne->prejdenych_vrcholov;
            ostatne->cela_cesta[koniec] = pr2;
            min_cena= c1;
            //printf("cena1: %d\n",c1);
        }
        else{
            dijkstra(graf, drak, ostatne);
            rekonstrukcia_cesty(ostatne, pr2);
            dijkstra(graf, pr2, ostatne);
            rekonstrukcia_cesty(ostatne, pr1);
            int koniec= ostatne->prejdenych_vrcholov;
            ostatne->cela_cesta[koniec] = pr1;
            min_cena= c2;
            //printf("cena2: %d\n",c2);
        }
        ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+1;
        //vypis_cestu(ostatne);
    }
    else {
        //ak je princezien viac najskor sa spusti dijkstra od draka pricom sa ulozia vzdialensoti k vsetkym princeznam
        int princezna;
        dijkstra(graf, drak, ostatne);

        for (i = 0; i < princezien; i++) {
            princezna = ostatne->pozicie_princezien[i];
            //printf("princezna na %d\n", princezna);
            ostatne->vzdialenosti_od_draka[i] = ostatne->vzdialenosti[princezna];
        }

        //potom sa pusti dijkstra od kazdej princeznej, pricom do tabulky sa ulozia vzdialenosti k ostatnym princeznam
        for (i = 0; i < princezien; i++) {
            princezna = ostatne->pozicie_princezien[i];
            dijkstra(graf, princezna, ostatne);
            for (j = 0; j < princezien; j++) {
                ostatne->tabulka[i][j] = ostatne->vzdialenosti[ostatne->pozicie_princezien[j]];
            }
        }

        //tu sa rozlisuju pocty princezien kvoli vytvaraniu permutacii
        //permutacie sa vytvaraju vnorenymi cyklami pricom je tolko cyklov kolko princezien
        //permutacie sa robia na tabulke vzdialenosti a uklada sa najmensia vypocitana vzdialenost aj poradie v akom boli princezne navstivne
        //po skonceni permustacii sa este vola dijkstra na to, aby sa vytvorila trasa
        if (princezien == 3) {
            //printf("3 princezne\n");
            for(i=0; i<3; i++){
                //printf("%d+ ", ostatne->vzdialenosti_od_draka[i]);
                usek1= ostatne->vzdialenosti_od_draka[i];
                for(j=0; j<3; j++){
                    if(i != j){
                        //printf("%d+ ", ostatne->tabulka[i][j]);
                        usek2= ostatne->tabulka[i][j];
                        for(k=0; k< 3; k++){
                            if(k != i && k != j){
                                //printf("%d\n", ostatne->tabulka[j][k]);
                                usek3= ostatne->tabulka[j][k];
                                poc= poc+1;
                                //printf("%d %d %d\n", i,j,k);
                                //printf("%d + %d + %d \n", ostatne->vzdialenosti_od_draka[i], ostatne->tabulka[i][j], ostatne->tabulka[j][k]);

                                celkova_cena= usek1+usek2+usek3;
                                if(celkova_cena< min_cena){
                                    min_cena= celkova_cena;
                                    p1= i;
                                    p2= j;
                                    p3= k;
                                }
                            }
                        }
                    }
                }
            }
            //printf("pocet permutacii: %d\n", poc);
            min_cena= min_cena+ ostatne->cena_k_drakovi;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d\n", min_cena, p1,p2,p3);
            dijkstra(graf, drak, ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p1]);
            dijkstra(graf, ostatne->pozicie_princezien[p1], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p2]);
            dijkstra(graf, ostatne->pozicie_princezien[p2], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p3]);

            int koniec= ostatne->prejdenych_vrcholov;
            ostatne->cela_cesta[koniec] = ostatne->pozicie_princezien[p3];
            ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+1;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d\n", min_cena, p1,p2,p3);
            //vypis_cestu(ostatne);
        }
        else if(princezien== 4)
        {
            for (i=0; i<4; i++){
                usek1= ostatne->vzdialenosti_od_draka[i];
                for(j=0; j<4; j++){
                    if(i != j){
                        usek2= ostatne->tabulka[i][j];
                        for(k=0; k<4; k++){
                            if(k != j && k != i){
                                usek3= ostatne->tabulka[j][k];
                                for(l=0; l<4; l++){
                                    if(l != k && l != j && l != i){
                                        usek4= ostatne->tabulka[k][l];
                                        celkova_cena= usek1+usek2+usek3+ usek4;
                                        poc= poc+1;
                                        if(celkova_cena< min_cena){
                                            min_cena= celkova_cena;
                                            p1= i;
                                            p2= j;
                                            p3= k;
                                            p4= l;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            //printf("pocet permutacii: %d\n", poc);
            min_cena= min_cena+ ostatne->cena_k_drakovi;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d\n", min_cena, p1,p2,p3);
            dijkstra(graf, drak, ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p1]);
            dijkstra(graf, ostatne->pozicie_princezien[p1], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p2]);
            dijkstra(graf, ostatne->pozicie_princezien[p2], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p3]);
            dijkstra(graf, ostatne->pozicie_princezien[p3], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p4]);

            int koniec= ostatne->prejdenych_vrcholov;
            ostatne->cela_cesta[koniec] = ostatne->pozicie_princezien[p4];
            ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+1;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d %d\n", min_cena, p1,p2,p3,p4);
            //vypis_cestu(ostatne);
        }
        else if(princezien == 5)
        {
            for(i=0; i<5; i++){
                usek1= ostatne->vzdialenosti_od_draka[i];
                for(j=0; j<5; j++){
                    if(i != j){
                        usek2= ostatne->tabulka[i][j];
                        for(k=0; k<5; k++){
                            if(j != k && k != i){
                                usek3= ostatne->tabulka[j][k];
                                for(l=0; l<5; l++){
                                    if(k != l && l != j && l!=i){
                                        usek4= ostatne->tabulka[k][l];
                                        for(o=0; o<5; o++){
                                            if(o != i && o != j && o != k && o != l) {
                                                usek5= ostatne->tabulka[l][o];
                                                // poc= poc+1;
                                                //printf("%d %d %d %d %d\n", i, j, k, l, m);
                                                celkova_cena= usek1+usek2+usek3+ usek4+ usek5;
                                                poc= poc+1;
                                                if(celkova_cena< min_cena){
                                                    min_cena= celkova_cena;
                                                    p1= i;
                                                    p2= j;
                                                    p3= k;
                                                    p4= l;
                                                    p5= o;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
           // printf("pocet permutacii: %d\n", poc);
            min_cena= min_cena+ ostatne->cena_k_drakovi;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d\n", min_cena, p1,p2,p3);
            dijkstra(graf, drak, ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p1]);
            dijkstra(graf, ostatne->pozicie_princezien[p1], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p2]);
            dijkstra(graf, ostatne->pozicie_princezien[p2], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p3]);
            dijkstra(graf, ostatne->pozicie_princezien[p3], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p4]);
            dijkstra(graf, ostatne->pozicie_princezien[p4], ostatne);
            rekonstrukcia_cesty(ostatne, ostatne->pozicie_princezien[p5]);

            int koniec= ostatne->prejdenych_vrcholov;
            ostatne->cela_cesta[koniec] = ostatne->pozicie_princezien[p5];
            ostatne->prejdenych_vrcholov= ostatne->prejdenych_vrcholov+1;
            //printf("najlacnejsia cesta: %d, kombinacia= %d %d %d %d %d\n", min_cena, p1,p2,p3,p4,p5);
            //vypis_cestu(ostatne);
        }
    }

    int dlzka= ostatne->prejdenych_vrcholov;
    int* finalna_cesta= (int*) malloc(2*dlzka*(sizeof(int)));
    int riadok, stlpec, meno, vnovom=0;

    //pole s trasou sa este prekopiruje do koncoveho pola, pricom uz sa nukladaju oznacenia vrcholov ale z mena sa vypocitaju suradnice
    for(i=0; i<dlzka; i++){
        meno= ostatne->cela_cesta[i];
        stlpec= meno % m;
        riadok= meno / m;
        finalna_cesta[vnovom]= stlpec;
        finalna_cesta[vnovom+1]= riadok;
        vnovom= vnovom+2;
    }
    //vypise sa vytvorene pole so suradnicami
    for(i=0; i<dlzka*2; i=i+2){
        printf("%d %d\n", finalna_cesta[i], finalna_cesta[i+1]);
    }
    printf("\n");

    //vystupny parameter sa nastavi na najkratsiu najdenu cestu a funkcia vrati pole suradnic
    *dlzka_cesty= min_cena;
    return finalna_cesta;
}

int main() {

    char **mapa;
    int i, test, dlzka_cesty, cas, *cesta;
    int n=0, m=0, t=0;

    while(1)
    {
        printf("Zadajte cislo testu (0 ukonci program):\n");
        scanf("%d", &test);
        dlzka_cesty=0;
        n=m=t=0;
        switch(test){
            case 0:
                return 0;
            case 1:
                n=10;
                m=10;
                t=12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCHCNHCCHN";
                mapa[1]="NNCCCHHCCC";
                mapa[2]="DNCCNNHHHC";
                mapa[3]="CHHHCCCCCC";
                mapa[4]="CCCCCNHHHH";
                mapa[5]="PCHCCCNNNN";
                mapa[6]="NNNNNHCCCC";
                mapa[7]="CCCCCPCCCC";
                mapa[8]="CCCNNHHHHH";
                mapa[9]="HHHPCCCCCC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 2:
                n=20;
                m=20;
                t=25;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CPCCHHHHHHHHHHHHHHH";
                mapa[1]= "HHHCCCCNNNNNNNNNNNN";
                mapa[2]= "NNNNNNCCCCCCCCCCCCC";
                mapa[3]= "CCCCCCCCCCCCCCCCCCC";
                mapa[4]= "CCCCCCCCCHHHHHHHHHH";
                mapa[5]="CCCCCCCCCCCCCHHHHHP";
                mapa[6]= "CCHHHHHHHHHHHHHHHHH";
                mapa[7]= "CCCCCCCCCCCCCCCCCCC";
                mapa[8]= "CHHCCCCCCCCCCCCCCCC";
                mapa[9]= "HHHHHHHHHHCCCCCCCCC";
                mapa[10]= "NNNNNNCCCCCCCCCCCCC";
                mapa[11]= "NPCCCCCCCNNNNDHCCCC";
                mapa[12]= "HNNNNNCCCCCCCNNNNNN";
                mapa[13]= "NHHHHHHHHHHHHHHHHHH";
                mapa[14]= "CNNNNNNNNNNNCCCCCCC";
                mapa[15]= "CNNNNNNNCCCCCCCCCCC";
                mapa[16]= "HCCCHHHCCCCCCHHHHHC";
                mapa[17]= "NHHHHCCCCCCCCCCCCCC",
                        mapa[18]= "NHHHHHNNNNNNNNNHHHH";
                mapa[19]= "CCCCHHHHCNNNNPCCCNC";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 3:
                n=1;
                m=32;
                t=100;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "PCCCCCCCCCCCCCCCPCCCPDCPCCCCCCCP";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 4:
                n=4,
                        m=4;
                t=3;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CNHP";
                mapa[1]= "CNHC";
                mapa[2]= "CNHD";
                mapa[3]= "PCNC";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 5:
                n=4;
                m=4;
                t=500;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CCCN";
                mapa[1]= "CCNP";
                mapa[2]= "DHNN";
                mapa[3]= "HHPH";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 6:
                n=4;
                m=4;
                t=500;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CCCN";
                mapa[1]= "CCND";
                mapa[2]= "PHNN";
                mapa[3]= "HHHH";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 7:
                n=4;
                m=5;
                t= 15;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CHHNP";
                mapa[1]= "CCHND";
                mapa[2]= "PHCNC";
                mapa[3]= "HHHCH";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 8:
                n=15;
                m=15;
                t= 25;
                mapa= (char**) malloc(n*sizeof(char*));
                mapa[0]= "CCCCHHHHHHHHHH";
                mapa[1]= "HHHCCCCNNNNNNN";
                mapa[2]= "NNNNNNCCCCCCCC";
                mapa[3]= "CCCCCCCCNNCCCC";
                mapa[4]= "CPCCCCCCCHHHHH";
                mapa[5]="CCCCCHHCCCCCCH";
                mapa[6]= "CCHHHHHHHHHHHH";
                mapa[7]= "CCCCCCCCCCCCCC";
                mapa[8]= "CHHCCCCCCCCCCH";
                mapa[9]= "HHHHHHHHHHCCCC";
                mapa[10]= "NNNNNNCCCCCCCC";
                mapa[11]= "NCCCCCCCCNNNND";
                mapa[12]= "HNNNNNCCCCCCCN";
                mapa[13]= "NHHHHHHHHHHHHH";
                mapa[14]= "CNNNNNNNNNNNCC";
                cesta= zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            default:
                continue;
        }

        printf("cas: %d\n", dlzka_cesty);

        free(cesta);
        for(i=0; i<n; i++){
            free(mapa[i]);
        }
        free(mapa);
    }

    return 0;
}