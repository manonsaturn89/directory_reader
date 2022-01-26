#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "sync_cout.h"

unsigned long int count_words (const std::string s) {
    if (s.size()==0)
        return 0;

    unsigned long int wordCount = 0;
    for (unsigned long int i=1; i<s.size(); i++) {
//        std::cout << s.at(i-1) << s.at(i) << std::endl;
        if ((s.at(i)==' ' && s.at(i-1) != ' ' ) || (s.at(i)=='\t' && s.at(i-1) != '\t'))
            wordCount++;
    }
    if (s.at(s.size()-1)!= ' ')
        wordCount++;
    return wordCount;
}


class File {
public:
    explicit File(std::filesystem::path path, bool autoread = true) {
        if (std::filesystem::is_directory(path)) {
            sync_cout()<< "File class trying to read directory!" << std::endl;
            exit(EXIT_FAILURE);
        }
        mPath = path;
        mFileName = path.filename();
        mDir = path.parent_path();
        if (autoread)
            readLines(path);
    }
    ~File() {    }
    void startReading() {
        if (std::filesystem::directory_entry(mPath).is_regular_file())
            readLines(mPath);
        else{
            sync_cout() << "The file is not a regular file" << std::endl;
            exit(EXIT_FAILURE);
        }
    }


    void print(std::filesystem::path basedir) {
        sync_cout() << getRelativeDir(basedir).string()
                    << '/' << mFileName
                    << '\t' << mLines
                    << '\t' << mLinesVoid
                    << '\t' << mLinesNonVoid
                    << '\t' << mWords
                    << '\t' << mCharacters
                    << std::endl;
    }

    unsigned long int getLines() {return mLines;}
    unsigned long int getLinesVoid() {return mLinesVoid;}
    unsigned long int getLinesNonVoid() {return mLinesNonVoid;}
    unsigned long int getCharacters() {return mCharacters;}
    unsigned long int getWords() {return mWords;}
    std::string getFileName() { return mFileName; }
    bool isRead() {return mIsRead;}
    std::filesystem::path getDir() {return mDir;}
    std::filesystem::path getRelativeDir(std::filesystem::path baseDir) {
        if (!std::filesystem::is_directory(baseDir)) {
            sync_cout()<< "File::gerRelativeDir: baseDir is not a directory!" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (baseDir.string().at(baseDir.string().size()-1)=='/')
            baseDir = std::filesystem::path(baseDir.string().substr(0,baseDir.string().size()-1));

        if (baseDir==mDir)
            return std::filesystem::path("");
        std::string relDirString ;
        relDirString = mDir.string().erase(mDir.u8string().find(baseDir.u8string()),baseDir.u8string().length());
        if (relDirString.at(0)=='/') relDirString.erase(0,1);
        std::filesystem::path relDir(relDirString);
        return relDir;
    }
    std::filesystem::path getFilePath() {return mPath;}


private:
    std::filesystem::path mPath;
    std::filesystem::path mDir;
    std::string mFileName;

    unsigned long int mLines = 0;
    unsigned long int mLinesVoid = 0;
    unsigned long int mLinesNonVoid = 0;
    unsigned long int mWords = 0;
    unsigned long int mCharacters = 0;
    bool mIsRead = false;

    void readLines(std::filesystem::path path) {
        std::ifstream inFile(path);

        mLines = 0;
        mLinesNonVoid = 0;
        mLinesVoid = 0;

        if (inFile.is_open()){
            std::string lineText;
            while (getline (inFile, lineText)) {
                if (lineText=="")
                    mLinesVoid++;
                else
                    mLinesNonVoid++;
                mLines++;
                mCharacters += lineText.size() + 1;
                mWords += count_words(lineText);
            }
            inFile.close();
            if (mLines!=mLinesNonVoid+mLinesVoid) {
                sync_cout()<< "Error with number of lines" << std::endl;
                exit(EXIT_FAILURE);
            }
            mIsRead = true;
        }
        else{
            sync_cout()<< "Could not open file " << path.string() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
};

#endif // FILE_H
