#ifndef DIRECTORYPATH_H
#define DIRECTORYPATH_H

#include <QMutex>
#include <QDebug>
#include <windows.h>

class DirectoryPath
{
public:
    DirectoryPath(QString dirPath);
    ~DirectoryPath();

    wchar_t* getDirPath() { return m_dirPath; }
    void setDirPath(QString dirPath);

    HANDLE getDHandler() { return m_hDir; }
    size_t getLenDirPath() const { return m_lenDirPath; }
    QMutex* getMutex() const { return m_mutex; }
private:
    QMutex *m_mutex;
    HANDLE m_hDir;
    size_t m_lenDirPath;
    Q_DISABLE_COPY(DirectoryPath);
    wchar_t *m_dirPath;
};

#endif // DIRECTORYPATH_H
