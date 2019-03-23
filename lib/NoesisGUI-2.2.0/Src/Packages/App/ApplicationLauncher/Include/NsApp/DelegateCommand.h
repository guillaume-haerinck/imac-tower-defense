////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_DELEGATECOMMAND_H__
#define __APP_DELEGATECOMMAND_H__


#include <NsCore/Noesis.h>
#include <NsCore/Delegate.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsGui/BaseCommand.h>
#include <NsApp/ApplicationLauncherApi.h>


namespace NoesisApp
{

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Command implemented using delegates
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_APPLICATIONLAUNCHER_API DelegateCommand final: public Noesis::BaseCommand
{
public:
    typedef Noesis::Delegate<void (BaseComponent*)> ExecuteFunc;
    typedef Noesis::Delegate<bool (BaseComponent*)> CanExecuteFunc;

    DelegateCommand();
    DelegateCommand(const ExecuteFunc& execute);
    DelegateCommand(const ExecuteFunc& execute, const CanExecuteFunc& canExecute);

    void SetExecuteFunc(const ExecuteFunc& execute);
    void SetCanExecuteFunc(const CanExecuteFunc& canExecute);

    // From BaseCommand
    //@{
    bool CanExecute(BaseComponent* param) const override;
    void Execute(BaseComponent* param) const override;
    //@}

private:
    ExecuteFunc _execute;
    CanExecuteFunc _canExecute;

    NS_DECLARE_REFLECTION(DelegateCommand, BaseCommand)
};

NS_WARNING_POP

}

#endif
