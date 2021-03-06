#include "BoxScene.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <glm/gtc/type_ptr.hpp>

#define NUM_POINT_LIGHTS 4
#define DEG2RAD(angleDegrees) (angleDegrees * M_PI / 180.0)



float pitch = 0;
float yaw = 0;

//Helper functions
glm::mat3 createNormalMatrix(const glm::mat4 & view, const glm::mat4 & model)
{
   return glm::mat3(glm::transpose(glm::inverse(view * model)));
}

void handleCameraInput(Camera & camera, Context * ctx)
{

   const float Cam_Speed = 1.0/30.0;
   const float Cam_Rot_Speed = M_PI;
   glm::vec3 translate(0.0);
   glm::vec3 rotate(0.0);
   if(Keyboard::isKeyDown(GLFW_KEY_W))
   {
      translate += camera.transform.forward() * Cam_Speed;
   }
   if(Keyboard::isKeyDown(GLFW_KEY_S))
   {
      translate -= camera.transform.forward() * Cam_Speed;
   }
   if(Keyboard::isKeyDown(GLFW_KEY_A))
   {
      translate-= camera.transform.right() * Cam_Speed;
   }
   if(Keyboard::isKeyDown(GLFW_KEY_D))
   {
      translate+= camera.transform.right() * Cam_Speed;
   }
   if(Mouse::pressed(GLFW_MOUSE_BUTTON_LEFT))
   {

      rotate.x = (Mouse::getLastY() - Mouse::getY())/(float)ctx->getWindowHeight()/2.0 * Cam_Rot_Speed;
      rotate.y = (Mouse::getLastX() - Mouse::getX())/(float)ctx->getWindowWidth()/2.0 * Cam_Rot_Speed;
      pitch += rotate.x;
      yaw += rotate.y;
      rotate.z = 0;
   }
   camera.translate(translate);
   camera.setRotation(glm::vec3(pitch,yaw,0));
}

//Const scene data

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };

glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
glm::vec3 pointLightColors[] = {
    glm::vec3(1.0f, 0.6f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 1.0, 0.0),
    glm::vec3(0.2f, 0.2f, 1.0f)
  };



BoxScene::BoxScene(Context * ctx) : Scene(ctx),
   cubeMaterial(
               "assets/textures/container2.png",
               32.0f,
               "assets/textures/container2_specular.png",
               "assets/textures/solid_black.png"
               ),
   lamp(
      glm::vec3(0.2),
      glm::vec3(0.5),
      glm::vec3(1.0),
      50.0f),
  
   dirLight(glm::vec3(0.2f, -1.0f, -0.3f),
                             glm::vec3( 0.3f, 0.24f, 0.14f),
                             glm::vec3(0.7f, 0.42f, 0.26f),
                             glm::vec3( 0.5f, 0.5f, 0.5f)),

   spotlight(glm::vec3(0.0),
                       glm::vec3(2.9),
                       glm::vec3(2.9),
                       DEG2RAD(12.5f),
                       DEG2RAD(17.5f),
                       50.0f)
{
   camera = new Camera(ctx->getWindowWidth(), ctx->getWindowHeight());
   GLfloat vertices[] = {
    // Positions           // Normals           // Texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };
  vbo.setData(vertices, 36*8);

  phongProg = createProgram("Phong Shade Program");
  lampProg = createProgram("Lamp display program");


}

BoxScene::~BoxScene()
{
   delete camera;
}
void BoxScene::initPrograms()
{
   phongProg->addVertexShader("assets/shaders/phong_vert_textured.vs");
   phongProg->addFragmentShader("assets/shaders/phong_frag_textured.fs");

   lampProg->addVertexShader("assets/shaders/debug_vert.vs");
   lampProg->addFragmentShader("assets/shaders/debug_frag.fs");

}
void BoxScene::initialBind()
{

   //Program setup
   phongProg->addAttribute("position");
   phongProg->addAttribute("normal");
   phongProg->addAttribute("vertTexCoords");

   phongProg->addUniform("M");
   phongProg->addUniform("V");
   phongProg->addUniform("P");
   phongProg->addUniform("N");

   phongProg->addStructArray("pointLights",NUM_POINT_LIGHTS,lampStruct);
   phongProg->addUniformStruct("material",TexturedMaterial::getStruct());
   phongProg->addUniformStruct("dirLight",DirectionalLight::getStruct());
   phongProg->addUniformStruct("flashLight",Spotlight::getStruct());

   lampProg->addAttribute("position");
   lampProg->addUniform("M");
   lampProg->addUniform("V");
   lampProg->addUniform("P");
   lampProg->addUniform("debugColor");

   //Scene setup
   //
   //
 
   phongProg->enable();
   boxVao.addAttribute(phongProg->getAttribute("position"),vbo,8*sizeof(GLfloat));
   boxVao.addAttribute(phongProg->getAttribute("normal"),vbo,8*sizeof(GLfloat),3*sizeof(GLfloat));
   boxVao.addAttribute(phongProg->getAttribute("vertTexCoords"),vbo,8*sizeof(GLfloat),6*sizeof(GLfloat),2);
   
   glEnable(GL_DEPTH_TEST);
   camera->setPosition(glm::vec3(0,0,5));
   dirLight.bind(phongProg->getUniformStruct("dirLight"));


   lampProg->enable();
   lightVao.addAttribute(lampProg->getAttribute("position"),vbo, 8*sizeof(GLfloat));
   glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
}
void BoxScene::render()
{
   //Clear the current buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glm::mat4 P,M,V;
   glm::mat3 NORM;
   //Perspective
   if(Keyboard::isKeyToggled(GLFW_KEY_O))
   {
      P = camera->getOrthographicMatrix();
   }
   else
   {
      P = camera->getPerspectiveMatrix();
   }
   phongProg->enable();
   //View
   V = camera->getViewMatrix();
   //Phong lighting section
   phongProg->enable();

   //Camera
   phongProg->getUniform("V").bind(V);
   phongProg->getUniform("P").bind(P);

   //Lighting
   Program::UniformStructArrayInfo lampArr = phongProg->getStructArray("pointLights");
      for(int i = 0; i < NUM_POINT_LIGHTS; i++)
      {
        Light tempLamp(lamp);
        tempLamp.transform.setPosition(pointLightPositions[i]);
        tempLamp.ambient = pointLightColors[i];
        tempLamp.diffuse = pointLightColors[i];
        tempLamp.specular = pointLightColors[i];

        tempLamp.bind(lampArr[i]);

      }



   //Spotlight lighting
   spotlight.transform().copy(camera->transform);
   spotlight.bind(phongProg->getUniformStruct("flashLight"));
   //Texturing
   cubeMaterial.bind(phongProg->getUniformStruct("material"));
   //Draw 10 cubes
   
   Transform cubeTransform;
   boxVao.bind();

   for(int i = 0; i < 10; i++)
      {
        cubeTransform.setPosition(cubePositions[i]);
        cubeTransform.setRotation(glm::vec3(M_PI/12 * i, M_PI/6 * i, 0.0));
        M = cubeTransform.getMatrix();
        NORM = createNormalMatrix(V, M);
        phongProg->getUniform("N").bind(NORM);
        phongProg->getUniform("M").bind(M);
        glDrawArrays(GL_TRIANGLES,0,36);
      }
    cubeMaterial.unbind();

   
   //Draw  lamps
   
   lampProg->enable();
   lightVao.bind();
   lampProg->getUniform("P").bind(P);
   lampProg->getUniform("V").bind(V);
   for(int i = 0; i < NUM_POINT_LIGHTS; i++)
   {
        Transform mTransform;
        mTransform.setPosition(pointLightPositions[i]);
        mTransform.setScale(glm::vec3(0.2));
        M = mTransform.getMatrix();
        lampProg->getUniform("debugColor").bind(pointLightColors[i]);
        lampProg->getUniform("M").bind(M);
        glDrawArrays(GL_TRIANGLES,0,36);
   }
   


}
void BoxScene::update()
{
   handleCameraInput(*camera,getContext());
   //std::cout << camera->transform.getPosition().x<< std::endl;
}
void BoxScene::cleanup()
{

}
