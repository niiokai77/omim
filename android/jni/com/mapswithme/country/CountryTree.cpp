#include <jni.h>

#include "../maps/Framework.hpp"
#include "../core/jni_helper.hpp"
#include "country_helper.hpp"

using namespace storage_utils;
using namespace storage;

extern "C"
{
  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_setDefaultRoot(JNIEnv * env, jclass clazz)
  {
    GetTree().SetDefaultRoot();
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_setParentAsRoot(JNIEnv * env, jclass clazz)
  {
    GetTree().SetParentAsRoot();
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_setChildAsRoot(JNIEnv * env, jclass clazz, jint position)
  {
    GetTree().SetChildAsRoot(position);
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_resetRoot(JNIEnv * env, jclass clazz)
  {
    GetTree().ResetRoot();
  }

  JNIEXPORT jboolean JNICALL
  Java_com_mapswithme_country_CountryTree_hasParent(JNIEnv * env, jclass clazz)
  {
    return GetTree().HasParent();
  }

  JNIEXPORT jint JNICALL
  Java_com_mapswithme_country_CountryTree_getChildCount(JNIEnv * env, jclass clazz)
  {
    return GetTree().GetChildCount();
  }

  JNIEXPORT jobject JNICALL
  Java_com_mapswithme_country_CountryTree_getChildItem(JNIEnv * env, jclass clazz, jint position)
  {
    CountryTree & tree = GetTree();
    int corePosition = static_cast<int>(position);
    jboolean isLeaf = tree.IsLeaf(corePosition) ? JNI_TRUE : JNI_FALSE;
    jstring name = jni::ToJavaString(env, tree.GetChildName(corePosition));
    jint status = isLeaf == JNI_TRUE ? static_cast<jint>(tree.GetLeafStatus(corePosition)) : 0;
    jint options = isLeaf == JNI_TRUE ? static_cast<jint>(tree.GetLeafOptions(corePosition)) : 0;

    jclass createClass = env->FindClass("com/mapswithme/country/CountryItem");
    ASSERT(createClass, ());

    jmethodID createMethodId = env->GetMethodID(createClass, "<init>", "(Ljava/lang/String;IIZ)V");
    ASSERT(methodId, ());

    return env->NewObject(createClass, createMethodId,
                          name, status, options, !isLeaf);
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_downloadCountry(JNIEnv * env, jclass clazz, jint position, jint options)
  {
    GetTree().DownloadCountry(position, ToOptions(options));
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_deleteCountry(JNIEnv * env, jclass clazz, jint position, jint options)
  {
    GetTree().DeleteCountry(position, ToOptions(options));
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_cancelDownloading(JNIEnv * env, jclass clazz, jint position)
  {
    GetTree().CancelDownloading(position);
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_retryDownloading(JNIEnv * env, jclass clazz, jint position)
  {
    // TODO uncomment when retry will be implemented
//    GetTree().RetryDownloading(position);
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_showLeafOnMap(JNIEnv * env, jclass clazz, jint position)
  {
    GetTree().ShowLeafOnMap(position);
    g_framework->DontLoadState();
  }

  JNIEXPORT jobject JNICALL
  Java_com_mapswithme_country_CountryTree_getLeafGuideInfo(JNIEnv * env, jclass clazz, jint position)
  {
    guides::GuideInfo info;
    if (GetTree().GetLeafGuideInfo(position, info))
      return guides::GuideNativeToJava(env, info);

    return NULL;
  }

  JNIEXPORT jlong JNICALL
  Java_com_mapswithme_country_CountryTree_getLeafSize(JNIEnv * env, jclass clazz, jint position, jint options, jboolean isLocal)
  {
    CountryTree & tree = GetTree();
    int pos = static_cast<int>(position);
    int local = (isLocal == JNI_TRUE) ? true : false;
    TMapOptions opt = ToOptions(options);

    if (options == -1 || local)
    {
      LocalAndRemoteSizeT sizes = options == -1 ? tree.GetDownloadableLeafSize(pos) : tree.GetLeafSize(pos, opt);
      return local ? sizes.first : sizes.second;
    }

    LocalAndRemoteSizeT sizes = tree.GetRemoteLeafSizes(pos);
    switch (opt)
    {
      case TMapOptions::EMapOnly:
        return sizes.first;
      case TMapOptions::ECarRouting:
        return sizes.second;
      default:
        return sizes.first + sizes.second;
    }
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_setListener(JNIEnv * env, jclass clazz, jobject listener)
  {
    g_framework->SetCountryTreeListener(jni::make_global_ref(listener));
  }

  JNIEXPORT void JNICALL
  Java_com_mapswithme_country_CountryTree_resetListener(JNIEnv * env, jclass clazz, jobject listener)
  {
    g_framework->ResetCountryTreeListener();
  }
}
