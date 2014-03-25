/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class jtux_UUtil */

#ifndef _Included_jtux_UUtil
#define _Included_jtux_UUtil
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     jtux_UUtil
 * Method:    check_type_sizes
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_check_1type_1sizes
  (JNIEnv *, jclass);

/*
 * Class:     jtux_UUtil
 * Method:    strerror
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jtux_UUtil_strerror
  (JNIEnv *, jclass, jint);

/*
 * Class:     jtux_UUtil
 * Method:    GetSymbol
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_jtux_UUtil_GetSymbol
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     jtux_UUtil
 * Method:    GetSymbolStr
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jtux_UUtil_GetSymbolStr
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     jtux_UUtil
 * Method:    jaddr_to_seg
 * Signature: (J[BI)V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_jaddr_1to_1seg
  (JNIEnv *, jclass, jlong, jbyteArray, jint);

/*
 * Class:     jtux_UUtil
 * Method:    jaddr_from_seg
 * Signature: (J[BI)V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_jaddr_1from_1seg
  (JNIEnv *, jclass, jlong, jbyteArray, jint);


/*
 * Class:     UUtil
 * Method:    jaddr_to_fifo
 * Signature: (J[BI)V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_jaddr_1to_1fifo
  (JNIEnv *, jclass, jlong, jlong, jbyteArray, jint, jlong, jlong);

/*
 * Class:     UUtil
 * Method:    jaddr_from_fifo
 * Signature: (J[BI)V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_jaddr_1from_1fifo
  (JNIEnv *, jclass, jlong, jlong, jbyteArray, jint, jlong, jlong);

/*
 * Class:    UUtil
 * Method:   jaddr_fifo_init
 * Signature: ([BI)J
 * 
 */
JNIEXPORT jlong JNICALL Java_jtux_UUtil_fifo_1init
  (JNIEnv *, jclass, jlong, jint, jint, jint, jint);

/*
 * Class:     UUtil
 * Method:    fifo_free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jtux_UUtil_fifo_1free
  (JNIEnv *, jclass, jlong);

/*
 * Class:     UUtil
 * Method:    fifo_in
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_jtux_UUtil_fifo_1in
  (JNIEnv *, jclass, jlong);

/*
 * Class:     UUtil
 * Method:    fifo_out
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_jtux_UUtil_fifo_1out
  (JNIEnv *, jclass, jlong);


/*
 * Class:     UUtil
 * Method:    fifo_stat
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_jtux_UUtil_fifo_1stat
  (JNIEnv *, jclass, jlong);

/*
 * Class:     UUtil
 * Method:    fifo_stat_empty
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_jtux_UUtil_fifo_1stat_1empty
  (JNIEnv *, jclass, jlong);

/*
 * Class:     UUtil
 * Method:    fifo_stat_full
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_jtux_UUtil_fifo_1stat_1full
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif