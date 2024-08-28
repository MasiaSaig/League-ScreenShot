#include "filemonitoring.h"

#include <QDebug>
#include <QString>
#include <QTime>
#include <QCoreApplication>

#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

// void DirectoryMonitoringController::printDir() const{
//     qDebug() << "Directory after replace and conversion QString: " << QString::fromWCharArray(m_dirPath);
//     qDebug() << "Directory after replace and conversion wstring: " << std::wstring(m_dirPath, m_lenDirPath);
//     qDebug() << "Directory after replace and conversion wchar_t*: " << m_dirPath;
// }

DirectoryMonitoringController::DirectoryMonitoringController(DirectoryPath *dirPath) :
    m_dirPath{dirPath}
{
    qDebug() << "Initializing DMC.";
}

DirectoryMonitoringController::~DirectoryMonitoringController()
{
    qDebug() << "Cleaning up, done.";
}


void DirectoryMonitoringController::run(){
    qDebug() << "Starting directory monitoring.";
    // main file monitoring loop
    while (m_running) {
        if (ReadDirectoryChangesW(
                m_dirPath->getDHandler(),      // Handle to directory
                m_buffer,                      // Buffer to receive changes
                BUFFERSIZE,                    // Buffer size
                FALSE,                         // Monitor subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor    //FILE_NOTIFY_CHANGE_FILE_NAME
                &m_bytesReturned,              // Number of bytes returned
                NULL,                          // Overlapped structure
                NULL                           // Completion routine
                )) {
            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
            if (fni->Action == FILE_ACTION_ADDED) {
                qDebug() << "On new file.";
                size_t lenFileName = fni->FileNameLength / sizeof(WCHAR);
                qDebug() << "New file created: " << std::wstring(fni->FileName, lenFileName);
                qDebug() << QString::fromWCharArray(m_dirPath->getDirPath());

                // concatenating folder path, with new file name
                size_t lenFullFilePath = m_dirPath->getLenDirPath() + lenFileName + 1;
                wchar_t* fullFilePath = new wchar_t[lenFullFilePath];
                if (fullFilePath == nullptr) {
                    qDebug() << "Memmory allocation failed!";
                    break;
                }
                wchar_t* fileName = fni->FileName;

                qDebug() << "Len: " << m_dirPath->getLenDirPath() << ". Dir path: " << QString::fromWCharArray(m_dirPath->getDirPath());
                wcscpy_s(fullFilePath, m_dirPath->getLenDirPath()+1, m_dirPath->getDirPath());
                qDebug() << "Full file Path: " << QString::fromWCharArray(fullFilePath);
                wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

                qDebug() << "Full new file path: "<< fullFilePath;
                qDebug() << "Full new file path(to QString): " << QString::fromWCharArray(fullFilePath);
                copyToClipboard(fullFilePath);

                delete [] fullFilePath;
            }
        }
        else {
            qDebug() << "Failed to read directory changes. Error: " << GetLastError();
            qDebug() << "Handler to directory: " << m_dirPath->getDHandler();
            qDebug() << "Directory path: " << QString::fromWCharArray(m_dirPath->getDirPath());
            break;
        }

    }
    qDebug() << "Ending directory monitoring.";
}

void DirectoryMonitoringController::copyToClipboard(const WCHAR* filePath)
{
    qDebug() << "Trying to copy image to clipboard.";
    qDebug() << QString::fromWCharArray(filePath);
    // wait for a file to finish being copied, for max time of maxWaitTime(2 seconds)
    QTime maxWaitTime = QTime::currentTime().addSecs(2);
    int bitmapStatus;
    Gdiplus::Bitmap* gdibmp = Gdiplus::Bitmap::FromFile(filePath);
    while((QTime::currentTime() < maxWaitTime) && (bitmapStatus != Gdiplus::Ok)){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        gdibmp = Gdiplus::Bitmap::FromFile(filePath);
        bitmapStatus = gdibmp->GetLastStatus();
    }
    if (bitmapStatus != Gdiplus::Ok){
        qDebug() << "Wrong gdibmp status." << bitmapStatus ;
        return;
    }
    HBITMAP hbitmap;
    auto status = gdibmp->GetHBITMAP(0, &hbitmap);
    if (status != Gdiplus::Ok) {
        qDebug() << "Wrong bitmap status. - " << status;
        return;
    }
    BITMAP bm;
    GetObject(hbitmap, sizeof bm, &bm);
    DIBSECTION ds;
    if (sizeof ds == GetObject(hbitmap, sizeof ds, &ds)){
        qDebug() << "Got object.";
        HDC hdc = GetDC(NULL);
        HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                                             ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdc);

        if (OpenClipboard(NULL)){
            qDebug() << "Copying image.";
            EmptyClipboard();
            //DWORD CF_PNG = RegisterClipboardFormat(L"PNG");
            SetClipboardData(CF_BITMAP, hbitmap_ddb);
            CloseClipboard();
        }
        else {
            qDebug() << "Couldn't open clipboard. - " << GetLastError();
        }
        DeleteObject(hbitmap_ddb);
    }
    DeleteObject(hbitmap);
    qDebug() << "Finished copying to clipboard.";
}

void DirectoryMonitoringController::quit(){
    m_running = false;
}




// void DirectoryMonitoringController::changeDirectory(QString dirPath)
// {
//     qDebug() << "Directory before replace: " << dirPath;
//     dirPath = dirPath.replace('/', "\\");
//     qDebug() << "Directory after replace: " << dirPath;
//     auto archiveWString = dirPath.toStdWString();
//     m_dirPath = const_cast<wchar_t *>(archiveWString.c_str());
//     m_lenDirPath = wcslen(m_dirPath);

//     m_hDir = CreateFile(
//         m_dirPath,    // Directory to monitor
//         FILE_LIST_DIRECTORY,                // Access type
//         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
//         NULL,                               // Security attributes
//         OPEN_EXISTING,                      // Creation disposition     //CREATE_NEW
//         FILE_FLAG_BACKUP_SEMANTICS,         // Flags and attributes, FILE_FLAG_BACKUP_SEMANTICS needed for directories
//         NULL                                // Template file
//         );

//     // if folder already exists, create handler to existing folder
//     if ((m_hDir == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_FILE_EXISTS)) {
//         m_hDir = CreateFile(
//             m_dirPath,
//             FILE_LIST_DIRECTORY,
//             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//             NULL,
//             OPEN_EXISTING,
//             FILE_FLAG_BACKUP_SEMANTICS,
//             NULL
//             );
//     }

//     if (m_hDir == INVALID_HANDLE_VALUE) {
//         qDebug() << "Failed to get directory handle. Error: " << GetLastError() << " Directory path:" << dirPath;
//     }
//     printDir();
// }

// void DirectoryMonitoringController::runDirectoryMonitoring()
// {
//     qDebug() << "Before directory monitoring.";
//     // main file monitoring loop
//     while (m_running) {
//         if (ReadDirectoryChangesW(
//                 m_dirPath->getDHandler(),                        // Handle to directory
//                 m_buffer,                      // Buffer to receive changes
//                 BUFFERSIZE,                    // Buffer size
//                 FALSE,                         // Monitor subdirectories
//                 FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor
//                 &m_bytesReturned,              // Number of bytes returned
//                 NULL,                          // Overlapped structure
//                 NULL                           // Completion routine
//                 )) {
//             FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
//             if (fni->Action == FILE_ACTION_ADDED) {
//                 qDebug() << "On new file.";
//                 size_t lenFileName = fni->FileNameLength / sizeof(WCHAR);
//                 qDebug() << "New file created: " << std::wstring(fni->FileName, lenFileName);
//                 qDebug() << std::wstring(m_dirPath->getDirPath(), m_dirPath->getLenDirPath());

//                 // concatenating folder path, with new file name
//                 size_t lenFullFilePath = m_dirPath->getLenDirPath() + lenFileName + 1;
//                 wchar_t* fullFilePath = new wchar_t[lenFullFilePath];
//                 if (fullFilePath == nullptr) {
//                     qDebug() << "Memmory allocation failed!";
//                     break;
//                 }
//                 wchar_t* fileName = fni->FileName;

//                 wcscpy_s(fullFilePath, m_dirPath->getLenDirPath()+1, m_dirPath->getDirPath());
//                 wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

//                 qDebug() << fullFilePath;
//                 qDebug() << std::wstring(fullFilePath, lenFullFilePath);
//                 copyToClipboard(fullFilePath);

//                 delete [] fullFilePath;
//             }
//         }
//         else {
//             qDebug() << "Failed to read directory changes. Error: " << GetLastError();
//             qDebug() << "Directory path: " << m_dirPath;
//             break;
//         }
//     }
// }
