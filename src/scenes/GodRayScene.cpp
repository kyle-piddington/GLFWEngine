#include "GodRayScene.h"
#include <glm/gtc/type_ptr.hpp>
#include "GL_Logger.h"

float randFloat()
{
   return (float)rand()/INT_MAX;
}
GodRayScene::GodRayScene(Context * ctx) :
CameraScene(ctx),
metal("assets/textures/metal.png"),
marble("assets/textures/container.jpg"),
fbo(FramebufferConfiguration::DefaultFramebuffer(ctx->getWindowWidth(),ctx->getWindowHeight())),
finalPass(FramebufferConfiguration::DefaultFramebuffer(ctx->getWindowWidth(),ctx->getWindowHeight()))
{
   texProg = createProgram("Diffuse texture program");
   postprocessProg = createProgram("Postprocess render program");
   plane.transform.setScale(glm::vec3(20.0f));
   plane.transform.setPosition(glm::vec3(0,-0.501,0));
   sun.transform.setPosition(glm::vec3(-50.0f, 15.0f, -1.0f));
   sun.transform.setScale(glm::vec3(10.0f));
   
   for(int i = 0; i < 200; i++)
   {
      Transform t;
      float rad = randFloat() * 40 + 5;
      float ang = randFloat() * M_PI/2  + M_PI - M_PI/4;
      glm::vec3 position(rad * cos(ang), 0, rad*sin(ang));
      float length = rad/10.0;
      glm::vec3 scale = glm::vec3(length* (randFloat() + 0.1), length * 10 * (randFloat() + 0.1), length * (randFloat() + 0.1));
      t.setPosition(position);
      t.setScale(scale);
      transforms.push_back(t.getMatrix());
   }
}
GodRayScene::~GodRayScene()
{

}
void GodRayScene::initPrograms()
{
   texProg->addVertexShader("assets/shaders/tex_vert_layout.vs");
   texProg->addFragmentShader("assets/shaders/debug_frag.fs");

   postprocessProg->addVertexShader("assets/shaders/postprocess_vert.vs");
   postprocessProg->addFragmentShader("assets/shaders/postprocess_frag_godray.fs");
}

void GodRayScene::initialBind()
{
   texProg->addUniform("M");
   texProg->addUniform("V");
   texProg->addUniform("P");
   texProg->addUniform("debugColor");
   postprocessProg->addUniform("occlusionLightTexture");
   postprocessProg->addUniform("screenTexture");
   postprocessProg->addUniform("M");
   postprocessProg->addUniform("ScreenLightPos");

   glm::mat4 P;
   texProg->enable();
   P = camera.getPerspectiveMatrix();
   texProg->getUniform("P").bind(P);
   texProg->disable();

   postprocessProg->enable();
   glm::mat4 I(1.0);
   postprocessProg->getUniform("M").bind(I);
   postprocessProg->disable();

}
void GodRayScene::render()
{
   fbo.bindFrameBuffer();
   glClearColor(0.0f, 0.00f, 0.0f, 1.0f);
   glEnable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   GL_Logger::LogError("Render setup", glGetError());
   renderOcclusionGeometry();

   finalPass.bindFrameBuffer();
   glClearColor(1.f, 0.2f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   renderCityGeometry();

   Framebuffer::BindDefaultFramebuffer();
   glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_DEPTH_TEST);
   renderFrameBuffer();

}

void GodRayScene::renderOcclusionGeometry()
{
   glm::mat4 M, V;
   V = camera.getViewMatrix();
   texProg->enable();
   
   texProg->getUniform("V").bind(V);
   //Draw the two containers
   texProg->getUniform("debugColor").bind(glm::vec3(0,0,0));
   
   for(glm::mat4 m : transforms)
   {
      texProg->getUniform("M").bind(m);
      cube.render();
   }

   texProg->getUniform("M").bind(plane.transform.getMatrix());
   plane.render();

   texProg->getUniform("debugColor").bind(glm::vec3(1,1,1));
   texProg->getUniform("M").bind(sun.transform.getMatrix());
   sun.render();

   

   texProg->disable();


}

void GodRayScene::renderCityGeometry()
{
   glm::mat4 M, V;
   V = camera.getViewMatrix();
   texProg->enable();
   
   texProg->getUniform("V").bind(V);
   //Draw the two containers

   texProg->getUniform("debugColor").bind(glm::vec3(.8,.4,.1));
   
   for(glm::mat4 m : transforms)
   {
      float dist = glm::length(m*glm::vec4(0,0,0,1));
      texProg->getUniform("debugColor").bind(glm::vec3(1,.8,.6) * (float)(1.0f/log(dist)));
      texProg->getUniform("M").bind(m);
      cube.render();
   }

   texProg->getUniform("debugColor").bind(glm::vec3(1,.8,.6) );

   texProg->getUniform("M").bind(plane.transform.getMatrix());
   plane.render();

   texProg->getUniform("debugColor").bind(glm::vec3(.8,.5,.5));
   texProg->getUniform("M").bind(sun.transform.getMatrix());
   sun.render();

   

   texProg->disable();


}

void GodRayScene::renderFrameBuffer()
{

   postprocessProg->enable();
   glm::vec4 sunPos = camera.getPerspectiveMatrix()*camera.getViewMatrix()*glm::vec4(sun.transform.getPosition(),1);
   sunPos/= sunPos.w;
   postprocessProg->getUniform("ScreenLightPos").bind(sunPos);
   fbo.enableTexture("color",postprocessProg->getUniform("occlusionLightTexture").getID());
   finalPass.enableTexture("color",postprocessProg->getUniform("screenTexture").getID());
   renderPlane.render();
   postprocessProg->disable();

}
void GodRayScene::update()
{
   CameraScene::update();
   if(Keyboard::key(GLFW_KEY_L))
   {
      if(Keyboard::isKeyToggled(GLFW_KEY_L))
      {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }
      else
      {
         glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      }
   }
}

void GodRayScene::cleanup()
{
   postprocessProg->enable();
   fbo.disableTexture("color");
   postprocessProg->disable();
}
