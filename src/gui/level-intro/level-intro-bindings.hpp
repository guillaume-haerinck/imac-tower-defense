#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>

class LevelIntroBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	LevelIntroBindings();

	const char* getTitle() const;
	void setTitle(const char* value);

	const char* getImagePath() const;
	void setImagePath(const char* value);

	const char* getText() const;
	void setText(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelIntroBindings, NotifyPropertyChangedBase)

	NsString m_title;
	NsString m_text;
	NsString m_imagePath;
};
