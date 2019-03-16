////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_DISPATCHEROBJECT_H__
#define __GUI_DISPATCHEROBJECT_H__


#include <NsCore/Noesis.h>
#include <NsGui/DependencySystemApi.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/ReflectionDeclare.h>


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Represents an object with thread affinity. This object can be accessed only from the thread on
/// which it was created.
///
/// https://msdn.microsoft.com/en-us/library/system.windows.threading.dispatcherobject.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_DEPENDENCYSYSTEM_API DispatcherObject: public BaseComponent
{
public:
    NS_DISABLE_COPY(DispatcherObject)

    virtual ~DispatcherObject() = 0;

protected:
    DispatcherObject();

    /// Enforces that the calling thread has access to this DispatcherObject
    void VerifyAccess() const;

    // Allows certain derived classes to break the dispatcher affinity
    void DetachFromDispatcher();

private:
    uint32_t mThreadId;

    NS_DECLARE_REFLECTION(DispatcherObject, BaseComponent)
};

}

#endif
