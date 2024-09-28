#pragma once


namespace mylib {

	class CharVectorFactory {
	public:
		std::vector<char> create(int size) {
			std::vector<char> a(size, 0);

			return a;
		}
	};

}