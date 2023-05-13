#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <windows.h>
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
static vector<string> TabLastModifiedOneOld;
static vector<string> TabLastModifiedTwoOld;
static vector<string> TabLastModifiedOne;
static vector<string> TabLastModifiedTwo;

// Path des dossiers à synchroniser
static fs::path dirPathOne("C:/Users/vpali/Desktop/TEST");
static fs::path dirPathTwo("C:/Users/vpali/Desktop/DESTINATION");
static fs::path dirPathTEMP("..TEMP");

// Fonctions
void init();
int Sync(int Indicator);
int CompareVersion();
void UpdateDate(vector<string> &TabFiles, fs::path dirPath);
string GetDirFilePaths(const fs::path &dirPath, vector<string> &TabFiles, vector<string> &TabLastModified);

int main()
{
    // Initialisation
    init();

    // Boucle infinie
    while (true)
    {
        sleep_for(3s);
        Sync(CompareVersion());
        cout << "Synchronisation effectuee" << endl;
    }

    return 0;
}

// Initialisation
void init()
{
    fs::remove_all(dirPathTEMP);

    // Initialisation des tableaux de path fichiers
    TabFilesOneOld.clear();
    TabFilesOne.clear();
    TabLastModifiedOneOld.clear();
    TabLastModifiedOne.clear();
    GetDirFilePaths(dirPathOne, TabFilesOne, TabLastModifiedOne);

    TabFilesTwoOld.clear();
    TabFilesTwo.clear();
    TabLastModifiedTwoOld.clear();
    TabLastModifiedTwo.clear();
    GetDirFilePaths(dirPathTwo, TabFilesTwo, TabLastModifiedTwo);

    // Dossier 1 à la priorité à l'initialisation
    Sync(1);
}

// Synchronisation en fonction du dossier modifié
int Sync(int Indicator)
{
    // suppression des fichiers du dossier plus ancien et copie du nouveau
    if (Indicator == 1)
    {
        try
        {
            // test de copie à un dossier temporaire avant la copie définitive pour éviter les pertes de données en cas d'erreur
            fs::remove_all(dirPathTEMP);
            fs::copy(dirPathOne, dirPathTEMP, fs::copy_options::recursive);
            fs::remove_all(dirPathTEMP);

            // synchronisation
            fs::remove_all(dirPathTwo);
            fs::copy(dirPathOne, dirPathTwo, fs::copy_options::recursive);

            // mise à jour de la date de modification des fichiers
            UpdateDate(TabFilesTwo, dirPathTwo);
        }
        catch (const fs::filesystem_error &ex)
        {
            cout << "ERREUR : Problème pendant la copie du fichier" << endl;
        }
    }
    if (Indicator == 2)
    {
        try
        {
            // test de copie à un dossier temporaire avant la copie définitive pour éviter les pertes de données en cas d'erreur
            fs::remove_all(dirPathTEMP);
            fs::copy(dirPathTwo, dirPathTEMP, fs::copy_options::recursive);
            fs::remove_all(dirPathTEMP);

            // synchronisation
            fs::remove_all(dirPathOne);
            fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);

            // mise à jour de la date de modification des fichiers
            UpdateDate(TabFilesOne, dirPathOne);
        }
        catch (const fs::filesystem_error &ex)
        {
            cout << "Erreur lors de la copie" << endl;
        }
    }
    return 0;
}

// change la date des fichiers du dossier pour éviter une boucle de synchronisation
// (sans cela le fichier copié aurait une date de modification plus récente que le fichier source)
void UpdateDate(vector<string> &TabFiles, fs::path dirPath)
{
    // pour tous les fichiers du dossier
    for (int i = 0; i < TabFiles.size(); i++)
    {
        // Date de dernière modification des fichiers
        string filePath = dirPath.string() + "/" + TabFiles[i];

        // conversion en wide string (UTF-16)
        wstring wideFilePath(filePath.begin(), filePath.end());
        HANDLE fileHandle = CreateFileW(wideFilePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            FILETIME newLastWriteTime;
            SYSTEMTIME newSystemTime;

            // nouvelle date de modification
            newSystemTime.wYear = 2000;
            newSystemTime.wMonth = 1;
            newSystemTime.wDay = 1;
            newSystemTime.wHour = 0;
            newSystemTime.wMinute = 0;
            newSystemTime.wSecond = 0;
            newSystemTime.wMilliseconds = 0;

            if (SystemTimeToFileTime(&newSystemTime, &newLastWriteTime))
            {
                if (SetFileTime(fileHandle, NULL, NULL, &newLastWriteTime))
                {
                    cout << "SUCCES : date de modification modifiee" << endl;
                }
                else
                {
                    cout << "ERROR : echec de la modification de la date de derniere modification" << endl;
                }
            }
            else
            {
                cout << "ERREUR : echec de la conversion de la date de derniere modification" << endl;
            }
            CloseHandle(fileHandle);
        }
        else
        {
            cout << "ERREUR : echec de l'ouverture du fichier" << endl;
            // TODO: Demander à l'utilisateur si il a utilisé des caractères spéciaux
        }
    }
}

// Recherche de modifications dans les dossiers
int CompareVersion()
{
    // mise à jour des tableaux de fichiers
    TabFilesOneOld.clear();
    TabFilesOneOld = TabFilesOne;
    TabFilesOne.clear();

    TabLastModifiedOneOld.clear();
    TabLastModifiedOneOld = TabLastModifiedOne;
    TabLastModifiedOne.clear();

    GetDirFilePaths(dirPathOne, TabFilesOne, TabLastModifiedOne);

    TabFilesTwoOld.clear();
    TabFilesTwoOld = TabFilesTwo;
    TabFilesTwo.clear();

    TabLastModifiedTwoOld.clear();
    TabLastModifiedTwoOld = TabLastModifiedTwo;
    TabLastModifiedTwo.clear();

    GetDirFilePaths(dirPathTwo, TabFilesTwo, TabLastModifiedTwo);

    // détermination de la source des modifications si il y en a
    if (TabFilesOne.size() != TabFilesOneOld.size())
    {
        cout << "size 1" << endl;
        return 1;
    }
    if (TabFilesTwo.size() != TabFilesTwoOld.size())
    {
        cout << "size 2" << endl;
        return 2;
    }
    for (int i = 0; i < TabFilesOne.size(); i++)
    {
        if (TabFilesOne[i] != TabFilesOneOld[i])
        {
            cout << "path 1" << endl;
            return 1;
        }
        if (TabLastModifiedOne[i] != TabLastModifiedOneOld[i])
        {
            cout << "date 1" << endl;
            return 1;
        }
    }
    for (int i = 0; i < TabFilesTwo.size(); i++)
    {
        if (TabFilesTwo[i] != TabFilesTwoOld[i])
        {
            cout << "path 2" << endl;
            return 2;
        }
        if (TabLastModifiedTwo[i] != TabLastModifiedTwoOld[i])
        {
            cout << "date 2" << endl;
            return 2;
        }
    }

    // si aucune modification n'a été détectée
    return 0;
}

// Récupération des path des fichiers d'un dossier et de leur date de mdoification
string GetDirFilePaths(const fs::path &dirPath, vector<string> &TabFiles, vector<string> &TabLastModified)
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
            // Path des fichiers
            string pathtemp = entry.path().string().substr(entry.path().string().find(dirPath.string()) + dirPath.string().size() + 1);
            TabFiles.push_back(pathtemp);

            // Date de dernière modification des fichiers
            const std::wstring filePathWide = std::wstring(entry.path().wstring());
            HANDLE fileHandle = CreateFileW(filePathWide.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (fileHandle != INVALID_HANDLE_VALUE)
            {
                FILETIME lastWriteTime;
                if (GetFileTime(fileHandle, NULL, NULL, &lastWriteTime))
                {
                    SYSTEMTIME systemTime;
                    if (FileTimeToSystemTime(&lastWriteTime, &systemTime))
                    {
                        TabLastModified.push_back(to_string(systemTime.wYear) + "-" + to_string(systemTime.wMonth) + "-" + to_string(systemTime.wDay) + " " + to_string(systemTime.wHour) + ":" + to_string(systemTime.wMinute) + ":" + to_string(systemTime.wSecond));
                    }
                }
                else
                {
                    return ("ERREUR : Impossible de récupérer la date de dernière modification du fichier : " + entry.path().string());
                }

                CloseHandle(fileHandle);
            }
            else
            {
                return ("ERREUR : Impossible d'ouvrir le fichier : " + entry.path().string());
            }
        }
    }

    return ("OK");
}