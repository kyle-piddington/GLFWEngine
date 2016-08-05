#ifndef __GODRAY_SCENE_H__
#define __GODRAY_SCENE_H__
#include "CameraScene.h"
#include "Cube.h"
#include "Texture2D.h"
#include "Plane.h"
#include "Framebuffer.h"
class GodRayScene :public CameraScene
{
public:
   GodRayScene(Context * ctx);
   virtual ~GodRayScene();
   virtual void initPrograms();

   virtual void initialBind();

   virtual void render();

   virtual void update();

   virtual void cleanup();
private:

   void renderOcclusionGeometry();
   void renderCityGeometry();

   void renderFrameBuffer();

   Program * texProg;
   Program * postprocessProg;
   Texture2D metal, marble;
   Plane plane, renderPlane;
   Cube sun;
   Cube cube;
   std::vector<glm::mat4> transforms;

   Framebuffer fbo;
   Framebuffer finalPass;

};
#endif