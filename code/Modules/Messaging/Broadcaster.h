#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Broadcaster
    @brief broadcast incoming messages to subscriber ports
    
    A Messaging Port which sends an incoming message to any number
    of subscriber Ports.
*/
#include "Messaging/Port.h"

namespace Oryol {
namespace Messaging {
    
class Broadcaster : public Port {
    OryolClassPoolAllocDecl(Broadcaster);
public:
    /// constructor
    Broadcaster();
    /// destructor
    virtual ~Broadcaster();
    
    /// subscribe to messages from this port
    void Subscribe(const Core::Ptr<Port>& port);
    /// unsubscribe from this port
    void Unsubscribe(const Core::Ptr<Port>& port);
    /// get subscribers
    const Core::Array<Core::Ptr<Port>>& GetSubscribers() const;

    /// put a message into the port
    virtual bool Put(const Core::Ptr<Message>& msg) override;
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork();
    
protected:
    Core::Array<Core::Ptr<Port>> subscribers;
};
    
} // namespace Messaging
} // namespace Oryol