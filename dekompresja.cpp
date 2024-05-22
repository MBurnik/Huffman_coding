#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <bitset>


using namespace std;

int main()
{
    vector<char> dictionary, key_in_char;
    vector<string> codes;
    char ch;
    int X, N, R, ilosc_znakow, byte_counter = 0, key_offset = 0;
    string code, key;

    cout << "Wprowadz klucz do deszyfrowania: ";
    cin >> key;

    for (int i = 0; i < key.size(); i++) {
        key_in_char.push_back(key[i]);
    }

    //Odczytanie skompresowanego pliku w trybie binarnym i odczytanie ilosci znakow (uzyte przy odczycie ostatniego bajta)
    ifstream fin(".\\skompresowany_moj.txt", ios::binary | ios::in);

    fin.seekg(0, ios::end);
    ilosc_znakow = fin.tellg();
    fin.seekg(0);

    //Odczytanie pierwszego bajta oznaczajacego dlugosc slownika i odczytanie X nastepnych bajtow po stworzenie slownika
    fin.read((char*) &ch, 1);
    byte_counter++;
    //Deszyfrowanie znaku
    X = (int)(char(ch - key_in_char[key_offset]));
    key_offset += 1;
    for (int i = 0; i < X; i++) {
        fin.read((char*) &ch, 1);
        byte_counter++;
        //Deszyfrowanie znaku
        dictionary.push_back(char(ch - key_in_char[key_offset]));
        key_offset += 1;
        if (key_offset >= key.size()) {
            key_offset = 0;
        }
    }

    //Obliczenie ilosci bitow na znak i stworzenie kodow
    N = ceil(log2(X));
    for (int i = 0; i < X; i++) {
        codes.push_back(bitset<8>(i).to_string().erase(0,8-N));
    }

    //Odczytanie 3 bitow oznaczajace nadmiarowosc w ostatnim bajcie
    ofstream fout(".\\zdekompresowany_moj.txt", ios::out);

    bitset<8> byte, bits;
    fin.read((char*) &ch, 1);
    //Deszyfrowanie znaku
    byte = bitset<8>(char(ch - key_in_char[key_offset]));
    key_offset += 1;
    if (key_offset >= key.size()) {
        key_offset = 0;
    }
    byte_counter++;
    int bit_counter = 0;
    int ch_counter = 0;

    R = (int)(char)(bitset<3>(byte.to_string().erase(3,8))).to_ulong();
    byte <<= 3;
    bit_counter += 3;

    //Odczytanie bitow i dekompresja znakow do pliku wyjsciowego
    while (!fin.eof()) {
        if (bit_counter == 8) {
            bit_counter = 0;
            fin.read((char*) &ch, 1);
            //Deszyfrowanie znaku
            byte = bitset<8>(char(ch - key_in_char[key_offset]));
            key_offset += 1;
            if (key_offset >= key.size()) {
                key_offset = 0;
            }
            byte_counter++;
        }

        if (byte_counter == ilosc_znakow) {
            bit_counter = R;
            byte_counter++;
        }

        bits = byte;
        code += (bits).to_string().erase(1,8);
        byte <<= 1;
        ch_counter++;
        if (ch_counter == N) {
            ch_counter = 0;
            for (int j = 0; j < X; j++) {
                if (codes.at(j) == code) {
                    fout << dictionary.at(j);
                    code = "";
                }
            }
        }
        bit_counter++;
    }

    return 0;
}
