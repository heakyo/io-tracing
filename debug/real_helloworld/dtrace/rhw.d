#!/usr/sbin/dtrace -s

#pragma D option flowindent

BEGIN
{
        /*proc = "proc"*/
}

/*common*************************************************************************************/

/*entry**************************************************************************************/

probefunc:entry
/execname == proc/
{
        /*this->args0 = args[0];*/
        /*printf("(args0--yyy) ", this->args[0]->yyy);*/
}

/*return*************************************************************************************/

probefunc:return
/execname == proc/
{}
