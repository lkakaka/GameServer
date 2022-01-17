
#include "ConnectionBase.h"
#include "Logger.h"
#include "../AsioService.h"

USE_NS_GAME_NET

ConnectionBase::ConnectionBase(std::shared_ptr<tcp::socket> socket, bool isConnected):
	m_socket(socket), m_isSending(false), m_isConnected(isConnected), m_sendThreadId(0)
{

}

void ConnectionBase::send(std::vector<unsigned char>&& dat) {
	boost::asio::post(MAIN_IO, [this, dat = std::move(dat)]() {
		{
			// 在同一线程执行，不需要锁
			//std::unique_lock<std::mutex> lock(m_sendLock);
			m_sendBuf.emplace_back(std::move(dat));
		}
		_trySend();
	});
}

void ConnectionBase::_trySend() {
	/*if (m_sendBuf.size() == 0) return;
	boost::asio::const_buffer buf(&m_sendBuf.front(), m_sendBuf.size());
	size_t len = m_socket->write_some(buf);
	if (len > 0) {
		m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + len);
		_send();
	}*/

	//DWORD tid = GetCurrentThreadId();//std::this_thread::get_id();
	//if (m_sendThreadId == 0) m_sendThreadId = tid;
	//if (tid != m_sendThreadId) {
	//	LOG_ERROR("-----------send data in other thread!!");
	//}

	if (!m_isConnected) return;
	if (m_isSending) return;

	if (!_prepareSendBuf()) return;

	m_isSending = true;
	m_socket->async_write_some(m_sendingCBuffer, [this](const boost::system::error_code err_code, size_t bytes_transferred) {
		m_isSending = false;
		if (err_code)
		{
			const std::string err_str = err_code.message();
			LOG_ERROR("send data error, %s", err_str.data());
			return;
		}

		//LOG_INFO("send data len: %d", bytes_transferred);
		_afterSend(bytes_transferred);
		_trySend();
	});
}

bool ConnectionBase::_prepareSendBuf() {
	if (m_sendBuf.size() > 0) {
		//std::unique_lock<std::mutex> lock(m_sendLock);
		this->m_sendingBuf.splice(this->m_sendingBuf.end(), this->m_sendBuf);
	}

	if (m_sendingBuf.size() == 0) return false;

	m_sendingCBuffer.clear();
	for (auto const& sendBuffer : this->m_sendingBuf) {
		m_sendingCBuffer.emplace_back(&sendBuffer.front(), sendBuffer.size());
	}
	return true;
}

//bool ConnectionBase::_prepareSendBuf() {
//	for (auto const& sendBuffer : this->m_sendBuf) {
//		m_sendingCBuffer.emplace_back(&sendBuffer.front(), sendBuffer.size());
//	}
//	if (m_sendingCBuffer.size() == 0) return false;
//	return true;
//}

void ConnectionBase::_afterSend(size_t bytes_transferred) {
	size_t less_bytes = bytes_transferred;
	size_t constBufferIndex = 0;
	while (less_bytes > 0) {
		if (m_sendingCBuffer.size() <= constBufferIndex) {
			LOG_ERROR("sending buf is empty");
			break;
		}
		auto& cbuf = m_sendingCBuffer[constBufferIndex];
		int buf_len = cbuf.size();//boost::asio::const_buffer::buffer_size_helper(cbuf);
		if (less_bytes < buf_len) {
			auto& buf = this->m_sendingBuf.front();
			buf.erase(buf.begin(), buf.begin() + less_bytes);
			if (buf.size() != (buf_len - less_bytes)) {
				LOG_ERROR("buff len error!!!, buff size:%d, require:%d", buf.size(), buf_len - less_bytes);
			}
			//this->m_sendingCBuffer[constBufferIndex] = boost::asio::const_buffer((char*)cbuf.data() + less_bytes, buf_len - less_bytes);
			//this->m_sendingCBuffer[constBufferIndex] = boost::asio::const_buffer(&buf.front(), buf_len - less_bytes);
			break;
		}
		less_bytes -= buf_len;
		constBufferIndex++;
		m_sendingBuf.pop_front();
	}

	//DWORD tid = GetCurrentThreadId();//std::this_thread::get_id();
	//if (m_sendThreadId == 0) m_sendThreadId = tid;
	//if (tid != m_sendThreadId) {
	//	LOG_ERROR("-----------after send data in other thread!!");
	//}

	/*if (constBufferIndex > 0) {
		this->m_sendingCBuffer.erase(this->m_sendingCBuffer.begin(), this->m_sendingCBuffer.begin() + constBufferIndex);
	}*/
}