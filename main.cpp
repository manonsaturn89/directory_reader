#include <condition_variable>
#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include "File.h"
#include "ThreadPool.h"
#include "DirectoryTree.h"
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

int main()
{
    cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << endl;

    std::string path;
    cout << "Insert the path of the folder to be read: ";

    cin >> path;
    if (!fs::directory_entry(path).is_directory()) {
        cout << "\"" << path << "\" is not a valid directory!" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Reading files from folder and subfolders of:\n\t" << path << endl;

    auto start = std::chrono::high_resolution_clock::now();
    int count_file = 0;
    int count_dir = 0;
    std::list<File *> fileList;
    {
        ThreadPool pool(std::thread::hardware_concurrency()-1);
        for (const auto & entry : fs::recursive_directory_iterator(path)){
            if (entry.is_regular_file()) {
                count_file++;

                File *f = new File(entry.path(), false);
                fileList.push_back(f);
                pool.enqueue( [=]{
                    f->startReading();
                });
            }
            else if (entry.is_directory()) {
                count_dir++;
            }
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto durationTest = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    sync_cout()<< "Duration in ms:      \t" << durationTest.count() << endl;
    sync_cout()<< "Number of files:     \t " << fileList.size() << endl;
    sync_cout()<< "Number of subfolders:\t" << count_dir << endl;


    sync_cout()<< "\n\n" << endl;

    start = std::chrono::high_resolution_clock::now();
    DirectoryTree tree(path);
    stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);

    sync_cout()<< "Duration in ms:      \t" << duration.count() << endl;
    sync_cout()<< "Number of files:     \t " << tree.files() << endl;
    sync_cout()<< "Number of subfolders:\t" << tree.folders() -1<< endl;

    cout << "================" << endl;
    tree.printInfo();


    sync_cout()<< "Duration in ms test multi thread:    \t" << durationTest.count() << endl;
    sync_cout()<< "Duration in ms:                      \t" << duration.count() << endl;

    cout << "Print every file info? (Y/n)";
    char a;
    cin >> a;
    if (a!='y' && a!='Y')
        exit(EXIT_SUCCESS);

    list<File> listFiles = tree.getAllFiles();
    if (listFiles.size()==0) {
        cout << "No files in the folder" << endl;
        exit(EXIT_SUCCESS);
    }
    cout << "=============================================================" << endl;
    cout << "PATH | LINES | LINES_VOID | LINES_NON_VOID | WORDS | CHARACTERS" << endl;
    cout << "=============================================================" << endl;
    for (File f: listFiles)
        f.print(path);

    cout << "=============================================================" << endl;
    cout << "PATH | LINES | LINES_VOID | LINES_NON_VOID | WORDS | CHARACTERS" << endl;
    cout << "=============================================================" << endl;

    return 0;
}
