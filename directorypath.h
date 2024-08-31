#ifndef DIRECTORYPATH_H
#define DIRECTORYPATH_H

#include <QMutex>
#include <windows.h>

#include <QWidget>

class DirectoryPath
{
public:
    DirectoryPath();
    ~DirectoryPath();

public:
    inline wchar_t* getDirPath() { return m_dirPath; }
    QString getQStringDirPath();
    bool setDirPath(QString dirPath);
    // void setDirPath(wchar_t *dirPath);
    inline void setDirPathCorrect(bool flag) { m_dirPathCorrect = flag; }

    HANDLE getDHandler() { return m_hDir; }
    size_t getLenDirPath() const { return m_lenDirPath; }
    QMutex* getMutex() const { return m_mutex; }
    inline bool isDirPathCorrect() const {return m_dirPathCorrect;}

    bool saveDirPathToFile(QWidget *parent);
    bool loadDirPathFromFile();

private:
    QMutex *m_mutex;
    HANDLE m_hDir;
    size_t m_lenDirPath;
    Q_DISABLE_COPY(DirectoryPath);
    wchar_t *m_dirPath;

    const QString m_saveFilePath = "settings.txt";
    bool m_dirPathCorrect = false;
};

#endif // DIRECTORYPATH_H
