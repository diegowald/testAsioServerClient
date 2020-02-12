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
  virtual ~baseMessage(){}

  virtual const char* header() const = 0;
  virtual char* header() = 0;

  virtual std::size_t headerSizeBytes() const = 0;

  virtual const char* payload() const = 0;
  virtual char* payload() = 0;

  virtual void applyToData() = 0;
  virtual bool validateHeader() = 0;

  virtual std::size_t payloadSizeBytes() const = 0;
};

class clientMessage : public baseMessage
{
  // baseMessage interface
public:

  clientMessage()
  {
    _buffer = nullptr;
  }

  virtual const char *header() const override
  {
    return reinterpret_cast<const char*>(&_vectorSize);
  }

  virtual char *header() override
  {
    return reinterpret_cast<char*>(&_vectorSize);
  }

  virtual const char *payload() const override
  {
    return reinterpret_cast<const char*>(&_elements[0]);
  }

  virtual char *payload() override
  {
    if (_elements.size() == _vectorSize)
    {
      return reinterpret_cast<char*>(&_elements[0]);
    }
    else
    {
      if (_buffer != nullptr)
      {
        delete []_buffer;
      }
      _buffer = new char[payloadSizeBytes()];
      return _buffer;
    }
  }

  virtual void applyToData() override
  {
    _elements.clear();
    _elements.resize(_vectorSize);
    for (int i = 0; i < _vectorSize; ++i)
    {
      double *value = reinterpret_cast<double*>(&_buffer[i * sizeof(double)]);
      _elements[i] = *value;
    }
    delete []_buffer;
    _buffer = nullptr;
  }

  virtual std::size_t headerSizeBytes() const override
  {
    return sizeof(std::size_t);
  }

  virtual std::size_t payloadSizeBytes() const override
  {
    return sizeof(double) * _vectorSize;
  }

  virtual bool validateHeader(std::size_t maxVectorSize)
  {
    return (0 < _vectorSize) && (_vectorSize <= maxVectorSize);
  }

  virtual bool validateHeader() override
  {
    return false;
  }

  std::size_t vectorSize() const
  {
    return _vectorSize;
  }

  void setElements(const std::vector<double> &elements)
  {
    _elements = elements;
    _vectorSize = elements.size();
  }

  std::vector<double> elements() const
  {
    return _elements;
  }

private:
  std::size_t _vectorSize;
  std::vector<double> _elements;
  char *_buffer;
};

class serverMessage : public baseMessage
{
public:


  enum class MessageType
  {
    UnknownMessage,
    GlobalAverage,
    MaxNumElements,
    VectorAverage
  };

  serverMessage(MessageType msgType)
  {
    _messageType = msgType;
  }

  virtual const char *header() const override
  {
    return reinterpret_cast<const char*>(&_messageType);
  }

  virtual char *header() override
  {
    return reinterpret_cast<char*>(&_messageType);
  }

  virtual std::size_t headerSizeBytes() const override
  {
    return sizeof (int);
  }

  virtual const char *payload() const override
  {
    switch (_messageType) {
    case MessageType::UnknownMessage:
      return nullptr;
      break;
    case MessageType::GlobalAverage:
    case MessageType::VectorAverage:
      return reinterpret_cast<const char*>(&_average);
      break;
    case MessageType::MaxNumElements:
      return reinterpret_cast<const char*>(&_maxNumElements);
      break;
    default:
      return nullptr;
    }
  }

  virtual char *payload() override
  {
    switch (_messageType) {
    case MessageType::UnknownMessage:
      return nullptr;
      break;
    case MessageType::GlobalAverage:
    case MessageType::VectorAverage:
      return reinterpret_cast<char*>(&_average);
      break;
    case MessageType::MaxNumElements:
      return reinterpret_cast<char*>(&_maxNumElements);
      break;
    default:
      return nullptr;
    }
  }

  virtual void applyToData() override
  {}

  virtual bool validateHeader() override
  {
    return (_messageType == MessageType::UnknownMessage)
        || (_messageType == MessageType::GlobalAverage)
        || (_messageType == MessageType::VectorAverage)
        || (_messageType == MessageType::MaxNumElements);
  }

  virtual std::size_t payloadSizeBytes() const override
  {
    std::size_t value = 0;
    switch (_messageType)
    {
    case MessageType::UnknownMessage:
      value = 0;
      break;
    case MessageType::GlobalAverage:
      value = sizeof (double);
      break;
    case MessageType::VectorAverage:
      value = sizeof (double);
      break;
    case MessageType::MaxNumElements:
      value = sizeof (std::size_t);
      break;
    }
    return value;
  }

  MessageType messageType() const
  {
    return _messageType;
  }

  double average() const
  {
    return _average;
  }

  std::size_t maxNumElements() const
  {
    return _maxNumElements;
  }

  void setMessageType(MessageType messageType)
  {
    _messageType = messageType;
  }

  void setAverage(double average)
  {
    _average= average;
  }

  void setMaxNumElements(std::size_t maxNumElements)
  {
    _maxNumElements = maxNumElements;
  }


private:
  MessageType _messageType;
  double _average;
  std::size_t _maxNumElements;

};

#endif // CHAT_MESSAGE_H
