
#pragma once

#include <map>
#include <string>

class HTTPRequest
{
private:
	std::string _method;
	std::string _requestTarget;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

	size_t _contentLength;  // TODO: check config max lenth

	typedef void (HTTPRequest::*ParseFunction)(std::string &line);
	ParseFunction _currentFunction;

	std::string _requestBuffer;
	bool _isComplete;

	size_t _chunkSizeRemaining;

	bool _setchunkSize();
	bool _addChunkData();
	void _trimChunked();

	void _setMethod(std::string &line);
	void _setHeader(std::string &line);

	void _setBody();
	void _parseChunkedBody();

	void _fillHeaders(std::string line);
	void _printRequest() const;

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

	void reset();
	bool hasCloseHeader() const; // TODO: ?
	void parseRequest(const char *rawRequest);
	bool isComplete() const;
};
