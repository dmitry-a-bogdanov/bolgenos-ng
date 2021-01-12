#pragma once

#include <cstddef.hpp>
#include <cstdint.hpp>

namespace lib {
class ostream;
}

namespace x86 {

namespace tss {

/**
 * \brief MemorySegmentDescriptor register info from TSS
 */
struct [[gnu::packed]] SegmentRegister {
	constexpr explicit SegmentRegister(uint16_t segment)
		: _segment{segment}
	{}

	[[nodiscard]] uint16_t segment() const { return _segment; }

private:
	uint16_t	_segment;
	uint16_t	_reserved{0};
};


/**
 * \brief Stack info for specific protection ring from TSS
 */
struct [[gnu::packed]] ProtectionRingStack {
public:
	constexpr ProtectionRingStack(const uint16_t segment, const lib::byte* const stack_ptr)
		: _pointer{stack_ptr}, _segment{segment}
	{}
	constexpr ProtectionRingStack() = default;

	[[nodiscard]] const lib::byte* pointer() const { return _pointer; }
	[[nodiscard]] uint16_t segment() const { return _segment.segment(); }
private:
	const lib::byte*	_pointer{nullptr};
	SegmentRegister		_segment{0};
};


/**
 * \brief pack of general purpose registers
 */
struct [[gnu::packed]] GPRegistersPack {
	constexpr GPRegistersPack(
		const uint32_t eax,
		const uint32_t ecx,
		const uint32_t edx,
		const uint32_t ebx,
		lib::byte* const esp,
		lib::byte* const ebp,
		const uint32_t esi,
		const uint32_t edi)
		: eax(eax), ecx(ecx), edx(edx), ebx(ebx), esp(esp), ebp(ebp), esi(esi), edi(edi)
	{}

	constexpr GPRegistersPack() = default;

	static constexpr GPRegistersPack zero_on_stack(lib::byte* stack_ptr) {
		return {0x0, 0x0, 0x0, 0x0, stack_ptr, stack_ptr, 0x0, 0x0};
	}

	uint32_t	eax{0};
	uint32_t	ecx{0};
	uint32_t	edx{0};
	uint32_t	ebx{0};
	lib::byte*	esp{nullptr};
	lib::byte*	ebp{nullptr};
	uint32_t	esi{0};
	uint32_t	edi{0};
};


/**
 * \brief pack of segment registers
 */
struct [[gnu::packed]] SegmentRegistersPack {
	constexpr SegmentRegistersPack() = default;
	constexpr SegmentRegistersPack(const uint16_t code_segment, const uint16_t data_segment):
		es{data_segment},
		cs{code_segment},
		ss{data_segment},
		ds{data_segment},
		fs{data_segment},
		gs{data_segment},
		ldt_segment_selector{0}
	{}


	SegmentRegister		es{0};
	SegmentRegister		cs{0};
	SegmentRegister		ss{0};
	SegmentRegister		ds{0};
	SegmentRegister		fs{0};
	SegmentRegister		gs{0};
	SegmentRegister		ldt_segment_selector{0};

	static SegmentRegistersPack kernel();
};


lib::ostream& operator<<(lib::ostream& out, const SegmentRegister& segment_register);
lib::ostream& operator<<(lib::ostream& out, const ProtectionRingStack& stack);

} // namespace tss

struct __attribute__((packed)) TaskStateSegment {

	constexpr TaskStateSegment(
		uint16_t prev_task_link_,
		uint16_t stack_segment_,
		lib::byte* stack_ptr_,
		lib::byte* instruction_ptr_,
		uint32_t eflags_,
		const tss::GPRegistersPack gp_registers,
		const tss::SegmentRegistersPack& segment_registers)
		:
		previous_task_link{prev_task_link_},
		stack{{stack_segment_, stack_ptr_}},
		instruction_ptr{instruction_ptr_},
		eflags{eflags_},
		_gp_registers_pack{gp_registers},
		_segment_registers{segment_registers},
		debug_trap{false},
		_reserved_02{0},
		io_map_base_address{0}
	{}

	constexpr TaskStateSegment()
		: TaskStateSegment{
		0,
		0,
		nullptr,
		nullptr,
		0,
		tss::GPRegistersPack{},
		tss::SegmentRegistersPack{}}
	{}

	uint16_t			previous_task_link;
	uint16_t			_reserved_01{0};
	tss::ProtectionRingStack	stack[3];
	/**
	 * \brief CR3
	 */
	lib::byte*			page_directory{nullptr};
	lib::byte*			instruction_ptr;
	uint32_t			eflags;
	tss::GPRegistersPack		_gp_registers_pack;
	tss::SegmentRegistersPack	_segment_registers;
	bool				debug_trap:1;
	uint16_t			_reserved_02:15;
	uint16_t			io_map_base_address;
};

lib::ostream& operator<<(lib::ostream& out, const TaskStateSegment& tss);

using TSS = TaskStateSegment;

} // namespace x86
