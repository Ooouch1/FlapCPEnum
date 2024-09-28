#pragma once

//
// WRITTEN IN C++14
//

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <mpi.h>

#include "FlapCPEnumeration.hpp"
#include "ItemCountingStream.hpp"
#include "ParallelEnumeration.hpp"

class AppMain {
	std::string createRightAlignedString(int val, int maxLength) {
		std::ostringstream oss;
		oss << std::setw(maxLength) << std::setfill('0') << val;

		return oss.str();
	}


	class FileOutStream {
		std::ofstream fout;
	public:
		FileOutStream(const std::string& path) : fout(path) {
		}

		template<typename T>
		FileOutStream& operator<<(const T& val) {
			fout << val << std::endl;
			return *this;
		}
	};

	typedef typename enumeration::ItemCountingStream<FileOutStream> CountingStream;

	template<typename TEnumerator, typename TOStream>
	TEnumerator run(const int placeCount, TOStream& os, bool patternOutputIsNeeded) {
		TEnumerator enumerator;

		if (patternOutputIsNeeded) {
			enumeration::origami::CPStringEncoder<CountingStream> cpStream(os);
			enumerator.enumerate(placeCount, cpStream);
		}
		else {
			enumeration::origami::CountOnlyEncoder<CountingStream> cpStream(os);
			enumerator.enumerate(placeCount, cpStream);

		}

		return enumerator;
	}


	void printParameterHelp() {
		std::cerr << "wrong parameters. please pass the followings:" << std::endl
			<< "placeCount, [\"cp\" | \"cp_MVLSL\" | \"cp_ExMVLSL\" | \"maekawa\" | \"kawasaki\" | "
			<< "\"cp_parallel\" | \"cp_exLSLparallel\" | \"maekawa_parallel\"] [output directory]";
	}

	std::string formatDirectoryText(const char* text) {
		std::string directory(text);
		directory += (directory.back() == '/') ? "" : "/";

		return directory;
	}

	void enableFileOutput(CountingStream& counting, const std::string& directory, const std::string& algorithmName, const u_int placeCount) {

		std::shared_ptr<FileOutStream> fout_ptr(new FileOutStream(
			directory + algorithmName + "_" + createRightAlignedString(placeCount, 3) + ".txt"));

		counting.setStream(fout_ptr);

	}

public:
	const int ARG_INDEX_SIZE = 1;
	const int ARG_INDEX_ALGORITHM = ARG_INDEX_SIZE + 1;
	const int ARG_INDEX_OUTPUT = ARG_INDEX_ALGORITHM + 1;
	const int ARG_COUNT = ARG_INDEX_OUTPUT + 1;

	int runMain(int argc, char *argv[])
	{
		int myID;
		MPI_Comm_rank(MPI_COMM_WORLD, &myID);

		//std::cout << "ID " << myID << " start." << std::endl;
	
		if (argc < ARG_INDEX_OUTPUT) {
			if (myID == 0)
				printParameterHelp();

			return 1;
		}

		using namespace enumeration::origami;
		
		const u_int placeCount = atoi(argv[ARG_INDEX_SIZE]);

		if (myID == 0) {
			std::cout << "---------------------------------------------------------" << placeCount << std::endl;
			std::cout << "#place = " << placeCount << std::endl;
		}

		CountingStream os(placeCount / 2);

		const std::string algorithmName(argv[ARG_INDEX_ALGORITHM]);

		//std::cout << "start " << algorithmName << " ID=" << myID << std::endl;

		bool fileOutputIsNeeded = (argc >= ARG_INDEX_OUTPUT + 1);
		if (fileOutputIsNeeded) {
			std::cout << "enables file output." << std::endl;
			// split file by ID
			std::stringstream ss;
			ss << myID;
			enableFileOutput(os, formatDirectoryText(argv[ARG_INDEX_OUTPUT]), algorithmName + "_ID_" + ss.str(), placeCount);
		}




		if (algorithmName == "cp_parallel" || algorithmName == "cp_exLSLparallel" || algorithmName == "maekawa_parallel") {
			double startTime, endTime;

			MPI_Barrier(MPI_COMM_WORLD);
			startTime = MPI_Wtime();

			unsigned long long int answerCount = 0ULL;
			if (algorithmName == "cp_parallel") {
				auto enumerator = run<FoldableFlapCPParallelEnumeration<> >(placeCount, os, fileOutputIsNeeded);
			}
			else if (algorithmName == "cp_exLSLparallel") {
				auto enumerator = run<ExLSLFoldableFlapCPParallelEnumeration<> >(placeCount, os, fileOutputIsNeeded);
			}
			else{
				auto enumerator = run<MaekawaFlapCPParallelEnumeration<> >(placeCount, os, fileOutputIsNeeded);
			}

			MPI_Barrier(MPI_COMM_WORLD);
			endTime = MPI_Wtime();


			//std::cout << "ID:" << myID << " #pattern = " << os.toString() << std::endl;

			unsigned long long int total = os.total();
			unsigned long long int totalAll = 0;
			MPI_Reduce(&total, &totalAll, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

			if (myID == 0) {
				std::cout << " #total_pattern = " << totalAll << std::endl;
				std::cout << "time: " << endTime - startTime << "[sec]" << std::endl;
			}
		}
		else if (myID == 0) { //  serial algorithms
			double startTime, endTime;

			MPI_Barrier(MPI_COMM_WORLD);
			startTime = MPI_Wtime();

			if (algorithmName == "kawasaki") {
				auto enumerator = run<KawasakiFlapEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
			}
			else if (algorithmName == "maekawa") {
				auto enumerator = run<MaekawaFlapCPEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
				std::cout << "mv efficiency       " << enumerator.mvStats().searchEfficiency() << std::endl;
				std::cout << "#pattern/#(k&m)     " << enumerator.sufficientRate() << std::endl;
			}
			else if (algorithmName == "cp") {
				auto enumerator = run<FoldableFlapCPEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
				std::cout << "mv efficiency       " << enumerator.mvStats().searchEfficiency() << std::endl;
				std::cout << "#pattern/#(k&m)     " << enumerator.sufficientRate() << std::endl;
			}
			else if (algorithmName == "cp_linearMV") {
				auto enumerator = run<LinearMVFoldableFlapCPEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
				std::cout << "mv efficiency       " << enumerator.mvStats().searchEfficiency() << std::endl;
				std::cout << "#pattern/#(k&m)     " << enumerator.sufficientRate() << std::endl;
			}
			else if (algorithmName == "cp_MVLSL") {
				auto enumerator = run<MVLSLFoldableFlapCPEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
				std::cout << "mv efficiency       " << enumerator.mvStats().searchEfficiency() << std::endl;
				std::cout << "#pattern/#(k&m)     " << enumerator.sufficientRate() << std::endl;
			}
			else if (algorithmName == "cp_ExMVLSL") {
				auto enumerator = run<ExMVLSLFoldableFlapCPEnumeration<true> >(placeCount, os, fileOutputIsNeeded);
				std::cout << "kawasaki efficiency " << enumerator.kawasakiStats().searchEfficiency() << std::endl;
				std::cout << "mv efficiency       " << enumerator.mvStats().searchEfficiency() << std::endl;
				std::cout << "#pattern/#(k&m)     " << enumerator.sufficientRate() << std::endl;
			}
			else {
				std::cerr << "No such algorithm: " << algorithmName << std::endl;
				printParameterHelp();
				return 1;
			}

			MPI_Barrier(MPI_COMM_WORLD);
			endTime = MPI_Wtime();

			std::cout << " #pattern = " << os.toString() << std::endl;

			std::cout << "time: " << endTime - startTime << "[sec]" << std::endl;

		}


		return 0;
	}

};

