#!/bin/bash
# start stop 命令自动补充
_complete_func()
{
	COMPREPLY=()
	local cur=${COMP_WORDS[COMP_CWORD]}
	local cmd=${COMP_WORDS[COMP_CWORD-1]}
	case $COMP_CWORD in
		1)
			#echo $COMP_CWORD
			#echo $COMP_WORDS
			#COMPREPLY=( $(compgen -W 'center 101 102' -- $cur) )
			COMPREPLY=( $(compgen -W '`ls ./conf`' -- $cur) )
			;;
		2)
			server_name=`ls ./conf/$cmd | awk '{print substr($1, 0, index($1, ".cfg")-1)}'`
			server_name=$server_name" all"
			COMPREPLY=( $(compgen -W '$server_name' -- $cur) )
			;;
		*)
			#echo $COMP_CWORD
			#echo $COMP_WORDS
			;;
	esac
	return 0
}

complete -F _complete_func ./start.sh
complete -F _complete_func ./stop.sh
