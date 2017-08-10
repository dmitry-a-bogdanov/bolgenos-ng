#pragma once

#include <bolgenos-ng/interrupt_controller.hpp>


namespace devices {


class PIC8259: public devices::InterruptController {
public:
	PIC8259(const PIC8259&) = delete;
	PIC8259(PIC8259&&) = delete;
	PIC8259& operator =(const PIC8259&) = delete;
	PIC8259& operator =(PIC8259&&) = delete;

	virtual ~PIC8259() = default;


	virtual int min_irq_vector() const override;

	virtual void initialize_controller() override;

	virtual void end_of_interrupt(irq::irq_t vector) override;

protected:
	PIC8259()
		: InterruptController(), _controller_initialized(false)
	{
	}
private:
	bool _controller_initialized;

	friend class InterruptController;
};


} // namespace pic
