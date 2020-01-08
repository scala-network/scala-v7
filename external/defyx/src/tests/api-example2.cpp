#include "../defyx.h"
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
	const char myKey[] = "DefyX example key";
	const char myInput[] = "DefyX example input";
	char hash[RANDOMX_HASH_SIZE];

	defyx_flags flags = defyx_get_flags();
	flags |= RANDOMX_FLAG_LARGE_PAGES;
	flags |= RANDOMX_FLAG_FULL_MEM;
	defyx_cache *myCache = defyx_alloc_cache(flags);
	if (myCache == nullptr) {
		std::cout << "Cache allocation failed" << std::endl;
		return 1;
	}
	defyx_init_cache(myCache, myKey, sizeof myKey);

	defyx_dataset *myDataset = defyx_alloc_dataset(flags);
	if (myDataset == nullptr) {
		std::cout << "Dataset allocation failed" << std::endl;
		return 1;
	}

	auto datasetItemCount = defyx_dataset_item_count();
	std::thread t1(&defyx_init_dataset, myDataset, myCache, 0, datasetItemCount / 2);
	std::thread t2(&defyx_init_dataset, myDataset, myCache, datasetItemCount / 2, datasetItemCount - datasetItemCount / 2);
	t1.join();
	t2.join();
	defyx_release_cache(myCache);

	defyx_vm *myMachine = defyx_create_vm(flags, nullptr, myDataset);
	if (myMachine == nullptr) {
		std::cout << "Failed to create a virtual machine" << std::endl;
		return 1;
	}

	defyx_calculate_hash(myMachine, &myInput, sizeof myInput, hash);

	defyx_destroy_vm(myMachine);
	defyx_release_dataset(myDataset);

	for (unsigned i = 0; i < RANDOMX_HASH_SIZE; ++i)
		std::cout << std::hex << std::setw(2) << std::setfill('0') << ((int)hash[i] & 0xff);

	std::cout << std::endl;

	return 0;
}