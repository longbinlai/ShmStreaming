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
#define __EXTENSIONS__
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "JtuxNetwork.h" // generated by javah
#include "jtux_util.h"
#include "JNI_macros.h"

static bool sockaddr_jtoc(JNIEnv *env, jobject sa, struct sockaddr *sa_c,
  socklen_t *sa_len_c)
{
	jclass cls_s_sockaddr_un = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_un");
	jclass cls_s_sockaddr_in = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in");
	jclass cls_s_sockaddr_in6 = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in6");
	jclass cls_s_in_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in_addr");
	jclass cls_s_in6_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in6_addr");

	if (cls_s_sockaddr_un == NULL || cls_s_sockaddr_in == NULL ||
	  cls_s_sockaddr_in6 == NULL || cls_s_in_addr == NULL || cls_s_in6_addr == NULL)
		return false;
	if ((*env)->IsInstanceOf(env, sa, cls_s_sockaddr_un)) {
		struct sockaddr_un *sp = (struct sockaddr_un *)sa_c;
		int n;

		if (!field_jtoc_int(env, cls_s_sockaddr_un, "sun_family", sa, &n))
			return false;
		sp->sun_family = n;
		if (!field_jtoc_string(env, cls_s_sockaddr_un, "sun_path", sa, sp->sun_path, sizeof(sp->sun_path)))
			return false;
		if (sa_len_c != NULL)
			*sa_len_c = sizeof(*sp);
	}
	else if ((*env)->IsInstanceOf(env, sa, cls_s_sockaddr_in)) {
		struct sockaddr_in *sp = (struct sockaddr_in *)sa_c;
		jobject addr;
		int n;
		if (!field_jtoc_int(env, cls_s_sockaddr_in, "sin_family", sa, &n))
			return false;
		sp->sin_family = n;
		if (!field_jtoc_short(env, cls_s_sockaddr_in, "sin_port", sa, &sp->sin_port))
			return false;
		if (!field_jtoc_object(env, cls_s_sockaddr_in, "sin_addr", "Ljtux/UNetwork$s_in_addr;",
		  sa, &addr))
			return false;
		if (!field_jtoc_int(env, cls_s_in_addr, "s_addr", addr, &sp->sin_addr.s_addr))
			return false;
		if (sa_len_c != NULL)
			*sa_len_c = sizeof(*sp);
	}
	else if ((*env)->IsInstanceOf(env, sa, cls_s_sockaddr_in6)) {
		// Following compiled but not tested.
		struct sockaddr_in6 *sp = (struct sockaddr_in6 *)sa_c;
		jobject addr;
		jbyteArray ba;
		char *bytes;
		int n;

		if (!field_jtoc_int(env, cls_s_sockaddr_in6, "sin6_family", sa, &n))
			return false;
		sp->sin6_family = n;
		if (!field_jtoc_short(env, cls_s_sockaddr_in6, "sin6_port", sa, &sp->sin6_port))
			return false;
		if (!field_jtoc_int(env, cls_s_sockaddr_in6, "sin6_flowinfo", sa, &sp->sin6_flowinfo))
			return false;
		if (!field_jtoc_object(env, cls_s_sockaddr_in6, "sin_addr", "Ljtux/UNetwork$s_in6_addr;",
		  sa, &addr))
			return false;


		if (!field_jtoc_bytearray(env, cls_s_in6_addr, "s6_addr", addr, (void **)&bytes,
		  &ba))
			return false;
		memcpy(sp->sin6_addr.s6_addr, bytes, 16);
		field_jtoc_bytearray_release_nocopy(env, ba, bytes);
		if (!field_jtoc_int(env, cls_s_sockaddr_in6, "sin6_scope_id", sa, &sp->sin6_scope_id))
			return false;
		if (sa_len_c != NULL)
			*sa_len_c = sizeof(*sp);
	}
	else
		setup_throw_errno(env, EINVAL);
	return true;
}

/*
	Creates new object if *sa is NULL going in.
*/
static bool sockaddr_ctoj(JNIEnv *env, jobject *sa, struct sockaddr *sa_c)
{
	jclass cls_s_sockaddr_in = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in");
	jclass cls_s_sockaddr_in6 = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in6");
	jclass cls_s_in_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in_addr");
	jclass cls_s_in6_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in6_addr");

	if (cls_s_sockaddr_in == NULL || cls_s_sockaddr_in6 == NULL ||
	  cls_s_in_addr == NULL || cls_s_in6_addr == NULL)
		return false;
	switch (sa_c->sa_family) {
	case AF_INET: {
		struct sockaddr_in *saddr = (struct sockaddr_in *)sa_c;
		jobject addrobj;
		jmethodID mid;

		if (*sa == NULL) {
			if ((mid = (*env)->GetMethodID(env, cls_s_sockaddr_in, "<init>", "()V")) == NULL)
				return false;
			if ((*sa = (*env)->NewObject(env, cls_s_sockaddr_in, mid)) == NULL)
				return false;
		}
		if (!field_ctoj_int(env, cls_s_sockaddr_in, "sin_family", *sa, saddr->sin_family))
			return false;
		if (!field_ctoj_short(env, cls_s_sockaddr_in, "sin_port", *sa, saddr->sin_port))
			return false;
		if ((mid = (*env)->GetMethodID(env, cls_s_in_addr, "<init>", "()V")) == NULL)
			return false;
		if ((addrobj = (*env)->NewObject(env, cls_s_in_addr, mid)) == NULL)
			return false;
		if (!field_ctoj_int(env, cls_s_in_addr, "s_addr", addrobj, saddr->sin_addr.s_addr))
			return false;
		if (!field_ctoj_object(env, cls_s_sockaddr_in, "sin_addr", "Ljtux/UNetwork$s_in_addr;", *sa, addrobj))
			return false;
	}
		break;
	case AF_INET6: {
		// Following compiled but not tested.
		struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)sa_c;
		jobject addrobj;
		jmethodID mid;
		jbyteArray ba;
		jbyte *ba_c;

		if (*sa == NULL) {
			if ((mid = (*env)->GetMethodID(env, cls_s_sockaddr_in6, "<init>", "()V")) == NULL)
				return false;
			if ((*sa = (*env)->NewObject(env, cls_s_sockaddr_in6, mid)) == NULL)
				return false;
		}
		if (!field_ctoj_int(env, cls_s_sockaddr_in6, "sin6_family", *sa, saddr->sin6_family))
			return false;
		if (!field_ctoj_short(env, cls_s_sockaddr_in6, "sin6_port", *sa, saddr->sin6_port))
			return false;
		if (!field_ctoj_int(env, cls_s_sockaddr_in6, "sin6_flowinfo", *sa, saddr->sin6_flowinfo))
			return false;
		if ((mid = (*env)->GetMethodID(env, cls_s_in6_addr, "<init>", "()V")) == NULL)
			return false;
		if ((addrobj = (*env)->NewObject(env, cls_s_in6_addr, mid)) == NULL)
			return false;
		// allocate a 16-byte array and put it into the s6_addr field
		if ((ba = (*env)->NewByteArray(env, 16)) == NULL)
			return false;
		if ((ba_c = (*env)->GetByteArrayElements(env, ba, NULL)) == NULL)
			return false;
		memcpy(ba_c, saddr->sin6_addr.s6_addr, 16);
		(*env)->ReleaseByteArrayElements(env, ba, ba_c, 0);
		if (!field_ctoj_object(env, cls_s_sockaddr_in6, "sin6_addr", "Ljtux/UNetwork$s_in6_addr;",
		  *sa, addrobj))
			return false;
		// Following needs full 32-bit C int
		if (!field_ctoj_int(env, cls_s_sockaddr_in6, "sin6_scope_id", *sa, saddr->sin6_scope_id))
			return false;
	}
		break;
	default:
		return false; // ??? can't deal with it
	}
	return true;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_accept(JNIEnv *env, jclass obj,
  jint socket_fd, jobject sa, jobject sa_len)
{
	int fd;
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c;

	JTHROW_neg1(fd = accept(socket_fd, (struct sockaddr *)&sa_c, &sa_len_c))
	if (fd != -1) {
		if (!sockaddr_ctoj(env, &sa, (struct sockaddr *)&sa_c))
			return -1;
		if (!set_IntHolder_int(env, sa_len, sa_len_c))
			return -1;
	}
	return fd;
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_bind(JNIEnv *env, jclass obj,
  jint socket_fd, jobject sa, jint sa_len)
{
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c;

	if (!sockaddr_jtoc(env, sa, (struct sockaddr *)&sa_c, &sa_len_c))
		return;
	JTHROW_neg1(bind(socket_fd, (struct sockaddr *)&sa_c, sa_len_c))
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_connect(JNIEnv *env, jclass obj,
  jint socket_fd, jobject sa, jint sa_len)
{
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c;

	if (!sockaddr_jtoc(env, sa, (struct sockaddr *)&sa_c, &sa_len_c))
		return;
	JTHROW_neg1(connect(socket_fd, (struct sockaddr *)&sa_c, sa_len_c))
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_freeaddrinfo(JNIEnv *env, jclass obj,
  jobject infop)
{
	// no-op -- getaddrinfo has already freed the C linked list
}

JNIEXPORT jstring JNICALL Java_jtux_UNetwork_gai_1strerror(JNIEnv *env, jclass obj,
  jint code)
{
	JSTR_RETURN(gai_strerror(code));
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_getaddrinfo(JNIEnv *env, jclass obj,
  jstring nodename, jstring servname, jobject hint, jobject infop)
{
	const char *nodename_c, *servname_c;
	struct addrinfo hint_buf, *hint_c = &hint_buf, *infop_c;
	jclass cls_s_addrinfo = (*env)->FindClass(env, "jtux/UNetwork$s_addrinfo");
	jclass cls_AddrInfoListHead = (*env)->FindClass(env, "jtux/UNetwork$AddrInfoListHead");
	jclass cls_s_sockaddr_in = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in");
	jclass cls_s_sockaddr_in6 = (*env)->FindClass(env, "jtux/UNetwork$s_sockaddr_in6");
	jclass cls_s_in_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in_addr");
	jclass cls_s_in6_addr = (*env)->FindClass(env, "jtux/UNetwork$s_in6_addr");
	int r;

	if (cls_s_addrinfo == NULL || cls_AddrInfoListHead == NULL ||
	  cls_s_sockaddr_in == NULL || cls_s_sockaddr_in6 == NULL ||
	  cls_s_in_addr == NULL || cls_s_in6_addr == NULL)
		return;
	if (hint == NULL)
		hint_c = NULL;
	else {
		memset(hint_c, 0, sizeof(struct addrinfo));
		if (!field_jtoc_int(env, cls_s_addrinfo, "ai_flags", hint, &hint_c->ai_flags))
			return;
		if (!field_jtoc_int(env, cls_s_addrinfo, "ai_family", hint, &hint_c->ai_family))
			return;
		if (!field_jtoc_int(env, cls_s_addrinfo, "ai_socktype", hint, &hint_c->ai_socktype))
			return;
		if (!field_jtoc_int(env, cls_s_addrinfo, "ai_protocol", hint, &hint_c->ai_protocol))
			return;
	}
	nodename_c = (*env)->GetStringUTFChars(env, nodename, NULL);
	servname_c = (*env)->GetStringUTFChars(env, servname, NULL);
	JSTR_NULLTEST(nodename_c)
	JSTR_NULLTEST(servname_c)
	if ((r = getaddrinfo(nodename_c, servname_c, hint_c, &infop_c)) > 0)
		setup_throw_errno_type(env, r, EC_EAI);
	JSTR_REL(nodename_c, nodename)
	JSTR_REL(servname_c, servname)
	if (r == 0) {
		struct addrinfo *p;
		jobject prev_node = infop;
		jobject node;
		for (p = infop_c; p != NULL; p = p->ai_next, prev_node = node) {
			jmethodID mid = (*env)->GetMethodID(env, cls_s_addrinfo, "<init>", "()V");
			jclass cls;
			jobject saobj = NULL;

			if (mid == NULL)
				break;
			if ((node = (*env)->NewObject(env, cls_s_addrinfo, mid)) == NULL)
				break;
			if (p == infop_c)
				cls = cls_AddrInfoListHead;
			else
				cls = cls_s_addrinfo;
			if (!field_ctoj_object(env, cls, "ai_next", "Ljtux/UNetwork$s_addrinfo;",
			  prev_node, node))
				break;
			if (!field_ctoj_int(env, cls_s_addrinfo, "ai_flags", node, p->ai_flags))
				break;
			if (!field_ctoj_int(env, cls_s_addrinfo, "ai_family", node, p->ai_family))
				break;
			if (!field_ctoj_int(env, cls_s_addrinfo, "ai_socktype", node, p->ai_socktype))
				break;
			if (!field_ctoj_int(env, cls_s_addrinfo, "ai_protocol", node, p->ai_protocol))
				break;
			if (!field_ctoj_int(env, cls_s_addrinfo, "ai_addrlen", node, p->ai_addrlen))
				break;
			if (!sockaddr_ctoj(env, &saobj, p->ai_addr))
				break;
			if (!field_ctoj_object(env, cls_s_addrinfo, "ai_addr", "Ljtux/UNetwork$s_sockaddr;",
			  node, saobj))
				break;
			if (!field_ctoj_string(env, cls_s_addrinfo, "ai_canonname", node, p->ai_canonname))
				break;
		}
		freeaddrinfo(infop_c);
	}
}

JNIEXPORT jlong JNICALL Java_jtux_UNetwork_gethostid(JNIEnv *env, jclass obj)
{
	return 0;
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_gethostname(JNIEnv *env, jclass obj,
  jobject name)
{
	long size = 1000;
	char *buf;

#ifdef _SC_HOST_NAME_MAX
	errno = 0;
	if ((size = sysconf(_SC_HOST_NAME_MAX)) == -1) 
		if (errno == 0)
			size = 1000;
		else {
			(void)setup_throw_errno(env, errno);
			return;
		}
#endif
	JTHROW_null(buf = malloc(size))
	if (buf != NULL) {
		JTHROW_neg1(gethostname(buf, size))
		(void)string_buffer_set(env, name, buf);
		free(buf);
	}
}

/*
	sa_len not used
*/
JNIEXPORT void JNICALL Java_jtux_UNetwork_getnameinfo(JNIEnv *env, jclass obj,
  jobject sa, jint sa_len, jobject nodename, jobject servname, jint flags)
{
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c;
	char nodename_c[1000], servname_c[1000];
	int r;

	if (!sockaddr_jtoc(env, sa, (struct sockaddr *)&sa_c, &sa_len_c))
		return;
	if ((r = getnameinfo((struct sockaddr *)&sa_c, sa_len_c, nodename_c,
	  sizeof(nodename_c), servname_c, sizeof(servname_c), flags)) > 0)
		setup_throw_errno_type(env, r, EC_EAI);
	if (nodename != NULL)
		if (!string_buffer_set(env, nodename, nodename_c))
			return;
	if (servname != NULL)
		if (!string_buffer_set(env, servname, servname_c))
			return;
}

// value_len may not be needed
/*
	Burden is on Java caller to supply correct object for value.
*/
JNIEXPORT void JNICALL Java_jtux_UNetwork_getsockopt(JNIEnv *env, jclass obj,
  jint socket_fd, jint level, jint option, jobject value, jobject value_len)
{
	jclass cls_SockOptValue_int = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_int");
	jclass cls_SockOptValue_boolean = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_boolean");
	jclass cls_SockOptValue_s_linger = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_s_linger");
	jclass cls_s_linger = (*env)->FindClass(env, "jtux/UNetwork$s_linger");
	jclass cls_SockOptValue_s_timeval = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_s_timeval");
	jclass cls_s_timeval = (*env)->FindClass(env, "jtux/UProcess$s_timeval");
	union {
		int value_int;
		int value_boolean;
		struct linger value_linger;
		struct timeval value_timeval;
	} all_in_one, *value_c = &all_in_one;
	socklen_t value_len_c;

	if (cls_SockOptValue_int == NULL || cls_SockOptValue_boolean == NULL ||
	  cls_SockOptValue_s_linger == NULL || cls_SockOptValue_s_timeval == NULL ||
	  cls_s_linger == NULL || cls_s_timeval == NULL)
		return;
	value_len_c = sizeof(*value_c);
	JTHROW_neg1(getsockopt(socket_fd, level, option, value_c, &value_len_c))
	if (!set_IntHolder_int(env, value_len, value_len_c))
		return;
	if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_int)) {
		if (!field_ctoj_int(env, cls_SockOptValue_int, "value", value, value_c->value_int))
			return;
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_boolean)) {
		if (!field_ctoj_boolean(env, cls_SockOptValue_boolean, "value", value, value_c->value_int))
			return;
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_s_linger)) {
		jobject linger;

		if (!field_jtoc_object(env, cls_SockOptValue_s_linger, "value", "Ljtux/UNetwork$s_linger;", value, &linger))
			return;
		if (!field_ctoj_int(env, cls_s_linger, "l_onoff", linger, value_c->value_linger.l_onoff))
			return;
		if (!field_ctoj_int(env, cls_s_linger, "l_linger", linger, value_c->value_linger.l_linger))
			return;
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_s_timeval)) {
		// timeval case compiled but not tested
		jobject timeval;

		if (!field_jtoc_object(env, cls_SockOptValue_s_timeval, "value", "Ljtux/UProcess$s_timeval;", value, &timeval))
			return;
		if (!field_ctoj_long(env, cls_s_timeval, "tv_sec", timeval, value_c->value_timeval.tv_sec))
			return;
		if (!field_ctoj_long(env, cls_s_timeval, "tv_usec", timeval, value_c->value_timeval.tv_usec))
			return;
	}
	else {
		(void)setup_throw_errno(env, EINVAL);
		return;
	}
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_htonl(JNIEnv *env, jclass obj,
  jint hostnum)
{
	return htonl(hostnum);
}

JNIEXPORT jshort JNICALL Java_jtux_UNetwork_htons(JNIEnv *env, jclass obj,
  jshort hostnum)
{
	return htons(hostnum);
}

JNIEXPORT jstring JNICALL Java_jtux_UNetwork_inet_1ntop__II(JNIEnv *env, jclass obj,
  jint domain, jint src)
{
	const char *s;
	uint32_t ipv4 = src;
	char dst[INET_ADDRSTRLEN];

	JTHROW_null(s = inet_ntop(domain, &ipv4, dst, sizeof(dst)))
	JSTR_RETURN(s);
}

JNIEXPORT jstring JNICALL Java_jtux_UNetwork_inet_1ntop__I_3B(JNIEnv *env, jclass obj,
  jint domain, jbyteArray src)
{
	return 0;
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_inet_1pton__ILjava_lang_String_2Ljtux_UUtil_00024IntHolder_2(JNIEnv *env, jclass obj,
  jint domain, jstring src, jobject dst)
{
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_inet_1pton__ILjava_lang_String_2_3B(JNIEnv *env, jclass obj,
  jint domain, jstring src, jbyteArray dst)
{
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_listen(JNIEnv *env, jclass obj,
  jint socket_fd, jint backlog)
{
	JTHROW_neg1(listen(socket_fd, backlog))
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_ntohl(JNIEnv *env, jclass obj,
  jint netnum)
{
	return ntohl(netnum);
}

JNIEXPORT jshort JNICALL Java_jtux_UNetwork_ntohs(JNIEnv *env, jclass obj,
  jshort netnum)
{
	return ntohs(netnum);
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_recv(JNIEnv *env, jclass obj,
  jint socket_fd, jbyteArray buffer, jint length, jint flags)
{
	void *buffer_c;
	ssize_t nrcv;

	if ((buffer_c = (*env)->GetByteArrayElements(env, buffer, NULL)) == NULL)
		return -1;
	JTHROW_neg1(nrcv = recv(socket_fd, buffer_c, length, flags))
	(*env)->ReleaseByteArrayElements(env, buffer, buffer_c, 0);
	return nrcv;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_recvfrom(JNIEnv *env, jclass obj,
  jint socket_fd, jbyteArray buffer, jint length, jint flags, jobject sa, jobject sa_len)
{
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c = sizeof(sa_c);
	void *buffer_c;
	ssize_t nrcv;

	if ((buffer_c = (*env)->GetByteArrayElements(env, buffer, NULL)) == NULL)
		return -1;
	JTHROW_neg1(nrcv = recvfrom(socket_fd, buffer_c, length, flags,
	  (struct sockaddr *)&sa_c, &sa_len_c))
	(*env)->ReleaseByteArrayElements(env, buffer, buffer_c, 0);
	if (nrcv != -1) {
		if (!set_IntHolder_int(env, sa_len, sa_len_c))
			return -1;
		if (!sockaddr_ctoj(env, &sa, (struct sockaddr *)&sa_c))
			return -1;
	}
	return nrcv;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_recvmsg(JNIEnv *env, jclass obj,
  jint socket_fd, jobject message, jint flags)
{
	jclass cls_s_msghdr = (*env)->FindClass(env, "jtux/UNetwork$s_msghdr");
	struct msghdr msg_c;
	jobject msg_name, msg_control, msg_iov;
	struct sockaddr_storage sa_c;
	ssize_t nrcv;
	jbyteArray *v_bytearray;

	msg_c.msg_name = &sa_c;
	if (!field_jtoc_object(env, cls_s_msghdr, "msg_iov", "[Ljtux/UFile$s_iovec;", message, &msg_iov))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_iovlen", message, &msg_c.msg_iovlen))
		return -1;
	if (!field_jtoc_object(env, cls_s_msghdr, "msg_control", "[B", message, &msg_control))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_controllen", message, &msg_c.msg_controllen))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_flags", message, &msg_c.msg_flags))
		return -1;
	if ((msg_c.msg_iov = iovec_jtoc(env, msg_iov, msg_c.msg_iovlen, &v_bytearray)) == NULL)
		return -1;
	if (msg_control == NULL)
		msg_c.msg_control = NULL;
	else if ((msg_c.msg_control = (*env)->GetByteArrayElements(env, msg_control, NULL)) == NULL) {
		iovec_jtoc_release(env, msg_c.msg_iov, msg_c.msg_iovlen, v_bytearray);
		return -1;
	}
	JTHROW_neg1(nrcv = recvmsg(socket_fd, &msg_c, flags))
	iovec_jtoc_release(env, msg_c.msg_iov, msg_c.msg_iovlen, v_bytearray);
	if (msg_control != NULL)
		(*env)->ReleaseByteArrayElements(env, msg_control, msg_c.msg_control, JNI_ABORT);
	if (!field_jtoc_object(env, cls_s_msghdr, "msg_name", "Ljtux/UNetwork$s_sockaddr;", message, &msg_name))
		return -1;
	if (msg_name != NULL)
		if (!sockaddr_ctoj(env, &msg_name, (struct sockaddr *)&sa_c))
			return -1;
	return nrcv;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_send(JNIEnv *env, jclass obj,
  jint socket_fd, jbyteArray data, jint length, jint flags)
{
	void *data_c;
	ssize_t nsent;

	if ((data_c = (*env)->GetByteArrayElements(env, data, NULL)) == NULL)
		return -1;
	JTHROW_neg1(nsent = send(socket_fd, data_c, length, flags))
	(*env)->ReleaseByteArrayElements(env, data, data_c, JNI_ABORT);
	return nsent;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_sendmsg(JNIEnv *env, jclass obj,
  jint socket_fd, jobject message, jint flags)
{
	jclass cls_s_msghdr = (*env)->FindClass(env, "jtux/UNetwork$s_msghdr");
	struct msghdr msg_c;
	jobject msg_name, msg_control, msg_iov;
	struct sockaddr_storage sa_c;
	ssize_t nsent;
	jbyteArray *v_bytearray;

	if (!field_jtoc_object(env, cls_s_msghdr, "msg_name", "Ljtux/UNetwork$s_sockaddr;", message, &msg_name))
		return -1;
	if (!sockaddr_jtoc(env, msg_name, (struct sockaddr *)&sa_c, &msg_c.msg_namelen))
		return -1;
	msg_c.msg_name = &sa_c;
	if (!field_jtoc_object(env, cls_s_msghdr, "msg_iov", "[Ljtux/UFile$s_iovec;", message, &msg_iov))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_iovlen", message, &msg_c.msg_iovlen))
		return -1;
	if (!field_jtoc_object(env, cls_s_msghdr, "msg_control", "[B", message, &msg_control))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_controllen", message, &msg_c.msg_controllen))
		return -1;
	if (!field_jtoc_int(env, cls_s_msghdr, "msg_flags", message, &msg_c.msg_flags))
		return -1;
	if ((msg_c.msg_iov = iovec_jtoc(env, msg_iov, msg_c.msg_iovlen, &v_bytearray)) == NULL)
		return -1;
	if (msg_control == NULL)
		msg_c.msg_control = NULL;
	else if ((msg_c.msg_control = (*env)->GetByteArrayElements(env, msg_control, NULL)) == NULL) {
		iovec_jtoc_release_nocopy(env, msg_c.msg_iov, msg_c.msg_iovlen, v_bytearray);
		return -1;
	}
	JTHROW_neg1(nsent = sendmsg(socket_fd, &msg_c, flags))
	iovec_jtoc_release_nocopy(env, msg_c.msg_iov, msg_c.msg_iovlen, v_bytearray);
	if (msg_control != NULL)
		(*env)->ReleaseByteArrayElements(env, msg_control, msg_c.msg_control, JNI_ABORT);
	return nsent;
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_sendto(JNIEnv *env, jclass obj,
  jint socket_fd, jbyteArray message, jint length, jint flags, jobject sa, jint sa_len)
{
	struct sockaddr_storage sa_c;
	socklen_t sa_len_c;
	void *message_c;
	ssize_t nsent;

	if (!sockaddr_jtoc(env, sa, (struct sockaddr *)&sa_c, &sa_len_c))
		return -1;
	if ((message_c = (*env)->GetByteArrayElements(env, message, NULL)) == NULL)
		return -1;
	JTHROW_neg1(nsent = sendto(socket_fd, message_c, length, flags,
	  (struct sockaddr *)&sa_c, sa_len_c))
	(*env)->ReleaseByteArrayElements(env, message, message_c, JNI_ABORT);
	return nsent;
}

JNIEXPORT void JNICALL Java_jtux_UNetwork_setsockopt(JNIEnv *env, jclass obj,
  jint socket_fd, jint level, jint option, jobject value, jint value_len)
{
	jclass cls_SockOptValue_int = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_int");
	jclass cls_SockOptValue_boolean = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_boolean");
	jclass cls_SockOptValue_s_linger = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_s_linger");
	jclass cls_s_linger = (*env)->FindClass(env, "jtux/UNetwork$s_linger");
	jclass cls_SockOptValue_s_timeval = (*env)->FindClass(env, "jtux/UNetwork$SockOptValue_s_timeval");
	jclass cls_s_timeval = (*env)->FindClass(env, "jtux/UProcess$s_timeval");
	void *value_c;
	int value_int;
	struct linger value_linger;
	struct timeval value_timeval;

	if (cls_SockOptValue_int == NULL || cls_SockOptValue_boolean == NULL ||
	  cls_SockOptValue_s_linger == NULL || cls_SockOptValue_s_timeval == NULL ||
	  cls_s_linger == NULL || cls_s_timeval == NULL)
		return;
	if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_int)) {
		if (!field_jtoc_int(env, cls_SockOptValue_int, "value", value, &value_int))
			return;
		value_c = &value_int;
		value_len = sizeof(value_int);
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_boolean)) {
		if (!field_jtoc_boolean(env, cls_SockOptValue_boolean, "value", value, &value_int))
			return;
		value_c = &value_int;
		value_len = sizeof(value_int);
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_s_linger)) {
		jobject linger;

		if (!field_jtoc_object(env, cls_SockOptValue_s_linger, "value", "Ljtux/UNetwork$s_linger;", value, &linger))
			return;
		if (!field_jtoc_int(env, cls_s_linger, "l_onoff", linger, &value_linger.l_onoff))
			return;
		if (!field_jtoc_int(env, cls_s_linger, "l_linger", linger, &value_linger.l_linger))
			return;
		value_c = &value_linger;
		value_len = sizeof(value_linger);
	}
	else if ((*env)->IsInstanceOf(env, value, cls_SockOptValue_s_timeval)) {
		// timeval case compiled but not tested
		jobject timeval;
		long n;

		if (!field_jtoc_object(env, cls_SockOptValue_s_timeval, "value", "Ljtux/UProcess$s_timeval;", value, &timeval))
			return;
		if (!field_jtoc_long(env, cls_s_timeval, "tv_sec", timeval, &n))
			return;
		value_timeval.tv_sec = (time_t)n;
		if (!field_jtoc_long(env, cls_s_timeval, "tv_usec", timeval, &n))
			return;
		value_timeval.tv_usec = (suseconds_t)n;
		value_c = &value_timeval;
		value_len = sizeof(value_timeval);
	}
	else {
		(void)setup_throw_errno(env, EINVAL);
		return;
	}
	JTHROW_neg1(setsockopt(socket_fd, level, option, value_c, value_len))
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_sockatmark(JNIEnv *env, jclass obj,
  jint socket_fd)
{
#if _XOPEN_VERSION >= 600 && !defined(DARWIN)
// Following compiled but not tested
	int r;

	JTHROW_neg1(r = sockatmark(socket_fd))
	return r;
#else
	(void)setup_throw_errno(env, ENOSYS);
	return -1;
#endif
}

JNIEXPORT jint JNICALL Java_jtux_UNetwork_socket(JNIEnv *env, jclass obj,
  jint domain, jint type, jint protocol)
{
	int fd;

	JTHROW_neg1(fd = socket(domain, type, protocol));
	return fd;
}


