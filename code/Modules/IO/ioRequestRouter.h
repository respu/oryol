#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::ioRequestRouter
    @brief private: front end router port of the IO system
    
    @todo: ioRequestRouter description
*/
#include "Core/Config.h"
#include "Messaging/Port.h"
#include "IO/ioLane.h"

namespace Oryol {
namespace IO {

class ioRequestRouter : public Messaging::Port {
    OryolClassDecl(ioRequestRouter);
public:
    /// constructor
    ioRequestRouter(int32 numLanes);
    /// destructor
    virtual ~ioRequestRouter();
    
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Messaging::Message>& msg) override;
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork() override;
    
private:
    int32 numLanes;
    Core::Array<Core::Ptr<IO::ioLane>> ioLanes;
};
    
} // namespace IO
} // namespace Oryol