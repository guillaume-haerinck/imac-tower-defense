////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_GROUPBOX_H__
#define __GUI_GROUPBOX_H__


#include <NsCore/Noesis.h>
#include <NsGui/HeaderedContentControl.h>


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Represents a control that displays a frame around a group of controls with an optional caption.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.forms.groupbox.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CORE_API GroupBox: public HeaderedContentControl
{
public:
    GroupBox();

    NS_DECLARE_REFLECTION(GroupBox, HeaderedContentControl)
};

}


#endif
