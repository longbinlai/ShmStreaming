/*
	Copyright 2003 by Marc J. Rochkind. All rights reserved.
	May be copied only for purposes and under conditions described
	on the Web page www.basepath.com/aup/copyright.htm.

	The Example Files are provided "as is," without any warranty;
	without even the implied warranty of merchantability or fitness
	for a particular purpose. The author and his publisher are not
	responsible for any damages, direct or incidental, resulting
	from the use or non-use of these Example Files.

	The Example Files may contain defects, and some contain deliberate
	coding mistakes that were included for educational reasons.
	You are responsible for determining if and how the Example Files
	are to be used.

*/

#include "defs.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#if _POSIX_MESSAGE_PASSING > 0
#include <mqueue.h>
#endif
#include "JtuxSysVIPC.h" // generated by javah
#include "jtux_util.h"
#include "JNI_macros.h"

static bool field_jtoc_perm(JNIEnv *env, jclass cls, const char *field, jobject obj,
  struct ipc_perm *perm)
{
	jfieldID fid;
	jclass clsperm = (*env)->FindClass(env, "jtux/USysVIPC$s_ipc_perm");
	jobject objperm;
	int mode;

	if (cls == NULL || clsperm == NULL)
		return false;
	if ((fid = (*env)->GetFieldID(env, cls, field, "Ljtux/USysVIPC$s_ipc_perm;")) == NULL)
		return false;
	if ((objperm = (*env)->GetObjectField(env, obj, fid)) == NULL) {
		JNU_ThrowByName(env, "NullPointerException", "s_ipc_perm field not initialized");
		return false;
	}
	if (!field_jtoc_long(env, clsperm, "uid", objperm, &perm->uid))
		return false;
	if (!field_jtoc_long(env, clsperm, "gid", objperm, &perm->gid))
		return false;
	if (!field_jtoc_long(env, clsperm, "cuid", objperm, &perm->cuid))
		return false;
	if (!field_jtoc_long(env, clsperm, "cgid", objperm, &perm->cgid))
		return false;
	if (!field_jtoc_int(env, clsperm, "mode", objperm, &mode))
		return false;
	perm->mode = mode;
	return true;
}

static bool field_ctoj_perm(JNIEnv *env, jclass cls, const char *field, jobject obj,
  struct ipc_perm *perm)
{
	jfieldID fid;
	jclass clsperm = (*env)->FindClass(env, "jtux/USysVIPC$s_ipc_perm");
	jobject objperm;

	if (cls == NULL || clsperm == NULL)
		return false;
	if ((fid = (*env)->GetFieldID(env, cls, field, "Ljtux/USysVIPC$s_ipc_perm;")) == NULL)
		return false;
	if ((objperm = (*env)->GetObjectField(env, obj, fid)) == NULL) {
		JNU_ThrowByName(env, "NullPointerException", "s_ipc_perm field not initialized");
		return false;
	}
	if (!field_ctoj_long(env, clsperm, "uid", objperm, perm->uid))
		return false;
	if (!field_ctoj_long(env, clsperm, "gid", objperm, perm->gid))
		return false;
	if (!field_ctoj_long(env, clsperm, "cuid", objperm, perm->cuid))
		return false;
	if (!field_ctoj_long(env, clsperm, "cgid", objperm, perm->cgid))
		return false;
	if (!field_ctoj_int(env, clsperm, "mode", objperm, perm->mode))
		return false;
	return true;
}

JNIEXPORT jlong JNICALL Java_jtux_USysVIPC_ftok(JNIEnv *env, jclass obj,
  jstring path, jint id)
{
	JSTR_GET_DECL(path_c, path)
	key_t k;

	JSTR_NULLTEST_V(path_c, -1)
	JTHROW_neg1(k = ftok(path_c, id))
	JSTR_REL(path_c, path)
	return k;
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_msg_1set_1type(JNIEnv *env, jclass obj,
  jlong msgtype, jbyteArray msgp)
{
	void *msgp_c;

	if ((msgp_c = (*env)->GetByteArrayElements(env, msgp, NULL)) == NULL)
		return 0;
	*(long *)msgp_c = (long)msgtype;
	(*env)->ReleaseByteArrayElements(env, msgp, msgp_c, 0);
	return sizeof(long);
}

JNIEXPORT void JNICALL Java_jtux_USysVIPC_msgctl(JNIEnv *env, jclass obj,
  jint msqid, jint cmd, jobject data)
{
	struct msqid_ds data_c;
	jclass cls = (*env)->FindClass(env, "jtux/USysVIPC$s_msqid_ds");
	int n, r;

	memset(&data_c, 0, sizeof(data_c));
	if (cmd == IPC_SET) {
		if (!field_jtoc_perm(env, cls, "msg_perm", data, &data_c.msg_perm))
			return;
		if (!field_jtoc_int(env, cls, "msg_qbytes", data, &n))
			return;
		data_c.msg_qbytes = n;
	}
	JTHROW_neg1(r = msgctl(msqid, cmd, &data_c))
	if (r == -1)
		return;
	if (cmd == IPC_STAT) {
		if (!field_ctoj_perm(env, cls, "msg_perm", data, &data_c.msg_perm))
			return;
		if (!field_ctoj_int(env, cls, "msg_qnum", data, data_c.msg_qnum))
			return;
		if (!field_ctoj_int(env, cls, "msg_qbytes", data, data_c.msg_qbytes))
			return;
		if (!field_ctoj_long(env, cls, "msg_lspid", data, data_c.msg_lspid))
			return;
		if (!field_ctoj_long(env, cls, "msg_lrpid", data, data_c.msg_lrpid))
			return;
		if (!field_ctoj_long(env, cls, "msg_stime", data, data_c.msg_stime))
			return;
		if (!field_ctoj_long(env, cls, "msg_rtime", data, data_c.msg_rtime))
			return;
		if (!field_ctoj_long(env, cls, "msg_ctime", data, data_c.msg_ctime))
			return;
	}
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_msgget(JNIEnv *env, jclass obj,
  jlong key, jint flags)
{
	int msqid;

	JTHROW_neg1(msqid = msgget(key, flags))
	return msqid;
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_msgrcv(JNIEnv *env, jclass obj,
  jint msqid, jbyteArray msgp, jint mtextsize, jlong msgtype, jint flags)
{
	void *msgp_c;
	ssize_t nrcv;

	if ((msgp_c = (*env)->GetByteArrayElements(env, msgp, NULL)) == NULL)
		return -1;
	JTHROW_neg1(nrcv = msgrcv(msqid, msgp_c, mtextsize, msgtype, flags))
	(*env)->ReleaseByteArrayElements(env, msgp, msgp_c, 0);
	return nrcv;
}

JNIEXPORT void JNICALL Java_jtux_USysVIPC_msgsnd(JNIEnv *env, jclass obj,
  jint msqid, jbyteArray msgp, jint msgsize, jint flags)
{
	void *msgp_c;

	if ((msgp_c = (*env)->GetByteArrayElements(env, msgp, NULL)) == NULL)
		return;
	JTHROW_neg1(msgsnd(msqid, msgp_c, msgsize, flags))
	(*env)->ReleaseByteArrayElements(env, msgp, msgp_c, 0);
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_semctl(JNIEnv *env, jclass obj,
  jint semid, jint semnum, jint cmd, jobject arg)
{
	struct semid_ds data_c;
	jclass cls_s_semid_ds = (*env)->FindClass(env, "jtux/USysVIPC$s_semid_ds");
	jclass cls_u_semun_int = (*env)->FindClass(env, "jtux/USysVIPC$u_semun_int");
	jclass cls_u_semun_struct = (*env)->FindClass(env, "jtux/USysVIPC$u_semun_struct");
	jclass cls_u_semun_array = (*env)->FindClass(env, "jtux/USysVIPC$u_semun_array");
	jfieldID fid;
	jobject objds;
	jshortArray array;
	int r;
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	} arg_c;



	memset(&data_c, 0, sizeof(data_c));
	switch (cmd) {
	case IPC_SET:
		if (cls_u_semun_struct == NULL)
			return -1;
		if ((fid = (*env)->GetFieldID(env, cls_u_semun_struct, "buf", "Ljtux/USysVIPC$s_semid_ds;")) == NULL)
			return -1;
		if ((objds = (*env)->GetObjectField(env, arg, fid)) == NULL) {
			JNU_ThrowByName(env, "NullPointerException", "buf field not initialized");
			return -1;
		}
		if (!field_jtoc_perm(env, cls_s_semid_ds, "sem_perm", objds, &data_c.sem_perm))
			return -1;
		/* fall through */
	case IPC_STAT:
		arg_c.buf = &data_c;
		break;
	case SETVAL:
		if (!field_jtoc_int(env, cls_u_semun_int, "val", arg, &arg_c.val))
			return -1;
		break;
	case SETALL:
	case GETALL:
		arg_c.buf = &data_c;
		JTHROW_neg1(r = semctl(semid, 0, IPC_STAT, arg_c))
		if (r == -1)
			return -1;
		if (cls_u_semun_array == NULL)
			return -1;
		if ((fid = (*env)->GetFieldID(env, cls_u_semun_array, "array", "[S")) == NULL)
			return -1;
		if ((array = (*env)->GetObjectField(env, arg, fid)) == NULL) {
			JNU_ThrowByName(env, "NullPointerException", "array field not initialized");
			return -1;
		}
		if ((arg_c.array = (*env)->GetShortArrayElements(env, array, NULL)) == NULL)
			return -1;
	}
	JTHROW_neg1(r = semctl(semid, semnum, cmd, arg_c))
	if (r == -1)
		return r;
	switch (cmd) {
	case IPC_STAT:
		if (cls_u_semun_struct == NULL)
			return -1;
		if ((fid = (*env)->GetFieldID(env, cls_u_semun_struct, "buf", "Ljtux/USysVIPC$s_semid_ds;")) == NULL)
			return -1;
		if ((objds = (*env)->GetObjectField(env, arg, fid)) == NULL) {
			JNU_ThrowByName(env, "NullPointerException", "s_semid_ds field not initialized");
			return -1;
		}
		if (!field_ctoj_perm(env, cls_s_semid_ds, "sem_perm", objds, &data_c.sem_perm))
			return -1;
		if (!field_ctoj_short(env, cls_s_semid_ds, "sem_nsems", objds, data_c.sem_nsems))
			return -1;
		if (!field_ctoj_long(env, cls_s_semid_ds, "sem_otime", objds, data_c.sem_otime))
			return -1;
		if (!field_ctoj_long(env, cls_s_semid_ds, "sem_ctime", objds, data_c.sem_ctime))
			return -1;
		break;
	case SETALL:
		(*env)->ReleaseShortArrayElements(env, array, arg_c.array, JNI_ABORT);
		break;
	case GETALL:
		(*env)->ReleaseShortArrayElements(env, array, arg_c.array, 0);
	}
	return r;
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_semget(JNIEnv *env, jclass obj,
  jlong key, jint msems, jint flags)
{
	int semid;

	JTHROW_neg1(semid = semget(key, msems, flags))
	return semid;
}

JNIEXPORT void JNICALL Java_jtux_USysVIPC_semop(JNIEnv *env, jclass obj,
  jint semid, jobjectArray sops, jint nsops)
{
	struct sembuf *sops_c;
	int i;
	jclass cls = (*env)->FindClass(env, "jtux/USysVIPC$s_sembuf");

	JTHROW_null(sops_c = malloc(nsops * sizeof(struct sembuf)))
	if (sops_c == NULL)
		return;
	for (i = 0; i < nsops; i++) {
		jobject sb_obj = (*env)->GetObjectArrayElement(env, sops, i);

		if (sb_obj == NULL) {
			free(sops_c);
			return;
		}
		if (!field_jtoc_short(env, cls, "sem_num", sb_obj, &sops_c[i].sem_num))
			return;
		if (!field_jtoc_short(env, cls, "sem_op", sb_obj, &sops_c[i].sem_op))
			return;
		if (!field_jtoc_short(env, cls, "sem_flg", sb_obj, &sops_c[i].sem_flg))
			return;
	}
	JTHROW_neg1(semop(semid, sops_c, nsops))
	free(sops_c);
}

JNIEXPORT jlong JNICALL Java_jtux_USysVIPC_shmat(JNIEnv *env, jclass obj,
  jint shmid, jlong shmaddr, jint flags)
{
	intptr_t p;

	JTHROW_neg1(p = (intptr_t)shmat(shmid, (const void *)(intptr_t)shmaddr, flags))
	return p;
}

JNIEXPORT void JNICALL Java_jtux_USysVIPC_shmctl(JNIEnv *env, jclass obj,
  jint shmid, jint cmd, jobject data)
{
	struct shmid_ds data_c;
	jclass cls = (*env)->FindClass(env, "jtux/USysVIPC$s_shmid_ds");
	int r;

	if (cmd == IPC_RMID) {
		JTHROW_neg1(r = shmctl(shmid, cmd, 0));
		return;
	} 
	else{

		memset(&data_c, 0, sizeof(data_c));
		if (cmd == IPC_SET) {
			if (!field_jtoc_perm(env, cls, "shm_perm", data, &data_c.shm_perm))
				return;
		}
		JTHROW_neg1(r = shmctl(shmid, cmd, &data_c));
		if (r == -1)
			return;
		if (cmd == IPC_STAT) {
			if (!field_ctoj_perm(env, cls, "shm_perm", data, &data_c.shm_perm))
				return;
			if (!field_ctoj_int(env, cls, "shm_segsz", data, data_c.shm_segsz))
				return;
			if (!field_ctoj_long(env, cls, "shm_lpid", data, data_c.shm_lpid))
				return;
			if (!field_ctoj_long(env, cls, "shm_cpid", data, data_c.shm_cpid))
				return;
			if (!field_ctoj_int(env, cls, "shm_nattch", data, data_c.shm_nattch))
				return;
			if (!field_ctoj_long(env, cls, "shm_atime", data, data_c.shm_atime))
				return;
			if (!field_ctoj_long(env, cls, "shm_dtime", data, data_c.shm_dtime))
				return;
			if (!field_ctoj_long(env, cls, "shm_ctime", data, data_c.shm_ctime))
				return;
		}
	}
}

JNIEXPORT void JNICALL Java_jtux_USysVIPC_shmdt(JNIEnv *env, jclass obj,
  jlong shmaddr)
{
	JTHROW_neg1(shmdt((void *)(intptr_t)shmaddr))
}

JNIEXPORT jint JNICALL Java_jtux_USysVIPC_shmget(JNIEnv *env, jclass obj,
  jlong key, jint size, jint flags)
{
	int shmid;

	JTHROW_neg1(shmid = shmget(key, size, flags))
	return shmid;
}
