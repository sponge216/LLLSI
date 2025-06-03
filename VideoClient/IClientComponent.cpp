#include "IClientComponent.h"

std::atomic<bool> IClientComponent::globalOn{ true };

bool IClientComponent::isProgramOn() {
	return IClientComponent::globalOn;
};

void IClientComponent::killProgram() {
	IClientComponent::globalOn = false;
};
