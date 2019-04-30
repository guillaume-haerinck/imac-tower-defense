#include "contact-listener.hpp"

#include <spdlog/spdlog.h>

ContactListener::ContactListener(EventEmitter& emitter): m_emitter(emitter) {}

void ContactListener::BeginContact(b2Contact* contact) {
	spdlog::info("New contact b2d");
}

void ContactListener::EndContact(b2Contact* contact) {
	spdlog::info("End contact b2d");
}
