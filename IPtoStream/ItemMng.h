#pragma once

typedef int ssize_t;

template<typename T, const ssize_t SIZE>
class ItemsMngConst {
	struct ItemsMng_el {
		T el;
		bool active = 0;
	};

	ItemsMng_el vec[SIZE] = {};
	ssize_t vec_used = 0;
public:
	ssize_t get_fist_free() {
		for (ssize_t i = 0; i < vec_used; ++i)
			if (!vec[i].active) 
				return i;
		return -1;
	}
	ssize_t push(const T& el) {
		int32_t i = get_fist_free();
		if (i == -1)
			return -1;

		vec[i].el = el;
		vec[i].active = true;
		return i;
	}
	bool is_active(ssize_t i) {
		if (i < 0 || i >= vec_used)
			return false;

		return vec[i].active;
	}
	T& operator[](ssize_t i) {
		if (i < 0 || i >= vec.size())
			abort();
		return vec[i].el;
	}
	void remove(ssize_t i) {
		if (!is_active(i))
			return;

		vec[i].active = false;
		while (vec_used && !vec[vec_used - 1].active)
			vec_used--;
	}
	ssize_t size() {
		return vec_used;
	}
	ssize_t max_size() {
		return SIZE;
	}
};