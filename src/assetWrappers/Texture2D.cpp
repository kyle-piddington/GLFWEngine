#include "Texture2D.h"
#include <SOIL.h>
#include "../logger/GL_Logger.h"
#include <easyLogging++.h>
#include "../util/FileUtils.h"
Texture2D::Texture2D(const std::string texName):
   textureName(texName),
   bfr(GL_TEXTURE_2D),
   texUnit(nullptr)
{
   int width, height;
   if(!FileUtils::fExists(textureName))
   {
      LOG(ERROR) << "Could not find texture at " + textureName;
      textureName = "assets/textures/missing_texture.png";
   }
   unsigned char* image = SOIL_load_image(textureName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
   if(width == 0 || height == 0)
   {

   }
   bfr.setData(image, width, height, GL_RGB, GL_UNSIGNED_BYTE);
   SOIL_free_image_data(image);
}

void Texture2D::enable(GLint samplerID)
{
   texUnit = std::make_shared<TextureUnit>(TextureUnitManager::requestTextureUnit());
   glActiveTexture(texUnit->getGlUnit());
   GL_Logger::LogError("Could not activate texture", glGetError());
   bfr.bind();
   glUniform1i(samplerID, texUnit->getTexUnit());
   GL_Logger::LogError("Could not set texture uniform", glGetError());
}
void Texture2D::disable()
{
   texUnit->release();
   bfr.unbind();
   texUnit = nullptr;
}