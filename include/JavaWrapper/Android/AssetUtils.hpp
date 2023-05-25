#pragma once 
 
#if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)

#include <string>

#if defined(__ANDROID__)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace claid
{
    namespace JavaWrapper
    {
        class AssetUtils
        {
            public:

            static bool loadAssetsFileToString(Context& context, const std::string& fileName, std::string& fileContent)
            {
                #if defined(__ANDROID__)

                if(!context.isValid())
                {
                    return false;
                }

                JNIEnv* env = java::JNIUtils::getEnv();
                jobject assetManagerObject = context.getAssetManagerObject();

                AAssetManager* assetManager = AAssetManager_fromJava(env, assetManagerObject);

                AAsset* asset = AAssetManager_open(assetManager, fileName.c_str(), AASSET_MODE_UNKNOWN);
                if (NULL == asset)
                {
                    return false;
                }

                long size = AAsset_getLength(asset);
                char* buffer = new char[size];
                AAsset_read (asset,buffer,size);
                AAsset_close(asset);

                fileContent = std::string(buffer, size);
                delete[] buffer;
                env->DeleteLocalRef(assetManagerObject);

                return true;
                #else
                    return false;
                #endif
            }
        };
    }
}



#endif