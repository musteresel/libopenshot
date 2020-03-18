/**
 * @file
 * @brief Source file for ConstantSamplesPerFrameReader class
 * @author Daniel Jour <musteresel@gmail.com>
 *
 * @ref License
 */

/* LICENSE
 *
 * Copyright (c) 2008-2019 OpenShot Studios, LLC
 * <http://www.openshotstudios.com/>. This file is part of
 * OpenShot Library (libopenshot), an open-source project dedicated to
 * delivering high quality video editing and animation solutions to the
 * world. For more information visit <http://www.openshot.org/>.
 *
 * OpenShot Library (libopenshot) is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * OpenShot Library (libopenshot) is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenShot Library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/ConstantSamplesPerFrameReader.h"
#include "../include/FrameMatching.h"

namespace openshot
{

	using namespace internal;

	std::shared_ptr<Frame>
	ConstantSamplesPerFrameReader::GetFrame(int64_t const number) {
		double const fps = wrappedReader->info.fps.ToDouble();
		int const sample_rate = wrappedReader->info.sample_rate;
		long const samples_per_frame = SamplesPerFrame(fps, sample_rate);
		if (number > samplesCalculatedUpToFrame)
		{
			samplesBeforeFrame.reserve(number + 1);
			while (number > samplesCalculatedUpToFrame)
			{
				++samplesCalculatedUpToFrame;
				// NOTE: Using a cache here does not make sense,
				// because if a frame has already been requested, then
				// the samplesCalculatedUpToFrame would be higher than
				// that frames index.
				std::shared_ptr<Frame> new_frame = wrappedReader->GetFrame(samplesCalculatedUpToFrame);
				samplesBeforeFrame[samplesCalculatedUpToFrame] = samplesBeforeFrame[samplesCalculatedUpToFrame - 1] + new_frame->GetAudioSamplesCount();
				// TODO: Insert just requested frame into cache.
			}
		}
		// Get this frame, and some statistics about it.
		std::shared_ptr<Frame> this_frame = wrappedReader->GetFrame(number);
		long available_samples = this_frame->GetAudioSamplesCount();
		// Perpare frame to return
		std::shared_ptr<Frame> frame_to_return = this_frame;
		bool frame_modified = false;
		// Calculate how many samples *should* be before this frame
		// Compare with how many samples *are* before this frame
		long const required_samples_before = FrameNumberToSampleNumber(number, fps, sample_rate);
		long const actual_samples_before = samplesBeforeFrame[number];
		// Make sure number of samples before this frame matches
		if (actual_samples_before < required_samples_before)
		{
			// DROP AT BEGIN
			long const samples_to_drop = required_samples_before - actual_samples_before;
			// TODO: copy
			frame_to_return = std::make_shared<Frame>();
			frame_to_return->DeepCopy(*this_frame);
			// TODO: drop
			frame_modified = true;
			available_samples -= samples_to_drop;
		}
		else if (actual_samples_before > required_samples_before)
		{
			// GET PREVIOUS FRAME
			assert(number > 0);
			std::shared_ptr<Frame> previous_frame = wrappedReader->GetFrame(number - 1);
			// TODO: copy
			// TODO: merge
			frame_modified = true;
			available_samples += previous_frame->GetAudioSamplesCount();
		}
		// Make sure number of samples is correct!
		if (available_samples > samples_per_frame)
		{
			// DROP AT END
			long const samples_to_drop = available_samples - samples_per_frame;
			if (! frame_modified)
			{
				// TODO: copy
			}
			// TODO: drop
		}
		else if (available_samples < samples_per_frame)
		{
			// GET NEXT FRAME
			std::shared_ptr<Frame> next_frame = wrappedReader->GetFrame(number + 1);
			if (! frame_modified)
			{
				// TODO: copy
			}
			// TODO: merge AND drop at end!
		}
		return frame_to_return;
	}
}
