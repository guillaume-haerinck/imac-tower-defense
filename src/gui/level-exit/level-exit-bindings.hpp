#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>

class LevelExitBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	LevelExitBindings();

	const char* getText() const;
	void setText(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelExitBindings, NotifyPropertyChangedBase)

	NsString m_text;
};
