#include "BlendingScene.h"
#include <glm/gtc/type_ptr.hpp>
#include "GL_Logger.h"
BlendingScene::BlendingScene(Context * ctx) :
CameraScene(ctx),
metal("assets/textures/metal.png"),
marble("assets/textures/marble.jpg"),
window(TextureConfig("assets/textures/window.png",GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE))
{
   alphaTexProg = createProgram("Alpha texture program");
   plane.transform.setScale(glm::vec3(5.0f));
   plane.transform.setPosition(glm::vec3(0,-0.501,0));
   cube1.transform.setPosition(glm::vec3(-1.0f, 0.0f, -1.0f));
   cube2.transform.setPosition(glm::vec3(2.0,0.0,0.0));
   windowQuad.transform.setRotation(glm::vec3(-M_PI/2.0,0,0));
   windowQuad.transform.setScale(glm::vec3(0.5));

   sortedTransparentObjects.push_back(glm::vec3(-1.5f,  0.0f, -0.4f));
   sortedTransparentObjects.push_back(glm::vec3( 1.5f,  0.0f,  0.4f));
   sortedTransparentObjects.push_back(glm::vec3( 0.0f,  0.0f,  0.8f));
   sortedTransparentObjects.push_back(glm::vec3(-0.3f,  0.0f, -2.4f));
   sortedTransparentObjects.push_back(glm::vec3( 0.5f,  0.0f, -0.7f));

 

}
BlendingScene::~BlendingScene()
{

}
void BlendingScene::initPrograms()
{
   alphaTexProg->addVertexShader("assets/shaders/tex_vert_layout.vs");
   alphaTexProg->addFragmentShader("assets/shaders/tex_frag_transparent.fs");


}

void BlendingScene::initialBind()
{
   alphaTexProg->addUniform("M");
   alphaTexProg->addUniform("V");
   alphaTexProg->addUniform("P");
   alphaTexProg->addUniform("tex");




   glm::mat4 P;
   alphaTexProg->enable();
   P = camera.getPerspectiveMatrix();
   alphaTexProg->getUniform("P").bind(P);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glClearColor(0.2,0.2,0.2,1.0);
}
void BlendingScene::render()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   GL_Logger::LogError("Render setup", glGetError());

   glm::mat4 M, V;
   V = camera.getViewMatrix();
   alphaTexProg->enable();
   cube1.transform.setScale(glm::vec3(1.0));
   cube2.transform.setScale(glm::vec3(1.0));

   alphaTexProg->getUniform("V").bind(V);
   //Draw the two containers
   marble.enable(alphaTexProg->getUniform("tex").getID());
   alphaTexProg->getUniform("M").bind(cube1.transform.getMatrix());
   cube1.render();
   alphaTexProg->getUniform("M").bind(cube2.transform.getMatrix());
   cube2.render();
   marble.disable();
   //Draw the base plane
   metal.enable(alphaTexProg->getUniform("tex").getID());
   alphaTexProg->getUniform("M").bind(plane.transform.getMatrix());
   plane.render();
   metal.disable();

   sort(sortedTransparentObjects.begin(),sortedTransparentObjects.end(),[&](const glm::vec3 a, glm::vec3 b) -> bool
   {
      return glm::length(a - camera.transform.getPosition()) > glm::length(b - camera.transform.getPosition());
   });


   window.enable(alphaTexProg->getUniform("tex").getID());
   for(int i = 0; i < 5; i++)
   {
      windowQuad.transform.setPosition(sortedTransparentObjects[i]);
      alphaTexProg->getUniform("M").bind(windowQuad.transform.getMatrix());
      windowQuad.render();
   }
   window.disable();

}

void BlendingScene::update()
{
   CameraScene::update();
}
