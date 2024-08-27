#include "directorypath.h"

DirectoryPath::DirectoryPath(QString dirPath) :
     m_mutex(new QMutex)
{
    m_dirPath = nullptr;
    setDirPath(dirPath);
}

DirectoryPath::~DirectoryPath()
{
    CloseHandle(m_hDir);
    delete m_mutex;
}


void DirectoryPath::setDirPath(QString dirPath){
    QMutexLocker ml(m_mutex);

    qDebug() << "Directory before replace: " << dirPath;
    dirPath = dirPath.replace('/', "\\");
    auto dirPathWString = dirPath.toStdWString();
    wchar_t *dirPathWChar = const_cast<wchar_t *>(dirPathWString.c_str());
    if(m_dirPath != nullptr){
        delete [] m_dirPath;
    }
    m_dirPath = new wchar_t[dirPathWString.length()+1];

    qDebug() << "temp dirpath: " << QString::fromWCharArray(dirPathWChar);
    size_t lenWChardirPath = wcslen(dirPathWChar);

    wcscpy_s(m_dirPath, lenWChardirPath+1, dirPathWChar);
    m_lenDirPath = wcslen(m_dirPath);

    qDebug() << "Directory after replace and conversion: " << QString::fromWCharArray(m_dirPath);

    m_hDir = CreateFile(
        m_dirPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
        );

    // m_hDir = CreateFile(
    //     m_dirPath,    // Directory to monitor
    //     FILE_LIST_DIRECTORY,                // Access type
    //     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
    //     NULL,                               // Security attributes
    //     OPEN_EXISTING,                      // Creation disposition     //CREATE_NEW
    //     FILE_FLAG_BACKUP_SEMANTICS,         // Flags and attributes, FILE_FLAG_BACKUP_SEMANTICS needed for directories
    //     NULL                                // Template file
    //     );

    // // if folder already exists, create handler to existing folder
    // if ((m_hDir == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_FILE_EXISTS)) {
    //     m_hDir = CreateFile(
    //         m_dirPath,
    //         FILE_LIST_DIRECTORY,
    //         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    //         NULL,
    //         OPEN_EXISTING,
    //         FILE_FLAG_BACKUP_SEMANTICS,
    //         NULL
    //         );
    // }

    if (m_hDir == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to get directory handle. Error: " << GetLastError() << " Directory path:" << dirPath;
    }
}
