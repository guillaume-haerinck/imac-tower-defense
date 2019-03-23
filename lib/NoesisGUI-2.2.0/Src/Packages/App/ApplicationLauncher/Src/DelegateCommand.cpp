////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/DelegateCommand.h>
#include <NsCore/ReflectionImplementEmpty.h>


using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
DelegateCommand::DelegateCommand()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
DelegateCommand::DelegateCommand(const ExecuteFunc& execute): _execute(execute)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
DelegateCommand::DelegateCommand(const ExecuteFunc& execute, const CanExecuteFunc& canExecute):
    _execute(execute), _canExecute(canExecute)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void DelegateCommand::SetExecuteFunc(const ExecuteFunc& execute)
{
    _execute = execute;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void DelegateCommand::SetCanExecuteFunc(const CanExecuteFunc& canExecute)
{
    _canExecute = canExecute;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool DelegateCommand::CanExecute(BaseComponent* param) const
{
    return _canExecute.Empty() || _canExecute(param);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void DelegateCommand::Execute(BaseComponent* param) const
{
    _execute(param);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(DelegateCommand)
