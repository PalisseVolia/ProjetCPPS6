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

// TOOD: temp path pc portable
// static fs::path dirPathOne("C:/Users/Palisse Volia/Desktop/TEST");
// static fs::path dirPathTwo("C:/Users/Palisse Volia/Desktop/DESTINATION");

// Fonctions
void init();
int Sync(int Indicator);
int CompareVersion();
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
    }

    // CompareVersion();

    // cout << "TabLastModifiedOne" << endl;
    // for (int i = 0; i < TabLastModifiedOne.size(); i++)
    // {
    //     cout << TabLastModifiedOne[i] << endl;
    // }
    // cout << "TabLastModifiedTwo" << endl;
    // for (int i = 0; i < TabLastModifiedTwo.size(); i++)
    // {
    //     cout << TabLastModifiedTwo[i] << endl;
    // }

    // string temp;
    // cin >> temp;

    // CompareVersion();

    // cout << "TabLastModifiedOne" << endl;
    // for (int i = 0; i < TabLastModifiedOne.size(); i++)
    // {
    //     cout << TabLastModifiedOne[i] << endl;
    // }
    // cout << "TabLastModifiedTwo" << endl;
    // for (int i = 0; i < TabLastModifiedTwo.size(); i++)
    // {
    //     cout << TabLastModifiedTwo[i] << endl;
    // }

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
            // TODO: clean & en faire une méthode
            fs::remove_all(dirPathTEMP);
            fs::copy(dirPathOne, dirPathTEMP, fs::copy_options::recursive);
            fs::remove_all(dirPathTEMP);
            fs::remove_all(dirPathTwo);
            fs::copy(dirPathOne, dirPathTwo, fs::copy_options::recursive);

            // TODO: clean & en faire une méthode
            for (int i = 0; i < TabFilesTwo.size(); i++)
            {
                // Date de dernière modification des fichiers
                string filePath = dirPathTwo.string() + "/" + TabFilesTwo[i];

                // Convert file path to wide character string
                wstring wideFilePath(filePath.begin(), filePath.end());
                HANDLE fileHandle = CreateFileW(wideFilePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                if (fileHandle != INVALID_HANDLE_VALUE)
                {
                    FILETIME newLastWriteTime;
                    SYSTEMTIME newSystemTime;

                    // Set the new last modified date
                    newSystemTime.wYear = 2000; // Modify the desired year
                    newSystemTime.wMonth = 1;   // Modify the desired month
                    newSystemTime.wDay = 1;     // Modify the desired day
                    newSystemTime.wHour = 0;    // Modify the desired hour
                    newSystemTime.wMinute = 0;  // Modify the desired minute
                    newSystemTime.wSecond = 0;  // Modify the desired second
                    newSystemTime.wMilliseconds = 0;

                    if (SystemTimeToFileTime(&newSystemTime, &newLastWriteTime))
                    {
                        if (SetFileTime(fileHandle, NULL, NULL, &newLastWriteTime))
                        {
                            std::wcout << L"Last modified date modified successfully." << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to set the new last modified date." << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "Failed to convert the system time to file time." << std::endl;
                    }

                    CloseHandle(fileHandle);
                }
                else
                {
                    std::cerr << "Failed to open the file: " << filePath << std::endl;
                }
            }
        }
        catch (const fs::filesystem_error &ex)
        {
            cout << "Erreur lors de la copie" << endl;
        }
    }
    if (Indicator == 2)
    {
        try
        {
            fs::remove_all(dirPathTEMP);
            fs::copy(dirPathTwo, dirPathTEMP, fs::copy_options::recursive);
            fs::remove_all(dirPathTEMP);
            fs::remove_all(dirPathOne);
            fs::copy(dirPathTwo, dirPathOne, fs::copy_options::recursive);

            for (int i = 0; i < TabFilesOne.size(); i++)
            {
                // Date de dernière modification des fichiers
                std::string filePath = dirPathOne.string() + "/" + TabFilesOne[i];

                // Convert file path to wide character string
                std::wstring wideFilePath(filePath.begin(), filePath.end());
                HANDLE fileHandle = CreateFileW(wideFilePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                if (fileHandle != INVALID_HANDLE_VALUE)
                {
                    FILETIME newLastWriteTime;
                    SYSTEMTIME newSystemTime;

                    // Set the new last modified date
                    newSystemTime.wYear = 2000; // Modify the desired year
                    newSystemTime.wMonth = 1;   // Modify the desired month
                    newSystemTime.wDay = 1;     // Modify the desired day
                    newSystemTime.wHour = 0;    // Modify the desired hour
                    newSystemTime.wMinute = 0;  // Modify the desired minute
                    newSystemTime.wSecond = 0;  // Modify the desired second
                    newSystemTime.wMilliseconds = 0;

                    if (SystemTimeToFileTime(&newSystemTime, &newLastWriteTime))
                    {
                        if (SetFileTime(fileHandle, NULL, NULL, &newLastWriteTime))
                        {
                            std::wcout << "Last modified date modified successfully." << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to set the new last modified date." << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "Failed to convert the system time to file time." << std::endl;
                    }

                    CloseHandle(fileHandle);
                }
                else
                {
                    std::cerr << "Failed to open the file: " << filePath << std::endl;
                }
            }
        }
        catch (const fs::filesystem_error &ex)
        {
            cout << "Erreur lors de la copie" << endl;
        }
    }
    return 0;
}

// Recherche de modifications dans les dossiers
int CompareVersion()
{
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