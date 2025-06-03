#include "IHostComponent.h"

std::atomic<bool> IHostComponent::globalOn{ true };

bool IHostComponent::isProgramOn() {
	return IHostComponent::globalOn;
};

void IHostComponent::killProgram() {
	IHostComponent::IHostComponent::globalOn = false;
};
