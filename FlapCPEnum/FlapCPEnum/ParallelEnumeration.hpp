#pragma once
#include "MasterWorkerModel.hpp"
#include "KawasakiFlapEnumeration.hpp"
#include "MVEnumeration.hpp"
#include "FlapCPEnumeration.hpp"

#include <mpi.h>

namespace enumeration {
	namespace origami {

		using namespace mylib;


		template <typename EncoderFunc,
			template<typename TOStream, typename TSet_Assignment, bool needStats_> class TMVEnumeration,
			typename TSet_Assignment = mylib::BitSet >		

			class ParallelEnumeration : public MasterWorkerBase {
			static const int TAG_JOB = 10;

			template<typename TFlapPattern>
			class ParallelPipe {
				MasterWorkerBase *comm;
			public:
				ParallelPipe(MasterWorkerBase *comm) : comm(comm) {}
	
				ParallelPipe& operator<<(const TFlapPattern& flap) {
					int workerID;
					//std::cout << "kawasaki found " << flap.toString() << std::endl;
					WorkerState state = comm->receiveWorkerStateFromAny(workerID);

					//std::cout << "receive from " << workerID << std::endl;

					if(state == WorkerState::IDLE) {
						comm->sendWorkerState(WorkerState::JOB_START, workerID);
						//std::cout << "send flap to " << workerID << std::endl;
						flap.MPISendAsSet(workerID, TAG_JOB);
					}
					return *this;
				}


			};

			const int placeCount;
			ParallelPipe<FlapPatternForBraceletEnum> pipe;
			IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory;

			mylib::EnumerationStats mvStats_;

			EncoderFunc encode;

		protected:
			virtual void masterTask() {
				//std::cout << "ID=" << myID << " run kawasaki enumeration" << std::endl;
				KawasakiFlapEnumeration<false> kawasaki;
				kawasaki.enumerate(placeCount, pipe);

				//std::cout << "kill workers." << std::endl;

				killWorkers();
			}

			virtual void workerTask() {
				//std::cout << "ID=" << myID << " task start" << std::endl;
				WorkerState state = WorkerState::IDLE;

				while (state != WorkerState::FINISH) {
					sendWorkerState(WorkerState::IDLE, masterID);

					state = receiveWorkerStateFrom(masterID);

					if (state != WorkerState::JOB_START) {
						continue;
					}

					FlapPatternForBraceletEnum flap;
					flap.MPIReceiveAsSet(masterID, TAG_JOB);

					CPEncoderStream<EncoderFunc> out(flap, encode);

					auto isAnswer = factory.create(flap);
					
					TMVEnumeration<CPEncoderStream<EncoderFunc>, TSet_Assignment, false> mvEnumeration;
					mvEnumeration.enumerate(flap, out, *isAnswer);

					// debug: turn on stats
					//TMVEnumeration<CPEncoderStream<EncoderFunc>, TSet_Assignment, true> mvEnumeration;
					//mvStats_ += mvEnumeration.enumerate(flap, out, *isAnswer);
					
					//std::cout << "ID=" << myID << "enumerated " 
					//	<< mvStats_.answerCount << " patterns" << std::endl;
					delete isAnswer;

				}
			}

		public:

			ParallelEnumeration(const int placeCount, IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory, EncoderFunc encode) :
				placeCount(placeCount), factory(factory), encode(encode), pipe(this) {

			}

			void enumerate() {
				run();
			}

			const mylib::EnumerationStats& mvStats() {
				return mvStats_;
			}

		};

		template<
			template <typename TOStream, typename TSet_Assignment, bool needStats_> class TMVEnumeration,
			typename TSet_Assignment = mylib::BitSet>
		class ParallelEnumerationRunner {
	
			IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory;
			mylib::EnumerationStats mvStats_;
		public:
			ParallelEnumerationRunner(IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory) : factory(factory) {}

			template<typename EncoderFunc>
			void enumerate(u_int placeCount, EncoderFunc& encode) {
				//std::cout << "enter enumerate() of runner" << std::endl;

				ParallelEnumeration<EncoderFunc, TMVEnumeration> parallel(placeCount, factory, encode);
				parallel.enumerate();
				mvStats_ += parallel.mvStats();
			}

			const mylib::EnumerationStats& mvStats() {
				return mvStats_;
			}

		};
		 
		template<typename TSet_Assignment = mylib::BitSet>
		class FoldableFlapCPParallelEnumeration : public ParallelEnumerationRunner<MVLSLEnumeration, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			FoldableFlapCPParallelEnumeration() : ParallelEnumerationRunner<MVLSLEnumeration, TSet_Assignment>(factory) {}
		};

		template<typename TSet_Assignment = mylib::BitSet>
		class ExLSLFoldableFlapCPParallelEnumeration : public ParallelEnumerationRunner<ExtendedMVLSLEnumeration, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			ExLSLFoldableFlapCPParallelEnumeration() : ParallelEnumerationRunner<ExtendedMVLSLEnumeration, TSet_Assignment>(factory) {}
		};

		template<typename TSet_Assignment = mylib::BitSet>
		class MaekawaFlapCPParallelEnumeration : public ParallelEnumerationRunner<MVEnumeration, TSet_Assignment> {
			MaekawaTheoremFactory<TSet_Assignment> factory;
		public:
			MaekawaFlapCPParallelEnumeration() : ParallelEnumerationRunner<MVEnumeration, TSet_Assignment>(factory) {}
		};
	}
}
