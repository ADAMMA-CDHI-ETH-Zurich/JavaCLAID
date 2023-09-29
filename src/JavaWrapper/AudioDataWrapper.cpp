/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

// #include "JavaWrapper/WrapperMaster.hpp"
// // #include "JavaWrapper/Wrapper.hpp"
// #include "Audio/AudioData.hpp"

// // template<typename T>
// // claid::JavaWrapper::Wrapper<claid::AudioData>;

// LAZY_WRAPPER("com.example.portaible.AudioData", claid::AudioData, AudioData)

// // Alternative:
// /*
// using namespace claid;
// using namespace claid::JavaWrapper;
// namespace claid
// {
//     class AudioDataWrapper : public Wrapper<AudioData>
//     {
//         DECLARE_WRAPPER(AudioData)
//     };
// }
// REGISTER_JAVA_WRAPPER("com.example.portaible.AudioData", claid::AudioData, AudioDataWrapper)
// */
// using namespace claid;
// extern "C"
// {
//     JNIEXPORT void Java_JavaCLAIDDataTypes_AudioData_setData
//       (JNIEnv* env, jobject thiz, jbyteArray data)
//     {
//         AudioData* audioData = JavaWrapper::JNIHandle::getHandle<AudioData>(env, thiz);
//         jbyte* bufferPtr = env->GetByteArrayElements(data, NULL);
//         jsize lengthOfArray = env->GetArrayLength(data);
        
//         audioData->data.resize(lengthOfArray);
        
//         Logger::printfln("Copying %d bytes of data to audio data", lengthOfArray);
//         for(size_t i = 0; i < lengthOfArray; i++)
//         {
//             audioData->data[i] = bufferPtr[i];
//         }

//         env->ReleaseByteArrayElements(data, bufferPtr, 0);
 
//     }

//     JNIEXPORT jbyteArray Java_com_example_portaible_AudioData_get
//       (JNIEnv* env, jobject thiz, jbyteArray data)
//     {
//         // AudioData* audioData = JNIHandle::getHandle<AudioData>(env, thiz);
//         // jbyteArray byte_arr = env->NewByteArray(audioData->data.size());

//         // env->SetByteArrayRegion( byte_arr, 0, audioData->data.size(), (const signed char*) audioData->data.data() );


//         // return byte_arr; //return the byte array..
//     }
// }