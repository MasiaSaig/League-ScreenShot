#ifndef FILEMONITORING_H
#define FILEMONITORING_H

#include <QString>
#include <windows.h>
#include <QThread>

class DirectoryMonitoringController : public QObject
{
    Q_OBJECT
public:
    DirectoryMonitoringController(const wchar_t* dirPath = nullptr);
    ~DirectoryMonitoringController();

public slots:
    void runDirectoryMonitoring();

    void changeDirectory(QString dirPath);
    void copyToClipboard(const wchar_t* dirPath);

    void quit();

signals:
    void finished();
    void error(QString err);

private:
    HANDLE m_hDir;
    wchar_t* m_dirPath;
    size_t m_lenDirPath;

private:
    static const int BUFFERSIZE = 1024;
    char m_buffer[BUFFERSIZE];
    DWORD m_bytesReturned;
    bool m_running = true;

private:
    ULONG_PTR m_gdiplusToken;
    // QThread* m_directoryMonitoringThread;
};

#endif // FILEMONITORING_H

