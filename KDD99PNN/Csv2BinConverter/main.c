#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#define INFO_FILENAME_SS "\\info.csv"
#define DATA_DIR_SUFFIX "\\data\\"
#define DATA_DIR_SUFFIX_STAR "\\data\\*"

#define FILE_EXTENSION_DELIMITER '.'
#define CSV_DELIMITER ","
#define BUF_SIZE 256

int _tmain(int argc, TCHAR * argv[])
{
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;

    TCHAR infoPath[MAX_PATH];
    TCHAR dataDir[MAX_PATH];

    char buf[BUF_SIZE];
    char * token, * next_token;
    rsize_t strmax = BUF_SIZE - 1;

    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    TCHAR * csv_out_dir;
    TCHAR * pnn_out_filename;

    HANDLE pnn_hFile;
    HANDLE tmp_hFile;
    BOOL write_error_flag;
    int buf_int;
    double * buf_double;

    DWORD bytesRead = 0;
    DWORD bytesWrite = 0;

    if (argc != 3)
    {
        _tprintf(TEXT("\nUsage: %s <csv out directory name> <binary out file name>\n"), argv[0]);
        return (-1);
    }

    csv_out_dir = argv[1];
    pnn_out_filename = argv[2];

    StringCchLength(csv_out_dir, MAX_PATH, &length_of_arg);
    if (length_of_arg > (MAX_PATH - 3))
    {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    _tprintf(TEXT("\nTarget directory is %s\n\n"), csv_out_dir);

    // =========================================================== Open output binary file =========

    pnn_hFile = CreateFile(pnn_out_filename,      // name of the write
                           GENERIC_WRITE,         // open for writing
                           0,                     // do not share
                           NULL,                  // default security
                           CREATE_ALWAYS,         // create new file only
                           FILE_ATTRIBUTE_NORMAL, // normal file
                           NULL);                 // no attr. template
    if (pnn_hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), pnn_out_filename);
        return (-1);
    }
    _tprintf(TEXT("Binary output: %s\n"), pnn_out_filename);

    // =========================================================== Info ============================

    StringCchCopy(infoPath, MAX_PATH, csv_out_dir);
    StringCchCat(infoPath, MAX_PATH, TEXT(INFO_FILENAME_SS));

    tmp_hFile = CreateFile(infoPath,              // file to open
                           GENERIC_READ,          // open for reading
                           FILE_SHARE_READ,       // share for reading
                           NULL,                  // default security
                           OPEN_EXISTING,         // existing file only
                           FILE_ATTRIBUTE_NORMAL, // normal file
                           NULL);                 // no attr. template
    if (tmp_hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), pnn_out_filename);
        return (-1);
    }

    if (!ReadFile(tmp_hFile, buf, BUF_SIZE - 1, &bytesRead, NULL)) {
        _tprintf(_T("Error reading file: %u\n"), GetLastError());
        CloseHandle(tmp_hFile);
        CloseHandle(pnn_hFile);
        return (-1);
    }
    buf[bytesRead] = '\0';
    
    token = strtok_s(buf, CSV_DELIMITER, &next_token);
    while(token)
    {
        buf_int = atoi(token);

        write_error_flag = WriteFile(pnn_hFile,   // open file handle
                                     &buf_int,    // start of data to write
                                     sizeof(int), // number of bytes to write
                                     &bytesWrite, // number of bytes that were written
                                     NULL);       // no overlapped structure
        if (FALSE == write_error_flag || bytesWrite != sizeof(int))
        {
            _tprintf(_T("Terminal failure: Unable to write to file.\n"));
            CloseHandle(tmp_hFile);
            CloseHandle(pnn_hFile);
            return (-1);
        }

        token = strtok_s(NULL, CSV_DELIMITER, &next_token);
    }

    CloseHandle(tmp_hFile);

    // =========================================================== Data ============================

    StringCchCopy(dataDir, MAX_PATH, csv_out_dir);
    StringCchCat(dataDir, MAX_PATH, TEXT(DATA_DIR_SUFFIX_STAR));

    hFind = FindFirstFile(dataDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return dwError;
    }

    char * class_name;
    size_t class_name_length;
    char * class_buf;
    int reference_count, reference_id, property_count;
    wchar_t class_filepath[MAX_PATH * 3];
    do
    {
        if (ffd.cFileName[0] != '.')
        {
            _tprintf(TEXT("Processing file %s\n"), ffd.cFileName);

            class_name_length = wcslen(ffd.cFileName) - 4;
            write_error_flag = WriteFile(pnn_hFile,          // open file handle
                                         &class_name_length, // start of data to write
                                         sizeof(size_t),     // number of bytes to write
                                         &bytesWrite,        // number of bytes that were written
                                         NULL);              // no overlapped structure
            if (FALSE == write_error_flag || bytesWrite != sizeof(size_t))
            {
                _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                CloseHandle(pnn_hFile);
                return (-1);
            }

            class_name = (char *)malloc(sizeof(char) * (class_name_length + 1));
            for (int i = 0; i < class_name_length; i++)
                class_name[i] = (char)ffd.cFileName[i];
            class_name[class_name_length] = '\0';
            write_error_flag = WriteFile(pnn_hFile,                                 // open file handle
                                         class_name,                                // start of data to write
                                         (DWORD)(sizeof(char) * class_name_length), // number of bytes to write
                                         &bytesWrite,                               // number of bytes that were written
                                         NULL);                                     // no overlapped structure
            if (FALSE == write_error_flag || bytesWrite != sizeof(char) * class_name_length)
            {
                _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                free(class_name);
                CloseHandle(pnn_hFile);
                return (-1);
            }

            sscanf_s(class_name, "%*[^-]-%s", buf, BUF_SIZE);
            reference_count = atoi(buf);
            free(class_name);
            write_error_flag = WriteFile(pnn_hFile,        // open file handle
                                         &reference_count, // start of data to write
                                         sizeof(int),      // number of bytes to write
                                         &bytesWrite,      // number of bytes that were written
                                         NULL);            // no overlapped structure
            if (FALSE == write_error_flag || bytesWrite != sizeof(int))
            {
                _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                CloseHandle(pnn_hFile);
                return (-1);
            }

            StringCchCopy(class_filepath, MAX_PATH, csv_out_dir);
            StringCchCat(class_filepath, MAX_PATH, TEXT(DATA_DIR_SUFFIX));
            StringCchCat(class_filepath, MAX_PATH, ffd.cFileName);
            tmp_hFile = CreateFile(class_filepath,        // file to open
                                   GENERIC_READ,          // open for reading
                                   FILE_SHARE_READ,       // share for reading
                                   NULL,                  // default security
                                   OPEN_EXISTING,         // existing file only
                                   FILE_ATTRIBUTE_NORMAL, // normal file
                                   NULL);                 // no attr. template
            if (tmp_hFile == INVALID_HANDLE_VALUE)
            {
                _tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), pnn_out_filename);
                CloseHandle(pnn_hFile);
                CloseHandle(tmp_hFile);
                return (-1);
            }

            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            class_buf = (char *)malloc(sizeof(char) * (filesize.QuadPart + 1));
            
            if (!ReadFile(tmp_hFile, class_buf, filesize.QuadPart, &bytesRead, NULL)) {
                _tprintf(_T("Error reading file: %u\n"), GetLastError());
                CloseHandle(tmp_hFile);
                CloseHandle(pnn_hFile);
                free(class_buf);
                return (-1);
            }
            class_buf[filesize.QuadPart] = '\0';

            token = strtok_s(class_buf, CSV_DELIMITER, &next_token);
            while(token)
            {
                reference_id = atoi(token);
                write_error_flag = WriteFile(pnn_hFile,     // open file handle
                                             &reference_id, // start of data to write
                                             sizeof(int),   // number of bytes to write
                                             &bytesWrite,   // number of bytes that were written
                                             NULL);         // no overlapped structure
                if (FALSE == write_error_flag || bytesWrite != sizeof(int))
                {
                    _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                    CloseHandle(tmp_hFile);
                    CloseHandle(pnn_hFile);
                    return (-1);
                }
                token = strtok_s(NULL, CSV_DELIMITER, &next_token);

                property_count = atoi(token);
                write_error_flag = WriteFile(pnn_hFile,     // open file handle
                                             &property_count, // start of data to write
                                             sizeof(int),   // number of bytes to write
                                             &bytesWrite,   // number of bytes that were written
                                             NULL);         // no overlapped structure
                if (FALSE == write_error_flag || bytesWrite != sizeof(int))
                {
                    _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                    CloseHandle(tmp_hFile);
                    CloseHandle(pnn_hFile);
                    return (-1);
                }
                token = strtok_s(NULL, CSV_DELIMITER, &next_token);

                buf_double = (double *)malloc(property_count * sizeof(double));
                for (int i = 0; i < property_count; i++)
                {
                    buf_double[i] = atof(token);
                    token = strtok_s(NULL, CSV_DELIMITER, &next_token);
                }
                write_error_flag = WriteFile(pnn_hFile,                       // open file handle
                                             buf_double,                      // start of data to write
                                             property_count * sizeof(double), // number of bytes to write
                                             &bytesWrite,                     // number of bytes that were written
                                             NULL);                           // no overlapped structure
                free(buf_double);
                if (FALSE == write_error_flag || bytesWrite != property_count * sizeof(double))
                {
                    _tprintf(_T("Terminal failure: Unable to write to file.\n"));
                    CloseHandle(tmp_hFile);
                    CloseHandle(pnn_hFile);
                    return (-1);
                }
            }

            free(class_buf);
            CloseHandle(tmp_hFile);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        return dwError;
    }

    CloseHandle(pnn_hFile);
    FindClose(hFind);

    return 0;
}
