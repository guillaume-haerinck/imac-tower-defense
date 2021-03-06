////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsCore/Error.h>
#include <NsCore/StringUtils.h>


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
TypeInfo::TypeInfo(): mId("")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
TypeInfo::TypeInfo(const char* id): mId(id)
{
    NS_ASSERT(mId != 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator==(const TypeInfo& typeInfo) const
{
    return String::Equals(mId, typeInfo.mId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator!=(const TypeInfo& typeInfo) const
{
    return !(*this == typeInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator<(const TypeInfo& typeInfo) const
{
    return String::Compare(mId, typeInfo.mId) < 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator<=(const TypeInfo& typeInfo) const
{
    return !(*this > typeInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator>(const TypeInfo& typeInfo) const
{
    return typeInfo < *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TypeInfo::operator>=(const TypeInfo& typeInfo) const
{
    return !(*this < typeInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* TypeInfo::GetId() const
{
    return mId;
}

}
