////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_BASEVIEW_H__
#define __GUI_BASEVIEW_H__


#include <NsCore/Noesis.h>
#include <NsGui/ControlsApi.h>
#include <NsGui/DependencyObject.h>
#include <NsGui/IUITreeNode.h>


namespace Noesis
{

class ListViewItem;

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Represents the base class for views that define the appearance of data in a ListView control.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.controls.viewbase.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CONTROLS_API BaseView: public DependencyObject, public IUITreeNode
{
public:
    BaseView();
    ~BaseView();

    /// Removes all bindings and styling that are set for an item
    virtual void ClearItem(ListViewItem* item);

    /// Prepares an item in the view for display, by setting bindings and styles
    virtual void PrepareItem(ListViewItem* item);

    /// From IUITreeNode
    //@{
    IUITreeNode* GetNodeParent() const override;
    void SetNodeParent(IUITreeNode* parent) override;
    BaseComponent* FindNodeResource(IResourceKey* key, bool fullElementSearch) const override;
    BaseComponent* FindNodeName(const char* name) const override;
    ObjectWithNameScope FindNodeNameAndScope(const char* name) const override;
    //@}

    NS_IMPLEMENT_INTERFACE_FIXUP

protected:
    /// From DependencyObject
    //@{
    void OnObjectValueSet(BaseComponent* oldValue, BaseComponent* newValue) override;
    //@}

private:
    IUITreeNode* mOwner;

    NS_DECLARE_REFLECTION(BaseView, DependencyObject)
};

NS_WARNING_POP

}


#endif
