#pragma once


namespace irq {

class InterruptsManager;

/// The function registers default handlers for exceptions in specified InterruptManager.
void install_traps(InterruptsManager *irq_manager);


} // namespace irq
