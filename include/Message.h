#ifndef MSG_H_
#define MSG_H_

#include <memory>

namespace doggy {

class SerializedData
{
public:
    size_t size();
    std::uint8_t* raw();
};

struct Message {
    struct {
      std::uint32_t id;
      std::uint32_t timestamp;
    } header;

    virtual SerializedData Serialize() = 0;

virtual ~Message() {}
};

}
#endif
