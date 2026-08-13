#include "../visited_list.h"

#include <cstdint>
#include <limits>

int main() {
	static_assert(sizeof(std::intptr_t) >= sizeof(int), "visited-list payload cost must fit in intptr_t");

	void* known = visited_list_payload::known_marker();
	if (known == nullptr) {
		return 1;
	}
	if (known != visited_list_payload::encode_cost(1)) {
		return 1;
	}

	const int costs[] = {1, 2, 17, 255, 4096, std::numeric_limits<int>::max()};
	for (int cost : costs) {
		void* encoded = visited_list_payload::encode_cost(cost);
		if (encoded == nullptr) {
			return 1;
		}
		if (visited_list_payload::decode_cost(encoded) != cost) {
			return 1;
		}
	}

	return 0;
}
