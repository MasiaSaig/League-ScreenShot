#include "filemonitoring.h"
#include "logfile.h"

#include <QString>
#include <QTime>
#include <QCoreApplication>
// #include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

DirectoryMonitoringController::DirectoryMonitoringController(DirectoryPath *dirPath) :
    m_dirPath{dirPath}//, ovl{0}
{
    LogFile::instance() << "Initializing Directory Monitoring Controller.\n";
}

void DirectoryMonitoringController::run(){
    LogFile::instance() << "Starting directory monitoring.\n";
    // main file monitoring loop

    size_t lenFileName;
    wchar_t* fileName;
    size_t lenFullFilePath;
    wchar_t* fullFilePath;

    while (m_running) {
        m_dirPath->setDirPathCorrect(true);
        if (ReadDirectoryChangesW(
                m_dirPath->getDHandler(),      // Handle to directory
                m_buffer,                      // Buffer to receive changes
                BUFFERSIZE,                    // Buffer size
                FALSE,                         // Monitor subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor            //FILE_NOTIFY_CHANGE_FILE_NAME | FILE_FLAG_OVERLAPPED
                &m_bytesReturned,              // Number of bytes returned
                NULL,                          // Overlapped structure          //  //&ovl
                NULL                           // Completion routine
                )) {

            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
            if (fni->Action == FILE_ACTION_ADDED) {
                lenFileName = fni->FileNameLength / sizeof(WCHAR);
                fileName = fni->FileName;
                // LogFile::instance() << "New file created: " << std::wstring(fni->FileName, lenFileName) << '\n';

                // concatenating folder path, with new file name
                lenFullFilePath = m_dirPath->getLenDirPath() + lenFileName + 1;
                fullFilePath = new wchar_t[lenFullFilePath];
                if (fullFilePath == nullptr) {
                    LogFile::instance() << "ERROR! Memmory allocation for path of new file, failed!\n";
                    break;
                }

                // LogFile::instance() << "Len: " << m_dirPath->getLenDirPath() << ". Dir path: " << QString::fromWCharArray(m_dirPath->getDirPath());
                wcscpy_s(fullFilePath, m_dirPath->getLenDirPath()+1, m_dirPath->getDirPath());
                // qDebug() << "Full file Path: " << QString::fromWCharArray(fullFilePath);
                wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

                LogFile::instance() << "New file created: "<< QString::fromWCharArray(fullFilePath) << '\n';
                copyToClipboard(fullFilePath);

                delete [] fullFilePath;
            }
        }
        else {
            int error_status = GetLastError();
            if(error_status == ERROR_SUCCESS){
                // LogFile::instance() << "Closing loop. Running loop again.\n";
            }else if(error_status == ERROR_OPERATION_ABORTED){
                // LogFile::instance() << "Directory changed. Running loop again.\n";
            }else if(error_status == ERROR_INVALID_HANDLE){
                // waiting for correct folder handler
                m_dirPath->setDirPathCorrect(false);
                QThread::sleep(1);
            }else{
                m_dirPath->setDirPathCorrect(false);
                LogFile::instance() << "\\/-------------------------------------------------\\/\n";
                LogFile::instance() << "ERROR! Failed to read directory changes. Error: " << error_status << '\n';
                LogFile::instance() << "ERROR! Handler to directory: " << m_dirPath->getDHandler() << '\n';
                LogFile::instance() << "ERROR! Directory path: " << QString::fromWCharArray(m_dirPath->getDirPath()) << '\n';
                LogFile::instance() << "/\\-------------------------------------------------/\\\n";
                quit();
            }
        }

        // LogFile::instance() << "New change in directory. " << QTime::currentTime().toString() << '\n';
        // QThread::sleep(1);
    }
    LogFile::instance() << "Ending directory monitoring.\n";
}

void DirectoryMonitoringController::copyToClipboard(const WCHAR* filePath)
{
    LogFile::instance() << "Trying to copy image to clipboard: " << QString::fromWCharArray(filePath) << '\n';
    // wait for a file to finish being copied, for max time of maxWaitTime(2 seconds)
    QTime maxWaitTime = QTime::currentTime().addSecs(2);
    int bitmapStatus;
    // TODO: this seems to not close files, so.. fix it mabe
    Gdiplus::Bitmap* gdibmp = Gdiplus::Bitmap::FromFile(filePath);
    while((QTime::currentTime() < maxWaitTime) && (bitmapStatus != Gdiplus::Ok)){
        QThread::sleep(static_cast<std::chrono::nanoseconds>(10000000));    // 100ms
        gdibmp = Gdiplus::Bitmap::FromFile(filePath);
        bitmapStatus = gdibmp->GetLastStatus();
    }

    if (bitmapStatus != Gdiplus::Ok){
        LogFile::instance() << "ERROR! Wrong gdibmp status." << bitmapStatus << '\n';
        return;
    }
    // setting up bitmap
    HBITMAP hbitmap;
    auto status = gdibmp->GetHBITMAP(0, &hbitmap);
    if (status != Gdiplus::Ok) {
        LogFile::instance() << "ERROR! Wrong bitmap status: " << status << '\n';
        return;
    }
    BITMAP bm;
    GetObject(hbitmap, sizeof bm, &bm);
    DIBSECTION ds;
    if (sizeof ds == GetObject(hbitmap, sizeof ds, &ds)){
        HDC hdc = GetDC(NULL);
        HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                                             ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdc);

        // copying bitmap to clipboard
        if (OpenClipboard(NULL)){
            LogFile::instance() << "Copying image.\n";
            EmptyClipboard();
            //DWORD CF_PNG = RegisterClipboardFormat(L"PNG");
            SetClipboardData(CF_BITMAP, hbitmap_ddb);
            CloseClipboard();
        }
        else {
            LogFile::instance() << "ERROR! Couldn't open clipboard: " << GetLastError() << '\n';
        }
        DeleteObject(hbitmap_ddb);
    }
    DeleteObject(hbitmap);

    delete gdibmp;
    LogFile::instance() << "Finished copying to clipboard.\n";

}

void DirectoryMonitoringController::quit(){
    m_running = false;
    CancelIoEx(m_dirPath->getDHandler(), NULL);
}
