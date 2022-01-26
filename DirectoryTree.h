#ifndef DIRECTORYTREE_H
#define DIRECTORYTREE_H

#include "File.h"
#include <list>

using namespace std;
namespace fs = std::filesystem;

class DirectoryTree {
public:
    explicit DirectoryTree (string path) {
        mPath = fs::path(path);
        readDirectory(path);
    }
    explicit DirectoryTree (string path, string path0) {
        mPath = fs::path(path);
        readDirectory(path0);
    }
    ~DirectoryTree() {};

    fs::path getPath() { return mPath; }

    unsigned long int files() {
        return mTotFiles;
    }

    unsigned long int lines() {
        return mTotLines;
    }

    unsigned long int linesVoid() {
        return mTotLinesVoid;
    }

    unsigned long int linesNonVoid() {
        return mTotLinesNonVoid;
    }

    unsigned long int characters() {
        return mTotCharacters;
    }

    unsigned long int words() {
        return mTotWords;
    }

    int folders() {
        int sub = 1;
        for (auto f: mSubdir)
            sub += f.folders();
        return sub;
    }

    void printInfo() {
        for (auto sd: mSubdir){
            sd.printInfo();
        }
        std::cout << "Subfolder:            \t" << mPath.string() << std::endl;
        std::cout << "Number of subfolders: \t" << folders() -1 << std::endl;
        std::cout << "Number of files:      \t" << mTotFiles << std::endl;
        std::cout << "Number of lines:              \t" << mTotLines << std::endl;
        std::cout << "Number of void lines:         \t" << mTotLinesVoid<< std::endl;
        std::cout << "Number of non void lines:     \t" << mTotLinesNonVoid << std::endl;
        std::cout << "Number of words:              \t" << mTotWords << std::endl;
        std::cout << "Number of characters:         \t" << mTotCharacters << std::endl;
        std::cout << "==============================" << std::endl;
    }

    list<File> getAllFiles() {
        list<File> l = mFiles;
        for (auto s : mSubdir) {
            list<File> ls = s.getAllFiles();
            for (File f: ls)
                l.push_back(f);
        }
        return l;
    }

private:
    fs::path mPath;
    list<DirectoryTree> mSubdir;
    list<File> mFiles;
    unsigned long int mTotFiles = 0;
    unsigned long int mTotLines = 0;
    unsigned long int mTotLinesVoid = 0;
    unsigned long int mTotLinesNonVoid = 0;
    unsigned long int mTotCharacters = 0;
    unsigned long int mTotWords = 0;
    bool mIsRead = false;

    void readDirectory(string path0) {
        mFiles.clear();
        mSubdir.clear();
        fs::path basePath(path0);

        for (const auto & entry : fs::directory_iterator(mPath)){
            if (entry.is_regular_file()) {
                File f(entry.path());
                mFiles.push_back(f);
                mTotLines += f.getLines();
                mTotLinesVoid += f.getLinesVoid();
                mTotLinesNonVoid += f.getLinesNonVoid();
                mTotCharacters += f.getCharacters();
                mTotFiles ++;
                mTotWords += f.getWords();

//                sync_cout()<<  f.getRelativeDir(basePath).string()<< '/' << f.getFileName() << "    " << f.getLines() << "  "
//                     << f.getLinesVoid() << "/" << f.getLinesNonVoid() << "    "<< f.getCharacters() << "   " << f.getWords() << std::endl;
            }
            else if (entry.is_directory()){
                DirectoryTree sub(entry.path(),basePath);
                mTotLines += sub.lines();
                mTotLinesVoid += sub.linesVoid();
                mTotLinesNonVoid += sub.linesNonVoid();
                mTotCharacters += sub.characters();
                mTotFiles += sub.files();
                mSubdir.push_back(sub);
                mTotWords += sub.words();
            }
        }
        mIsRead = true;
    }
};


#endif // DIRECTORYTREE_H
