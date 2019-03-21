#include <NsGui/Grid.h>
#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>

class MyGrid: public Noesis::StackPanel
{
public:
    MyGrid()
    {
        InitializeComponent();
    }

private:
    void InitializeComponent()
    {
        Noesis::GUI::LoadComponent(this, "Grid.xaml");
    }

    bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override
    {
        NS_CONNECT_EVENT(Noesis::Button, Click, OnButton1Click);
        NS_CONNECT_EVENT(Noesis::Button, Click, OnButton2Click);
        return false;
    }

    void OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args)
    {
        printf("Button1 was clicked\n");
    }

    void OnButton2Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args)
    {
        printf("Button2 was clicked\n");
    }

    NS_IMPLEMENT_INLINE_REFLECTION(MyGrid, StackPanel)
    {
        NsMeta<Noesis::TypeId>("MyGrid");
    }
};