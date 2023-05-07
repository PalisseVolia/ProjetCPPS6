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
static vector<string> TabFilesOneOld;
static vector<string> TabFilesTwoOld;
static vector<string> TabFilesOne;
static vector<string> TabFilesTwo;

// Path des dossiers à synchroniser
static fs::path dirPathOne("C:/Users/vpali/Desktop/TEST");
static fs::path dirPathTwo("C:/Users/vpali/Desktop/DESTINATION");

// Fonctions
void init();
void Sync(int Indicator);
int CompareVersion();
string GetDirFilePaths(const fs::path &dirPath, vector<string> &TabFiles);

int main()
{
    // Initialisation
    init();

    // Boucle infinie
    while (true)
    {
        sleep_for(1s);
        Sync(CompareVersion());
    }

    return 0;
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

    // Dossier 1 à la priorité à l'initialisation
    Sync(1);
}

// Synchronisation en fonction du dossier modifié
void Sync(int Indicator)
{
    // suppression des fichiers du dossier plus ancien et copie du nouveau
    if (Indicator == 1)
    {
        fs::remove_all(dirPathTwo);
        fs::copy(dirPathOne, dirPathTwo, fs::copy_options::recursive);
    }
    if (Indicator == 2)
    {
        fs::remove_all(dirPathOne);
        fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);
    }
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