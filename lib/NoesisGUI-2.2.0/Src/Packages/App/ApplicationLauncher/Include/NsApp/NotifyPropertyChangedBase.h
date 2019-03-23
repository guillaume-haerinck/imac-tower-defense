////////////////////////////////////////////////////////////////////////////////////////////////////
// Noesis Engine - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_NOTIFYPROPERTYCHANGEDBASE_H__
#define __APP_NOTIFYPROPERTYCHANGEDBASE_H__


#include <NsCore/Noesis.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/Delegate.h>
#include <NsGui/INotifyPropertyChanged.h>
#include <NsApp/ApplicationLauncherApi.h>


namespace NoesisApp
{

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_APPLICATIONLAUNCHER_API NotifyPropertyChangedBase: public Noesis::BaseComponent,
    public Noesis::INotifyPropertyChanged
{
public:
    /// From INotifyPropertyChanged
    //@{
    Noesis::PropertyChangedEventHandler& PropertyChanged() override final;
    //@}

    NS_IMPLEMENT_INTERFACE_FIXUP

protected:
    void OnPropertyChanged(const char* name);

private:
    Noesis::PropertyChangedEventHandler _propertyChanged;

    NS_DECLARE_REFLECTION(NotifyPropertyChangedBase, BaseComponent)
};

NS_WARNING_POP

}

#endif
