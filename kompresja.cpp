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
    int X, N, ilosc_znakow = 0, R, key_offset = 0;
    string code, key;
    cout << "Wprowadz klucz do szyfrowania: ";
    cin >> key;

    for (int i = 0; i < key.size(); i++) {
        key_in_char.push_back(key[i]);
    }

    //Stworzenie slownika i podliczenie ilosci znakow
    ifstream fin;
    fin.open(".\\do_kompresji.txt", ios::in);
    while (fin >> noskipws >> ch) {
        if (count(dictionary.begin(), dictionary.end(), ch)) {

        }
        else {
            dictionary.push_back(ch);
        }
        ilosc_znakow++;
    }

    sort(dictionary.begin(), dictionary.end());

    //Obliczenie rozmiaru slownika, ilosci bitów na kod znaku, nadmiarowosc bitow
    X = (int)dictionary.size();
    cout << "Slownik: ";
    for (int i = 0; i < X; i++){
        cout << dictionary.at(i);
    }
    cout << endl << "X: " << X << endl;

    N = ceil(log2(X));
    cout << "N: " << N << endl;

    R = (8-(3 + ilosc_znakow*N)%8)%8;
    cout << "R: " << R << endl;

    cout << "Dlugosc tekstu: " << ilosc_znakow << endl;

    //Stworzenie kodow dla znakow
    for (int i = 0; i < X; i++){
        codes.push_back(bitset<8>(i).to_string().erase(0,8-N));
    }

    fin.clear();
    fin.seekg(0);

    //Zapisanie ilosci elementów slownika i oznaczen nadmiaru bitow w formie zahaslowanej
    ofstream fout(".\\skompresowany_moj.txt", ios::binary | ios::out);

    fout << char(char(bitset<8>(X).to_ulong()) + key_in_char[key_offset]);
    key_offset += 1;

    for (int i = 0; i < X; i++) {

        fout << char(dictionary.at(i) + key_in_char[key_offset]);
        key_offset += 1;
        if (key_offset >= key.size()) {
            key_offset = 0;
        }
    }

    bitset<8> byte = 0;
    int bit_counter = 0;

    bitset<3> temp = R;
    string last_bits;
    last_bits = temp.to_string();

    for (int i = 0; i<last_bits.size(); i++) {
        if (last_bits[i] != '0') {
            byte |= 0b00000001;
        }
        byte <<= 1;
        bit_counter++;
    }

    //Zapisanie znakow skompresowanych w formie zahaslowanej
    while (fin >> ch) {
        for (int i = 0; i < X; i++) {
            if (ch == dictionary.at(i)) {
                code = codes.at(i);
                for (int j = 0; j<code.size(); j++) {
                    if (code[j] != '0') {
                        byte |= 0b00000001;
                    }
                    bit_counter++;
                    if (bit_counter == 8) {
                        fout << char(char(byte.to_ulong()) + key_in_char[key_offset]);
                        key_offset += 1;
                        if (key_offset >= key.size()) {
                            key_offset = 0;
                        }
                        bit_counter = 0;
                        byte = 0;
                    }
                    byte <<= 1;
                }
            }
        }
    }

    //Ewentualne dopelnienie bitami nadmiarowymi do bajta i zapisanie go w formie zahaslowanej
    byte >>= 1;
    if ((bit_counter > 0) && (bit_counter < 8)) {
        for (int i = 0; i < R; i++) {
            byte <<= 1;
            byte |= 0b00000001;
            bit_counter++;
        }
        fout << char(char(byte.to_ulong()) + key_in_char[key_offset]);
    }
    fin.close();
    fout.close();

    //Przeliczenie dlugosci znakow w skompresowanym pliku
    fin.open(".\\skompresowany_moj.txt", ios::in);
    fin.seekg(0, ios::end);
    ilosc_znakow = fin.tellg();
    fin.close();
    cout << "Dlugosc tekstu skompresowanego: " << ilosc_znakow;

    return 0;
}
