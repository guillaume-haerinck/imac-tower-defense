#include "start-menu.hpp"

#include <NsGui/Button.h>

/* TODO Data binding
    https://www.noesisengine.com/docs/Gui.Core.DataBindingTutorial.html
    https://www.noesisengine.com/docs/Gui.Core.UserControlTutorial.html
    https://www.noesisengine.com/docs/Gui.Core.CustomControlTutorial.html
*/

StartMenu::StartMenu() {
    InitializeComponent();
}

void StartMenu::InitializeComponent() {
    Noesis::GUI::LoadComponent(this, "start-menu.xaml");
}

bool StartMenu::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
    NS_CONNECT_EVENT(Noesis::Button, Click, OnButton1Click);
    NS_CONNECT_EVENT(Noesis::Button, Click, OnButton2Click);
    return false;
}

void StartMenu::OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
    printf("Button1 was clicked\n");
}

void StartMenu::OnButton2Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
    printf("Button2 was clicked\n");
}
