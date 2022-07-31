#!/bin/bash
on_exit() {
	echo "Bye..."
	kill_pid
	exit
}

on_enter() {
	echo "Hello..."
	make clean >/dev/null 2>&1
	run
}

kill_pid() {
	pids_to_kill=$(pgrep fluid)
	if [ "$pids_to_kill" != "" ]; then
		kill -9 "$pids_to_kill" >/dev/null 2>&1
	fi
}

run() {
	echo "Running..."
	kill_pid
	make && make run &
}

trap "on_exit" SIGINT SIGTERM
on_enter
while true; do
	inotifywait -e modify,create,delete -r ./src &&
		run
done
