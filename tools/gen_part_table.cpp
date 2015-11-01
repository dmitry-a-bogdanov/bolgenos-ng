// Usage:
// without args: print disk geometry
// with one argument: print partition table table to file specified by arg
// other arguments are ignored

#include <cstring>

#include <iostream>
#include <fstream>

constexpr size_t chs_size = 3;
constexpr size_t partition_entry_size = 0x10;
constexpr size_t number_of_partitions = 4;
constexpr size_t partition_table_size =
		number_of_partitions*partition_entry_size;

constexpr int active_partition = 0x80;
constexpr int inactive_partition = 0x00;
constexpr int partition_type = 0xbb;

constexpr int cylinder_h_mask = 0x03;
constexpr int cylinder_h_offset = 6;
constexpr int cylinder_l_mask = 0x3f;
constexpr int cylinder_l_offset = 0;

struct chs {
	unsigned int head_:8;
	unsigned int sector_:6;			// non-portable bitfield order
	unsigned int cylinder_:10;		// but supporting by gcc is ok
} __attribute__((packed));

static_assert(sizeof(chs) == chs_size, "CHS address size is incorrect");

struct partition_entry {
	unsigned int active_:8;
	chs start_;
	unsigned int type_:8;
	chs end_;

	// LBA addresses is unused becase BIOSes of some VMs doesn't support it
	unsigned int lba_start_:32;
	unsigned int lba_size_:32;
} __attribute__((packed));

static_assert(sizeof(partition_entry) == partition_entry_size,
		"Partition entry size is incorrect");

struct partition_table {
	partition_entry partitions[number_of_partitions];
} __attribute__((packed));

static_assert(sizeof(partition_table) == partition_table_size,
		"Partition table is incrrect");

size_t partition_sizes[number_of_partitions] = {0x100, 0x100, 0x100, 0x100};

namespace geometry {
	constexpr int sectors_per_track = 32;
	constexpr int heads = 100;
} // geometry

chs lba_to_chs(const size_t lba) {
	chs addr;
	int sector = lba % geometry::sectors_per_track + 1;
	int head = ((lba - (sector - 1)) / geometry::sectors_per_track)
			% geometry::heads;
	int cylinder = (lba - (sector - 1) - head*geometry::sectors_per_track)
			/ (geometry::heads * geometry::sectors_per_track);
	addr.sector_ = sector;
	addr.head_ = head;
	addr.cylinder_ = cylinder;
	return addr;
}


int main(int argc, char **argv) {
	partition_table pt;
	size_t current_lba = 0;
	for(size_t i = 0; i < number_of_partitions; ++i) {
		partition_entry &p = pt.partitions[i];
		if (partition_sizes[i] == 0) {
			memset(&p, 0, partition_entry_size);
			continue;
		}
		++current_lba;
		p.active_ = (i == 0) ? active_partition : inactive_partition;
		p.start_ = lba_to_chs(current_lba);
		p.type_ = partition_type;
		current_lba += partition_sizes[i];
		p.end_ = lba_to_chs(current_lba);
		p.lba_start_ = current_lba;
		p.lba_size_ = partition_sizes[i];
	}

	char mbr[partition_table_size];
	std::memcpy(mbr, &pt, partition_table_size);

	if (argc == 1) {
		std::cout
			<< "geometry:" << std::endl
			<< "\tsectors per track: "
				<< geometry::sectors_per_track << std::endl
			<< "\tnumber of heads: "
				<< geometry::heads << std::endl;
	} else {
		std::ofstream out(argv[1]);
		out.write(mbr, partition_table_size);
	}
	return 0;
}
