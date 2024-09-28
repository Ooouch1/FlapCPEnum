#pragma once

#include <mpi.h>


namespace mylib {
	class MasterWorkerBase {
	protected:
		const int masterID = 0;
		int myID;
		int processCount;
		const int minID = 0;

		enum WorkerState { IDLE, JOB_START, JOB_DONE, FINISH };
		const int TAG_WORKER_STATE = 1;

		virtual void masterTask() = 0;

		virtual void workerTask() = 0;

	public:
		void killWorkers() {

			for (int id = minID; id < minID + processCount; id++) {
				if (id == masterID) {
					continue;
				}
				sendWorkerState(WorkerState::FINISH, id);
			}

		}

		// MP library specific: send worker's state
		void sendWorkerState(const WorkerState& state, int receiverID) {
			MPI_Send(&state, 1, MPI_INT, receiverID, TAG_WORKER_STATE, MPI_COMM_WORLD);
		}

		WorkerState receiveWorkerStateFrom(int senderID) {
			WorkerState state;
			MPI_Status status;

			MPI_Recv(&state, 1, MPI_INT, senderID, TAG_WORKER_STATE, MPI_COMM_WORLD, &status);

			return state;
		}

		// MP library specific: receive worker's state
		// senderID: output value. the ID of message sender.
		WorkerState receiveWorkerStateFromAny(int& senderID) {
			WorkerState state;
			MPI_Status status;

			MPI_Recv(&state, 1, MPI_INT, MPI_ANY_SOURCE, TAG_WORKER_STATE, MPI_COMM_WORLD, &status);

			senderID = status.MPI_SOURCE;

			return state;
		}

		void run() {
			MPI_Comm_rank(MPI_COMM_WORLD, &myID);
			MPI_Comm_size(MPI_COMM_WORLD, &processCount);

			if (myID == masterID) {
				masterTask();
			}
			else {
				workerTask();
			}
		}
	};

	// abstract master-worker model in message passing archetecture.
	// JobCompletionFunc : should be a function unsigned long long int->bool. the parameter is count of finished jobs. It should return true if all job has completed.

	class AbstractMasterWorkerModel : public MasterWorkerBase {

	protected:
		unsigned long long int jobCount;

		template <typename JobCompletionFunc>
		void masterTask() {
			unsigned long long int finishedCount = 0;

			while (finishedCount < jobCount) {
				int workerID;
				WorkerState workerState = receiveWorkerStateFromAny(&workerID);

				switch (workerState) {
				case WorkerState::IDLE:

					sendWorkerState(WorkerState::JOB_START, workerID);
					master_sendJob(workerID);

					break;

				case WorkerState::JOB_DONE:

					master_receiveResultFrom(workerID);

					finishedCount++;
					break;
				}
			}

			killWorkers();
		}

		void workerTask() {
			WorkerState state = WorkerState::IDLE;

			while (state != WorkerState::FINISH) {
				sendWorkerState(WorkerState::IDLE, masterID);

				int senderID;
				state = receiveWorkerStateFromAny(senderID);

				if (state != WorkerState::JOB_START) {
					continue;
				}

				worker_job();

				sendWorkerState(WorkerState::JOB_DONE, masterID);

				worker_sendResult();
			}

		}
		const int TAG_RESULT = 2;

		// application specific: prepare for parallelization
		virtual void master_setup() = 0;
		// application specific: send job to workerID process
		virtual void master_sendJob(const int workerID) = 0;
		// application specific: receive result from workerID process
		virtual void master_receiveResultFrom(const int workerID) = 0;

		// application specific: receive job for myID process
		virtual void worker_job() = 0;
		// application specific: send result to masterID process
		virtual void worker_sendResult() = 0;

	public:

		AbstractMasterWorkerModel(unsigned long long int jobCount): jobCount(jobCount) {

		}

		//// MP library specific: init
		//void initialize(int argc, char* argv[]) {
		//	MPI_Init(&argc, &argv);
		//}
		//// MP library specific: finish
		//virtual void finalize() {
		//	MPI_Finalize();
		//}


	};
}