//------------------------------------------------------------------------------
//  SimpleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class SimpleRenderTargetApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos);

    RenderFacade* render = nullptr;
    Resource::Id renderTarget;
    Resource::Id sphere;
    Resource::Id torus;
    Resource::Id rtProg;
    Resource::Id dispProg;
    Resource::Id state;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol Simple Render Target Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();

    // create an offscreen render target, we explicitely want repeat texture wrap mode
    // and linear blending...
    auto rtSetup = TextureSetup::AsRenderTarget("rt", 128, 128, PixelFormat::R8G8B8, PixelFormat::D16);
    rtSetup.SetWrapU(TextureWrapMode::Repeat);
    rtSetup.SetWrapV(TextureWrapMode::Repeat);
    rtSetup.SetMagFilter(TextureFilterMode::Linear);
    rtSetup.SetMinFilter(TextureFilterMode::Linear);
    this->renderTarget = this->render->CreateResource(rtSetup);
    
    // create a donut (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.Build();
    this->torus = this->render->CreateResource(MeshSetup::FromData("sphere"), shapeBuilder.GetStream());
    
    // create a sphere mesh with normals and uv coords
    shapeBuilder.Clear();
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddComponent(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddSphere(0.5f, 72.0f, 40.0f);
    shapeBuilder.Build();
    this->sphere = this->render->CreateResource(MeshSetup::FromData("torus"), shapeBuilder.GetStream());

    // create shaders
    this->rtProg = this->render->CreateResource(Shaders::RenderTarget::CreateSetup());
    this->dispProg = this->render->CreateResource(Shaders::Main::CreateSetup());
    
    // constant state
    StateBlockSetup stateSetup("state");
    stateSetup.AddState(Render::State::DepthMask, true);
    stateSetup.AddState(Render::State::DepthTestEnabled, true);
    stateSetup.AddState(Render::State::DepthFunc, Render::State::LessEqual);
    stateSetup.AddState(Render::State::ClearDepth, 1.0f);
    stateSetup.AddState(Render::State::ClearColor, 0.25f, 0.25f, 0.25f, 0.0f);
    this->state = this->render->CreateResource(stateSetup);
    
    // setup static transform matrices
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
SimpleRenderTargetApp::computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        
        // apply general states
        this->render->ApplyStateBlock(this->state);
        
        // render donut to offscreen render target
        this->render->ApplyRenderTarget(this->renderTarget);
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->torus);
        this->render->ApplyProgram(this->rtProg, 0);
        glm::mat4 donutMVP = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
        this->render->ApplyVariable(Shaders::RenderTarget::ModelViewProjection, donutMVP);
        this->render->Draw(0);
        
        // render sphere to display, with offscreen render target as texture
        this->render->ApplyRenderTarget(Resource::Id());
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->sphere);
        this->render->ApplyProgram(this->dispProg, 0);
        glm::mat4 sphereMVP = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, sphereMVP);
        this->render->ApplyVariable(Shaders::Main::Texture, this->renderTarget);
        this->render->Draw(0);
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->state);
    this->render->DiscardResource(this->rtProg);
    this->render->DiscardResource(this->dispProg);
    this->render->DiscardResource(this->sphere);
    this->render->DiscardResource(this->torus);
    this->render->DiscardResource(this->renderTarget);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
