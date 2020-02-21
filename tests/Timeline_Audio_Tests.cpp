/**
 * @file
 * @brief Unit tests for audio handling of openshot::Timeline
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

#include "UnitTest++.h"
// Prevent name clashes with juce::UnitTest
#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "../include/OpenShot.h"

using namespace openshot;


struct SilenceAudioReader : public ReaderBase
{
	void Open() override {};
	void Close() override {};
	CacheBase * GetCache() override { return nullptr; }
	bool IsOpen() override { return true; }
	std::string Json() override { return ""; }
	Json::Value JsonValue() override { return {}; }
	std::string Name() override { return "SilenceAudioReader"; }
	void SetJson(std::string) override {}
	void SetJsonValue(Json::Value) override {}
	std::shared_ptr<Frame> GetFrame(int64_t) override { return nullptr; }
};

TEST(Timeline_Audio_Empty)
{
	Timeline t(640, 480, Fraction(24, 1), 44100, 2, LAYOUT_STEREO);
	SilenceAudioReader ar;
	Clip clip(&ar);
	t.AddClip(&clip);
	t.Open();
	t.GetFrame(0);
	t.Close();
}
