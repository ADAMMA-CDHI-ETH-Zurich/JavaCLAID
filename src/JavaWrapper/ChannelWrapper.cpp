// #include "Channel/Channel.hpp"

// #include "Logger/Logger.hpp"
// #include "JavaWrapper/JNIUtils.hpp"
// #include "JavaWrapper/WrapperMaster.hpp"

// #include <jni.h>

// using namespace claid;
// using namespace claid::JavaWrapper;
// extern "C"
// {
//     JNIEXPORT void Java_JavaCLAID_Channel_post
//       (JNIEnv* env, jobject wrappedChannel, jobject data)
//     {
//         // From the java side, it is guaranteed that the data type of data
//         // matches the data type that was specified when the channel was created using publish.
//         // As Channel is a templated/generic class Channel<T> on java side,
//         // it was created like Channel<AudioData> audioChannel = module.publish(AudioData.class, channelID).
//         // Therefore, audioChannel.post() has to be called with an object of type AudioData, which
//         // is ensured at compile time.

//         // Assign wrapper if not done already:
//         WrapperMaster::getInstance()->assignWrapperToJavaClassOfObject(env, data);
    
//         std::string className = JNIUtils::getNameOfClassOfObject(env, data);
//         if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
//         {
//             CLAID_THROW(Exception, "Error, post was called for an object of java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
//         }

//         WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);
//         wrapper->post(env, wrappedChannel, data);
//     }
// }
