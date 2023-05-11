#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace this_thread;
using namespace chrono_literals;
namespace fs = filesystem;

// Tableaux des fichiers des dossiers à synchroniser
static vector<string> TokenizedPathOne;
static vector<string> TokenizedPathTwo;
static vector<string> TabFilesOneOld;
static vector<string> TabFilesTwoOld;
static vector<string> FileNamesOne;
static vector<string> FileNamesTwo;
static vector<string> TabFilesOne;
static vector<string> TabFilesTwo;

// Path des dossiers à synchroniser
static fs::path dirPathOne("C:/Users/vpali/Desktop/TEST");
static fs::path dirPathTwo("C:/Users/vpali/Desktop/DESTINATION");

// Fonctions
void init();
void GetFileNames();
void Sync(int Indicator);
void TokenizePath(int i);
int CompareVersion();
string GetDirFilePaths(const fs::path &dirPath, vector<string> &TabFiles);

// temp fonction
void Affiche(int Indicator);
void AfficheFileNames(int Indicator);
void AfficheTokenizedPath(int Indicator);

int main()
{
    // Initialisation
    init();
    Affiche(1);
    Affiche(2);
    AfficheFileNames(1);

    // string temp;
    // cin >> temp;

    // Affiche(1);
    // Affiche(2);

    // // Boucle infinie
    // while (true)
    // {
    //     sleep_for(1s);
    //     Sync(CompareVersion());
    // }

    // return 0;
}

// Synchronisation en fonction du dossier modifié
void Sync(int Indicator)
{
    GetFileNames();
    int DiffPos;
    // suppression des fichiers du dossier plus ancien et copie du nouveau
    if (Indicator == 1)
    {
        for (int i = 0; i < FileNamesOne.size(); i++)
        {
            if (FileNamesOne[i] != FileNamesTwo[i])
            {
                DiffPos = i;
                break;
            }
        }
    }
    if (Indicator == 2)
    {
        fs::remove_all(dirPathOne);
        fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);
    }
}

// void Sync(int Indicator)
// {
//     string newpathOne = dirPathOne.string();
//     string newpathTwo = dirPathTwo.string();
//     // suppression des fichiers du dossier plus ancien et copie du nouveau
//     if (Indicator == 1)
//     {
//         for (int i = 0; i < TabFilesOne[i].size(); i++)
//         {
//             if (TabFilesOne[i] != TabFilesTwo[i])
//             {
//                 TokenizePath(i);

//                 for (int j = 0; j < TokenizedPathOne.size() - 1; j++)
//                 {
//                     newpathOne = newpathOne + "\\" + TokenizedPathOne[j];
//                     newpathTwo = newpathTwo + "\\" + TokenizedPathOne[j];
//                 }
//                 fs::remove_all(newpathOne);
//             }
//         }
//     }
//     if (Indicator == 2)
//     {
//         fs::remove_all(dirPathOne);
//         fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);
//     }
// }

// void Sync(int Indicator)
// {
//     // suppression des fichiers du dossier plus ancien et copie du nouveau
//     if (Indicator == 1)
//     {
//         fs::remove_all(dirPathTwo);
//         fs::copy(dirPathOne, dirPathTwo, fs::copy_options::recursive);
//     }
//     if (Indicator == 2)
//     {
//         fs::remove_all(dirPathOne);
//         fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);
//     }
// }

void GetFileNames()
{
    FileNamesOne.clear();
    FileNamesTwo.clear();

    for (int i = 0; i < TabFilesOne.size(); i++)
    {
        TokenizePath(i);
        FileNamesOne.push_back(TokenizedPathOne[TokenizedPathOne.size() - 1]);
    }
    for (int i = 0; i < TabFilesTwo.size(); i++)
    {
        TokenizePath(i);
        FileNamesTwo.push_back(TokenizedPathTwo[TokenizedPathTwo.size() - 1]);
    }
}

void TokenizePath(int i)
{
    TokenizedPathOne.clear();
    TokenizedPathTwo.clear();

    string temp;

    temp = TabFilesOne[i];
    while (temp.find("\\") != string::npos)
    {
        TokenizedPathOne.push_back(temp.substr(0, temp.find("\\")));
        temp = temp.substr(temp.find("\\") + 1);
    }
    TokenizedPathOne.push_back(temp);

    temp = TabFilesTwo[i];
    while (temp.find("\\") != string::npos)
    {
        TokenizedPathTwo.push_back(temp.substr(0, temp.find("\\")));
        temp = temp.substr(temp.find("\\") + 1);
    }
    TokenizedPathTwo.push_back(temp);
}

// Initialisation
void init()
{
    // Initialisation des tableaux de path fichiers
    TabFilesOneOld.clear();
    TabFilesOne.clear();
    GetDirFilePaths(dirPathOne, TabFilesOne);

    TabFilesTwoOld.clear();
    TabFilesTwo.clear();
    GetDirFilePaths(dirPathTwo, TabFilesTwo);

    // Dossier 1 à la priorité à l'initialisation TODO: enlever comm
    // Sync(1);
}

// Recherche de modifications dans les dossiers
int CompareVersion()
{
    TabFilesOneOld.clear();
    TabFilesOneOld = TabFilesOne;
    TabFilesOne.clear();
    GetDirFilePaths(dirPathOne, TabFilesOne);

    TabFilesTwoOld.clear();
    TabFilesTwoOld = TabFilesTwo;
    TabFilesTwo.clear();
    GetDirFilePaths(dirPathTwo, TabFilesTwo);

    if (TabFilesOne.size() != TabFilesOneOld.size())
    {
        return 1;
    }
    if (TabFilesTwo.size() != TabFilesTwoOld.size())
    {
        return 2;
    }
    for (int i = 0; i < TabFilesOne.size(); i++)
    {
        if (TabFilesOne[i] != TabFilesOneOld[i])
        {
            return 1;
        }
    }
    for (int i = 0; i < TabFilesTwo.size(); i++)
    {
        if (TabFilesTwo[i] != TabFilesTwoOld[i])
        {
            return 2;
        }
    }

    return 0;
}

// Récupération des path des fichiers d'un dossier
string GetDirFilePaths(const fs::path &dirPath, vector<string> &TabFiles)
{
    if (!fs::exists(dirPath))
    {
        return ("ERREUR : Le dossier " + dirPath.string() + " N'existe pas");
    }

    if (!fs::is_directory(dirPath))
    {
        return ("ERREUR : " + dirPath.string() + " n'est pas un dossier");
    }

    for (auto &entry : fs::recursive_directory_iterator(dirPath))
    {
        if (fs::is_regular_file(entry.path()))
        {
            string pathtemp = entry.path().string().substr(entry.path().string().find(dirPath.string()) + dirPath.string().size() + 1);
            TabFiles.push_back(pathtemp);
        }
    }

    return ("OK");
}

// temp fonction

void Affiche(int Indicator)
{
    if (Indicator == 1)
    {
        cout << "Dossier 1" << endl;
        for (int i = 0; i < TabFilesOne.size(); i++)
        {
            cout << TabFilesOne[i] << endl;
        }
    }
    if (Indicator == 2)
    {
        cout << "Dossier 2" << endl;
        for (int i = 0; i < TabFilesTwo.size(); i++)
        {
            cout << TabFilesTwo[i] << endl;
        }
    }
}

void AfficheTokenizedPath(int Indicator)
{
    if (Indicator == 1)
    {
        cout << "Path 1" << endl;
        for (int i = 0; i < TokenizedPathOne.size(); i++)
        {
            cout << TokenizedPathOne[i] << endl;
        }
    }
    if (Indicator == 2)
    {
        cout << "Path 2" << endl;
        for (int i = 0; i < TokenizedPathTwo.size(); i++)
        {
            cout << TokenizedPathTwo[i] << endl;
        }
    }
}

void Affiche(int Indicator)
{
    if (Indicator == 1)
    {
        cout << "Dossier 1" << endl;
        for (int i = 0; i < TabFilesOne.size(); i++)
        {
            cout << TabFilesOne[i] << endl;
        }
    }
    if (Indicator == 2)
    {
        cout << "Dossier 2" << endl;
        for (int i = 0; i < TabFilesTwo.size(); i++)
        {
            cout << TabFilesTwo[i] << endl;
        }
    }
    if (Indicator == 11)
    {
        cout << "Dossier 1 Old Ver" << endl;
        for (int i = 0; i < TabFilesOneOld.size(); i++)
        {
            cout << TabFilesOneOld[i] << endl;
        }
    }
    if (Indicator == 22)
    {
        cout << "Dossier 2 Old Ver" << endl;
        for (int i = 0; i < TabFilesTwoOld.size(); i++)
        {
            cout << TabFilesTwoOld[i] << endl;
        }
    }
}