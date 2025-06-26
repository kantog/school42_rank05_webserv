
#pragma once

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>


class HTTPRequest
{
private:
	std::string _method;
	std::string _rawPath;
	std::string _requestTarget;
	std::string _requestFile;
	std::string _pathInfo;
	std::string _query;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

	size_t _contentLength;
	size_t _maxContentLength;
	size_t _bodyBytesReceived;

	typedef void (HTTPRequest::*ParseFunction)(std::string &line, const std::string &serverKey);
	ParseFunction _currentFunction;

	std::vector<char> _requestBuffer;
	bool _isComplete;
	int _errorCode;

	size_t _chunkSizeRemaining;

	bool _setChunkSize();
	bool _addChunkData();
	void _trimChunked();

	void _setMethod(std::string &line, const std::string &serverKey);
	void _setHeader(std::string &line, const std::string &serverKey);

	void _setBody();
	void _parseChunkedBody();

	void _fillHeaders(std::string line);
	void _printRequest() const;

	void _parsePath(const std::string &serverKey);
	void _setMaxBodySize(const std::string &serverKey);

public:
	HTTPRequest();
	HTTPRequest(const HTTPRequest &other);
	HTTPRequest &operator=(const HTTPRequest &other);
	~HTTPRequest();

	const std::string &getMethod() const;
	const std::string &getRequestTarget() const;
	const std::string &getHostURL() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getHeader(const std::string &key) const;
	const std::string &getBody() const;
	const std::string &getPathInfo() const;
	const std::string &getQuery() const;
	const std::string &getVersion() const;
	const std::string &getRequestFile() const;
	const std::string &getRawPath() const;

	void reset();
	bool hasCloseHeader() const;
	void parseRequest(const char *rawRequest, ssize_t bytesRead, const std::string &serverKey);
	bool isComplete() const;
	bool isError() const;
	int getErrorCode() const { return this->_errorCode; }
};
