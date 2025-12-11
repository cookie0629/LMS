if (PAM_INCLUDE_DIR AND PAM_LIBRARY)
	set(PAM_FIND_QUIETLY TRUE)
endif (PAM_INCLUDE_DIR AND PAM_LIBRARY)

find_path(PAM_INCLUDE_DIR NAMES security/pam_appl.h pam/pam_appl.h) # 查找PAM头文件的位置# NAMES参数指定可能存在的头文件名# 会在系统默认路径中查找security/pam_appl.h或pam/pam_appl.h
find_library(PAM_LIBRARY pam) # 查找PAM库文件# 会在系统默认路径中查找名为pam的库
find_library(DL_LIBRARY dl) # 查找动态链接库dl# 会在系统默认路径中查找名为dl的库

if (PAM_INCLUDE_DIR AND PAM_LIBRARY) # 再次检查PAM头文件和库是否都被找到
	set(PAM_FOUND TRUE)
	if (DL_LIBRARY)
		set(PAM_LIBRARIES ${PAM_LIBRARY} ${DL_LIBRARY})
	else (DL_LIBRARY)
		set(PAM_LIBRARIES ${PAM_LIBRARY})
	endif (DL_LIBRARY)

	if (EXISTS ${PAM_INCLUDE_DIR}/pam/pam_appl.h)
		set(HAVE_PAM_PAM_APPL_H 1)
	endif (EXISTS ${PAM_INCLUDE_DIR}/pam/pam_appl.h)

	if (NOT DEFINED PAM_MESSAGE_CONST)
		include(CheckCXXSourceCompiles)
		check_cxx_source_compiles("
#if ${HAVE_PAM_PAM_APPL_H}+0
# include <pam/pam_appl.h>
#else
# include <security/pam_appl.h>
#endif
static int PAM_conv(
	int num_msg,
	const struct pam_message **msg, /* this is the culprit */
	struct pam_response **resp,
	void *ctx)
{
	return 0;
}
int main(void)
{
	struct pam_conv PAM_conversation = {
		&PAM_conv, /* this bombs out if the above does not match */
		0
	};
	return 0;
}
" PAM_MESSAGE_CONST)
	endif (NOT DEFINED PAM_MESSAGE_CONST)
	set(PAM_MESSAGE_CONST ${PAM_MESSAGE_CONST} CACHE BOOL "PAM expects a conversation function with const pam_message")

endif (PAM_INCLUDE_DIR AND PAM_LIBRARY)

if (PAM_FOUND)
	if (NOT PAM_FIND_QUIETLY)
		message(STATUS "Found PAM: ${PAM_LIBRARIES}")
	endif (NOT PAM_FIND_QUIETLY)
else (PAM_FOUND)
	if (PAM_FIND_REQUIRED)
		message(FATAL_ERROR "PAM was not found")
	endif(PAM_FIND_REQUIRED)
endif (PAM_FOUND)

mark_as_advanced(PAM_INCLUDE_DIR PAM_LIBRARY DL_LIBRARY PAM_MESSAGE_CONST) 
