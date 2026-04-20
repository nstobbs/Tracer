#include "Application/Application.hpp"

#include "Object/Mesh.hpp"
#include "Material/Material.hpp"

#include <iostream>
#include <cstdlib>

namespace {
    const int kWidth = 1280;
    const int kHeight = 720;
    const bool kHalfRes = true;

    const Tracer::f32 kCameraSensitivity = 0.25f; /* Pan and Tilt Speed */
    const Tracer::f32 kCameraSpeed = 0.5f; /* Moving Position Speed */

    constexpr int kWindowWidth = kHalfRes ? kWidth / 2 : kWidth;
    constexpr int kWindowHeight = kHalfRes ? kHeight / 2 : kHeight;
}

/* Application Settings */
ApplicationSettings::ApplicationSettings(int argc, char** argv) {
    /* Input Model File Path */
    if (argc > 1) {
        m_inputFile = std::string(argv[1]);
        std::printf("Input Model File Path: %s\n", m_inputFile.c_str());
    }

    if (argc > 2) {
        m_width = atoi(argv[2]);
        std::printf("Window Width: %i\n", m_width);
    }

    if (argc > 3) {
        m_height = atoi(argv[3]);
        std::printf("Window Height: %i\n", m_height);
    }

    if (argc > 4) {
        m_tileSize = atoi(argv[4]);
        std::printf("Tile Size: %i\n", m_tileSize);
    }
    
    if (argc > 5) {
        m_sampleCount = atoi(argv[5]);
        std::printf("Sample Count: %i\n", m_sampleCount);
    }

    if (m_inputFile == "") {
        std::printf("No Input Model File Path was set...\n./Application.exe <Model File Path> <Window Width> <Window Height> <Tile Size> <Sample Count>\n");
        throw std::runtime_error("Exiting...\n");
    }

    if (m_width == 0) {
        m_width = kWindowWidth;
        std::printf("Width was 0. Using %i instead.\n", m_width);
    }

    if (m_height == 0) {
        m_height = kWindowHeight;
        std::printf("Height was 0. Using %i instead.\n", m_height);
    }

    if (m_tileSize == 0) {
        m_tileSize = 32;
        std::printf("Tile Size was 0. Using %i instead.\n", m_tileSize);
    }

    if (m_sampleCount == 0) {
        m_sampleCount = 1;
        std::printf("Sample Count was 0. Using %i instead.\n", m_sampleCount);
    }
};

/* Main Application */
Application::Application(ApplicationSettings settings) {
    using namespace Tracer;

    /* Application Init */
    std::printf("Starting Application.\n");
    m_window = std::make_unique<Window>(settings.width(), settings.height());

    /* Tracer Engine & Scene Setup */
    m_engine = std::make_unique<Engine>();
    m_engine->SetActiveTilesRecord(m_window->getActiveTilesRecord());

    m_scene = std::make_unique<Scene>();
    m_camera = std::make_unique<Camera>();

#if 1

    //auto texture = Image::ReadImage("./Textures/1K_Test_PNG_Texture.png", "RGBA");
    //auto textureSurface = UVTexture(&texture, "RGBA");
    //auto wireframeSurface = Wireframe(Color4(0.5f, 0.5f, 0.5f, 0.5f));
    //auto solidSurface = SolidColor(Color4(0.75f, 0.25f, 0.25f, 1.0f));
    //auto geometricNormalSurface = GeometricNormals();
    //auto surfaceNormalsSurface = SurfaceNormals();
    //auto surface = MergeSurfaces(static_cast<Surface*>(&wireframeSurface), 
    //                             static_cast<Surface*>(&surfaceNormalsSurface),
    //                             MergeOperation::Plus);

    Diffuse diffuseMaterial(Color4(0.5f, 0.5f, 0.5f, 1.0f));
    auto meshes = Mesh::ReadFile(settings.inputFile());
    for (auto& mesh : meshes) {
        mesh.setMaterial(static_cast<Material*>(&diffuseMaterial));
        m_scene->addObject(static_cast<Object*>(&mesh));
    }

#else

    //auto surface = SolidColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
    auto surface = VertexColor();
    auto mesh = Mesh::TriangleMesh();
    mesh.SetSurface(&surface);
    m_scene->AddObject(static_cast<Object*>(&mesh));

#endif

    m_engine->SetScene(m_scene.get());
    m_engine->SetCamera(m_camera.get());
    
    /* Image Layer Setup */
    std::string targetLayerName = "Color";
    m_image = std::make_unique<Image>(settings.width(), settings.height());
    m_image->CreateLayer(targetLayerName);
    
    m_engine->SetImage(m_image.get());
    m_engine->SetTargetLayer(targetLayerName);
    m_engine->SetSamplesPerPixel(settings.sampleCount());
    m_engine->SetTileSize(settings.tileSize());
    m_engine->StartRendering();

    m_window->setTarget(m_image->GetLayer(targetLayerName), m_camera.get());
    m_window->getBBoxDisplayHUD()->setScene(m_scene.get());


    /* Main Loop */
    SDL_Event event;
    while (!m_shutdown) {
        if (SDL_PollEvent(&event) != 0) {

            /* Shutdown Application*/
            if (event.type == SDL_QUIT) {
                m_engine->StopRendering();
                m_shutdown = true;
            
            /* Keybindings */
            } else if (event.type == SDL_KEYDOWN) {
                /* Move Camera Position Keybinds
                W = Move Forward
                A = Move Left
                S = Move Right
                D = Move Backward */
                if (event.key.keysym.sym == SDLK_w) {
                    m_camera->Transform(kCameraSpeed, CameraDirection::eForward);
                } else if (event.key.keysym.sym == SDLK_s) {
                    m_camera->Transform(kCameraSpeed, CameraDirection::eBackward);
                } else if (event.key.keysym.sym == SDLK_a) {
                    m_camera->Transform(kCameraSpeed, CameraDirection::eLeft);
                } else if (event.key.keysym.sym == SDLK_d) {
                    m_camera->Transform(kCameraSpeed, CameraDirection::eRight);
                }
                /* Rotate Camera Direction Keybinds
                Up Arrow = Tilt Up
                Left Arrow = Pan Left
                Right Arrow = Pan Right
                Down Arrow = Tilt Down */
                else if (event.key.keysym.sym == SDLK_UP) {
                    m_camera->Transform(kCameraSensitivity, CameraDirection::eTiltUp);
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    m_camera->Transform(kCameraSensitivity, CameraDirection::eTiltDown);
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    m_camera->Transform(kCameraSensitivity, CameraDirection::ePanLeft);
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    m_camera->Transform(kCameraSensitivity, CameraDirection::ePanRight);
                }
                /* Toggle Different HUD Elements 
                1 = Toggle Active Tile Crosshair 
                2 = Toggle Bounding Box Display 
                3 = Toggle Bounding Box Heatmap */
                else if (event.key.keysym.sym == SDLK_1) {
                    auto* hudElement = m_window->getTileCrossHairHUD();
                    if (hudElement->isEnabled()) {
                        hudElement->disable();
                    } else {
                        hudElement->enable();
                    }
                } else if (event.key.keysym.sym == SDLK_2) {
                    auto* hudElement = m_window->getBBoxDisplayHUD();
                    if (hudElement->isEnabled()) {
                        hudElement->disable();
                    } else {
                        hudElement->enable();
                    }
                } else if (event.key.keysym.sym == SDLK_3) {

                }
            }
        };

        /* Render */
        m_engine->Tick();
        m_window->renderWindow();
    }
};

Application::~Application() {
    std::printf("Shuting Down Application.\n");
    m_engine->StopRendering();
    m_shutdown = true;
    SDL_Quit();
};