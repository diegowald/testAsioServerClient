#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>

// bodylength;
// vector

class baseMessage
{
public:
  virtual ~baseMessage() {}

  const char* data()
  {
    populateBuffer();
    return _buffer.c_str();
  }

  std::size_t dataLengthBytes() const
  {
    return _buffer.length();
  }

protected:

  virtual void populateBuffer() = 0;

  template<class T>
  void addToBuffer(const T& value)
  {
    std::ostringstream oss;

    oss << value;
    _buffer.append(oss.str());
  }

private:
  std::string _buffer;
};

class serverMessage
{
public:
  enum class MessageType {
    UnknownMessage,
    GlobalAverage,
    MaxNumElements,
    VectorAverage
  };

  serverMessage(MessageType messageType) {
    _messageType = messageType;
    _average = 0.0;
    _maxNumElements = 0;
  }

  const char* header() const
  {
    return reinterpret_cast<const char*>(&_messageType);
  }

  char* header()
  {
    return reinterpret_cast<char *>(&_messageType);
  }

  const char* data()
  {
    if (_data.empty())
    {
      calculateData();
    }
    return _data.c_str();
  }

  const char* body() const
  {
    switch (_messageType) {
    case serverMessage::MessageType::GlobalAverage:
      return reinterpret_cast<const char*>(&_average);
      break;
    case serverMessage::MessageType::VectorAverage:
      return reinterpret_cast<const char*>(&_average);
      break;
    case serverMessage::MessageType::MaxNumElements:
      return reinterpret_cast<const char*>(&_maxNumElements);
      break;
    default:
      return nullptr;
    }
  }

  char* body()
  {
    switch (_messageType) {
    case serverMessage::MessageType::GlobalAverage:
      return reinterpret_cast<char*>(&_average);
      break;
    case serverMessage::MessageType::VectorAverage:
      return reinterpret_cast<char*>(&_average);
      break;
    case serverMessage::MessageType::MaxNumElements:
      return reinterpret_cast<char*>(&_maxNumElements);
      break;
    default:
      return nullptr;
    }
  }


  std::size_t body_lengthBytes()
  {
    switch (_messageType)
    {
    case serverMessage::MessageType::GlobalAverage:
      return sizeof(double);
      break;
    case serverMessage::MessageType::VectorAverage:
      return sizeof(double);
      break;
    case serverMessage::MessageType::MaxNumElements:
      return sizeof(std::size_t);
      break;
    default:
      return 0;
    }
  }


  /*char* data()
  {
    std::ostringstream oss;

    oss << static_cast<int>(_messageType) << ",";
    switch (_messageType)
    {
    case serverMessage::MessageType::GlobalAverage:
      oss << _average;
      break;
    case serverMessage::MessageType::VectorAverage:
      oss << _average;
      break;
    case serverMessage::MessageType::MaxNumElements:
      oss << _maxNumElements;
      break;
    }

    return oss.str().c_str();
  }*/

  std::size_t length()
  {
    if (_data.empty())
    {
      calculateData();
    }
    return _data.size();
  }

  void calculateData()
  {
    std::ostringstream oss;

    oss << static_cast<int>(_messageType) << ",";
    switch (_messageType)
    {
    case serverMessage::MessageType::GlobalAverage:
      oss << _average;
      break;
    case serverMessage::MessageType::VectorAverage:
      oss << _average;
      break;
    case serverMessage::MessageType::MaxNumElements:
      oss << _maxNumElements;
      break;
    }

    _data = oss.str();
  }

  bool decode_header()
  {
    return true;
  }

  MessageType messageType() const
  {
    return _messageType;
  }


  void setMaxNumElements(std::size_t maxNumElements)
  {
    _maxNumElements = maxNumElements;
  }

  void setAverage(double &average)
  {
    _average = average;
  }

  std::size_t maxNumElements() const
  {
    return _maxNumElements;
  }

  double average() const
  {
    return _average;
  }

private:
  MessageType _messageType;
  double _average;
  std::size_t _maxNumElements;
  std::string _data;
};

class clientMessage : public baseMessage
{
public:
  const char* header() const
  {
    return reinterpret_cast<const char*>(&_vectorSize);
  }

  char* header()
  {
    return reinterpret_cast<char*>(&_vectorSize);
  }

  bool validateHeader(std::size_t maxNumElements)
  {
    bool valid = (0 <= _vectorSize) && (_vectorSize <= maxNumElements);
    if (valid)
    {
      _elements.clear();
      _elements.resize(_vectorSize);
    }
    return valid;
  }

  const char* vector() const
  {
    return reinterpret_cast<const char*>(&_elements[0]);
  }

  char* vector()
  {
    return reinterpret_cast<char *>(&_elements[0]);
  }

  std::size_t vectorSizeBytes() const
  {
    return _vectorSize * sizeof(std::size_t);
  }

  std::vector<double> elements() const
  {
    return _elements;
  }

  void setElements(const std::vector<double> elements)
  {
    _elements.clear();
    _elements = elements;
  }

  // baseMessage interface
protected:
  virtual void populateBuffer() override
  {
    addToBuffer<std::size_t>(_vectorSize);
    for (double element: _elements)
    {
      addToBuffer<double>(element);
    }
  }


private:
  std::size_t _vectorSize;
  std::vector<double> _elements;  
};


/*class message
{
public:

  enum { header_length = 4 };
  enum { max_body_length = 512 };

  message()
    : body_length_(0)
  {
  }
  
  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
  
};
*/
#endif // CHAT_MESSAGE_H
