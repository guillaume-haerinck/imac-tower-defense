#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>

class TitleScreen: public Noesis::StackPanel {
public:
    TitleScreen();

private:
    /* Events */
    void OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
    void OnButton2Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
    bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

    /* Init */
    void InitializeComponent();
    NS_IMPLEMENT_INLINE_REFLECTION(TitleScreen, StackPanel) {
        NsMeta<Noesis::TypeId>("TitleScreen");
    }
};
