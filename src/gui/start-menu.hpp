#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>

class StartMenu: public Noesis::StackPanel {
public:
    StartMenu();

private:
    /* Events */
    void OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
    void OnButton2Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
    bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

    /* Init */
    void InitializeComponent();
    NS_IMPLEMENT_INLINE_REFLECTION(StartMenu, StackPanel) {
        NsMeta<Noesis::TypeId>("StartMenu");
    }
};
