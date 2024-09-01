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
    LogFile::instance() << "Cleaning up Directory Path Controller.\n";
    CloseHandle(m_hDir);
    delete m_mutex;
}


QString DirectoryPath::getQStringDirPath(){
    return QString::fromWCharArray(m_dirPath, m_lenDirPath);
}

bool DirectoryPath::setDirPath(QString dirPath){
    QMutexLocker ml(m_mutex);

    LogFile::instance() << "Changing directory to: " << dirPath << '\n';
    dirPath = dirPath.replace('/', "\\");
    // LogFile::instance() << "Directory after replace: " << dirPath << '\n';

    if(m_dirPath != nullptr){
        delete [] m_dirPath;
    }
    m_dirPath = new wchar_t[dirPath.length()+1];
    if(!m_dirPath){
        return false;
    }

    m_lenDirPath = dirPath.toWCharArray(m_dirPath);
    m_dirPath[dirPath.length()] = 0;

    // LogFile::instance() << "Directory after replace and conversion: " << QString::fromWCharArray(m_dirPath) << '\n';

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

bool DirectoryPath::saveDirPathToFile(QWidget *parent){
    QFile file(m_saveFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QMessageBox::information(parent, "Unable to open a file.", file.errorString());
        return false;
    }
    QTextStream out(&file);
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

    file.close();

    return setDirPath(retrivedPath);
}
