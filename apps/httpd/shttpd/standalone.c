/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */
#include "captiveportal.h"

#include "defs.h"
#include "shttpdext.h"

static int	exit_flag;	/* Program termination flag	*/

static void
signal_handler(int sig_num)
{
	switch (sig_num) {
#ifndef _WIN32
	case SIGCHLD:
		while (waitpid(-1, &sig_num, WNOHANG) > 0) ;
		break;
#endif /* !_WIN32 */
	default:
		exit_flag = sig_num;
		break;
	}
}

struct shttpd_ctx	*ctx;

#if 0
/* successfully return 0, else -1, and strsplitted will be null string */
static int strsplit(const char * const strorgin, /* need to split string */
                    char * strsplitted,          /* splitted string */
                    unsigned int length,         /* strsplit buffer length */
                    int token,                   /* split token  */
                    unsigned int index)          /* get substring index */
{
    int i = 0;
    char * prev, * next;
    prev = (char *)strorgin;
    next = (char *)strorgin;

    memset(strsplitted, '\0', length);

    if(strorgin == NULL)
        return -1;

    for(; i < index - 1; i++)
    {
        prev = strchr(prev, token);
        if(prev == NULL)
        {
            if(index == 1)
                prev = (char *)strorgin;
            else
                return -1;
        }
        else
            while(*(++prev) == token);
    }

    if((next = strchr(prev, token)) != NULL)
    {
        if(length < next - prev + 1) /* buffer length not enough */
            return -1;
        memcpy(strsplitted, prev, next - prev);
        DBG((stderr, "===> strsplitted = %s", strsplitted));
        return 0;
    }
    else
    {
        if(length < strlen(prev) + 1) /* buffer length not enough */
            return -1;
        strcpy(strsplitted, prev);
        DBG((stderr, "===> strsplitted = %s", strsplitted));
        return 0;
    }
}
#endif

int
main(int argc, char *argv[])
{
    printf("This daemon go ....!");
#if !defined(NO_AUTH)
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'A') {
		if (argc != 6)
			_shttpd_usage(argv[0]);
		exit(_shttpd_edit_passwords(argv[2],argv[3],argv[4],argv[5]));
	}
#endif /* NO_AUTH */

	if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
		_shttpd_usage(argv[0]);

#if defined(_WIN32)
	try_to_run_as_nt_service();
#endif /* _WIN32 */

#ifndef _WIN32
	(void) signal(SIGCHLD, signal_handler);
	(void) signal(SIGPIPE, SIG_IGN);
#endif /* _WIN32 */

	(void) signal(SIGTERM, signal_handler);
	(void) signal(SIGINT, signal_handler);

	if ((ctx = shttpd_init(argc, argv)) == NULL)
		_shttpd_elog(E_FATAL, NULL, "%s",
		    "Cannot initialize SHTTPD context");
#ifdef IPV6_ENABLE
	_shttpd_elog(E_LOG, NULL, "shttpd %s isIpv6 %d started on port(s) %s, serving %s",
	    VERSION, isIpv6, ctx->options[OPT_PORTS], ctx->options[OPT_ROOT]);
#else
	_shttpd_elog(E_LOG, NULL, "shttpd %s started on port(s) %s, serving %s",
	    VERSION, ctx->options[OPT_PORTS], ctx->options[OPT_ROOT]);
#endif //IPV6_ENABLE

        /* add by bowen for extend cgi function */

        /* Change index page to the first html in modcntr.cfg */
    shttpdext_init(ctx);

	while (exit_flag == 0)
		shttpd_poll(ctx, 10 * 1000);

	_shttpd_elog(E_LOG, NULL, "Exit on signal %d", exit_flag);
	shttpd_fini(ctx);

	return (EXIT_SUCCESS);
}
