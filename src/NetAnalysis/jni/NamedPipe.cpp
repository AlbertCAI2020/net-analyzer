/*
 * NamedPipe.cpp
 *
 *  Created on: 2015-10-28
 *      Author: Administrator
 */
#include <jni.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

enum PIPE_MODE
{
	PIPE_READ = 1,
	PIPE_WRITE
};

extern "C"{

jboolean Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeOpen(JNIEnv* env, jobject obj,
		jstring path, jint mode);

void Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeClose(JNIEnv* env, jobject obj);

jint Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeRead(JNIEnv* env,
		jobject obj, jbyteArray buf, jint count);

jint Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeWrite(JNIEnv* env, jobject obj,
		jbyteArray buf, jint count);

}

jboolean Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeOpen(JNIEnv* env,
		jobject obj, jstring path, jint mode)
{
	int err = 0;
	int fd = -1;
	bool bRet = false;

	jclass c = env->GetObjectClass(obj);
	jfieldID mLastErrID = env->GetFieldID(c, "mLastErr", "I");
	jfieldID mFdID = env->GetFieldID(c, "mFd", "I");

	const char* szpath = env->GetStringUTFChars(path, NULL);

	int ret = mkfifo(szpath, 0777);

	if(0 != ret && EEXIST != errno)
	{
		err = errno;
		goto fail;
	}

	fd = open(szpath, mode==PIPE_READ?O_RDONLY:O_WRONLY);
	if(-1 == fd)
	{
		err = errno;
		goto fail;
	}

	bRet = true;

fail:
	env->ReleaseStringUTFChars(path, szpath);
	env->SetIntField(obj, mLastErrID, err);
	env->SetIntField(obj, mFdID, fd);
	return bRet;
}

void Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeClose(JNIEnv* env, jobject obj)
{
	jclass c = env->GetObjectClass(obj);
	jfieldID mLastErrID = env->GetFieldID(c, "mLastErr", "I");
	jfieldID mFdID = env->GetFieldID(c, "mFd", "I");

	jint fd = env->GetIntField(obj, mFdID);
	if(-1 != fd)
	{
		close(fd);
	}
	env->SetIntField(obj, mLastErrID, 0);
	env->SetIntField(obj, mFdID, -1);
}

jint Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeRead(JNIEnv* env,
		jobject obj, jbyteArray buf, jint count)
{
	jclass c = env->GetObjectClass(obj);
	jfieldID mLastErrID = env->GetFieldID(c, "mLastErr", "I");
	jfieldID mFdID = env->GetFieldID(c, "mFd", "I");

	jint fd = env->GetIntField(obj, mFdID);
	jsize len = env->GetArrayLength(buf);
	int toread = len>count?count:len;

	jbyte* pbuf = env->GetByteArrayElements(buf, NULL);
	int ret = read(fd, pbuf, toread);
	if(ret < 0){
		env->SetIntField(obj, mLastErrID, errno);
	}
	env->ReleaseByteArrayElements(buf, pbuf, 0);

	return ret;
}

jint Java_com_zte_netanalysis_capcontrol_NamedPipe_NativeWrite(JNIEnv* env, jobject obj,
		jbyteArray buf, jint count)
{
	jclass c = env->GetObjectClass(obj);
	jfieldID mLastErrID = env->GetFieldID(c, "mLastErr", "I");
	jfieldID mFdID = env->GetFieldID(c, "mFd", "I");

	jint fd = env->GetIntField(obj, mFdID);
	jsize len = env->GetArrayLength(buf);
	int towrite = len>count?count:len;

	jbyte* pbuf = env->GetByteArrayElements(buf, NULL);
	int ret = write(fd, pbuf, towrite);
	if(ret < 0){
		env->SetIntField(obj, mLastErrID, errno);
	}
	env->ReleaseByteArrayElements(buf, pbuf, 0);

	return ret;
}

