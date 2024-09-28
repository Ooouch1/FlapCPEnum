#include "gtest/gtest.h"
#include <mpi.h>
#include "BitSet.hpp"
#include <iostream>

namespace {

	//class ParallelEnumerationTest : public ::testing::Test {
	//};

	//TEST(ParallelEnumerationTest, testBitSetMPI) {
	//	using namespace std;

	//	mylib::BitSet a(16), b(32);
	//	MPI_Init(nullptr, nullptr);

	//	int myID, processCount;
	//	MPI_Comm_rank(MPI_COMM_WORLD, &myID);
	//	MPI_Comm_size(MPI_COMM_WORLD, &processCount);

	//	cout <<"#threads" << processCount << endl;

	//	if (myID == 0){
	//		a.add(4);
	//		a.MPISend(1, 0);
	//	}
	//	else if (myID == 1) {
	//		b.MPIReceive(0, 0);
	//		ASSERT_TRUE(b.contains(4));
	//	}
	//	
	//	MPI_Finalize();
	//}
}
