#include <iostream>
#include <fstream>
#include "Decryptor.h"

using namespace std;

Decryptor::Decryptor() {
    baseCode = deep = indent = 0;
    for ( unsigned short i = 128; i < 256; ++i) {
        frequency [i] = 0;
    }
}
// true  == ERROR
// false == GOOD
bool 
Decryptor::start (const char* iName, const char* oName) {

    if (readFile (iName) == true) {
        cout << "The file not found." << endl; //
        return true;
    }

    if (findMostPopular() == true) {
        cout << "The file contains only ASCII symbols." << endl;
        return false;
    }
    
    if (findWay() == true) {
        cout << "Unable to restore file." << endl;
        return true;
    }

    if (deep == 0) {
        cout << "The file is correct." << endl;
        return false;
    }
    else {
        writeFile (iName, oName);
    }      
    return false;
}

bool 
Decryptor::readFile (const char* iFileName) { //
    unsigned char c;
    ifstream iFile;
    
    iFile.open (iFileName);
    if (!iFile) {
        return true;
    }

    for ( unsigned short i = 0; i < 5000; i++) {
        iFile >> c;
        if (iFile.eof()) {
            break;
        }
        ++frequency [c];
    }
    
    iFile.close();
    return false;
}

bool 
Decryptor::findMostPopular() {
    top = 128;
    for (unsigned short i = 129; i < 256; i++) {
        if (frequency [top] < frequency [i]) {
            top = i;
        }
    }
    if (frequency [top] == 0) {
        return true;
    }
    return false;
}

void
Decryptor::writeFile (const char* iFileName, const char* oFileName) {
    unsigned char   fromTo [256],
                    rules  [deep],
                    c;
    unsigned short i, j;
    j = indent;

    for ( i = deep ; i != 0 ; --i) {
        rules [i - 1] = j % 6;
        j = j / 6;
    }
    
    cout << '#' << baseCodeName [baseCode - 1] << endl;
    
    for ( j = 0; j < deep; ++j) {
        cout << '#' << j + 1 << ":  " << tablesName [rules[j]] << endl;
    }

    for (i = 0; i < 256; ++i) {
        fromTo [i] = i ;
    }

    for (i = 128; i < 256; ++i) {
        for( j = 0; j < deep; ++j) {
            // tables [rule] [symbol]
            fromTo [i] = tables [rules[j]] [fromTo[i]&0x7f];
        }
    }
    ifstream iFile;
    ofstream oFile;
    iFile.open (iFileName,ios_base::binary);
    oFile.open (oFileName);
    
    iFile.unsetf(std::ios::skipws);
    for (iFile >> c; !iFile.eof(); iFile >> c) {
        oFile << fromTo [c];
    }

    iFile.close();
    oFile.close();
}


bool 
Decryptor::findWay () {
    unsigned char* letter [4];
    unsigned short t, max = 1;
    
    if (checkSymbol (top) == false) {
        return false;
    }
        
    letter[0] = new unsigned char (top);
    for (unsigned short d = 1; d < 4; ++d) {
        letter[d] = new unsigned char [max * 6];
        for (unsigned short j = 0; j < max; ++j) {
            for (unsigned char rule = 0; rule < 6; ++rule) {
                t = j * 6 + rule;
                letter [d][t] = tables[rule] [(letter [d -1][j])&0x7f];
                if (checkSymbol (letter [d][t]) == false) {
                    cout << "answer  " << t  << endl;
                    deep   = d;
                    indent = t;
                    delete [] letter[d - 1];
                    delete [] letter[d];
                    return false;
                }
            }
        }
        max *= 6;
        delete [] letter[d - 1];
    }
    return true;
}

int 
Decryptor::checkSymbol (unsigned char letter) {
    switch (letter) {
        case CP1251_SYMBOL :
            baseCode = CP1251_INDEX;
            return false;
        case CP866_SYMBOL  :
            baseCode = CP866_INDEX;
            return false;
        case KOI8R_SYMBOL  :
            baseCode = KOI8R_INDEX;
            return false;
        default :
            return true;
    }    
}

int main (int argc, char *argv[]) {
    Decryptor decryptor;

    if (argc < 2) {
        cout << "I need name of file." << endl;
        return 1;
    }
    if (argc < 3) {
        cout << "I need name of result file." << endl;
        return 1;
    }
    decryptor.start (argv[1], argv[2]);
    return 0;
}