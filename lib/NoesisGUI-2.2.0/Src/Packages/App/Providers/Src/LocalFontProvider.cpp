////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/LocalFontProvider.h>
#include <NsCore/File.h>
#include <NsCore/Find.h>
#include <NsCore/Stream.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
LocalFontProvider::LocalFontProvider(const char* rootPath)
{
    String::Copy(mRootPath, sizeof(mRootPath), rootPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LocalFontProvider::ScanFolder(const char* folder)
{
    char uri[512] = "";

    if (!String::IsNullOrEmpty(mRootPath))
    {
        String::Copy(uri, sizeof(uri), mRootPath);
        String::Append(uri, sizeof(uri), "/");
    }

    String::Append(uri, sizeof(uri), folder);

    ScanFolder(uri, folder, ".ttf");
    ScanFolder(uri, folder, ".otf");
    ScanFolder(uri, folder, ".ttc");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Stream> LocalFontProvider::OpenFont(const char* folder, const char* filename) const
{
    char uri[512] = "";

    if (!String::IsNullOrEmpty(mRootPath))
    {
        String::Copy(uri, sizeof(uri), mRootPath);
        String::Append(uri, sizeof(uri), "/");
    }

    String::Append(uri, sizeof(uri), folder);
    String::Append(uri, sizeof(uri), "/");
    String::Append(uri, sizeof(uri), filename);

    return File::OpenStream(uri);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LocalFontProvider::ScanFolder(const char* path, const char* folder, const char* ext)
{
    FindData findData;

    if (FindFirst(path, ext, findData))
    {
        do
        {
            RegisterFont(folder, findData.filename);
        }
        while (FindNext(findData));

        FindClose(findData);
    }
}
