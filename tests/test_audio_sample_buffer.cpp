#include <SDL3/SDL.h>

#include <AssetManagement/AudioSampleBuffer.hpp>
#include <SystemManagement/IAudioSink.hpp>
#include <SystemManagement/IClock.hpp>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace YerbEngine;

namespace {

/** Records what the buffer asked to play, in the order it asked. */
struct RecordingSink : IAudioSink {
   std::vector<AudioSampleId> played;
   std::vector<int> loops;

   void playSample(std::string_view const sample,
                   int const loopCount) override {
      played.emplace_back(sample);
      loops.push_back(loopCount);
   }
};

/** A clock the test moves by hand. */
struct FakeClock : IClock {
   Uint64 now = 0;

   Uint64 nowMs() const override { return now; }
   void advance(Uint64 const ms) { now += ms; }
};

/*
 * The buffer's limits are private, so they are restated here. A test that
 * disagrees with the implementation is the point: these numbers are the
 * contract callers tune their audio around.
 */
constexpr size_t BUFFER_CAPACITY = 64;
constexpr size_t TRIM_LIMIT = 20;
constexpr Uint64 STALE_SAMPLE_MS = 500;
constexpr Uint64 MIN_REPLAY_INTERVAL = 50;

bool wasPlayed(RecordingSink const& sink, AudioSampleId const& sample) {
   return std::ranges::find(sink.played, sample) != sink.played.end();
}

}  // namespace

struct AudioSampleBufferTestFixture {
   RecordingSink sink;
   FakeClock clock;
   AudioSampleBuffer buffer{sink, clock};

   /** Runs updates until the buffer stops starting samples. */
   void drain() {
      for (size_t frame = 0; frame < BUFFER_CAPACITY; ++frame) {
         size_t const before = sink.played.size();
         buffer.update();
         if (sink.played.size() == before) {
            return;
         }
      }
   }
};

BOOST_FIXTURE_TEST_SUITE(AudioSampleBufferTests, AudioSampleBufferTestFixture)

BOOST_AUTO_TEST_CASE(empty_buffer_starts_nothing) {
   buffer.update();

   BOOST_CHECK(sink.played.empty());
}

BOOST_AUTO_TEST_CASE(queued_sample_reaches_the_sink) {
   buffer.queueSample("menu_select", PriorityLevel::STANDARD);
   buffer.update();

   BOOST_REQUIRE_EQUAL(sink.played.size(), 1u);
   BOOST_CHECK_EQUAL(sink.played.front(), "menu_select");
   BOOST_CHECK_EQUAL(sink.loops.front(), 0);
}

BOOST_AUTO_TEST_CASE(plays_highest_priority_first) {
   buffer.queueSample("bg", PriorityLevel::BACKGROUND);
   buffer.queueSample("std", PriorityLevel::STANDARD);
   buffer.queueSample("imp", PriorityLevel::IMPORTANT);
   buffer.queueSample("crit", PriorityLevel::CRITICAL);

   buffer.update();

   std::vector<AudioSampleId> const expected{"crit", "imp", "std", "bg"};
   BOOST_TEST(sink.played == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(equal_priority_plays_in_arrival_order) {
   buffer.queueSample("first", PriorityLevel::STANDARD);
   buffer.queueSample("second", PriorityLevel::STANDARD);
   buffer.queueSample("third", PriorityLevel::STANDARD);

   buffer.update();

   std::vector<AudioSampleId> const expected{"first", "second", "third"};
   BOOST_TEST(sink.played == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(caps_samples_started_per_frame) {
   for (size_t i = 0; i < IAudioSink::MAX_SAMPLES_PER_FRAME + 2; ++i) {
      buffer.queueSample("sample" + std::to_string(i), PriorityLevel::STANDARD);
   }

   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), IAudioSink::MAX_SAMPLES_PER_FRAME);

   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), IAudioSink::MAX_SAMPLES_PER_FRAME + 2);
}

BOOST_AUTO_TEST_CASE(replay_is_rate_limited) {
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();

   clock.advance(MIN_REPLAY_INTERVAL - 1);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 1u);

   clock.advance(1);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 2u);
}

BOOST_AUTO_TEST_CASE(per_sample_cooldown_replaces_the_default) {
   constexpr Uint64 COOLDOWN = MIN_REPLAY_INTERVAL * 4;
   buffer.setCooldown("beep", COOLDOWN);

   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();

   // Past the default interval, still inside this sample's own cooldown.
   clock.advance(MIN_REPLAY_INTERVAL);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 1u);

   clock.advance(COOLDOWN - MIN_REPLAY_INTERVAL);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 2u);
}

BOOST_AUTO_TEST_CASE(cooldowns_can_be_set_in_bulk) {
   constexpr Uint64 COOLDOWN = MIN_REPLAY_INTERVAL * 4;
   buffer.setCooldowns({{"beep", COOLDOWN}});

   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();

   clock.advance(MIN_REPLAY_INTERVAL);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 1u);

   clock.advance(COOLDOWN - MIN_REPLAY_INTERVAL);
   buffer.queueSample("beep", PriorityLevel::STANDARD);
   buffer.update();
   BOOST_CHECK_EQUAL(sink.played.size(), 2u);
}

BOOST_AUTO_TEST_CASE(higher_priority_sample_evicts_a_queued_background_sample) {
   for (size_t i = 0; i < BUFFER_CAPACITY - 1; ++i) {
      buffer.queueSample("filler", PriorityLevel::BACKGROUND);
   }
   buffer.queueSample("important", PriorityLevel::IMPORTANT);

   buffer.queueSample("critical", PriorityLevel::CRITICAL);
   buffer.update();

   BOOST_REQUIRE_GE(sink.played.size(), 2u);
   BOOST_CHECK_EQUAL(sink.played[0], "critical");
   BOOST_CHECK_EQUAL(sink.played[1], "important");
}

BOOST_AUTO_TEST_CASE(full_buffer_drops_a_sample_it_cannot_outrank) {
   for (size_t i = 0; i < BUFFER_CAPACITY; ++i) {
      buffer.queueSample("filler", PriorityLevel::IMPORTANT);
   }

   buffer.queueSample("late", PriorityLevel::BACKGROUND);
   drain();

   BOOST_CHECK_EQUAL(sink.played.size(), BUFFER_CAPACITY);
   BOOST_CHECK(!wasPlayed(sink, "late"));
}

BOOST_AUTO_TEST_CASE(trims_background_samples_when_the_queue_backs_up) {
   for (size_t i = 0; i < TRIM_LIMIT + 5; ++i) {
      buffer.queueSample("bg" + std::to_string(i), PriorityLevel::BACKGROUND);
   }

   buffer.update();
   BOOST_REQUIRE_EQUAL(sink.played.size(), IAudioSink::MAX_SAMPLES_PER_FRAME);

   drain();
   BOOST_CHECK_EQUAL(sink.played.size(), IAudioSink::MAX_SAMPLES_PER_FRAME);
}

BOOST_AUTO_TEST_CASE(keeps_standard_samples_when_the_queue_backs_up) {
   for (size_t i = 0; i < TRIM_LIMIT + 5; ++i) {
      buffer.queueSample("std" + std::to_string(i), PriorityLevel::STANDARD);
   }

   drain();

   BOOST_CHECK_EQUAL(sink.played.size(), TRIM_LIMIT + 5);
}

BOOST_AUTO_TEST_CASE(stale_samples_are_dropped_before_playing) {
   buffer.queueSample("beep", PriorityLevel::STANDARD);

   clock.advance(STALE_SAMPLE_MS + 1);
   buffer.update();

   BOOST_CHECK(sink.played.empty());
}

BOOST_AUTO_TEST_CASE(samples_at_the_staleness_limit_still_play) {
   buffer.queueSample("beep", PriorityLevel::STANDARD);

   clock.advance(STALE_SAMPLE_MS);
   buffer.update();

   BOOST_CHECK_EQUAL(sink.played.size(), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
