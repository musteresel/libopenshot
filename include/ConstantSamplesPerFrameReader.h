/**
 * @file
 * @brief Header file for ConstantSamplesPerFrameReader class
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

#ifndef OPENSHOT_CONSTANT_SAMPLES_PER_FRAME_READER_H
#define OPENSHOT_CONSTANT_SAMPLES_PER_FRAME_READER_H

#include <cstdint>
#include <memory>

#include "Frame.h"
#include "Json.h"
#include "ReaderBase.h"

namespace openshot
{
	class ConstantSamplesPerFrameReader : public ReaderBase
	{
	private:
		ReaderBase * wrappedReader;
		std::vector<long> samplesBeforeFrame;
		int64_t samplesCalculatedUpToFrame;
	public:
		CacheBase* GetCache() override {
			return nullptr;
		}

		bool IsOpen() override {
			return wrappedReader->IsOpen();
		}
		void Open() override {
			wrappedReader->Open();
			samplesBeforeFrame.clear();
			samplesBeforeFrame.resize(1);
			samplesBeforeFrame.at(0) = 0;
			samplesCalculatedUpToFrame = 0;
		}
		void Close() override {
			wrappedReader->Close();
			samplesBeforeFrame.clear();
			samplesCalculatedUpToFrame = 0;
		}

		std::shared_ptr<Frame> GetFrame(int64_t const number) override;

		std::string Name() override {
			return "ConstantSamplesPerFrameReader";
		}
		std::string Json() const override {
			return JsonValue().toStyledString();
		}
		void SetJson(std::string value) override {
			try
			{
				const Json::Value root = stringToJson(value);
				SetJsonValue(root);
			}
			catch (const std::exception& e)
			{
				throw InvalidJSON("JSON is invalid (missing keys or invalid data types)");
			}
		}
		Json::Value JsonValue() const override
		{
			// todo
		}
		void SetJsonValue(Json::Value root) override
		{
			// todo
		}
	};

}

#endif
