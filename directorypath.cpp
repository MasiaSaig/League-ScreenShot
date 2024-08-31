#include "directorypath.h"
#include "logfile.h"

#include <QMessageBox>


DirectoryPath::DirectoryPath() :
    m_mutex(new QMutex), m_dirPath{nullptr}, m_lenDirPath{0}
{
    LogFile::instance() << "Initializing Directory Path Controller.\n";
}


DirectoryPath::~DirectoryPath()
{
    CloseHandle(m_hDir);
    delete m_mutex;
}


QString DirectoryPath::getQStringDirPath(){
    return QString::fromWCharArray(m_dirPath, m_lenDirPath);
}

bool DirectoryPath::setDirPath(QString dirPath){
    QMutexLocker ml(m_mutex);

    LogFile::instance() << "Changing directory: " << dirPath << '\n';
    dirPath = dirPath.replace('/', "\\");
    LogFile::instance() << "Directory after replace: " << dirPath << '\n';

    // converting to utf16
    // QByteArray stringByteArray;
    // dirPath.fromUtf8(stringByteArray);
    // auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
    // dirPath = toUtf16(stringByteArray);

    // auto dirPathWString = dirPath.toStdWString();
    // wchar_t *dirPathWChar = const_cast<wchar_t *>(dirPathWString.c_str());
    if(m_dirPath != nullptr){
        delete [] m_dirPath;
    }
    m_dirPath = new wchar_t[dirPath.length()+1];
    if(!m_dirPath){
        return false;
    }

    m_lenDirPath = dirPath.toWCharArray(m_dirPath);
    m_dirPath[dirPath.length()] = 0;
    // size_t lenWChardirPath = wcslen(dirPathWChar);

    // wcscpy_s(m_dirPath, lenWChardirPath+1, dirPathWChar);
    // m_lenDirPath = wcslen(m_dirPath);

    LogFile::instance() << "Directory after replace and conversion: " << QString::fromWCharArray(m_dirPath) << '\n';

    HANDLE previous_hdir = m_hDir;
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
        setDirPathCorrect(false);
        LogFile::instance() << "ERROR! Failed to get directory handle. Error code: " << GetLastError() << " Directory path: " << dirPath << '\n';
        return false;
    }else{
        setDirPathCorrect(true);
        // needed inorder to stop ReadDirectoryChangesW() function from waiting for a file change, and get new directory handler
        CancelIoEx(previous_hdir, NULL);
    }
    return true;
}

// void DirectoryPath::setDirPath(wchar_t *dirPath){
//     QMutexLocker ml(m_mutex);

//     LogFile::instance() << "Changing directory: " << QString::fromWCharArray(dirPath) << '\n';

//     if(m_dirPath != nullptr){
//         delete [] m_dirPath;
//     }
//     m_lenDirPath = wcslen(dirPath)+1;
//     m_dirPath = new wchar_t[m_lenDirPath];

//     wcscpy_s(m_dirPath, m_lenDirPath, dirPath);

//     LogFile::instance() << "Directory after being set: " << m_dirPath;

//     HANDLE previous_hdir = m_hDir;
//     m_hDir = CreateFile(
//         m_dirPath,
//         FILE_LIST_DIRECTORY,
//         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//         NULL,
//         OPEN_EXISTING,
//         FILE_FLAG_BACKUP_SEMANTICS,
//         NULL
//         );

//     if (m_hDir == INVALID_HANDLE_VALUE) {
//         LogFile::instance() << "ERROR! Failed to get directory handle. Error: " << GetLastError() << " Directory path:" << dirPath << '\n';
//     }
//     // needed inorder to stop ReadDirectoryChangesW() function from waiting for a file change, and get new directory handler
//     CancelIoEx(previous_hdir, NULL);
// }

bool DirectoryPath::saveDirPathToFile(QWidget *parent){
    QFile file(m_saveFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QMessageBox::information(parent, "Unable to open a file.", file.errorString());
        return false;
    }
    QTextStream out(&file);
    // out << m_lenDirPath << ' ';
    out << getQStringDirPath();

    LogFile::instance() << "Saved directory path to file.\n";
    file.close();

    return true;
}

bool DirectoryPath::loadDirPathFromFile(){
    LogFile::instance() << "Loading directory path from file.\n";

    QFile file(m_saveFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }

    QTextStream in(&file);
    QString retrivedPath = in.readLine();
    // QString fragment;
    // in >> retrivedPath;
    // while(!in.atEnd()){
        // in >> fragment;
        // retrivedPath += ' ' + fragment;
    // }

    file.close();

    return setDirPath(retrivedPath);
}
