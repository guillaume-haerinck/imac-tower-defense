////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __CORE_REFLECTIONIMPLEMENTENUM_H__
#define __CORE_REFLECTIONIMPLEMENTENUM_H__


#include <NsCore/Noesis.h> 
#include <NsCore/CompilerTools.h>
#include <NsCore/TypeEnumCreator.h>
#include <NsCore/TypeEnumHelper.h>
#include <NsCore/TypeEnumImpl.h>
#include <NsCore/ReflectionDeclareEnum.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Shortcuts for defining reflection members
////////////////////////////////////////////////////////////////////////////////////////////////////

/// Adds metadatas
#define NsMeta helper.Meta

/// Defines a value for an enum
#define NsVal helper.Val


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implements reflection for an enum.
////////////////////////////////////////////////////////////////////////////////////////////////////
#define NS_IMPLEMENT_REFLECTION_ENUM(T) \
    const Noesis::TypeEnum* Noesis::TypeEnumFiller<T>::GetType() \
    { \
        return Noesis::TypeEnumHelper<T>::GetType(); \
    } \
    void Noesis::TypeEnumFiller<T>::Fill(Noesis::TypeEnumCreator<T>& helper)


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implements inline the reflection for an enum.
////////////////////////////////////////////////////////////////////////////////////////////////////
#define NS_IMPLEMENT_INLINE_REFLECTION_ENUM(T) \
template<> struct TypeEnumFiller<T> \
{ \
    inline static const Noesis::TypeEnum* GetType() \
    { \
        return Noesis::TypeEnumHelper<T>::GetType(); \
    } \
    inline static void Fill(TypeEnumCreator<T>& helper); \
}; \
inline void Noesis::TypeEnumFiller<T>::Fill(Noesis::TypeEnumCreator<T>& helper)


#endif
