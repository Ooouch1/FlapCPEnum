#pragma once

#include <memory>
#include <map>

#include "KawasakiFlapEnumeration.hpp"
#include "MVEnumeration.hpp"
#include "FoldableMVEnumeration.hpp"
#include "ItemCountingStream.hpp"

#include "IsFoldable.hpp"
#include "StatsMaekawaTheorem.hpp"
#include "searchtool.hpp"
#include "CharVecFactory.hpp"

namespace enumeration {
	namespace origami {

		// for implementing enumeration.
		// user doesn't have to care this class.
		template<typename EncoderFunc>
		class CPEncoderStream {
			EncoderFunc& encode;
			const EncodablePatternBase& flap;
		public:
			CPEncoderStream(const EncodablePatternBase& flap, EncoderFunc& encode) :
				flap(flap), encode(encode) {
			}

			template<typename TSet_Assignment>
			CPEncoderStream& operator<<(const TSet_Assignment& mvPattern) {
				encode(flap, mvPattern);
				return *this;
			}
		};


		// for implementing enumeration.
		// user doesn't have to care this class.
		template<
			typename EncoderFunc,
			template<typename TOStream, typename TSet_Assignment, bool needStats_> class TMVEnumeration,
			typename TSet_Assignment = mylib::BitSet, bool needStats = true >
			class EnumerationPipe {
			EncoderFunc& encode;
			mylib::EnumerationStats totalStats_;

			unsigned long long int maekawaValidCount;

			IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory;

			public:
				EnumerationPipe(EncoderFunc& encode, IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory) : encode(encode), factory(factory) {
					maekawaValidCount = 0ULL;
				}

				EnumerationPipe& operator<<(const EncodablePatternBase& flap) {
					typedef CPEncoderStream<EncoderFunc> EncoderStream;
					EncoderStream out(flap, encode);

					TMVEnumeration<EncoderStream, TSet_Assignment, needStats> mv;

					//IsFoldable<TSet> isAnswer(flap);
					auto isAnswer = factory.create(flap);

					totalStats_ += mv.enumerate(flap, out, *isAnswer);

					maekawaValidCount += isAnswer->maekawaValidCount();

					delete isAnswer;

					return *this;
				}

				const mylib::EnumerationStats& totalStats() {
					return totalStats_;
				}

				// #answer/#necessaryConditionsHold
				long double sufficientRate() {
					return (long double)totalStats_.answerCount / maekawaValidCount;
				}
		};






		/**
		* TOStream << enumeration::Countable(#line, crease pattern string);
		*/
		template<typename TOStream>
		class CPStringEncoder {
			TOStream& outStream;

		public:
			CPStringEncoder(TOStream& os) : outStream(os) {

			}

			void operator()(const EncodablePatternBase& flap) {
				outStream << createCountable(flap.count() / 2 - 1, flap.encode());
			}

			CPStringEncoder& operator<<(const EncodablePatternBase& flap) {
				(*this)(flap);
				return *this;
			}

			template <typename TBitSet>
			void operator()(const EncodablePatternBase& flap, const TBitSet& minors) {
				outStream << createCountable(flap.count() / 2 - 1, flap.encode(minors));
			}
		};

		template<typename TOStream>
		class CountOnlyEncoder {
			TOStream& outStream;
		public:
			CountOnlyEncoder(TOStream& os) : outStream(os) {

			}

			void operator()(const EncodablePatternBase& flap) {
				outStream << createCountable(flap.count() / 2 - 1, NULL);
			}

			CountOnlyEncoder& operator<<(const EncodablePatternBase& flap) {
				(*this)(flap);
				return *this;
			}

			template <typename TBitSet>
			void operator()(const EncodablePatternBase& flap, const TBitSet& minors) {
				outStream << createCountable(flap.count() / 2 - 1, NULL);
			}
		};

		/**
		 * KawasakiFlapEnumeration outputs FlapPattern
		 * => MVEnumeration<TOStream> outputs MVPattern
		 * => Call EncoderFunc(FlapPattern& flap, ISet<u_int>& minorLines).
		 *
		 * encoder template:
		 * shrinkedIndex = 0;
		 * for i do
		 * if flap.contains(i)
		 *     if minorLines.contains(shrinkedIndex) then i is a minor crease.
		 *     else i is a major crease.
		 *     shrinkedIndex++;
		 * else i is an empty place.
		 */
		template<
			template <typename TOStream, typename TSet_Assignment, bool needStats_> class TMVEnumeration,
			bool needStats = true, typename TSet_Assignment = mylib::BitSet>

		class FlapCPEnumeration {
			mylib::EnumerationStats kawasakiStats_;
			mylib::EnumerationStats mvStats_;
			long double sufficientRate_;

			IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory;

		public:
			FlapCPEnumeration(IFlapCPAnswerDetecterFactory<TSet_Assignment>& factory) : factory(factory) {}

			const mylib::EnumerationStats& kawasakiStats() {
				return kawasakiStats_;
			}

			const mylib::EnumerationStats& mvStats() {
				return mvStats_;
			}

			mylib::EnumerationStats totalStats() {

				return kawasakiStats_ + mvStats_;
			}

			const long double& sufficientRate() {
				return sufficientRate_;
			}

			/**
				* enumerates and passes every result to encode(flap, minors).
				*/
			template<typename EncoderFunc>
			void enumerate(u_int placeCount, EncoderFunc& encode) {
				EnumerationPipe<EncoderFunc, TMVEnumeration, TSet_Assignment> pipe(encode, factory);

				KawasakiFlapEnumeration<needStats> kawasaki;
				kawasakiStats_ = kawasaki.enumerate(placeCount, pipe);
	
				mvStats_ = pipe.totalStats();
				sufficientRate_ = pipe.sufficientRate();
			}

			/**
				* enumerates and passes every CP string encoded by CPStringEncoder to os.operator<<().
				*/
			template <typename TOStream>
			void enumerateCPString(u_int placeCount, TOStream& os) {
				CPStringEncoder<TOStream> encoder(os);
				enumerate(placeCount, encoder);
			}

		};





		//===========================================================================================
		// FACADE

		template<typename TSet>
		class EverythingIsAnswer : public AbstractFlapCPAnswerDetecter<TSet> {

		public:
			EverythingIsAnswer() : AbstractFlapCPAnswerDetecter<TSet>() {
			}
			virtual ~EverythingIsAnswer() {}


			virtual bool isAnswer(const TSet& assignments) {
				return true;
			}

		};


		template<typename TSet_Assignment>
		class DummyDetecterFactory : public IFlapCPAnswerDetecterFactory<TSet_Assignment> {
		public:
			virtual ~DummyDetecterFactory() {}
			virtual AbstractFlapCPAnswerDetecter<TSet_Assignment>* create(const EncodablePatternBase& flap) const {
				return new EverythingIsAnswer<TSet_Assignment>();
			}
		};


		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class FoldableFlapCPEnumeration : public FlapCPEnumeration<MVEnumeration, needStats, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			FoldableFlapCPEnumeration() : FlapCPEnumeration<MVEnumeration, needStats, TSet_Assignment>(factory) {}
		};

		// not fast due to bad impl of LinearMV
		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class LinearMVFoldableFlapCPEnumeration : public FlapCPEnumeration<LinearMVEnumeration, needStats, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			LinearMVFoldableFlapCPEnumeration() : FlapCPEnumeration<LinearMVEnumeration, needStats, TSet_Assignment>(factory) {}
		};

		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class MVLSLFoldableFlapCPEnumeration : public FlapCPEnumeration<MVLSLEnumeration, needStats, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			MVLSLFoldableFlapCPEnumeration() : FlapCPEnumeration<MVLSLEnumeration, needStats, TSet_Assignment>(factory) {}
		};

		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class ExMVLSLFoldableFlapCPEnumeration : public FlapCPEnumeration<ExtendedMVLSLEnumeration, needStats, TSet_Assignment> {
			FoldabilityDetecterFactory<TSet_Assignment> factory;
		public:
			ExMVLSLFoldableFlapCPEnumeration() : FlapCPEnumeration<ExtendedMVLSLEnumeration, needStats, TSet_Assignment>(factory) {}
		};


		// just an idea
		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class FoldableFlapCPCrimpBasedEnumeration : public FlapCPEnumeration<FoldableMVEnumeration, needStats, TSet_Assignment> {
			DummyDetecterFactory<TSet_Assignment> factory;
		public:
			FoldableFlapCPCrimpBasedEnumeration() : FlapCPEnumeration<FoldableMVEnumeration, needStats, TSet_Assignment>(factory) {}
		};

		template<bool needStats = true, typename TSet_Assignment = mylib::BitSet>
		class MaekawaFlapCPEnumeration : public FlapCPEnumeration<MVEnumeration, needStats, TSet_Assignment> {
			MaekawaTheoremFactory<TSet_Assignment> factory;
		public:
			MaekawaFlapCPEnumeration() : FlapCPEnumeration<MVEnumeration, needStats, TSet_Assignment>(factory) {}
		};
	}
}
