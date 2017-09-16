TAIR_DIR=`dirname $0`
TAIR_BIN_DIR=/usr/local/storage/tair_bin/sbin
TAIR_ETC_DIR=/usr/local/storage/tair_bin/etc

cd ${TAIR_DIR}

ulimit -c unlimited
ulimit -n 344800

export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH:/usr/local/lib:/opt/csr/common/lib

VAL_LOG_PATH=./val_log
VAL_CMD="valgrind --tool=memcheck --leak-check=full --show-reachable=yes --log-file=${VAL_LOG_PATH}/valgrind.log."`date +%m%d%s`

DS_CMD=${TAIR_BIN_DIR}/tair_server
CS_CMD=${TAIR_BIN_DIR}/tair_cfg_svr
IV_CMD=${TAIR_BIN_DIR}/inval_server

VAL_DS_CMD="${VAL_CMD} ${TAIR_BIN_DIR}/tair_server"
VAL_CS_CMD="${VAL_CMD} ${TAIR_BIN_DIR}/tair_cfg_svr"
VAL_IV_CMD="${VAL_CMD} ${TAIR_BIN_DIR}/inval_server"
check_folder()
{
  	if [ ! -d "$VAL_LOG_PATH" ]; then
    	mkdir "$VAL_LOG_PATH"
  	fi
}

fail_echo()                                                                                                                                                            
{
    printf "\033[31m $* ... CHECK IT\033[0m\n"
}

warn_echo()
{
    printf  "\033[36m $* \033[0m\n"
}

#$1 dataserver.conf*
check_shm_size()
{
  #check slab_mem_size and tmpfs size
	if [ ! -e $1 ];then
		fail_echo "NO $1"
		return 1
	fi
 	engine=`grep "storage_engine *=" $1 | grep -v "#" | awk -F '[ =]' '{print $NF}'`
  	if [ "$engine" == "" ]
  	then
    	echo "can not find storage_engine"
    	return 1
  	fi

 	if [ $engine == "mdb" ]
  	then
    	safety_free_size=100;
    	tmpfs_size=`df -m |grep tmpfs | awk '{print $2}'`
    	let "safety_size = $tmpfs_size - $safety_free_size"
    	slab_mem_size=`grep slab_mem_size $1 | grep -v "#" | awk -F '[ =]' '{print $NF}'`

    	if [ "$slab_mem_size" == "" ]
    	then
      		echo "can not find slab_mem_size"
      		return 1
    	fi

    	echo "mdb engine. tmpfs_size: "$tmpfs_size"  safety_size: "$safety_size"  slab_mem_size: "$slab_mem_size > /dev/null
    	if [ $slab_mem_size -ge $safety_size ]
    	then
    	 	echo "slab_mem_size is not little then safety_size."
      		return 1
    	else
      		return 0
    	fi
  	else
    	return 0
  	fi
}

get_index()
{
    local ds_index=""
  # range index type
    range_index=`echo "$1" | egrep -o '[0-9]+-[0-9]+'` # ignore non-digit and '-' signal chars
    for index in $range_index
    do  
    	start_index=`echo $index | awk -F '-' '{print $1}'`
    	end_index=`echo $index | awk -F '-' '{print $2}'`
    	if [ $start_index -gt $end_index ]
    	then
        	echo ""
        	exit 1;
    	fi  
    	ds_index="$ds_index `seq $start_index $end_index`"
    done

  # individual index type
    in_index=`echo " $1 " | tr ',' ' '| sed 's/ /  /g' | egrep -o ' [0-9]+ '`
    ds_index="$ds_index $in_index"
    if [ -z "$range_index" ] && [ -z "$in_index" ]
    then
    	echo ""
    else
    	echo "$ds_index"
    fi  
}
check_run()
{
	local run_pid
	case $1 in
	cs)
		run_pid=`ps -ef| grep "${CS_CMD}" | grep -v 'grep' | awk '{print $2}'`
		;;
	ssd)
		run_pid=`ps -ef| grep "${TAIR_ETC_DIR}/dataserver.conf.ssd.${2}" | grep -v 'grep' | awk '{print $2}'`
		;;
	ds)
		run_pid=`ps -ef| grep "${TAIR_ETC_DIR}/dataserver.conf.${2}" | grep -v 'grep' | awk '{print $2}'`
		;;
	mem)
		run_pid=`ps -ef| grep "${TAIR_ETC_DIR}/dataserver.conf.mem" | grep -v 'grep' | awk '{print $2}'`
		;;
	other)
		run_pid=`ps -ef| grep "$2" | grep -v grep | awk '{print $2}'`
		;;
	*)
		warn_echo "invalid range"
	esac
	if [ -z "$run_pid" ]; then
		echo 0
	else
		echo $run_pid
	fi
}
#$1 CMD
start_cs()
{
	local ret_pid
	ret_pid=`check_run "cs"`
	if [ $ret_pid -gt 0 ]; then
		fail_echo "cs is already running pid: $ret_pid"
		exit 1
	else
	  	$1 -f ${TAIR_ETC_DIR}/configserver.conf
		sleep 4
		ret_pid=`check_run "cs"`
		if [ $ret_pid -gt 0 ]; then
			warn_echo "cs is up SUCCESSFULLY pid: ${ret_pid}"
			exit 0
		else 
			fail_echo "cs FAIL to up"
			exit 1
		fi
	fi
}
start_mem()
{
	local ret_pid
	ret_pid=`check_run "mem"`
	if [ $ret_pid -gt 0 ]; then
		fail_echo "mem is already running pid: $ret_pid"
		exit 1
	else
		if check_shm_size ${TAIR_ETC_DIR}/dataserver.conf.mem
		then
			$1 -f ${TAIR_ETC_DIR}/dataserver.conf.mem
			sleep 4
			ret_pid=`check_run "mem"`
			if [ $ret_pid -gt 0 ]; then
				warn_echo "mem is up SUCCESSFULLY pid: $ret_pid"
				exit 0
			else
				fail_echo "mem FALL to up"
				exit 1
			fi
		else
			fail_echo "check tmpfs"
			exit 1
		fi
	fi
}
start_dataserver()
{
	local ret_pid
	local flag=0
	local index=`get_index $2`
	local conf_file=""
	declare -a array
	if [ $3 == "ssd" ]; then
		conf_file="dataserver.conf.ssd."
	else
		conf_file="dataserver.conf."
	fi
	for i in $index
	do
		ret_pid=`check_run ${3} $i`
		if [ $ret_pid -gt 0 ]; then
			array[$i]="$i"
		else
			if check_shm_size ${TAIR_ETC_DIR}/${conf_file}${i}
			then
				$1 -f ${TAIR_ETC_DIR}/${conf_file}${i}
			else
				fail_echo "check tmpfs"
				flag=1
			fi
		fi
	done
	sleep 4
	for i in $index
	do
		ret_pid=`check_run ${3} $i`
        if [ $ret_pid -gt 0 ] && [ -z ${array[$i]} ]; then
			warn_echo "${3} ${i} is up SUCCESSFULLY pid: $ret_pid"
        elif [ -z ${array[$i]} ] && [ $ret_pid -le 0 ]; then
            fail_echo "${3} ${i} FALL to up"
			flag=1
		else
			fail_echo "${3} $i is already running pid: $ret_pid"
			flag=1
		fi
	done
	if [ $flag -eq 0 ]; then
		return 0
	else
		return 1
	fi
}
start_iv()
{
	$1 -f ${TAIR_ETC_DIR}/invalserver.conf
}

stop_cs()
{
	local ret_pid
	ret_pid=`check_run "cs"`
	if [ $ret_pid -gt 0 ]; then
		kill -15 $ret_pid
		sleep 8
		ret_pid=`check_run "cs"`
		if [ $ret_pid -gt 0 ]; then
			fail_echo "cs FAIL to stop pid: ${ret_pid}"
			return 1
		else 
			warn_echo "cs exit SUCCESSFULLY"
			return 0
		fi
	elif [ $ret_pid -eq 0 ]; then
		fail_echo "cs is NOT running"
		return 1
	else
		return 1
	fi
}
stop_mem()
{
	local ret_pid
	ret_pid=`check_run "mem"`
	if [ $ret_pid -gt 0 ]; then
		kill -15 $ret_pid
		sleep 8
		ret_pid=`check_run "mem"`
		if [ $ret_pid -gt 0 ]; then
			fail_echo "mem FALL to stop pid: $ret_pid"
			return 1
		else
			warn_echo "mem exit SUCCESSFULLY"
			return 0
		fi
	else
		fail_echo "mem is NOT running"
		return 1
	fi
}
stop_dataserver()
{
	local ret_pid
	local flag=0
	local index=`get_index $1`
	local falg=0
	array=("god" "god" "god" "god" "god")
	for i in $index
	do
		ret_pid=`check_run $2 $i`
		if [ $ret_pid -gt 0 ]; then
			kill -15 $ret_pid
			falg=1
		else
			array[$i]="bad"
		fi
	done
	if [ $falg -eq 1 ]; then
		sleep 8
		falg=0
	fi
	for i in $index
	do
		ret_pid=`check_run $2 $i`
		if [ $ret_pid -gt 0 ]; then
			fail_echo "$2 $i FALL to stop pid: ${ret_pid}"
			falg=1
		elif [ ${array[$i]} == "bad" ]; then
			fail_echo "$2 $i is NOT running"
		else
			warn_echo "$2 $i exit SUCCESSFULLY"
		fi
	done
	if [ $flag -eq 0 ]; then
		return 0
	else
		return 1
	fi
}
start_or_stop_all()
{
	#############################disk#############################
	local index=`ls ${TAIR_ETC_DIR} | grep 'dataserver.conf.' | grep -v -E 'default|ssd|mem'`
	local ds_index=""
	for i in $index
	do
		tmp_index=`echo $i | awk -F. '{print $3}'`
		if [ -z "${ds_index}" ];then
			ds_index="${tmp_index}"
		else
			ds_index="${ds_index},${tmp_index}"
		fi
	done
	if [ -n "${ds_index}"  ]; then
		if [ $1 = "start" ];then
			start_dataserver "${DS_CMD}" $ds_index "ds"
		else
			stop_dataserver $ds_index "ds"	
		fi
	fi
	############################ssd################################
	local ssd_index=""
	index=`ls ${TAIR_ETC_DIR} | grep 'dataserver.conf.ssd.'`
	for i in $index
	do
		tmp_index=`echo $i | awk -F. '{print $4}'`
		if [ -z "${ssd_index}" ];then
			ssd_index="${tmp_index}"
		else
			ssd_index="${ssd_index},${tmp_index}"
		fi
	done
	if [ -n "${ssd_index}" ]; then
		if [ $1 = "start" ];then
			start_dataserver "${DS_CMD}" $ssd_index "ssd"
		else
			stop_dataserver $ssd_index "ssd"
		fi
	fi
	#############################mem######################
	if [ -e ${TAIR_ETC_DIR}/dataserver.conf.mem ];then
		if [ $1 = "start" ];then
			start_mem "${DS_CMD}"
		else
			stop_mem
		fi
	fi
}
stop_iv()
{
  kill `cat logs/inval.pid`
}


valgrind_stop_all()
{
  killall -15 memcheck
}


clean()
{
  cd ${TAIR_DIR}
  rm logs/* -f
  rm fdb/* -rf
  rm data/* -rf
  rm /dev/shm/mdb_shm* -f
  rm bdb/* -rf
}

log_debug2warn()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/log_level=debug/log_level=warn/" *.conf
}

log_warn2debug()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/log_level=warn/log_level=debug/" *.conf
}

log_debug2info()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/log_level=debug/log_level=info/" *.conf
}

log_info2debug()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/log_level=info/log_level=debug/" *.conf
}

tomdb()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/^storage_engine.*$/storage_engine=mdb/" *.conf
  cd ${TAIR_ETC_DIR} && sed -i "s/mdb_type=mdb_shm.*$/mdb_type=mdb/" *.conf
}

tomdb_shm()
{
  cd ${TAIR_ETC_DIR} && sed -i "s/^storage_engine.*$/storage_engine=mdb/" *.conf
  cd ${TAIR_ETC_DIR} && sed -i "s/mdb_type=mdb.*$/mdb_type=mdb_shm/" *.conf
}

tofdb()
{
  cd ${TAIR_ETC_DIR} &&  sed -i "s/^storage_engine.*$/storage_engine=fdb/" *.conf
}

tobdb()
{
  cd ${TAIR_ETC_DIR} &&  sed -i "s/^storage_engine.*$/storage_engine=bdb/" *.conf
}
#######################################
case "$1" in
  	start_cs)
  	start_cs  "${CS_CMD}"
  	;;
  	stop_cs)
  	stop_cs
  	;;
  	start_ds)
  	start_dataserver "${DS_CMD}" $2 "ds"
  	;;
  	stop_ds)
  	stop_dataserver $2 "ds"
  	;;
  	start_ssd)
  	start_dataserver "${DS_CMD}" $2 "ssd"
  	;;
	stop_ssd)
	stop_dataserver $2 "ssd"
	;;
  	start_mem)
  	start_mem "${DS_CMD}"
  	;;
	stop_mem)
	stop_mem
	;;
	start_all)
	start_or_stop_all "start"
	;;
	stop_all)
	start_or_stop_all "stop"
	;;
  	start_iv)
  	start_iv "${IV_CMD}"
  	;;
  	stop_iv)
  	stop_iv
  	;;
  	valgrind_start_cs)
  	check_folder
  	start_cs "${VAL_CS_CMD}"
  	;;
  	valgrind_start_ds)
  	check_folder
  	start_ds "${VAL_DS_CMD}"
  	;;
  	valgrind_start_iv)
  	check_folder
  	start_iv "${VAL_IV_CMD}"
  	;;
  	valgrind_stop)
  	valgrind_stop_all
  	;;
  	clean)
  	clean
  	;;
  	log_debug2warn)
  	log_debug2warn
  	;;
  	log_warn2debug)
  	log_warn2debug
  	;;
  	log_debug2info)
  	log_debug2info
  	;;
  	log_info2debug)
  	log_info2debug
  	;;
  	tomdb)
  	tomdb
  	;;
  	tofdb)
  	tofdb
  	;;
  	tobdb)
  	tobdb
  	;;
  	tomdb_shm)
  	tomdb_shm
  	;;
  	*)
  	warn_echo "usage: {start_cs|stop_cs|start_ds ds_index | stop_ds ds_index | start_ssd ds_index | stop_ssd ds_index | start_all | stop_all }"
	warn_echo "       {ds_index format: 1,2,3 OR 1-3 }"
esac

