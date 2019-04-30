#pragma once

#include <Box2D/Box2D.h>

#include "event-emitter.hpp"

/* One of the rigidbody must be dynamic */

class ContactListener : public b2ContactListener {
public:
	ContactListener(EventEmitter& emitter);
	
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

private:
	EventEmitter& m_emitter;
};

