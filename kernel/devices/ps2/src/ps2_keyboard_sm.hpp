#pragma once

#include <cstdint.hpp>
#include <loggable.hpp>

// forward declaration
namespace ps2 {
namespace keyboard {
class PS2DefaultKeyboard;
}
}


namespace ps2_kbd_impl {


enum class handle_status_t {
	done,
	cont,
};

// forward declaration
class KbdStateMachine;

class IState: protected Loggable("ps/2.state_handler")
{
public:
	IState() = delete;
	IState(const IState&) = delete;
	IState& operator=(const IState&) = delete;

	IState(KbdStateMachine *machine)
		: _machine(machine)
	{
	}

	virtual ~IState() {}

	virtual handle_status_t handle_byte(uint8_t byte) = 0;
protected:
	KbdStateMachine *_machine = nullptr;
};



class WaitState: public ps2_kbd_impl::IState
{
public:
	WaitState(ps2_kbd_impl::KbdStateMachine* machine)
		: IState(machine)
	{
	}

	handle_status_t handle_byte(uint8_t byte) override;

	virtual ~WaitState() {}
};



class ErrorState: public IState
{
public:
	ErrorState(ps2_kbd_impl::KbdStateMachine* machine)
			: IState(machine)
	{
	}

	handle_status_t handle_byte(uint8_t byte) override;
};


class OneByteKeyReleasedState: public IState
{
public:
	OneByteKeyReleasedState(KbdStateMachine *machine)
			: IState(machine) {}
	handle_status_t handle_byte(uint8_t byte) override;
};


class PrintState: public IState
{
public:
	PrintState(KbdStateMachine *machine)
			: IState(machine) {}

	handle_status_t handle_byte(uint8_t byte) override;
};


class OneByteKeyPressedState: public IState
{
public:
	OneByteKeyPressedState(KbdStateMachine *machine)
			: IState(machine)
	{
	}

	handle_status_t handle_byte(uint8_t byte) override;
};



class KbdStateMachine {
public:
	KbdStateMachine() = delete;
	explicit KbdStateMachine(ps2::keyboard::PS2DefaultKeyboard *device)
	{
		state_ = wait_state();
		device_ = device;
	}
	KbdStateMachine(const KbdStateMachine&) = delete;
	KbdStateMachine& operator=(const KbdStateMachine&) = delete;

	void set_state(IState *next_state)
	{
		state_ = next_state;
	}

	void handle_byte(uint8_t byte)
	{
		while(state_->handle_byte(byte) != handle_status_t::done);
	}

	IState *wait_state()
	{
		return &wait_state_;
	}

	IState *error_state()
	{
		return &error_state_;
	}

	IState *one_byte_key_pressed()
	{
		return &one_byte_key_pressed_;
	}

	IState *one_byte_key_released()
	{
		return &one_byte_key_released_;
	}

	IState *print_state()
	{
		return &print_state_;
	}

	ps2::keyboard::PS2DefaultKeyboard *get_device()
	{
		return device_;
	}

private:
	WaitState wait_state_{this};
	ErrorState error_state_{this};
	OneByteKeyPressedState one_byte_key_pressed_{this};
	OneByteKeyReleasedState one_byte_key_released_{this};
	PrintState print_state_{this};
	IState *state_ = nullptr;
	ps2::keyboard::PS2DefaultKeyboard *device_ = nullptr;

}; // class KbdStateMachine

} // namespace ps2_kbd_impl
