// 312CB_Picior_Catalin
#include "bmp_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// lungimea maxima a liniei din fisierul input.txt
#define LINE_LNGTH_MAX 150
// functie de citire din fisierul input.txt
void read_input(char ** img, char ** filter, char ** pooling, char ** cluster) {
    int lngth;
    // alocam memorie la adresele argumentelor functiei
    * img = (char * ) malloc(sizeof(char) * LINE_LNGTH_MAX);
    * filter = (char * ) malloc(sizeof(char) * LINE_LNGTH_MAX);
    * pooling = (char * ) malloc(sizeof(char) * LINE_LNGTH_MAX);
    * cluster = (char * ) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // deschidem fisierul
    FILE * input = fopen("input.txt", "r");
    // citim prima linie a fisierului
    fgets(*img, LINE_LNGTH_MAX, input);
    // calculam lungimea liniei
    lngth = strlen(*img);
    // eliminam caracterul \n adaugat de fgets
    *(*img + lngth - 1) = '\0';
    // citim a doua linie a fisierului
    fgets(*filter, LINE_LNGTH_MAX, input);
    // calculam lungimea liniei
    lngth = strlen(*filter);
    // eliminam caracterul \n adaugat de fgets
    *(*filter + lngth - 1) = '\0';
    // citim a treia linie a fisierului
    fgets(*pooling, LINE_LNGTH_MAX, input);
    // calculam lungimea liniei
    lngth = strlen(*pooling);
    // eliminam caracterul \n adaugat de fgets
    *(*pooling + lngth - 1) = '\0';
    // citim a patra linie a fisierului
    fgets(*cluster, LINE_LNGTH_MAX, input);
    // calculam lungimea liniei
    lngth = strlen(*cluster);
    // eliminam caracterul \n adaugat de fgets
    *(*cluster + lngth - 1) = '\0';
    // inchidem fisierul
    fclose(input);
}
// functie de citire a unei matrice de tip int
int **read_matrix(char *filter, int *dimension) {
    // deschidem fisierul
    FILE *input = fopen(filter, "rt");
    // contori
    int i, j;
    // citim dimensiunea matricei
    fscanf(input, "%d", dimension);
    // alocam memorie pentru linii
    int **matrix = (int **) malloc(sizeof(int *) * (*dimension));
    for (i = 0; i < * dimension; i++) {
        // alocam memorie pentru coloane
        matrix[i] = (int *) malloc(sizeof(int) * (*dimension));
    }
    // citim elementele matricei
    for (i = 0; i < *dimension; i++) {
        for (j = 0; j < *dimension; j++) {
            fscanf(input, "%d", &matrix[i][j]);
        }
    }
    // inchidem fisierul
    fclose(input);
    // returnam adresa matricei
    return matrix;
}
void read_pooling(char *pooling, char *type, int *dimension) {
    FILE *input = fopen(pooling, "rt");
    fscanf(input, "%c", type);
    fscanf(input, "%d", dimension);
    fclose(input);
}
// functie de citire a fisierului cluster
void read_cluster(char *cluster, int *threshold) {
    // deschidem fisierul
    FILE *input = fopen(cluster, "rt");
    // citim valoarea
    fscanf(input, "%d", threshold);
    // inchidem fisierul
    fclose(input);
}
// functie de alocare a unei matrice cu toate elementele 0
int **zero_matrix(int height, int width) {
    // contor
    int i;
    // alocam memorie pentru linii
    int **matrix = calloc(height, sizeof(int *));
    for (i = 0; i < height; i++) {
        // alocam memorie pentru coloane
        matrix[i] = calloc(width, sizeof(int));
    }
    // returnam adresa matricei
    return matrix;
}
// functie de inversare a liniilor unei matrice
void reverse_matrix(int **matrix, int dimension) {
    // contori + valoarea temporara
    int temp, i, j;
    // parcurgem prima jumatate a liniilor matricei
    for (i = 0; i < dimension / 2; i++) {
        for (j = 0; j < dimension; j++) {
            /*
            interschimbam fiecare element al liniilor din
            prima jumatate a matricei cu elementele corespunzatoare
            din a doua jumatate
            */
            temp = matrix[i][j];
            matrix[i][j] = matrix[dimension - i - 1][j];
            matrix[dimension - i - 1][j] = temp;
        }
    }
}
// functie de scriere a unei imagini bmp in fisier
void write_file(bmp_fileheader fheader, bmp_infoheader iheader,
    Pixel **bitmap, char *img) {
    // contori
    int i, j;
    // deschidem fisierul
    FILE *output = fopen(img, "ab");
    // scriem fileheader si infoheader
    fwrite(&fheader, sizeof(bmp_fileheader), 1, output);
    fwrite(&iheader, sizeof(bmp_infoheader), 1, output);
    // calculam padding-ul
    // scriem bitmap-ul imaginii
    int padding_per_line = iheader.width % 4;
    for (i = 0; i < iheader.height; i++) {
        // scriem fiecare linie de pixeli
        fwrite(bitmap[i], sizeof(Pixel), iheader.width, output);
        // adaugam bitii de padding
        for (j = 0; j < padding_per_line; j++) {
            fputc(0, output);
        }
    }
    // inchidem fisierul
    fclose(output);
}
// functie de eliberare a bitmapului
void free_bitmap(Pixel **bitmap, int height) {
    // contor
    int i;
    // eliberam memoria alocata pentru linii
    for (i = 0; i < height; i++) {
        free(bitmap[i]);
    }
    // eliberam matricea de pointeri
    free(bitmap);
}
// functie de eliberare matrice
void free_matrix(int **matrix, int height) {
    // contor
    int i;
    // eliberam memoria alocata pentru linii
    for (i = 0; i < height; i++) {
        free(matrix[i]);
    }
    // eliberam matricea de pointeri
    free(matrix);
}
// functie de verificare a conditiei de adaugare a unui nou pixel intr-o zona
int below_threshold(Pixel crt_pixel, Pixel new_pixel, int threshold) {
    // valorile diferentelor dintre pixelul curent si pixelul nou
    int diff_b = abs(crt_pixel.blue - new_pixel.blue);
    int diff_g = abs(crt_pixel.green - new_pixel.green);
    int diff_r = abs(crt_pixel.red - new_pixel.red);
    // se intoarce 1 daca poate fi adaugat intr-o zona
    if (diff_r + diff_g + diff_b <= threshold) {
        return 1;
    }
    // se intoare 0 daca nu poate fi adaugat
    return 0;
}
// functie de schimbare a imaginii in alb-negru
void black_white(Pixel **bitmap, bmp_fileheader fheader,
    bmp_infoheader iheader, char *img) {
    // declarii contori+ alte variabile
    int i, j;
    int sum_value;
    // pentru fiecare pixel calculam valoarea medie a valorii sale
    for (i = 0; i < iheader.height; i++) {
        for (j = 0; j < iheader.width; j++) {
            sum_value =
            bitmap[i][j].red + bitmap[i][j].blue + bitmap[i][j].green;
            // actualizam fiecare canal de culoare
            bitmap[i][j].red = sum_value / 3;
            bitmap[i][j].green = sum_value / 3;
            bitmap[i][j].blue = sum_value / 3;
        }
    }
    // alocam memorie pentru numele nou al fisierului bmp
    char *write_filename = (char *) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // copiem numele imaginii initiale
    strcpy(write_filename, img);
    // retinem denumirea propriu-zisa, fara extensia .bmp
    write_filename = strtok(write_filename, ".");
    // adaugam numele specific task-ului
    strcat(write_filename, "_black_white.bmp");
    // scriem imaginea rezultata intr-un fisier
    write_file(fheader, iheader, bitmap, write_filename);
    // eliberam sirul alocat pentru numele noii imagini
    free(write_filename);
}
// functie de completare a unei imagini pentru a fi patratica
void no_crop(Pixel **bitmap, bmp_fileheader fheader,
    bmp_infoheader iheader, char *img) {
    // contori
    int i, j;
    // difinim un pixel alb
    Pixel white = {
        255,
        255,
        255
    };
    // alocam memorie pentru numele nou al fisierului bmp
    char *write_filename = (char *) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // copiem numele imaginii initiale
    strcpy(write_filename, img);
    // retinem denumirea propriu-zisa, fara extensia .bmp
    write_filename = strtok(write_filename, ".");
    // adaugam numele specific task-ului
    strcat(write_filename, "_nocrop.bmp");
    // daca imaginea e deja patrata scriem imaginea nemodificata
    if (iheader.width == iheader.height) {
        write_file(fheader, iheader, bitmap, write_filename);
        free(write_filename);
    }
    // daca latimea > lungimea imaginii
    if (iheader.width > iheader.height) {
        // dimensiunea devine latimea
        int size = iheader.width;
        /*
         numarul de linii cu pixeli albi de trebuie adaugate
         deasupra si dedesubtul imaginii
         */ 
        int up = (iheader.width - iheader.height) / 2;
        int down = (iheader.width - iheader.height) - up;
        // alocam o matrice de pixeli de dimensiune size
        Pixel **no_crop_bitmap = (Pixel **) malloc(sizeof(Pixel *) * size);
        for (i = 0; i < size; i++) {
            no_crop_bitmap[i] = (Pixel *) malloc(sizeof(Pixel) * size);
        }
        // plasam mai intai pixelii albi de jos(imaginea este citata invers)
        for (i = 0; i < down; i++) {
            for (j = 0; j < size; j++) {
                no_crop_bitmap[i][j] = white;
            }
        }
        // plasam pixelii imaginii initiale
        for (i = 0; i < iheader.height; i++) {
            for (j = 0; j < iheader.width; j++) {
                no_crop_bitmap[i + down][j] = bitmap[i][j];
            }
        }
        // plasam pixelii albi de sus
        for (i = 0; i < up; i++) {
            for (j = 0; j < size; j++) {
                no_crop_bitmap[i + down + iheader.height][j] = white;
            }
        }
        // modificam inaltimea imaginii
        iheader.height = iheader.width;
        // scriem imaginea rezultata intr-un fisier
        write_file(fheader, iheader, no_crop_bitmap, write_filename);
        // eliberam sirul alocat pentru numele noii imagini
        free(write_filename);
        // eliberam matricea de pixeli a noii imagini
        free_bitmap(no_crop_bitmap, size);
        // daca inaltimea > latimea si imaginea nu e deja patratica
    } else if (iheader.width != iheader.height) {
        // calculam noua dimensiune a bitmapului
        int size = iheader.height;
        // calculam numarul de coloane de pixeli albi de pe marginile imaginii
        int left = (iheader.height - iheader.width) / 2;
        int right = (iheader.height - iheader.width) - left;
        // alocam memorie pentru o matrice de pixeli de dimensiune size
        Pixel **no_crop_bitmap = (Pixel **) malloc(sizeof(Pixel *) * size);
        for (i = 0; i < size; i++) {
            no_crop_bitmap[i] = (Pixel *) malloc(sizeof(Pixel) * size);
        }
        // pentru fiecare linie a bitmapului nou
        for (i = 0; i < iheader.height; i++) {
            // scriem pixelii albi din marginea stanga
            for (j = 0; j < left; j++) {
                no_crop_bitmap[i][j] = white;
            }
            // scriem pixelii imaginii initiale
            for (j = 0; j < iheader.width; j++) {
                no_crop_bitmap[i][j + left] = bitmap[i][j];
            }
            // scriem pixelii albi din marginea dreapta
            for (j = 0; j < right; j++) {
                no_crop_bitmap[i][j + left + iheader.width] = white;
            }
        }
        // actualizam latimea imaginii in infoheader
        iheader.width = iheader.height;
        // scriem noua imagine intr-un fisier
        write_file(fheader, iheader, no_crop_bitmap, write_filename);
        // eliberam sirul alocat pentru numele noii imagini
        free(write_filename);
        // eliberam matricea de pixeli a bitmapului nou
        free_bitmap(no_crop_bitmap, size);
    }
}
// functie de aplicare a filtrelor de convolutie
void convolution(Pixel **bitmap, Pixel **copy, bmp_fileheader fheader,
    bmp_infoheader iheader, char *img, char *filter) {
    // declaratii variabile
    int dimension, **filter_matrix, i, j, k, l;
    int sum_b, sum_g, sum_r;
    int height = iheader.height;
    int width = iheader.width;
    int line, col;
    // citim matricea de filtru
    filter_matrix = read_matrix(filter, &dimension);
    // inversam matricea de filtru
    reverse_matrix(filter_matrix, dimension);
    // aplicam algoritmul de prelucrare a pixelilor
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // setam suma fiecarui canal de culoare cu 0
            sum_b = 0;
            sum_g = 0;
            sum_r = 0;
            for (k = i - dimension / 2; k <= i + dimension / 2; k++) {
                for (l = j - dimension / 2; l <= j + dimension / 2; l++) {
                    // verificam daca vecinii pixelului curent exista
                    if (k >= 0 && l >= 0 && k < height && l < width) {
                        /*
                        calculam linia si coloana corespunzatoare
                        elementului din matricea de filtru
                        */
                        line = k - i + dimension / 2;
                        col = l - j + dimension / 2;
                        // iteram suma valorilo pentru fiecare culoare
                        sum_b += bitmap[k][l].blue * filter_matrix[line][col];
                        sum_g += bitmap[k][l].green * filter_matrix[line][col];
                        sum_r += bitmap[k][l].red * filter_matrix[line][col];
                    }
                }
            }
            /*
            verificam daca sumele trebuie modificate si actualizam
            pixelul nou din copia imaginii initiale
            */
            if (sum_b < 0) {
                copy[i][j].blue = 0;
            }
            if (sum_b > 255) {
                copy[i][j].blue = 255;
            }
            if (sum_b >= 0 && sum_b <= 255) {
                copy[i][j].blue = sum_b;
            }
            if (sum_g < 0) {
                copy[i][j].green = 0;
            }
            if (sum_g > 255) {
                copy[i][j].green = 255;
            }
            if (sum_g >= 0 && sum_g <= 255) {
                copy[i][j].green = sum_g;
            }
            if (sum_r < 0) {
                copy[i][j].red = 0;
            }
            if (sum_r > 255) {
                copy[i][j].red = 255;
            }
            if (sum_r >= 0 && sum_r <= 255) {
                copy[i][j].red = sum_r;
            }
        }
    }
    // alocam memorie pentru numele nou al fisierului bmp
    char *write_filename = (char *) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // copiem denumirea initiala a fisierului bmp
    strcpy(write_filename, img);
    // pastram denumirea initiala a fisierului, mai putin extensia .bmp
    write_filename = strtok(write_filename, ".");
    // adugam numele specific task-ului
    strcat(write_filename, "_filter.bmp");
    // scriem noua imagine intr-un fisier
    write_file(fheader, iheader, copy, write_filename);
    // eliberam sirul alocat pentru numele noii imagini
    free(write_filename);
    // eliberam matricea de filtru
    free_matrix(filter_matrix, dimension);
}
// functie de aplicare a operatiei de pooling
void pooling(Pixel **bitmap, Pixel **copy, bmp_fileheader fheader,
    bmp_infoheader iheader, char *img, char *pooling) {
    // declaratii varibile
    int i, j, k, l;
    int max_b, max_g, max_r, min_b, min_g, min_r, size, crt_b, crt_g, crt_r;
    char type;
    int height = iheader.height;
    int width = iheader.width;
    // citim fiserul de pooling
    read_pooling(pooling, &type, &size);
    /*
    calculam distanta fata de pixelul curent
    pentru a determina cu cati vecini ai acestuia
    lucram
    */
    int distance = size / 2;
    // cazul pentru filtru de minim
    if (type == 'm') {
        // aplicam algoritmul pentru fiecare pixel al imaginii
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                /*
                initializam minimul pentru fiecare culoare cu valorile
                culorilor pixelului curent
                */
                min_b = bitmap[i][j].blue;
                min_g = bitmap[i][j].green;
                min_r = bitmap[i][j].red;
                for (k = i - distance; k <= i + distance; k++) {
                    for (l = j - distance; l <= j + distance; l++) {
                        // verificam daca vecinii pixelului exista
                        if (k >= 0 && l >= 0 && k < height && l < width) {
                            // setam valorile culorilor pixelilor vecini
                            crt_b = bitmap[k][l].blue;
                            crt_g = bitmap[k][l].green;
                            crt_r = bitmap[k][l].red;
                            // actualizam fiecare minim, daca este cazul
                            if (crt_b < min_b) {
                                min_b = crt_b;
                            }
                            if (crt_g < min_g) {
                                min_g = crt_g;
                            }
                            if (crt_r < min_r) {
                                min_r = crt_r;
                            }
                            /*
                            actualizam fiecare minim cu 0 daca
                            vecinii pixelului nu se afla in matrice
                            */
                        } else {
                            if (min_b > 0) {
                                min_b = 0;
                            }
                            if (min_g > 0) {
                                min_g = 0;
                            }
                            if (min_r > 0) {
                                min_r = 0;
                            }
                        }
                    }
                }
                /*
                actualizam cu valorile necesare pixelul
                corespunzator din copie
                */
                copy[i][j].blue = min_b;
                copy[i][j].green = min_g;
                copy[i][j].red = min_r;
            }
        }
    }
    // cazul pentru filtru de maxim
    if (type == 'M') {
        // aplicam algoritmul pentru fiecacre pixel al imaginii
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                 /*
                initializam maximul pentru fiecare culoare cu valorile
                culorilor pixelului curent
                */
                max_b = bitmap[i][j].blue;
                max_g = bitmap[i][j].green;
                max_r = bitmap[i][j].red;
                for (k = i - distance; k <= i + distance; k++) {
                    for (l = j - distance; l <= j + distance; l++) {
                        // verificam daca vecinii pixelului exista
                        if (k >= 0 && l >= 0 && k < height && l < width) {
                            // setam valorile culorilor pixelilor vecini
                            crt_b = bitmap[k][l].blue;
                            crt_g = bitmap[k][l].green;
                            crt_r = bitmap[k][l].red;
                            // actualizam valorile de maxim daca este cazul
                            if (crt_b > max_b) {
                                max_b = crt_b;
                            }
                            if (crt_g > max_g) {
                                max_g = crt_g;
                            }
                            if (crt_r > max_r) {
                                max_r = crt_r;
                            }
                            /*
                            actualizam fiecare maxim cu 0 daca
                            vecinii pixelului nu se afla in matrice
                            */
                        } else {
                            if (max_b < 0) {
                                max_b = 0;
                            }
                            if (max_g < 0) {
                                max_g = 0;
                            }
                            if (max_r < 0) {
                                max_r = 0;
                            }
                        }
                    }
                }
                /*
                actualizam cu valorile necesare pixelul
                corespunzator din copie
                */ 
                copy[i][j].blue = max_b;
                copy[i][j].green = max_g;
                copy[i][j].red = max_r;
            }
        }
    }
    // alocam memorie pentru numele noului fisier bmp
    char *write_filename = (char *) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // copiem numele fisierului initial
    strcpy(write_filename, img);
    // pastram numele fisierului initial, mai putin extensia .bmp
    write_filename = strtok(write_filename, ".");
    // adaugam numele specific task-ului
    strcat(write_filename, "_pooling.bmp");
    // scriem noua imagine bmp
    write_file(fheader, iheader, copy, write_filename);
    // eliberam sirul alocat pentru numele noii imagini
    free(write_filename);
}
void clustering(Pixel ** bitmap, Pixel **copy, bmp_fileheader fheader,
    bmp_infoheader iheader, char *img, char *cluster) {
    // declaratii variabile
    int checked = 1, first, last;
    int threshold;
    int *queue_line, *queue_col, sum_b, sum_g, sum_r, count;
    int i, j, k, l, m, n, t, q;
    int height = iheader.height;
    int width = iheader.width;
    /*
    cream doua cozi in care vom stoca indicii elementelor din
    matrice care verifica conditiile algoritmului
    */ 
    queue_col = calloc(height * width, sizeof(int));
    queue_line = calloc(height * width, sizeof(int));
    /*
    cream doi vectori in care stocam indicii pixelilor care
    trebuie modificati
    */
    int * saved_line = calloc(height * width, sizeof(int));
    int * saved_col = calloc(height * width, sizeof(int));
    /*
    cream doi vectori de 4 elemente fiecare ce reprezinta
    numere ce trebuie adaugate pentru a ne putea deplasa la
    vecinii unui pixel(stanga, sus, dreapta si jos)
    */
    int direction_line[] = {
        -1,
        0,
        1,
        0
    };
    int direction_col[] = {
        0,
        1,
        0,
        -1
    };
    /* 
    cream o matrice cu dimensiunile bitmap-ului
    si initializam cu 0 fiecare element
    */
    int ** ok = zero_matrix(height, width);
    // citim valoarea de threshold din fisierul de cluster
    read_cluster(cluster, & threshold);
    // parcurgem fiecare pixel al bitmap-ului
    for (i = height - 1; i >= 0; i--) {
        for (j = 0; j < width; j++) {
            // daca pixelul nu e deja intr-o zona
            if (ok[i][j] == 0) {
                // initializam sumele canalelor de culoare
                sum_b = bitmap[i][j].blue;
                sum_g = bitmap[i][j].green;
                sum_r = bitmap[i][j].red;
                // zona are momentam un pixel
                count = 1;
                // stocam indicii pixelului
                saved_line[0] = i;
                saved_col[0] = j;
                // adaugam pixelul curent intr-o zona
                ok[i][j] = checked;
                /*
                setam primul element al fiecarei cozi cu
                indicii pixelului care nu facea parte din nicio zona
                */
                first = 0;
                last = 0;
                queue_line[first] = i;
                queue_col[last] = j;
                // cautam cat timp coada este nevida
                while (first <= last) {
                    // setam indicii pixelului pe care il vom analiza
                    k = queue_line[first];
                    l = queue_col[first];
                    /*
                    verificam daca vecinii pixelului fac si ei parte
                    din aceeasi zona
                    */ 
                    for (t = 0; t <= 3; t++) {
                        // retinem indicii pixelilor vecini
                        m = k + direction_line[t];
                        n = l + direction_col[t];
                        // verificam daca pixelul este in bitmap
                        if (m >= 0 && n >= 0 && m < height && n < width) {
                            /*
                            conditia de apartenenta la aceeasi zona +
                            vecinul sa nu faca deja parte dintr-o zona
                            */
                            if (below_threshold(bitmap[i][j], bitmap[m][n],
                                threshold) == 1 && ok[m][n] == 0) {
                                // marcam pixelul ca fiind intr-o zona
                                ok[m][n] = checked;
                                // crestem numarul de pixeli din zona aferenta
                                count++;
                                /*
                                retinem indicii pixelilor care
                                trebuie modificati
                                */
                                saved_line[count - 1] = m;
                                saved_col[count - 1] = n;
                                /*
                                adaugam pixelii care satisfac conditiile
                                in coada
                                */
                                last++;
                                queue_line[last] = m;
                                queue_col[last] = n;
                                // iteram sumele canalelor de culori
                                sum_b += bitmap[m][n].blue;
                                sum_g += bitmap[m][n].green;
                                sum_r += bitmap[m][n].red;
                            }
                        }
                    }
                    // am terminat de analizat pixelul(il eliminam din coada)
                    first++;
                }
                // modificam toti pixelii dintr-o zona cu valoare medie
                for (q = 0; q < count; q++) {
                    copy[saved_line[q]][saved_col[q]].blue = sum_b / count;
                    copy[saved_line[q]][saved_col[q]].green = sum_g / count;
                    copy[saved_line[q]][saved_col[q]].red = sum_r / count;
                }
            }
        }
    }
    // alocam memorie pentru numele noului fisier bmp
    char *write_filename = (char *) malloc(sizeof(char) * LINE_LNGTH_MAX);
    // copiem numele fisierului initial
    strcpy(write_filename, img);
    // pastram numele fisierului initial, mai putin extensia .bmp
    write_filename = strtok(write_filename, ".");
    // adaugam numele specific task-ului
    strcat(write_filename, "_clustered.bmp");
    // copiem noua imagine intr-un fisier
    write_file(fheader, iheader, copy, write_filename);
    // eliberam sirul alocat pentru numele noii imagini
    free(write_filename);
    // eliberam cozile
    free(queue_col);
    free(queue_line);
    // eliberam vectorii in care am retinut toti pixelii unei zone
    free(saved_line);
    free(saved_col);
    /*
    eliberam matricea in care se retinea daca pixelii fac parte
    dintr-o anumita zona
    */
    free_matrix(ok, height);
}
// functie de citire a unei imagini bmp
Pixel **read_img(bmp_fileheader *fheader, bmp_infoheader *iheader,
    char *img) {
    // contor
    int i;
    // deschidem fisierul
    FILE *bmp_file = fopen(img, "rb");
    // citim anteturile fileheader si infoheader
    fread(fheader, sizeof(bmp_fileheader), 1, bmp_file);
    fread(iheader, sizeof(bmp_infoheader), 1, bmp_file);
    // calculam valoarea de padding
    int padding_per_line = iheader -> width % 4;
    // alocam memorie pentru zona bitmap-ului
    Pixel **bitmap = (Pixel ** ) malloc(sizeof(Pixel *) * iheader -> height);
    // setam pozitia cursorului din fisier exact la inceputul bitmap-ului
    fseek(bmp_file, fheader -> imageDataOffset, SEEK_SET);
    for (i = 0; i < iheader -> height; i++) {
        bitmap[i] = (Pixel *) malloc(sizeof(Pixel) * iheader -> width);
        // citim imaginea linie cu linie
        fread(bitmap[i], sizeof(Pixel), iheader -> width, bmp_file);
        // ignoram padding-ul
        fseek(bmp_file, padding_per_line, SEEK_CUR);
    }
    // inchidem fisierul
    fclose(bmp_file);
    // returnam adresa bitmap-ului
    return bitmap;
}
// functie de copiere a unei imagini(bitmap)
Pixel **copy_img(Pixel **bitmap, int height, int width) {
    // contor
    int i;
    // alocam memorie pentru un nou bitmap
    Pixel **copy = (Pixel **) malloc(sizeof(Pixel *) * height);
    for (i = 0; i < height; i++) {
        copy[i] = (Pixel *) malloc(sizeof(Pixel) * width);
        // copiem linie cu linie bitmap-ul original
        memcpy(copy[i], bitmap[i], sizeof(Pixel) * width);
    }
    // returnam adresa copiei de bitmap
    return copy;
}
int main() {
    // citim liniile fisierului de input
    char *image = NULL, *filter = NULL, *pool = NULL, *cluster = NULL;
    read_input(&image, &filter, &pool, &cluster);
    // declaram headerele
    bmp_fileheader fheader;
    bmp_infoheader iheader;
    // citim imaginea din fisierul de input
    Pixel **bitmap = read_img(&fheader, &iheader, image);
    // cream o copie a imaginii
    Pixel **B_W_bitmap = copy_img(bitmap, iheader.height, iheader.width);
    // task 1
    black_white(B_W_bitmap, fheader, iheader, image);
    // eliberam copia
    free_bitmap(B_W_bitmap, iheader.height);
    // task 2
    no_crop(bitmap, fheader, iheader, image);
    // cream o copie a imaginii
    Pixel **convo_bitmap = copy_img(bitmap, iheader.height, iheader.width);
    // task 3
    convolution(bitmap, convo_bitmap, fheader, iheader, image, filter);
    // eliberam copia
    free_bitmap(convo_bitmap, iheader.height);
    // cream o copie a imaginii
    Pixel **pooling_bitmap = copy_img(bitmap, iheader.height, iheader.width);
    // task 4
    pooling(bitmap, pooling_bitmap, fheader, iheader, image, pool);
    // eliberam copia
    free_bitmap(pooling_bitmap, iheader.height);
    // cream o copie a imaginii
    Pixel **cluster_bitmap = copy_img(bitmap, iheader.height, iheader.width);
    // task 5
    clustering(bitmap, cluster_bitmap, fheader, iheader, image, cluster);
    // eliberam copia
    free_bitmap(cluster_bitmap, iheader.height);
    // eliberam imaginea
    free_bitmap(bitmap, iheader.height);
    // eliberam memoria alocata pentru cele 4 linii ale fisierului de input
    free(image);
    free(filter);
    free(pool);
    free(cluster);
    return 0;
}