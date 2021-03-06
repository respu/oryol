//------------------------------------------------------------------------------
//  DDSCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IOFacade.h"
#include "HTTP/HTTPFileSystem.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Util/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::HTTP;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class DDSCubeMapApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    IOFacade* io = nullptr;
    RenderFacade* render = nullptr;
    Resource::Id meshId;
    Resource::Id progId;
    Resource::Id texId;
    Resource::Id stateId;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(DDSCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    this->io = IOFacade::CreateSingle();
    this->io->RegisterFileSystem("http", Creator<HTTPFileSystem, FileSystem>());
    this->io->SetAssign("tex:", "http://localhost:8000/");

    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 400, "Oryol DXT Cube Map Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->AttachLoader(TextureLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();

    // start loading textures
    TextureSetup texBluePrint;
    texBluePrint.SetMinFilter(TextureFilterMode::LinearMipmapLinear);
    texBluePrint.SetMagFilter(TextureFilterMode::Linear);
    texBluePrint.SetWrapU(TextureWrapMode::ClampToEdge);
    texBluePrint.SetWrapV(TextureWrapMode::ClampToEdge);
    this->texId = this->render->CreateResource(TextureSetup::FromFile("tex:romechurch_dxt1.dds", texBluePrint));

    // create a shape with normals
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetTransform(rot90);
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(1.0f, 36, 20);
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("shape"), shapeBuilder.GetStream());

    // build a shader program from a vertex- and fragment shader
    this->progId = this->render->CreateResource(Shaders::Main::CreateSetup());
    
    // setup static state block object
    StateBlockSetup stateSetup("state");
    stateSetup.AddState(Render::State::DepthMask, true);
    stateSetup.AddState(Render::State::DepthTestEnabled, true);
    stateSetup.AddState(Render::State::DepthFunc, Render::State::LessEqual);
    stateSetup.AddState(Render::State::ClearDepth, 1.0f);
    stateSetup.AddState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 1.0f);
    this->stateId = this->render->CreateResource(stateSetup);
    
    // setup projection and view matrices
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
DDSCubeMapApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        // update rotation angles
        this->angleY += 0.02f;
        this->angleX += 0.01f;
        
        // clear, apply mesh and shader program, and draw
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyMesh(this->meshId);
        this->render->ApplyStateBlock(this->stateId);
        this->render->Clear(true, true, true);
        
        const auto resState = this->render->QueryResourceState(this->texId);
        if (resState == Resource::State::Valid) {
            this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f)));
            this->render->ApplyVariable(Shaders::Main::Texture, this->texId);
            this->render->Draw(0);
        }
        else if (resState == Resource::State::Failed) {
            Log::Warn("Failed to load texture resource!\n");
        }
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->stateId);
    this->render->DiscardResource(this->texId);
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    this->io = nullptr;
    IOFacade::DestroySingle();
    
    return App::OnCleanup();
}
