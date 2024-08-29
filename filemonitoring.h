#ifndef FILEMONITORING_H
#define FILEMONITORING_H

#include "directorypath.h"
#include <QString>
#include <windows.h>
#include <QThread>

class DirectoryMonitoringController : public QThread
{
    Q_OBJECT
public:
    DirectoryMonitoringController(DirectoryPath *dirPath);
    // ~DirectoryMonitoringController();

    void quit();

private:
    void copyToClipboard(const wchar_t* dirPath);

private:
    DirectoryPath *m_dirPath;

    static const int BUFFERSIZE = 1024;
    char m_buffer[BUFFERSIZE];
    DWORD m_bytesReturned;
    // OVERLAPPED ovl;

    bool m_running = true;

protected:
    void run();

};

#endif // FILEMONITORING_H

