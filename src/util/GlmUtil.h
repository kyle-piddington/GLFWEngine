#ifndef __GLM_UTILS_H__
#define __GLM_UTILS_H__
struct GlmUtil
{
   static glm::mat3 createNormalMatrix(const glm::mat4  view, const glm::mat4 model)
   {
      return glm::mat3(glm::transpose(glm::inverse(view * model)));
   }
   static float lerp(float a, float b, float f)
   {
      return a + f * (b - a);
   }
   
};
#endif
