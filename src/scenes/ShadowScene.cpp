#include "ShadowScene.h"
#include <glm/gtc/type_ptr.hpp>
#include "GlmUtil.h"
ShadowScene::ShadowScene(Context * ctx) :
   CameraScene(ctx),
   woodTexture("assets/textures/wood.png"),
   pointLight(glm::vec3(0.1),glm::vec3(0.7),glm::vec3(0.8),250.0f),
   model("assets/models/nanosuit/nanosuit.obj")
   {
      pointLight.transform.setPosition(glm::vec3(-2.0f, 4.0f, -1.0f));
      pointLight.transform.lookAt(glm::vec3(0.0),glm::vec3(1.0));
      FramebufferConfiguration config(1024,1024);
      config.addTexturebuffer(
         TextureAttachment(
            TextureConfig("depth",GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT),
            GL_DEPTH_ATTACHMENT));
      
      depthBuffer.init(config);
      depthBuffer.bindFrameBuffer();
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      depthBuffer.unbindFrameBuffer();


      renderCube[0].transform.setPosition(glm::vec3(0.0f, 1.5f, 0.0));
      renderCube[1].transform.setPosition(glm::vec3(2.0f, 0.3f, 1.0));
      renderCube[2].transform.setPosition(glm::vec3(-1.0f, 0.0f, 2.0));
      renderCube[2].transform.rotate(2*M_PI/3.0, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      renderCube[2].transform.setScale(glm::vec3(0.5));
      postprocessPlane.transform.setScale(glm::vec3(0.125));
      postprocessPlane.transform.setPosition(glm::vec3(1 - 0.125,-1 + 0.125,0));

      geomPlane.transform.setScale(glm::vec3(25.0f));
      geomPlane.transform.setPosition(glm::vec3(0,-0.5,0));
      depthPassProg = createProgram("Depth pass program");
      postprocessProg = createProgram("Depth display program");
      phongTexShadowProg = createProgram("Phong lighting with texture");

      model.transform.setScale(glm::vec3(0.2));
      model.transform.translate(glm::vec3(0,-0.5,1.0));
   }



void ShadowScene::initPrograms()
{
   depthPassProg->addVertexShader("assets/shaders/depthpass_vert.vs");
   depthPassProg->addFragmentShader("assets/shaders/depthpass_frag.fs");

   postprocessProg->addVertexShader("assets/shaders/postprocess_vert.vs");
   postprocessProg->addFragmentShader("assets/shaders/postprocess_frag_displayDepth.fs");

   phongTexShadowProg->addVertexShader("assets/shaders/shadowShaders/phong_vert_shadow.vs");
   phongTexShadowProg->addFragmentShader("assets/shaders/shadowShaders/phong_frag_shadow.fs");


}
void ShadowScene::initialBind()
{
   depthPassProg->addUniform("VPMatrix");
   depthPassProg->addUniform("model");

   postprocessProg->addUniform("screenTexture");

   postprocessProg->addUniform("M");
   postprocessProg->enable();
   postprocessProg->getUniform("M").bind(postprocessPlane.transform.getMatrix());
   postprocessProg->disable();

   phongTexShadowProg->addUniform("P");
   phongTexShadowProg->addUniform("V");
   phongTexShadowProg->addUniform("T");
   phongTexShadowProg->addUniform("M");
   phongTexShadowProg->addUniform("NORM");
   phongTexShadowProg->addUniform("lightSpaceMatrix");
   phongTexShadowProg->addUniformStruct("pLight",Light::getStruct());
   phongTexShadowProg->addUniform("diffuseTexture");
   phongTexShadowProg->addUniform("shadowMap");

   phongTexShadowProg->enable();
   glm::mat4 P = camera.getPerspectiveMatrix();
   phongTexShadowProg->getUniform("P").bind(P);
   phongTexShadowProg->disable();

   glClearColor(0.2,0.2,0.2,1.0);
   glEnable(GL_CULL_FACE);

}

void ShadowScene::render()

{
   //Cull to fix peter panning
   glCullFace(GL_FRONT);
   renderDepthPass();
   glCullFace(GL_BACK);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   renderGeometryWithShadows();
   glClear(GL_DEPTH_BUFFER_BIT);
   renderDepthMap();
}

void ShadowScene::renderGeometryWithShadows()
{
   glViewport(0, 0, getContext()->getWindowWidth(), getContext()->getWindowHeight());
   phongTexShadowProg->enable();
   glm::mat4 V = camera.getViewMatrix();
   woodTexture.enable(phongTexShadowProg->getUniform("diffuseTexture").getID());
   depthBuffer.enableTexture("depth",phongTexShadowProg->getUniform("shadowMap").getID());
   phongTexShadowProg->getUniform("V").bind(V);

   GLfloat near_plane = 1.0f, far_plane = 7.5f;
   glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
   glm::mat4 VPMatrix = lightProjection * glm::inverse(pointLight.transform.getMatrix());
   phongTexShadowProg->getUniform("lightSpaceMatrix").bind(VPMatrix);

   pointLight.bind(phongTexShadowProg->getUniformStruct("pLight"));
   renderGeometry(*phongTexShadowProg, phongTexShadowProg->getUniform("M"), phongTexShadowProg->getUniform("NORM"), phongTexShadowProg->getUniform("T"));
   

   woodTexture.disable();
   depthBuffer.disableTexture("depth");

}
void ShadowScene::renderDepthPass()
{
   depthBuffer.bindFrameBuffer();
   glClear(GL_DEPTH_BUFFER_BIT);

   glViewport(0, 0, depthBuffer.getWidth(), depthBuffer.getHeight());
   depthPassProg->enable();
   GLfloat near_plane = 1.0f, far_plane = 7.5f;
   glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
   glm::mat4 VPMatrix = lightProjection * glm::inverse(pointLight.transform.getMatrix());
   depthPassProg->getUniform("VPMatrix").bind(VPMatrix);
   renderGeometry(*depthPassProg, depthPassProg->getUniform("model"));
   depthBuffer.unbindFrameBuffer();
}
void ShadowScene::renderGeometry(Program & prog, const GLSLParser::UniformObject  modelMtx, const GLSLParser::UniformObject  normalMtx, const GLSLParser::UniformObject  texMtx)
{
   glm::mat3 T(1.0);
   if(texMtx.getID() > -1)
   {
      texMtx.bind(T);
   }
   for(int i = 0; i < 3; i++)
   {
      modelMtx.bind(renderCube[i].transform.getMatrix());
      if(normalMtx.getID() > -1)
      {
         glm::mat3 NORM = GlmUtil::createNormalMatrix(camera.getViewMatrix(),renderCube[i].transform.getMatrix());
         normalMtx.bind(NORM);
      }
      renderCube[i].render();
   }
   modelMtx.bind(geomPlane.transform.getMatrix());
   if(normalMtx.getID() > -1)
   {
      glm::mat3 NORM = GlmUtil::createNormalMatrix(camera.getViewMatrix(),geomPlane.transform.getMatrix());
      normalMtx.bind(NORM);
   }
   if(texMtx.getID() > -1)
   {
      T[0][0] = 25;
      T[1][1] = 25;
      texMtx.bind(T);

   }
   geomPlane.render();
   modelMtx.bind(model.transform.getMatrix());
   model.render(prog);



}

void ShadowScene::update()
{
   CameraScene::update();
   pointLight.transform.setPosition(glm::vec3(2*sin(glfwGetTime()),4.0,2*cos(glfwGetTime())));
   pointLight.transform.lookAt(glm::vec3(0.0));
}

void ShadowScene::renderDepthMap()
{
   glViewport(0, 0, CameraScene::getContext()->getWindowWidth(), CameraScene::getContext()->getWindowHeight());
   postprocessProg->enable();
   depthBuffer.enableTexture("depth",postprocessProg->getUniform("screenTexture").getID());
   postprocessPlane.render();
   depthBuffer.disableTexture("depth");
   postprocessProg->disable();
}
void ShadowScene::cleanup()
{

}
