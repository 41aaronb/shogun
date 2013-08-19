#include "machine/MockMachine.h"
#include "features/MockFeatures.h"
#include "labels/MockLabels.h"
#include <shogun/machine/BaggingMachine.h>
#include <shogun/ensemble/MajorityVote.h>
#include <gtest/gtest.h>

using namespace shogun;
using ::testing::Return;

TEST(BaggingMachine, mock_train)
{
	using ::testing::NiceMock;
	using ::testing::_;
	using ::testing::InSequence;
	using ::testing::Mock;
	using ::testing::DefaultValue;

	int32_t bag_size = 20;
	int32_t num_bags = 10;

	NiceMock<MockCFeatures> features; features.ref();
	NiceMock<MockCLabels> labels; labels.ref();
	CBaggingMachine* bm = new CBaggingMachine(&features, &labels);
	NiceMock<MockCMachine> mm; mm.ref();
	CMajorityVote* mv = new CMajorityVote();

	bm->set_machine(&mm);
	bm->set_bag_size(bag_size);
	bm->set_num_bags(num_bags);
	bm->set_combination_rule(mv);

	ON_CALL(mm, train_machine(_))
		.WillByDefault(Return(true));

	{
		InSequence s;
		for (int i = 0; i < num_bags; i++) {
			EXPECT_CALL(mm, clone())
				.Times(1)
				.WillRepeatedly(Return(&mm));

			EXPECT_CALL(features, get_num_vectors())
				.Times(1)
				.WillRepeatedly(Return(100));
		}
	}

	bm->train();

	SG_UNREF(bm);
}
