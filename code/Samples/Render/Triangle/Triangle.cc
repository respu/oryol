//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class TriangleApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    Resource::Id meshId;
    Resource::Id progId;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 400, "Oryol Triangle Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    
    // create a triangle mesh, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.SetNumVertices(3);
    meshBuilder.SetIndexType(IndexType::None);
    meshBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    meshBuilder.AddComponent(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 3);
    meshBuilder.Begin();
    // first vertex pos and color
    meshBuilder.Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f);
    meshBuilder.Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f);
    // second vertex pos and color
    meshBuilder.Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f);
    // third vertex pos and color
    meshBuilder.Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f);
    meshBuilder.End();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("msh"), meshBuilder.GetStream());
    
    // setup shader program from generated shader source (see shaders.shd)
    this->progId = this->render->CreateResource(Shaders::Triangle::CreateSetup());
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, false, false);
        this->render->ApplyMesh(this->meshId);
        this->render->ApplyProgram(this->progId, 0);
        this->render->Draw(0);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
