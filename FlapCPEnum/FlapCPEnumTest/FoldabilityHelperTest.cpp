#include "gtest/gtest.h"
#include "foldabilityhelpers.hpp"
#include "inverters.hpp"
#include <vector>

namespace {
	using namespace enumeration::origami;
	using namespace mylib;
	using namespace std;

	class FoldabilityHelperTest : public ::testing::Test {
	protected:
		MinimalityChecker minimalityChecker;

		Crimp createCrimp(const u_int& angle, int centerIndex, int nextIndex, const RingArrayList<LineGap>& ring) {
			return Crimp(angle, centerIndex, nextIndex,
				Crease::createMVPairIndex(ring[centerIndex].lineType, ring[nextIndex].lineType));

		}
	};

	TEST_F(FoldabilityHelperTest, testMinimalCrimpSequence) {
		RingArrayList<LineGap> ring(8);

		// ring: +-+--+++
		ring.set(0, LineGap(1, Crease::MAJOR));
		ring.set(1, LineGap(1, Crease::MINOR));
		ring.set(2, LineGap(1, Crease::MAJOR));
		ring.set(3, LineGap(1, Crease::MINOR));
		ring.set(4, LineGap(1, Crease::MINOR));
		ring.set(5, LineGap(1, Crease::MAJOR));
		ring.set(6, LineGap(1, Crease::MAJOR));
		ring.set(7, LineGap(1, Crease::MAJOR));

		ring.makeLinks();

		auto inverter = enumeration::DummyInverter();
		vector<Crimp> minCrimps = minimalityChecker.createMinimalSequence(ring);

		ASSERT_EQ(3, minCrimps.size());

		ASSERT_EQ(createCrimp(1, 0, 1, ring), minCrimps[0]);
		ASSERT_EQ(createCrimp(1, 2, 3, ring), minCrimps[1]);
		ASSERT_EQ(createCrimp(1, 4, 5, ring), minCrimps[2]);
	}
}
