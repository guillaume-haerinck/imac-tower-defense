#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <entt/entt.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

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

	char m_title[256];
	char m_text[256];
	char m_imagePath[256];
};
