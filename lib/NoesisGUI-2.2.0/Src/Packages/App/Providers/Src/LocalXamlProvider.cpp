////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/LocalXamlProvider.h>
#include <NsCore/Ptr.h>
#include <NsCore/Stream.h>
#include <NsCore/File.h>
#include <NsCore/StringUtils.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
LocalXamlProvider::LocalXamlProvider(const char* rootPath)
{
    String::Copy(mRootPath, sizeof(mRootPath), rootPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Stream> LocalXamlProvider::LoadXaml(const char* filename)
{
    char uri[512];

    if (String::IsNullOrEmpty(mRootPath))
    {
        String::Copy(uri, sizeof(uri), filename);
    }
    else
    {
        String::Copy(uri, sizeof(uri), mRootPath);
        String::Append(uri, sizeof(uri), "/");
        String::Append(uri, sizeof(uri), filename);
    }

    return File::OpenStream(uri);
}
