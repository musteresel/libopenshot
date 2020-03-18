/**
 * @file
 * @brief Header file for Frame / samples matching code
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

#ifndef OPENSHOT_FRAME_MATCHING_H
#define OPENSHOT_FRAME_MATCHING_H


namespace openshot
{
	namespace internal
	{
		struct TimelineConfig
		{
			double fps;
			double sample_rate;
		};


		struct ClipConfig
		{
			double fps;
			double start;
			double end;
			double position;
		};


		struct MatchResult
		{
			int clip_frame_start;
			int clip_frame_end;
			long samples_to_drop_start;
			long samples_to_drop_end;
			bool use_image;
			long sample_offset;
			int displayed_frame;
		};


		MatchResult
		MatchFrames(int const tl_frame_n,
					TimelineConfig const timeline,
					ClipConfig const clip);

		long
		FrameNumberToSampleNumber(int const frame,
								  double const fps,
								  double const sample_rate);

		long
		SamplesPerFrame(double const fps,
						double const sample_rate);
	}
}

#endif
