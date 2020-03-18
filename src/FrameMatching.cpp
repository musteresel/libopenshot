/**
 * @file
 * @brief Source file for Frame / samples matching code
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

#include "../include/FrameMatching.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

namespace
{
	long
	TimeToSampleNumber(double const time,
					   double const sample_rate)
	{
		return floor(time * sample_rate);
	}


	int
	TimeToFrameNumber(double const time,
					  double const fps)
	{
		return floor(time * fps);
	}


	double
	FrameNumberToStartTime(int const frame,
						   double const fps)
	{
		return frame / fps;
	}


	double
	SampleNumberToStartTime(long const sample,
							double const sample_rate)
	{
		return sample / sample_rate;
	}


	int
	SampleNumberToFrameNumber(long const sample,
							  double const fps,
							  double const sample_rate,
							  bool const prefer_lower = true)
	{
		double const start_time = SampleNumberToStartTime(sample, sample_rate);
		double const start_time_next = SampleNumberToStartTime(sample + 1, sample_rate);
		double const end_time = std::nexttoward(start_time_next, std::numeric_limits<double>::lowest());
		int const start_frame = TimeToFrameNumber(start_time, fps);
		int const end_frame = TimeToFrameNumber(end_time, fps);
		assert(start_frame <= end_frame);
		if (start_frame != end_frame) {
			return prefer_lower ? start_frame : end_frame;
		} else {
			return start_frame;
		}
	}
}


namespace openshot
{
	namespace internal
	{
		long
		SamplesPerFrame(double const fps,
						double const sample_rate)
		{
			return ceil(sample_rate / fps);
		}


		long
		FrameNumberToSampleNumber(int const frame,
								  double const fps,
								  double const sample_rate)
		{
			double const start_time = FrameNumberToStartTime(frame, fps);
			return TimeToSampleNumber(start_time, sample_rate);
		}


		MatchResult
		MatchFrames(int const tl_frame_n,
					TimelineConfig const timeline,
					ClipConfig const clip)
		{
			// First calculate time and sample offset for the given clip.  Using
			// the sample offset it is possible to translate a timeline sample
			// number to a clip sample number (and vice versa).
			double const time_offset = clip.position - clip.start;
			long const sample_offset = floor(time_offset * timeline.sample_rate);
			// Obtain start and end timeline sample numbers and translate them
			// to clip sample numbers.
			long const tl_sample_n = FrameNumberToSampleNumber(tl_frame_n, timeline.fps, timeline.sample_rate);
			long const tl_sample_end_n = tl_sample_n + SamplesPerFrame(timeline.fps, timeline.sample_rate) - 1;
			long const cl_sample_n = tl_sample_n - sample_offset;
			long const cl_sample_end_n = tl_sample_end_n - sample_offset;
			// Using clip start and end time, obtain first and last "allowed"
			// samples.
			long const first_allowed_sample = TimeToSampleNumber(clip.start, timeline.sample_rate);
			long const last_allowed_sample = TimeToSampleNumber(clip.end, timeline.sample_rate);
			// Determine first used frame and last used frame.  These take into
			// account both the "needed" sample numbers as well as the allowed
			// sample numbers.
			int const cl_frame_start_n = SampleNumberToFrameNumber(std::max(cl_sample_n, first_allowed_sample), clip.fps, timeline.sample_rate, false);
			int const cl_frame_end_n = SampleNumberToFrameNumber(std::min(cl_sample_end_n, last_allowed_sample), clip.fps, timeline.sample_rate, true);
			MatchResult r;
			r.clip_frame_start = cl_frame_start_n;
			r.clip_frame_end = cl_frame_end_n;
			// Given the first and last used frame: Determine the first and last
			// sample which is going to be requested.
			long const cl_first_sample_n = FrameNumberToSampleNumber(cl_frame_start_n, clip.fps, timeline.sample_rate);
			long const cl_last_sample_n =
				FrameNumberToSampleNumber(cl_frame_end_n, clip.fps, timeline.sample_rate)
				+ SamplesPerFrame(clip.fps, timeline.sample_rate)
				- 1;
			// There are two cases when samples need to be dropped at the start:
			// One when the start (cl_first_sample_n) is before the first
			// allowed sample and two if the start is before the first needed
			// sample (cl_sample_n).
			r.samples_to_drop_start = std::max(first_allowed_sample - cl_first_sample_n,
											   cl_sample_n - cl_first_sample_n);
			// There are two cases when samples need to be dropped at the end:
			// One when the end (cl_last_sample_n) is after the last allowed
			// sample and two if the end is after the last needed sample
			// (cl_sample_end_n).
			r.samples_to_drop_end = std::max(cl_last_sample_n - last_allowed_sample,
											 cl_last_sample_n - cl_sample_end_n);
			// There's just one case where samples need to be offset (in the
			// timeline frame, that is the first samples of the timeline frame
			// won't get filled with samples from the clip):
			//
			// The first needed sample is before the first allowed sample.
			r.sample_offset = std::max(0l, first_allowed_sample - cl_sample_n);
			// Translate timeline time to clip time in oder to get the clip
			// frame number which should be displayed.  Only use the image when
			// the time (in clip time) is after the start but before the end.
			double const time_in_clip = FrameNumberToStartTime(tl_frame_n, timeline.fps) - time_offset;
			int const displayed_frame = TimeToFrameNumber(time_in_clip, clip.fps);
			r.use_image = (time_in_clip >= clip.start) && (time_in_clip <= clip.end);
			r.displayed_frame = displayed_frame;
			return r;
		}
	}
}
