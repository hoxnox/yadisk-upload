/*
Copyright 2017 hoxnox <hoxnox@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <string>
#include <thread>
#include <memory>
#include <atomic>

class BaseEcho
{
public:
	BaseEcho() { }
	virtual ~BaseEcho() { Stop(); if (thr_) thr_->join(); };

	void Start(std::string addr, unsigned short port) { loop(addr, port); }
	void Dispatch(std::string addr, unsigned short port);
	void Stop() { stop_ = true; }
	bool IsStarted() const { return !stop_; }

	void SetMultiplier(size_t multiplier) { multiplier_ = multiplier; }

protected:
	virtual void loop(std::string addr, unsigned short port) = 0;
	std::atomic<bool> stop_{false};
	size_t multiplier_{1};

private:
	std::unique_ptr<std::thread> thr_{nullptr};
};

inline void
BaseEcho::Dispatch(std::string addr, unsigned short port)
{
	thr_.reset(new std::thread(
		[this, addr, port]()
		{
			loop(addr, port);
		}));
}

