#!/usr/sbin/dtrace -s

/*
 *      ./rhw_template.d -c './main'
 */

#pragma D option flowindent

BEGIN
{
        proc = "main"
}

/*-----------------------------------------------------------------------------*/

