#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::eglDisplayMgr
    @brief display manager class for EGL platforms
*/
#include "Render/base/displayMgrBase.h"
#include "Render/gl/gl_decl.h"
#include <EGL/egl.h>

namespace Oryol {
namespace Render {

class eglDisplayMgr : public displayMgrBase {
public:
    /// constructor
    eglDisplayMgr();
    /// destructor
    ~eglDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const RenderSetup& renderSetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();
    
private:
    EGLDisplay eglDisplay;
    EGLConfig eglConfig;
    EGLSurface eglSurface;
    EGLContext eglContext;
};

} // namespace Render
} // namespace Oryol
