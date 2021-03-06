////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_XAMLPROVIDER_H__
#define __GUI_XAMLPROVIDER_H__


#include <NsCore/Noesis.h>
#include <NsCore/BaseComponent.h>


namespace Noesis
{

class Stream;
template<class T> class Ptr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Base class for implementing providers of XAMLs
////////////////////////////////////////////////////////////////////////////////////////////////////
class XamlProvider: public BaseComponent
{
public:
    // Loads the specified XAML file. Returns null if no xaml found
    virtual Ptr<Stream> LoadXaml(const char* uri) = 0;
};

}

#endif
