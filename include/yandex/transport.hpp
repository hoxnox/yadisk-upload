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
#include <functional>
#include <vector>
#include <memory>
#include <iostream>

namespace yandex {

/**@brief API transport interface
 *
 * @warning Realisation MUST be synchronous. When some operation is
 * called (get or put) the caller must be locked until server response
 * will arrive. It can be canceled by `cancel()` from alternate
 * thread. When op is in the progress, another thread can't call any op.
 * Attempt to do it MUST return INPROGRESS.*/
class transport
{
public:
	class result_t
	{
	public:
		enum types: uint8_t
		{
			SUCCESS    = 0,
			FAILED     = 10,
			CANCELED   = 20,
			INPROGRESS = 30,
		};
		result_t(types tp, uint16_t code = 0, std::string msg = "")
			: type_(tp), code_(code), msg_(msg) {}
		operator bool() const { return type_ == 0; }
		uint8_t  type() const { return type_; }
		uint16_t code() const { return code_; }
		std::string message() const { return msg_; }
	private:
		types       type_;
		uint16_t    code_{0};
		std::string msg_;
	friend class transport;
	};

	/**@brief called by operations when data arrives, can be called
	 * several times during one operation. */
	using response_handler_t = std::function<void(const std::string& url,
	                                              const uint8_t* data,
	                                              const size_t datasz)>;

	transport(std::string token = "")
		: token_(token) {}

	/**@brief perform HTTP GET request*/
	virtual result_t get(std::string url, response_handler_t handler = nullptr) = 0;

	/**@brief perform HTTP PUT request
	 * @param bodysz if set to 0, send until body.good()*/
	virtual result_t put(std::string url,
	                     std::basic_istream<char>& body,
	                     size_t bodysz = 0,
	                     response_handler_t handler = nullptr) = 0;

	/**@brief Can be used from another thread to cancel current operation.*/
	virtual void cancel(uint16_t code = 0, std::string message = "") = 0;

	virtual ~transport() {}

	/**@brief Create new transport with the same type.
	 * @param chunksz if not zero, prepare chunked transport*/
	virtual std::shared_ptr<transport>
		make_transport(std::string host, uint16_t port, size_t chunksz = 0) = 0;

protected:
	std::string token_;
};

} // namespace

std::ostream& operator<<(std::ostream& os, const yandex::transport::result_t& rs);

